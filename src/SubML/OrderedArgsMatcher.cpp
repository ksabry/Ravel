#include "OrderedArgsMatcher.h"
#include "Assert.h"

namespace Ravel::SubML
{
	OrderedArgsMatcher::OrderedArgsMatcher(OrderedQuantifiedExpressionMatcher ** matchers, uint32_t matcher_count)
		: matchers(matchers), matcher_count(matcher_count), stack(nullptr), stack_idx(0)
	{
	}
	OrderedArgsMatcher::~OrderedArgsMatcher()
	{
		for (uint32_t idx = 0; idx < matcher_count; idx++)
		{
			delete matchers[idx];
		}
		delete[] matchers;
		// TODO: delete stack
	}

	void OrderedArgsMatcher::BeginInternal()
	{
		Expression * parent = MatchArgument<0>();
		exprs = parent->Args();
		expr_count = parent->ArgCount();

		stack = new Frame[matcher_count];
		stack_idx = 0;

		BeginFrame(0, match_captures, exprs, matchers, nullptr);
	}

	uint64_t * OrderedArgsMatcher::NextInternal()
	{
		while (stack_idx >= 0)
		{
			if (!stack[stack_idx].initialized)
			{
				stack_idx--;
				continue;
			}

			uint64_t * frame_captures = stack[stack_idx].matcher->Next();
			if (!frame_captures)
			{
				FinishFrame(stack_idx);
				stack_idx--;
				continue;
			}

			if (stack_idx == matcher_count - 1)
			{
				if (IsComplete(stack[stack_idx].remaining_exprs)) return frame_captures;
			}
			else
			{
				BeginFrame(
					++stack_idx, 
					frame_captures, 
					stack[stack_idx].remaining_exprs, 
					stack[stack_idx].remaining_matchers,
					stack[stack_idx].remaining_bounds
				);
			}
		}
		Finish();
		return nullptr;
	}

	void OrderedArgsMatcher::BeginFrame(
		uint32_t idx,
		uint64_t * incoming_captures, 
		Expression ** remaining_exprs, 
		OrderedQuantifiedExpressionMatcher ** remaining_matchers, 
		Bounds * remaining_bounds)
	{
		if (idx >= matcher_count) return;

		if (!remaining_bounds)
		{
			if (!CalculateBounds(stack[idx].remaining_bounds, stack[idx].remaining_matchers)) return;
		}
		Bounds * bounds = stack[idx].remaining_bounds;

		uint32_t next_matcher_idx;
		for (uint32_t i = 0; i < matcher_count; i++)
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
		while (next_end_idx < expr_count && !exprs[next_end_idx]) next_end_idx++;

		stack[idx].matcher->Begin(incoming_captures, match_capture_count, exprs + next_start_idx, next_end_idx - next_start_idx);
		stack[idx].initialized = true;

		stack[idx].remaining_matchers = new OrderedQuantifiedExpressionMatcher * [matcher_count];
		ArrCpy(stack[idx].remaining_matchers, remaining_matchers, matcher_count);
		stack[idx].remaining_matchers[next_matcher_idx] = nullptr;
		
		if (stack[idx].matcher->GetQuantifier().low == stack[idx].matcher->GetQuantifier().high)
		{
			stack[idx].remaining_bounds = new Bounds [matcher_count];
			ArrCpy(stack[idx].remaining_bounds, remaining_bounds, matcher_count);
		}
		else
		{
			stack[idx].remaining_bounds = nullptr;
		}
	}

	void OrderedArgsMatcher::FinishFrame(uint32_t idx)
	{
		Assert(stack[idx].initialized);

		if (stack[idx].remaining_exprs) delete[] stack[idx].remaining_exprs;
		if (stack[idx].remaining_matchers) delete[] stack[idx].remaining_matchers;
		if (stack[idx].remaining_bounds) delete[] stack[idx].remaining_bounds;
		stack[idx].initialized = false;
	}

	bool OrderedArgsMatcher::IsComplete(Expression ** remaining_exprs)
	{
		for (uint32_t i = 0; i < expr_count; i++)
		{
			if (remaining_exprs[i]) return false;
		}
		return true;
	}

	bool OrderedArgsMatcher::CalculateBounds(Bounds * & result, OrderedQuantifiedExpressionMatcher ** remaining_matchers)
	{
		result = new Bounds[matcher_count];

		uint32_t start_low, start_high;
		Bounds * ltr_bounds = new Bounds[matcher_count];
		Bounds * rtl_bounds = new Bounds[matcher_count];
		uint32_t qlow, qhigh;

		start_low = start_high = 0;
		for (int32_t i = 0; i < matcher_count; i++)
		{
			GetMatcherLowHigh(remaining_matchers, i, &qlow, &qhigh);
			ltr_bounds[i] = { start_low, start_high };

			start_low += qlow;
			start_high += qhigh;
			start_high = start_high > expr_count ? expr_count : start_high;
		}

		start_low = start_high = expr_count;
		for (int32_t i = matcher_count - 1; i >= 0; i++)
		{
			GetMatcherLowHigh(remaining_matchers, i, &qlow, &qhigh);

			start_low -= qhigh;
			start_high -= qlow;
			start_low = start_low < 0 ? 0 : start_low;

			rtl_bounds[i] = { start_low, start_high };
		}

		bool success = true;
		for (int32_t i = 0; i < matcher_count; i++)
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
		OrderedQuantifiedExpressionMatcher ** remaining_matchers, 
		uint32_t matcher_idx, 
		uint32_t * low, 
		uint32_t * high)
	{
		if (remaining_matchers[matcher_idx] == nullptr)
		{
			*low = matchers[matcher_idx]->GetQuantifier().low;
			*high = matchers[matcher_idx]->GetQuantifier().high;
		}
		else
		{
			*low = *high = matchers[matcher_idx]->MatchLength();
		}
	}

	void OrderedArgsMatcher::PPrint(std::ostream & output)
	{
		output << "OrderedArgsmatcher {\n";

		std::stringstream inner;
		if (matcher_count > 0) matchers[0]->PPrint(inner);
		for (uint32_t i = 1; i < matcher_count; i++)
		{
			inner << ",\n";
			matchers[i]->PPrint(inner);
		}
		output << Indent() << inner.str();

		output << "\n}";
	}
}
