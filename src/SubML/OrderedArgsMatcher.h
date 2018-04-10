#pragma once

#include "Matcher.h"
#include "OrderedQuantifiedExpressionMatcher.h"

namespace Ravel::SubML
{
	class OrderedArgsMatcher : public Matcher<Expression *>
	{
	public:
		OrderedArgsMatcher(OrderedQuantifiedExpressionMatcher ** matchers, uint32_t matcher_count);
		~OrderedArgsMatcher();

	protected:
		virtual void BeginInternal() override;
		virtual uint64_t * NextInternal() override;

	private:
		OrderedQuantifiedExpressionMatcher ** matchers;
		uint32_t matcher_count;
		
		Expression ** exprs;
		uint32_t expr_count;

		struct Bounds
		{
			uint32_t start;
			uint32_t end;
		};

		struct Frame
		{
			bool initialized = false;
			OrderedQuantifiedExpressionMatcher * matcher = nullptr;

			Expression ** remaining_exprs = nullptr;
			OrderedQuantifiedExpressionMatcher ** remaining_matchers = nullptr;
			Bounds * remaining_bounds = nullptr;
		};

		Frame * stack;
		int32_t stack_idx;

		void BeginFrame(
			uint32_t idx,
			uint64_t * incoming_captures,
			Expression ** remaining_exprs,
			OrderedQuantifiedExpressionMatcher ** remaining_matchers,
			Bounds * remaining_bounds);

		void FinishFrame(uint32_t idx);

		bool IsComplete(Expression ** exprs);

		bool CalculateBounds(Bounds * & result, OrderedQuantifiedExpressionMatcher ** remaining_matchers);
		void GetMatcherLowHigh(
			OrderedQuantifiedExpressionMatcher ** remaining_matchers, 
			uint32_t matcher_idx, 
			uint32_t * low, 
			uint32_t * high);

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}
