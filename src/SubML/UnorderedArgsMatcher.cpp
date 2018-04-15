#include "UnorderedArgsMatcher.hpp"
#include "Util/Assert.hpp"
#include "Util/ArrCpy.hpp"

namespace Ravel::SubML
{
	UnorderedArgsMatcher::UnorderedArgsMatcher(UnorderedQuantifiedExpressionMatcher ** matchers, uint32_t matcher_count)
		: matchers(matchers), matcher_count(matcher_count), stack(nullptr), stack_idx(0)
	{
	}
	UnorderedArgsMatcher::~UnorderedArgsMatcher()
	{
		for (uint32_t idx = 0; idx < matcher_count; idx++)
		{
			delete matchers[idx];
		}
		delete[] matchers;

		for (uint32_t i = 0; i < matcher_count; i++)
		{
			if (stack[i].initialized) FinishFrame(i);
		}
		delete[] stack;
	}

	void UnorderedArgsMatcher::BeginInternal()
	{
		Expression * parent = MatchArgument<0>();

		exprs = parent->Args();
		expr_count = parent->ArgCount();

		stack = new Frame[matcher_count];
		stack_idx = 0;
		
		if (matcher_count > 0) BeginFrame(0, match_captures, exprs);
	}

	uint64_t * UnorderedArgsMatcher::NextInternal()
	{
		if (matcher_count == 0)
		{
			Finish();
			if (expr_count == 0) return match_captures;
			return nullptr;
		}

		while (stack_idx >= 0)
		{
			Assert(stack[stack_idx].initialized);

			uint64_t * frame_captures = matchers[stack_idx]->Next();
			if (!frame_captures)
			{
				FinishFrame(stack_idx);
				stack_idx--;
				continue;
			}

			int32_t * used_indices;
			uint32_t used_index_count;
			matchers[stack_idx]->GetUsedIndices(used_indices, used_index_count);
			
			auto new_remaining_exprs = new Expression * [expr_count];
			ArrCpy(new_remaining_exprs, stack[stack_idx].incoming_remaining_exprs, expr_count);
			for (uint32_t i = 0; i < used_index_count; i++)
			{
				new_remaining_exprs[used_indices[i]] = nullptr;
			}

			if (stack_idx == matcher_count - 1)
			{
				bool complete = IsComplete(new_remaining_exprs);
				delete[] new_remaining_exprs;
				if (complete) return frame_captures;
			}
			else
			{
				BeginFrame(
					++stack_idx,
					frame_captures,
					new_remaining_exprs
				);
			}
		}

		Finish();
		return nullptr;
	}

	void UnorderedArgsMatcher::BeginFrame(
		uint32_t idx,
		uint64_t * incoming_captures,
		Expression ** new_remaining_exprs)
	{
		Assert(idx < matcher_count);

		matchers[idx]->Begin(incoming_captures, match_capture_count, new_remaining_exprs, expr_count);
		
		stack[idx].incoming_remaining_exprs = new_remaining_exprs;
		stack[idx].initialized = true;
	}

	void UnorderedArgsMatcher::FinishFrame(uint32_t idx)
	{
		Assert(stack[idx].initialized);

		if (stack[idx].incoming_remaining_exprs) delete[] stack[idx].incoming_remaining_exprs;
		stack[idx].initialized = false;
	}

	bool UnorderedArgsMatcher::IsComplete(Expression ** remaining_exprs)
	{
		for (uint32_t i = 0; i < expr_count; i++)
		{
			if (remaining_exprs[i]) return false;
		}
		return true;
	}

	UnorderedArgsMatcher * UnorderedArgsMatcher::DeepCopy()
	{
		auto new_matchers = new UnorderedQuantifiedExpressionMatcher * [matcher_count];
		for (uint32_t i = 0; i < matcher_count; i++)
		{
			new_matchers[i] = matchers[i]->DeepCopy();
		}
		return new UnorderedArgsMatcher(new_matchers, matcher_count);
	}

	void UnorderedArgsMatcher::PPrint(std::ostream & output)
	{
		output << "UnorderedArgsMatcher {\n";
		
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