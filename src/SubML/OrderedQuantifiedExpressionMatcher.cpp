#include "OrderedQuantifiedExpressionMatcher.h"

namespace Ravel::SubML
{
	OrderedQuantifiedExpressionMatcher::OrderedQuantifiedExpressionMatcher(Matcher<Expression *> * expression, Quantifier quantifier, CaptureMatcher<Expression *> * capture)
		: expression(expression), quantifier(quantifier), capture(capture)
	{
	}
	OrderedQuantifiedExpressionMatcher::~OrderedQuantifiedExpressionMatcher()
	{
		delete expression;
		if (capture) delete capture;
	}

	void OrderedQuantifiedExpressionMatcher::BeginInternal()
	{
	}

	uint64_t * OrderedQuantifiedExpressionMatcher::NextInternal()
	{
		return nullptr;
	}

	void OrderedQuantifiedExpressionMatcher::PPrint(std::ostream & output)
	{
		output << "OrderedQuantifiedExpressionMatcher {\n";

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
