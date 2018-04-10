#include "OperatorMatcher.h"

namespace Ravel::SubML
{
	OperatorMatcher::OperatorMatcher(Matcher<ExpressionOperator> * value_matcher, CaptureMatcher<ExpressionOperator> * capture_matcher)
		: value_matcher(value_matcher), capture_matcher(capture_matcher)
	{
	}
	OperatorMatcher::~OperatorMatcher()
	{
		if (value_matcher) delete value_matcher;
		if (capture_matcher) delete capture_matcher;
	}

	void OperatorMatcher::BeginInternal()
	{
		ExpressionOperator oper = MatchArgument<0>();

		if (value_matcher && capture_matcher)
		{
			value_matcher->Begin(match_captures, match_capture_count, oper);
			capture_matcher->Begin(value_matcher->Next(), match_capture_count, oper);
		}
		else if (!value_matcher && capture_matcher)
		{
			capture_matcher->Begin(match_captures, match_capture_count, oper);
		}
		else if (value_matcher && !capture_matcher)
		{
			value_matcher->Begin(match_captures, match_capture_count, oper);
		}
	}

	uint64_t * OperatorMatcher::NextInternal()
	{
		if (value_matcher && capture_matcher)
		{
			auto capture_captures = capture_matcher->Next();
			while (!capture_captures)
			{
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
		else if (!value_matcher && capture_matcher)
		{
			auto capture_captures = capture_matcher->Next();
			if (!capture_captures)
			{
				Finish();
				return nullptr;
			}
			return capture_captures;
		}
		else if (value_matcher && !capture_matcher)
		{
			auto value_captures = value_matcher->Next();
			if (!value_captures)
			{
				Finish();
				return nullptr;
			}
			return value_captures;
		}
		else
		{
			Finish();
			return match_captures;
		}
	}

	void OperatorMatcher::PPrint(std::ostream & output)
	{
		output << "OperatorMatcher {\n";

		std::stringstream inner;
		if (value_matcher) value_matcher->PPrint(inner);
		else inner << "NULL";
		inner << ",\n";
		if (capture_matcher) capture_matcher->PPrint(inner);
		else inner << "NULL";
		output << Indent() << inner.str();

		output << "\n}";
	}
}
