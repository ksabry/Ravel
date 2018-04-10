#pragma once

#include "Matcher.h"
#include "QuantifiedExpressionMatcher.h"

namespace Ravel::SubML
{
	class OrderedArgsMatcher : public Matcher<Expression *>
	{
	public:
		OrderedArgsMatcher(QuantifiedExpressionMatcher ** matchers, uint32_t matcher_count);
		~OrderedArgsMatcher();

	protected:
		virtual void BeginInternal() override;
		virtual uint64_t * NextInternal() override;

	private:
		QuantifiedExpressionMatcher ** matchers;
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
			QuantifiedExpressionMatcher * matcher = nullptr;

			Expression ** remaining_exprs = nullptr;
			QuantifiedExpressionMatcher ** remaining_matchers = nullptr;
			Bounds * remaining_bounds = nullptr;
		};

		Frame * stack;
		int32_t stack_idx;

		void BeginFrame(
			uint32_t idx,
			uint64_t * incoming_captures,
			Expression ** remaining_exprs,
			QuantifiedExpressionMatcher ** remaining_matchers,
			Bounds * remaining_bounds);

		void FinishFrame(uint32_t idx);

		bool IsComplete(Expression ** exprs);

		bool CalculateBounds(Bounds * & result, QuantifiedExpressionMatcher ** remaining_matchers);
		void GetMatcherLowHigh(QuantifiedExpressionMatcher ** remaining_matchers, uint32_t matcher_idx, uint32_t * low, uint32_t * high);

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}
