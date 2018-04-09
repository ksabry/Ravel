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

	void QuantifiedExpressionMatcher::PPrint(std::ostream & output)
	{
		output << "QuantifiedExpressionMatcher {\n";

		std::stringstream inner;
		expression->PPrint(inner);
		inner << ",\n";
		quantifier.PPrint(inner);
		inner << ",\n";
		capture->PPrint(inner);
	}
}
