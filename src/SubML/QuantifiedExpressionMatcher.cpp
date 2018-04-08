#include "QuantifiedExpressionMatcher.h"

namespace Ravel::SubML
{
	QuantifiedExpressionMatcher::QuantifiedExpressionMatcher(Matcher<Expression *> * expression, Quantifier quantifier, CaptureMatcher<Expression *> * capture)
		: expression(expression), quantifier(quantifier), capture(capture)
	{
	}
	QuantifiedExpressionMatcher::~QuantifiedExpressionMatcher()
	{
		delete expression;
		delete capture;
	}

	void QuantifiedExpressionMatcher::BeginInternal()
	{
	}

	uint64_t * QuantifiedExpressionMatcher::NextInternal()
	{
		return nullptr;
	}
}
