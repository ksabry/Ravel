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
		if (capture) delete capture;
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
		if (expression) expression->PPrint(inner);
		else inner << "NULL";
		inner << ",\n";
		quantifier.PPrint(inner);
		inner << ",\n";
		if (capture) capture->PPrint(inner);
		else inner << "NULL";
		output << Indent() << inner.str();
		
		output << "\n}";
	}
}
