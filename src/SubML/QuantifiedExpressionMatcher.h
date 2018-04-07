#pragma once

#include "ExpressionMatcher.h"
#include "CaptureMatcher.h"
#include "Quantifier.h"

namespace Ravel::SubML
{
	class QuantifiedExpressionMatcher : public Matcher<Expression **, uint32_t>
	{
	public:
		QuantifiedExpressionMatcher(ExpressionMatcher * expression, Quantifier quantifier, CaptureMatcher * capture);
		~QuantifiedExpressionMatcher();

		inline virtual Quantifier GetQuantifier()
		{
			return quantifier;
		}

		inline uint32_t MatchLength()
		{
			return match_length;
		}

	protected:
		virtual void BeginInternal() override;
		virtual void ** NextInternal() override;

	private:
		ExpressionMatcher * expression;
		Quantifier quantifier;
		CaptureMatcher * capture;
		uint32_t match_length;
	};
}
