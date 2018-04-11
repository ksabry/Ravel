#pragma once

#include <vector>
#include "CaptureMatcher.h"
#include "Quantifier.h"

namespace Ravel::SubML
{
	class OrderedQuantifiedExpressionMatcher : public Matcher<Expression **, uint32_t>
	{
	public:
		OrderedQuantifiedExpressionMatcher(
			Matcher<Expression *> * expression_matcher, 
			Quantifier quantifier, 
			CaptureMatcher<Expression *> * capture_matcher);
		~OrderedQuantifiedExpressionMatcher();

		inline virtual Quantifier GetQuantifier()
		{
			return quantifier;
		}

		inline uint32_t MatchLength()
		{
			return match_length;
		}

		virtual OrderedQuantifiedExpressionMatcher * DeepCopy() override;

	protected:
		virtual void BeginInternal() override;
		virtual uint64_t * NextInternal() override;

	private:
		Matcher<Expression *> * expression_matcher;
		Quantifier quantifier;
		CaptureMatcher<Expression *> * capture_matcher;
		
		uint32_t match_length;
		
		std::vector<uint64_t *> intermediate_captures;
		std::vector<uint64_t *> next_intermediate_captures;
		uint32_t intermediate_captures_idx;

		void CalculateNextMatches();

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}
