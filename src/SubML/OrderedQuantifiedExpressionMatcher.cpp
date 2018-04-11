#include "OrderedQuantifiedExpressionMatcher.h"

namespace Ravel::SubML
{
	OrderedQuantifiedExpressionMatcher::OrderedQuantifiedExpressionMatcher(
		Matcher<Expression *> * expression_matcher, 
		Quantifier quantifier, 
		CaptureMatcher<Expression *> * capture_matcher)
		: expression_matcher(expression_matcher), quantifier(quantifier), capture_matcher(capture_matcher)
	{
	}
	OrderedQuantifiedExpressionMatcher::~OrderedQuantifiedExpressionMatcher()
	{
		if (expression_matcher) delete expression_matcher;
		if (capture_matcher) delete capture_matcher;
		for (uint64_t * captures : intermediate_captures) delete[] captures;
		for (uint64_t * captures : next_intermediate_captures) delete[] captures;
	}

	void OrderedQuantifiedExpressionMatcher::BeginInternal()
	{
		match_length = 0;
		intermediate_captures_idx = 0;
		intermediate_captures.push_back(match_captures);
		while (match_length < quantifier.low) CalculateNextMatchLength();
	}

	uint64_t * OrderedQuantifiedExpressionMatcher::NextInternal()
	{
		Expression ** exprs = MatchArgument<0>();
		uint32_t expr_count = MatchArgument<1>();

		while (intermediate_captures_idx >= intermediate_captures.size())
		{
			if (match_length + 1 >= expr_count || match_length + 1 > quantifier.high)
			{
				Finish();
				return nullptr;
			}

			CalculateNextMatchLength();
			intermediate_captures_idx = 0;
		}
		return intermediate_captures[intermediate_captures_idx++];
	}

	void OrderedQuantifiedExpressionMatcher::CalculateNextMatchLength()
	{
		Expression ** exprs = MatchArgument<0>();
		uint32_t expr_count = MatchArgument<1>();

		for (uint64_t * captures : intermediate_captures)
		{
			expression_matcher->Begin(captures, match_capture_count, exprs[match_length]);
			uint64_t * expression_captures = expression_matcher->Next();
			while (expression_captures)
			{
				capture_matcher->Begin(expression_captures, match_capture_count, exprs[match_length]);
				uint64_t * capture_captures = capture_matcher->Next();
				while (capture_captures)
				{
					uint64_t * copied = new uint64_t [match_capture_count];
					ArrCpy(copied, capture_captures, match_capture_count);
					next_intermediate_captures.push_back(copied);

					capture_captures = capture_matcher->Next();
				}
				expression_captures = expression_matcher->Next();
			}
			delete[] captures;
		}

		intermediate_captures = next_intermediate_captures;
		next_intermediate_captures.clear();
		match_length++;
	}

	void OrderedQuantifiedExpressionMatcher::PPrint(std::ostream & output)
	{
		output << "OrderedQuantifiedExpressionMatcher {\n";

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
