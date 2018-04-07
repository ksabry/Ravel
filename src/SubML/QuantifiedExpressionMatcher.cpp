#include "QuantifiedExpressionMatcher.h"

namespace Ravel::SubML
{
	QuantifiedExpressionMatcher::QuantifiedExpressionMatcher(ExpressionMatcher * expression, Quantifier quantifier, CaptureMatcher * capture)
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

	void ** QuantifiedExpressionMatcher::NextInternal()
	{
		return nullptr;
	}
}
