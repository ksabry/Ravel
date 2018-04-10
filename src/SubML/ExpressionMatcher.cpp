#include "ExpressionMatcher.h"
#include "String.h"

namespace Ravel::SubML
{
	ExpressionMatcher::ExpressionMatcher(Matcher<ExpressionOperator> * oper_matcher, Matcher<Expression *> * group_matcher)
		: oper_matcher(oper_matcher), group_matcher(group_matcher)
	{
	}

	ExpressionMatcher::~ExpressionMatcher()
	{
		if (oper_matcher) delete oper_matcher;
		if (group_matcher) delete group_matcher;
	}

	void ExpressionMatcher::BeginInternal()
	{
		Expression * expr = MatchArgument<0>();
		
		if (oper_matcher && group_matcher)
		{
			oper_matcher->Begin(match_captures, match_capture_count, expr->Oper());
			group_matcher->Begin(oper_matcher->Next(), match_capture_count, expr);
		}
		else if (!oper_matcher && group_matcher)
		{
			group_matcher->Begin(match_captures, match_capture_count, expr);
		}
		else if (oper_matcher && !group_matcher)
		{
			oper_matcher->Begin(match_captures, match_capture_count, expr->Oper());
		}
	}

	uint64_t * ExpressionMatcher::NextInternal()
	{
		if (oper_matcher && group_matcher)
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
		else if (!oper_matcher && group_matcher)
		{
			auto group_captures = group_matcher->Next();
			if (!group_captures)
			{
				Finish();
				return nullptr;
			}
			return group_captures;
		}
		else if (oper_matcher && !group_matcher)
		{
			auto oper_captures = oper_matcher->Next();
			if (!oper_captures)
			{
				Finish();
				return nullptr;
			}
			return oper_captures;
		}
		else
		{
			Finish();
			return match_captures;
		}
	}

	void ExpressionMatcher::PPrint(std::ostream & output)
	{
		output << "ExpressionMatcher {\n";
		
		std::stringstream inner;
		if (oper_matcher) oper_matcher->PPrint(inner);
		else inner << "NULL";
		inner << ",\n";
		if (group_matcher) group_matcher->PPrint(inner);
		else inner << "NULL";
		output << Indent() << inner.str();

		output << "\n}";
	}
}
