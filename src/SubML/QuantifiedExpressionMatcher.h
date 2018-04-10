#pragma once

#include "CaptureMatcher.h"
#include "Quantifier.h"

namespace Ravel::SubML
{
	class QuantifiedExpressionMatcher : public Matcher<Expression **, uint32_t>
	{
	public:
		QuantifiedExpressionMatcher(Matcher<Expression *> * expression, Quantifier quantifier, CaptureMatcher<Expression *> * capture);
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
		virtual uint64_t * NextInternal() override;

	private:
		Matcher<Expression *> * expression;
		Quantifier quantifier;
		CaptureMatcher<Expression *> * capture;
		uint32_t match_length;

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}
