#include "OperatorMatcher.hpp"

namespace Ravel::SubML
{
	OperatorMatcher::OperatorMatcher(Matcher<ExpressionOperator> * value_matcher, Matcher<ExpressionOperator> * capture_matcher)
		: value_matcher(value_matcher), capture_matcher(capture_matcher)
	{
	}
	OperatorMatcher::~OperatorMatcher()
	{
		delete value_matcher;
		delete capture_matcher;
	}

	void OperatorMatcher::BeginInternal()
	{
	}

	uint64_t * OperatorMatcher::NextInternal()
	{
		ExpressionOperator oper = MatchArgument<0>();

		auto capture_captures = capture_matcher->HasBegun() ? capture_matcher->Next() : nullptr;
		while (!capture_captures)
		{
			if (!value_matcher->HasBegun()) value_matcher->Begin(match_captures, match_capture_count, oper);
			auto value_captures = value_matcher->Next();
			if (!value_captures)
			{
				Finish();
				return nullptr;
			}
			ExpressionOperator oper = MatchArgument<0>();
			capture_matcher->Begin(value_captures, match_capture_count, oper);
			capture_captures = capture_matcher->Next();
		}
		return capture_captures;
	}

	OperatorMatcher * OperatorMatcher::DeepCopy()
	{
		return new OperatorMatcher(value_matcher->DeepCopy(), capture_matcher->DeepCopy());
	}

	void OperatorMatcher::PPrint(std::ostream & output)
	{
		output << "OperatorMatcher {\n";

		std::stringstream inner;
		value_matcher->PPrint(inner);
		inner << ",\n";
		capture_matcher->PPrint(inner);
		output << Indent() << inner.str();

		output << "\n}";
	}
}
