#include "UnorderedQuantifiedExpressionMatcher.hpp"

namespace Ravel::SubML
{
	UnorderedQuantifiedExpressionMatcher::UnorderedQuantifiedExpressionMatcher(
		Matcher<Expression *> * expression, 
		Quantifier quantifier, 
		CaptureMatcher<Expression *> * capture)
		: expression_matcher(expression_matcher), quantifier(quantifier), capture_matcher(capture_matcher), expr_indices(nullptr)
	{
	}
	UnorderedQuantifiedExpressionMatcher::~UnorderedQuantifiedExpressionMatcher()
	{
		if (expression_matcher) delete expression_matcher;
		if (capture_matcher) delete capture_matcher;
		if (expr_indices) delete[] expr_indices;
	}

	void UnorderedQuantifiedExpressionMatcher::BeginInternal()
	{
		uint32_t expr_count = MatchArgument<1>();
		if (expr_indices) delete[] expr_indices;
		expr_indices = new uint32_t [expr_count];
	}

	uint64_t * UnorderedQuantifiedExpressionMatcher::NextInternal()
	{
		return nullptr;
	}

	UnorderedQuantifiedExpressionMatcher * UnorderedQuantifiedExpressionMatcher::DeepCopy()
	{
		return new UnorderedQuantifiedExpressionMatcher(expression_matcher->DeepCopy(), quantifier, capture_matcher->DeepCopy());
	}

	void UnorderedQuantifiedExpressionMatcher::PPrint(std::ostream & output)
	{
		output << "UnorderedQuantifiedExpressionMatcher {\n";

		std::stringstream inner;
		if (expression_matcher) expression_matcher->PPrint(inner);
		else inner << "NULL";
		inner << ",\n";
		quantifier.PPrint(inner);
		inner << ",\n";
		if (capture_matcher) capture_matcher->PPrint(inner);
		else inner << "NULL";
		output << Indent() << inner.str();
		
		output << "\n}";
	}
}
