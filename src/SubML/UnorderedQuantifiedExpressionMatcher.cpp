#include "UnorderedQuantifiedExpressionMatcher.h"

namespace Ravel::SubML
{
	UnorderedQuantifiedExpressionMatcher::UnorderedQuantifiedExpressionMatcher(Matcher<Expression *> * expression, Quantifier quantifier, CaptureMatcher<Expression *> * capture)
		: expression(expression), quantifier(quantifier), capture(capture)
	{
	}
	UnorderedQuantifiedExpressionMatcher::~UnorderedQuantifiedExpressionMatcher()
	{
		delete expression;
		if (capture) delete capture;
	}

	void UnorderedQuantifiedExpressionMatcher::BeginInternal()
	{
	}

	uint64_t * UnorderedQuantifiedExpressionMatcher::NextInternal()
	{
		return nullptr;
	}

	void UnorderedQuantifiedExpressionMatcher::PPrint(std::ostream & output)
	{
		output << "UnorderedQuantifiedExpressionMatcher {\n";

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
