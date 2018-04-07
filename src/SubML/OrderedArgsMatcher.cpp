#include "OrderedArgsMatcher.h"

#include <cstring>

namespace Ravel::SubML
{
	OrderedArgsMatcher::OrderedArgsMatcher(bool ordered, QuantifiedExpressionMatcher ** matchers, uint32_t matcher_count)
		: ordered(ordered), matchers(matchers), matcher_count(matcher_count), 
			stack_idx(0), matchers_idx_stack(nullptr), exprs_bounds_stack(nullptr), exprs_stack(nullptr), bounds_stack(nullptr), captures_stack(nullptr)
	{
	}

	OrderedArgsMatcher::~OrderedArgsMatcher()
	{
		for (uint32_t idx = 0; idx < matcher_count; idx++)
		{
			delete matchers[idx];
		}
		delete[] matchers;

		if (matchers_idx_stack) delete[] matchers_idx_stack;
		if (exprs_bounds_stack) delete[] exprs_bounds_stack;
		if (exprs_stack) delete[] exprs_stack;
		if (bounds_stack) delete[] bounds_stack;
		if (captures_stack) delete[] captures_stack;
	}

	void OrderedArgsMatcher::BeginInternal()
	{
		Expression * parent = MatchArgument<0>();
		Expression ** exprs = parent->Args();
		uint32_t expr_count = parent->ArgCount();

		stack_idx = 0;
		matchers_idx_stack = new uint32_t [matcher_count] { 0 };
		exprs_bounds_stack = new Bounds [matcher_count];
		exprs_stack = new Expression *[expr_count * matcher_count];
		bounds_stack = new Bounds [matcher_count * matcher_count];
		captures_stack = new void *[match_capture_count * matcher_count] { nullptr };

		memcpy(exprs_stack, exprs, expr_count * sizeof(Expression *));
		SetupStack(false);
		PushStackUntilLast();
	}

	void ** OrderedArgsMatcher::NextInternal()
	{
		uint32_t expr_count = MatchArgument<0>()->ArgCount();

		void ** result = nullptr;
		while (!result)
		{
			uint32_t matchers_idx = matchers_idx_stack[stack_idx];
			void ** result = matchers[matchers_idx]->Next();
			if (!result)
			{
				stack_idx--;
				if (!PushStackUntilLast())
				{
					Finish();
					return nullptr;
				}
			}

			bool all_exprs_used = true;
			Expression ** exprs = exprs_stack + (expr_count * stack_idx);
			for (int32_t i = 0; i < static_cast<int32_t>(expr_count); i++)
			{
				if (exprs[i])
				{
					all_exprs_used = false;
					break;
				}
			}
			if (!all_exprs_used) result = nullptr;
		}

		return result;
	}

	bool OrderedArgsMatcher::PushStackUntilLast()
	{
		while (stack_idx < static_cast<int32_t>(matcher_count) - 1)
		{
			if (stack_idx < 0)
			{
				return false;
			}

			uint32_t matchers_idx = matchers_idx_stack[stack_idx];
			void ** next_captures = matchers[matchers_idx]->Next();
			if (!next_captures)
			{
				stack_idx--;
				continue;
			}

			uint32_t exprs_start_idx = bounds_stack[matcher_count * stack_idx + matchers_idx].start;
			if (!PushStack(next_captures, { exprs_start_idx, exprs_start_idx + matchers[matchers_idx]->MatchLength() }))
			{
				stack_idx--;
			}
		}
		return true;
	}

	bool OrderedArgsMatcher::PushStack(void ** new_captures, Bounds exprs_bounds)
	{
		uint32_t expr_count = MatchArgument<0>()->ArgCount();

		Expression ** old_exprs = exprs_stack + (expr_count * stack_idx);
		uint32_t old_matchers_idx = matchers_idx_stack[stack_idx];

		stack_idx++;
		
		exprs_bounds_stack[stack_idx] = exprs_bounds;

		Expression ** exprs = exprs_stack + (expr_count * stack_idx);
		memcpy(exprs, old_exprs, expr_count * sizeof(Expression *));
		for (int32_t i = exprs_bounds.start; i < static_cast<int32_t>(exprs_bounds.end); i++)
		{
			exprs[i] = nullptr;
		}

		bool bounds_set = false;
		if (matchers[old_matchers_idx]->GetQuantifier().low == matchers[old_matchers_idx]->GetQuantifier().high)
		{
			bounds_stack[stack_idx] = bounds_stack[stack_idx - 1];
			bounds_set = true;
		}

		void ** captures = captures_stack + (match_capture_count * stack_idx);
		memcpy(captures, new_captures, match_capture_count * sizeof(void *));

		return SetupStack(bounds_set);
	}

	bool OrderedArgsMatcher::SetupStack(bool bounds_set)
	{
		uint32_t expr_count = MatchArgument<0>()->ArgCount();

		Bounds * bounds = bounds_stack + (matcher_count * stack_idx);
		Expression ** exprs = exprs_stack + (expr_count * stack_idx);
		void ** captures = captures_stack + (match_capture_count * stack_idx);

		if (!bounds_set &&
			!CalculateBounds(expr_count, bounds))
		{
			return false;
		}

		int32_t next_idx = -1;
		for (int32_t i = 0; i < matcher_count; i++)
		{
			if (bounds[i].start == bounds[i].end && MatcherAvailable(i) && 
				(next_idx == -1 || matchers[i]->GetQuantifier().high < matchers[next_idx]->GetQuantifier().high))
			{
				next_idx = i;
			}
		}
		matchers_idx_stack[stack_idx] = next_idx;

		uint32_t next_start_idx = bounds[next_idx].start;
		uint32_t next_end_idx = next_start_idx;
		while (next_end_idx < expr_count && !exprs[next_end_idx]) next_end_idx++;

		matchers[next_idx]->Begin(captures, match_capture_count, exprs + next_start_idx, next_end_idx - next_start_idx);
		return true;
	}

	bool OrderedArgsMatcher::CalculateBounds(uint32_t max_length, Bounds * result)
	{
		uint32_t start_low, start_high;
		Bounds * ltr_bounds = new Bounds[matcher_count];
		Bounds * rtl_bounds = new Bounds[matcher_count];
		uint32_t qlow, qhigh;

		start_low = start_high = 0;
		for (int32_t i = 0; i < matcher_count; i++)
		{
			GetMatcherLowHigh(i, &qlow, &qhigh);
			ltr_bounds[i] = { start_low, start_high };

			start_low += qlow;
			start_high += qhigh;
			start_high = start_high > max_length ? max_length : start_high;
		}

		start_low = start_high = max_length;
		for (int32_t i = matcher_count - 1; i >= 0; i++)
		{
			GetMatcherLowHigh(i, &qlow, &qhigh);

			start_low -= qhigh;
			start_high -= qlow;
			start_low = start_low < 0 ? 0 : start_low;

			rtl_bounds[i] = { start_low, start_high };
		}

		for (int32_t i = 0; i < matcher_count; i++)
		{
			result[i].start = ltr_bounds[i].start > rtl_bounds[i].start ? ltr_bounds[i].start : rtl_bounds[i].start;
			result[i].end = ltr_bounds[i].end   < rtl_bounds[i].end ? ltr_bounds[i].end : rtl_bounds[i].end;

			if (result[i].start > result[i].end) return false;
		}

		return true;
	}

	bool OrderedArgsMatcher::MatcherAvailable(uint32_t idx)
	{
		for (int32_t i = 0; i < stack_idx; i++)
		{
			if (matchers_idx_stack[i] == idx) return false;
		}
		return true;
	}

	void OrderedArgsMatcher::GetMatcherLowHigh(uint32_t idx, uint32_t * low, uint32_t * high)
	{
		if (MatcherAvailable(idx))
		{
			*low = matchers[idx]->GetQuantifier().low;
			*high = matchers[idx]->GetQuantifier().high;
		}
		else
		{
			*low = *high = matchers[idx]->MatchLength();
		}
	}
}
