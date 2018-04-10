#pragma once

#include "CaptureMatcher.h"
#include "Quantifier.h"

namespace Ravel::SubML
{
	class UnorderedQuantifiedExpressionMatcher : public Matcher<Expression **, uint32_t>
	{
	public:
		UnorderedQuantifiedExpressionMatcher(Matcher<Expression *> * expression, Quantifier quantifier, CaptureMatcher<Expression *> * capture);
		~UnorderedQuantifiedExpressionMatcher();

	protected:
		virtual void BeginInternal() override;
		virtual uint64_t * NextInternal() override;

	private:
		Matcher<Expression *> * expression;
		Quantifier quantifier;
		CaptureMatcher<Expression *> * capture;

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}
