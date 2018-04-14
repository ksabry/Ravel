#include "ExpressionMatcher.hpp"
#include "Util/String.hpp"

namespace Ravel::SubML
{
	ExpressionMatcher::ExpressionMatcher(Matcher<ExpressionOperator> * oper_matcher, Matcher<Expression *> * args_matcher)
		: oper_matcher(oper_matcher), args_matcher(args_matcher)
	{
	}

	ExpressionMatcher::~ExpressionMatcher()
	{
		delete oper_matcher;
		delete args_matcher;
	}

	void ExpressionMatcher::BeginInternal()
	{
	}

	uint64_t * ExpressionMatcher::NextInternal()
	{
		Expression * expr = MatchArgument<0>();

		auto arg_captures = args_matcher->HasBegun() ? args_matcher->Next() : nullptr;
		while (!arg_captures)
		{
			if (!oper_matcher->HasBegun()) oper_matcher->Begin(match_captures, match_capture_count, expr->Oper());
			auto oper_captures = oper_matcher->Next();
			if (!oper_captures)
			{
				Finish();
				return nullptr;
			}
			Expression * expr = MatchArgument<0>();
			args_matcher->Begin(oper_captures, match_capture_count, expr);
			arg_captures = args_matcher->Next();
		}
		return arg_captures;
	}

	ExpressionMatcher * ExpressionMatcher::DeepCopy()
	{
		return new ExpressionMatcher(oper_matcher->DeepCopy(), args_matcher->DeepCopy());
	}

	void ExpressionMatcher::PPrint(std::ostream & output)
	{
		output << "ExpressionMatcher {\n";
		
		std::stringstream inner;
		oper_matcher->PPrint(inner);
		inner << ",\n";
		args_matcher->PPrint(inner);
		output << Indent() << inner.str();

		output << "\n}";
	}
}
