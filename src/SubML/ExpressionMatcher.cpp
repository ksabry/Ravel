#include "ExpressionMatcher.h"

namespace Ravel::SubML
{
	ExpressionMatcher::ExpressionMatcher(Matcher<ExpressionOperator> * oper_matcher, Matcher<Expression *> * group_matcher)
		: oper_matcher(oper_matcher), group_matcher(group_matcher)
	{
	}

	ExpressionMatcher::~ExpressionMatcher()
	{
		delete oper_matcher;
		delete group_matcher;
	}

	void ExpressionMatcher::BeginInternal()
	{
		Expression * expr = MatchArgument<0>();
		oper_matcher->Begin(match_captures, match_capture_count, expr->Oper());
		group_matcher->Begin(oper_matcher->Next(), match_capture_count, expr);
	}

	uint64_t * ExpressionMatcher::NextInternal()
	{
		auto group_captures = group_matcher->Next();
		while (!group_captures)
		{
			auto oper_captures = oper_matcher->Next();
			if (!oper_captures)
			{
				Finish();
				return nullptr;
			}

			Expression * expr = MatchArgument<0>();
			group_matcher->Begin(oper_captures, match_capture_count, expr);
			group_captures = group_matcher->Next();
		}
		return group_captures;
	}
}
