#pragma once

#include "ArgsMatcher.h"
#include "QuantifiedExpressionMatcher.h"

namespace Ravel::SubML
{
	struct Bounds
	{
		uint32_t start;
		uint32_t end;
	};

	class OrderedArgsMatcher : public ArgsMatcher
	{
	public:
		OrderedArgsMatcher(bool ordered, QuantifiedExpressionMatcher ** matchers, uint32_t matcher_count);
		~OrderedArgsMatcher();

	protected:
		virtual void BeginInternal() override;
		virtual void ** NextInternal() override;

	private:
		bool ordered;
		QuantifiedExpressionMatcher ** matchers;
		uint32_t matcher_count;

		int32_t stack_idx;
		
		uint32_t * matchers_idx_stack;
		Bounds * exprs_bounds_stack;
		Expression ** exprs_stack;
		Bounds * bounds_stack;
		void ** captures_stack;

		bool PushStackUntilLast();
		bool PushStack(void ** new_captures, Bounds exprs_bounds);
		bool SetupStack(bool bounds_set);

		bool CalculateBounds(uint32_t max_length, Bounds * result);
		
		bool MatcherAvailable(uint32_t idx);
		void GetMatcherLowHigh(uint32_t idx, uint32_t * low, uint32_t * high);
	};
}
