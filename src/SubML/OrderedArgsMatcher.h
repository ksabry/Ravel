#pragma once

#include "ArgsMatcher.h"
#include "QuantifiedExpressionMatcher.h"

namespace Ravel
{
	namespace SubML
	{
		struct Bounds
		{
			size_t start;
			size_t end;
		};

		class OrderedArgsMatcher : public ArgsMatcher
		{
		public:
			OrderedArgsMatcher(bool ordered, QuantifiedExpressionMatcher ** matchers, size_t matcher_count);
			~OrderedArgsMatcher();

		protected:
			virtual void BeginInternal() override;
			virtual void ** NextInternal() override;

		private:
			bool ordered;
			QuantifiedExpressionMatcher ** matchers;
			size_t matcher_count;

			int32_t stack_idx;
			
			size_t * matchers_idx_stack;
			Bounds * exprs_bounds_stack;
			Expression ** exprs_stack;
			Bounds * bounds_stack;
			void ** captures_stack;

			bool PushStackUntilLast();
			bool PushStack(void ** new_captures, Bounds exprs_bounds);
			bool SetupStack(bool bounds_set);

			bool CalculateBounds(size_t max_length, Bounds * result);
			
			bool MatcherAvailable(size_t idx);
			void GetMatcherLowHigh(size_t idx, size_t * low, size_t * high);
		};
	}
}
