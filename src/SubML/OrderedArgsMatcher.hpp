#pragma once

#include "Matcher.hpp"
#include "OrderedQuantifiedExpressionMatcher.hpp"

namespace Ravel::SubML
{
	class OrderedArgsMatcher : public Matcher<Expression *>
	{
	public:
		OrderedArgsMatcher(std::vector<OrderedQuantifiedExpressionMatcher *> matchers);
		~OrderedArgsMatcher();

		virtual OrderedArgsMatcher * DeepCopy() override;

	protected:
		virtual void BeginInternal() override;
		virtual bool NextInternal() override;

	private:
		std::vector<OrderedQuantifiedExpressionMatcher *> matchers;

		struct Bounds
		{
			uint32_t start;
			uint32_t end;
		};

		struct Frame
		{

			bool initialized = false;
			OrderedQuantifiedExpressionMatcher * matcher = nullptr;

			uint32_t expr_start_idx;
			std::vector<Expression *> incoming_remaining_exprs;
			std::vector<OrderedQuantifiedExpressionMatcher *> remaining_matchers;
			std::vector<Bounds> remaining_bounds;
			bool has_bounds = false;
		};

		Frame * stack;
		int32_t stack_idx;

		void BeginFrame(
			uint32_t idx,
			std::vector<uint64_t> & incoming_captures,
			std::vector<Expression *> & new_remaining_exprs,
			std::vector<OrderedQuantifiedExpressionMatcher *> & remaining_matchers,
			std::vector<Bounds> * remaining_bounds);

		void FinishFrame(uint32_t idx);

		bool IsComplete(std::vector<Expression *> & remaining_exprs);

		bool CalculateBounds(std::vector<Bounds> & result, std::vector<OrderedQuantifiedExpressionMatcher *> & remaining_matchers);

		void GetMatcherLowHigh(
			std::vector<OrderedQuantifiedExpressionMatcher *> & remaining_matchers, 
			uint32_t matcher_idx, 
			uint32_t & low, 
			uint32_t & high);

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}
