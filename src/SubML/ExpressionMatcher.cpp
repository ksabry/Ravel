#include "ExpressionMatcher.hpp"
#include "String.hpp"

namespace Ravel::SubML
{
	ExpressionMatcher::ExpressionMatcher(Matcher<ExpressionOperator> * oper_matcher, Matcher<Expression *> * args_matcher)
		: oper_matcher(oper_matcher), args_matcher(args_matcher)
	{
	}

	ExpressionMatcher::~ExpressionMatcher()
	{
		if (oper_matcher) delete oper_matcher;
		if (args_matcher) delete args_matcher;
	}

	void ExpressionMatcher::BeginInternal()
	{
		Expression * expr = MatchArgument<0>();
		
		if (oper_matcher && args_matcher)
		{
			oper_matcher->Begin(match_captures, match_capture_count, expr->Oper());
			args_matcher->Begin(oper_matcher->Next(), match_capture_count, expr);
		}
		else if (!oper_matcher && args_matcher)
		{
			args_matcher->Begin(match_captures, match_capture_count, expr);
		}
		else if (oper_matcher && !args_matcher)
		{
			oper_matcher->Begin(match_captures, match_capture_count, expr->Oper());
		}
	}

	uint64_t * ExpressionMatcher::NextInternal()
	{
		if (oper_matcher && args_matcher)
		{
			auto group_captures = args_matcher->Next();
			while (!group_captures)
			{
				auto oper_captures = oper_matcher->Next();
				if (!oper_captures)
				{
					Finish();
					return nullptr;
				}
				Expression * expr = MatchArgument<0>();
				args_matcher->Begin(oper_captures, match_capture_count, expr);
				group_captures = args_matcher->Next();
			}
			return group_captures;
		}
		else if (!oper_matcher && args_matcher)
		{
			auto group_captures = args_matcher->Next();
			if (!group_captures)
			{
				Finish();
				return nullptr;
			}
			return group_captures;
		}
		else if (oper_matcher && !args_matcher)
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

	ExpressionMatcher * ExpressionMatcher::DeepCopy()
	{
		auto new_oper_matcher = oper_matcher == nullptr ? nullptr : oper_matcher->DeepCopy();
		auto new_args_matcher = args_matcher == nullptr ? nullptr : args_matcher->DeepCopy();
		return new ExpressionMatcher(new_oper_matcher, new_args_matcher);
	}

	void ExpressionMatcher::PPrint(std::ostream & output)
	{
		output << "ExpressionMatcher {\n";
		
		std::stringstream inner;
		if (oper_matcher) oper_matcher->PPrint(inner);
		else inner << "NULL";
		inner << ",\n";
		if (args_matcher) args_matcher->PPrint(inner);
		else inner << "NULL";
		output << Indent() << inner.str();

		output << "\n}";
	}
}
