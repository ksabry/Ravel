#include "OrderedArgsMatcher.hpp"
#include "Util/Assert.hpp"

namespace Ravel::SubML
{
	OrderedArgsMatcher::OrderedArgsMatcher(std::vector<OrderedQuantifiedExpressionMatcher *> matchers)
		: matchers(matchers), stack(nullptr), stack_idx(0)
	{
	}
	OrderedArgsMatcher::~OrderedArgsMatcher()
	{
		DeleteStack();
		for (auto matcher : matchers) delete matcher;
	}

	void OrderedArgsMatcher::BeginInternal()
	{
		Expression * parent = MatchArgument<0>();

		if (stack) DeleteStack();
		stack = new Frame[matchers.size()];
		stack_idx = 0;

		if (matchers.size() > 0) BeginFrame(0, input_captures, parent->Args(), matchers, nullptr);
	}

	bool OrderedArgsMatcher::NextInternal()
	{
		Expression * parent = MatchArgument<0>();

		if (matchers.size() == 0)
		{
			if (parent->Args().size() == 0)
			{
				output_captures = input_captures;
				Finish(); return true;
			}
			return false;
		}

		while (stack_idx >= 0)
		{
			if (!stack[stack_idx].initialized)
			{
				stack_idx--;
				continue;
			}

			auto frame_captures = stack[stack_idx].matcher->Next();
			if (!frame_captures)
			{
				FinishFrame(stack_idx);
				stack_idx--;
				continue;
			}

			auto incoming_remaining_exprs = stack[stack_idx].incoming_remaining_exprs;
			for (uint32_t i = 0; i < stack[stack_idx].matcher->MatchLength(); i++)
			{
				incoming_remaining_exprs[stack[stack_idx].expr_start_idx + i] = nullptr;
			}

			if (stack_idx == matchers.size() - 1)
			{
				if (IsComplete(incoming_remaining_exprs))
				{
					output_captures = *frame_captures;
					return true;
				}
			}
			else
			{
				BeginFrame(
					++stack_idx, 
					*frame_captures, 
					incoming_remaining_exprs, 
					stack[stack_idx].remaining_matchers,
					stack[stack_idx].has_bounds ? &stack[stack_idx].remaining_bounds : nullptr
				);
			}
		}

		return false;
	}

	void OrderedArgsMatcher::DeleteStack()
	{
		for (uint32_t i = 0; i < matchers.size(); i++)
		{
			if (stack[i].initialized) FinishFrame(i);
		}
		delete[] stack;
	}

	void OrderedArgsMatcher::BeginFrame(
		uint32_t idx,
		std::vector<uint64_t> & incoming_captures,
		std::vector<Expression *> & incoming_remaining_exprs,
		std::vector<OrderedQuantifiedExpressionMatcher *> & remaining_matchers,
		std::vector<Bounds> * remaining_bounds)
	{
		Assert(idx < matchers.size());
		Expression * parent = MatchArgument<0>();

		if (!remaining_bounds && !CalculateBounds(stack[idx].remaining_bounds, stack[idx].remaining_matchers)) return;
		auto bounds = stack[idx].remaining_bounds;

		uint32_t next_matcher_idx;
		for (uint32_t i = 0; i < matchers.size(); i++)
		{
			if (bounds[i].start == bounds[i].end && matchers[i])
			{
				next_matcher_idx = i;
				break;
			}
		}
		stack[idx].matcher = matchers[next_matcher_idx];

		uint32_t next_start_idx = bounds[next_matcher_idx].start;
		uint32_t next_end_idx = next_start_idx;
		while (next_end_idx < parent->Args().size() && !parent->Args()[next_end_idx]) next_end_idx++;

		stack[idx].matcher->Begin(incoming_captures, &parent->Args(), next_start_idx, next_end_idx);

		stack[idx].expr_start_idx = next_start_idx;
		stack[idx].incoming_remaining_exprs = incoming_remaining_exprs;

		stack[idx].remaining_matchers = remaining_matchers;
		stack[idx].remaining_matchers[next_matcher_idx] = nullptr;
		
		if (stack[idx].matcher->GetQuantifier().low == stack[idx].matcher->GetQuantifier().high)
		{
			stack[idx].remaining_bounds = *remaining_bounds;
			stack[idx].has_bounds = true;
		}
		else
		{
			stack[idx].has_bounds = false;
		}
		stack[idx].initialized = true;
	}

	void OrderedArgsMatcher::FinishFrame(uint32_t idx)
	{
		Assert(stack[idx].initialized);
		stack[idx].initialized = false;
	}

	bool OrderedArgsMatcher::IsComplete(std::vector<Expression *> & remaining_exprs)
	{
		for (auto expr : remaining_exprs)
		{
			if (expr) return false;
		}
		return true;
	}

	bool OrderedArgsMatcher::CalculateBounds(std::vector<Bounds> & result, std::vector<OrderedQuantifiedExpressionMatcher *> & remaining_matchers)
	{
		Expression * parent = MatchArgument<0>();
		uint32_t expr_count = parent->Args().size();

		uint32_t start_low, start_high;
		Bounds * ltr_bounds = new Bounds[matchers.size()];
		Bounds * rtl_bounds = new Bounds[matchers.size()];
		uint32_t qlow, qhigh;

		start_low = start_high = 0;
		for (int32_t i = 0; i < matchers.size(); i++)
		{
			GetMatcherLowHigh(remaining_matchers, i, qlow, qhigh);
			ltr_bounds[i] = { start_low, start_high };

			start_low += qlow;
			start_high += qhigh;
			start_high = start_high > expr_count ? expr_count : start_high;
		}

		start_low = start_high = expr_count;
		for (int32_t i = matchers.size() - 1; i >= 0; i++)
		{
			GetMatcherLowHigh(remaining_matchers, i, qlow, qhigh);

			start_low -= qhigh;
			start_high -= qlow;
			start_low = start_low < 0 ? 0 : start_low;

			rtl_bounds[i] = { start_low, start_high };
		}

		bool success = true;
		for (int32_t i = 0; i < matchers.size(); i++)
		{
			result[i].start = ltr_bounds[i].start > rtl_bounds[i].start ? ltr_bounds[i].start : rtl_bounds[i].start;
			result[i].end = ltr_bounds[i].end   < rtl_bounds[i].end ? ltr_bounds[i].end : rtl_bounds[i].end;

			if (result[i].start > result[i].end)
			{
				success = false;
				break;
			}
		}

		delete[] ltr_bounds;
		delete[] rtl_bounds;
		return success;
	}

	void OrderedArgsMatcher::GetMatcherLowHigh(
		std::vector<OrderedQuantifiedExpressionMatcher *> & remaining_matchers, 
		uint32_t matcher_idx, 
		uint32_t & low, 
		uint32_t & high)
	{
		if (remaining_matchers[matcher_idx] == nullptr)
		{
			low = matchers[matcher_idx]->GetQuantifier().low;
			high = matchers[matcher_idx]->GetQuantifier().high;
		}
		else
		{
			low = high = matchers[matcher_idx]->MatchLength();
		}
	}

	OrderedArgsMatcher * OrderedArgsMatcher::DeepCopy()
	{
		std::vector<OrderedQuantifiedExpressionMatcher *> new_matchers;
		for (auto matcher : matchers)
		{
			new_matchers.push_back(matcher->DeepCopy());
		}
		return new OrderedArgsMatcher(new_matchers);
	}

	void OrderedArgsMatcher::PPrint(std::ostream & output)
	{
		output << "OrderedArgsmatcher {\n";

		std::stringstream inner;
		if (matchers.size() > 0) matchers[0]->PPrint(inner);
		for (uint32_t i = 1; i < matchers.size(); i++)
		{
			inner << ",\n";
			matchers[i]->PPrint(inner);
		}
		output << Indent() << inner.str();

		output << "\n}";
	}
}
