#include "ExpressionMatcher.h"

namespace Ravel::SubML
{
	ExpressionMatcher::ExpressionMatcher(OperatorMatcher * oper, ArgsMatcher * group_matcher)
		: oper(oper), group_matcher(group_matcher)
	{
	}

	ExpressionMatcher::~ExpressionMatcher()
	{
		delete oper;
		delete group_matcher;
	}

	void ExpressionMatcher::BeginInternal()
	{
		Expression * expr = MatchArgument<0>();
		oper->Begin(match_captures, match_capture_count, expr->Oper());
		group_matcher->Begin(oper->Next(), match_capture_count, expr);
	}

	void ** ExpressionMatcher::NextInternal()
	{
		auto group_captures = group_matcher->Next();
		while (!group_captures)
		{
			auto oper_aptures = oper->Next();
			if (!oper_aptures)
			{
				Finish();
				return nullptr;
			}

			Expression * expr = MatchArgument<0>();
			group_matcher->Begin(oper_aptures, match_capture_count, expr);
			group_captures = group_matcher->Next();
		}
		return group_captures;
	}
}
