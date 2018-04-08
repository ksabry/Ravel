#include "OperatorMatcher.h"

namespace Ravel::SubML
{
	OperatorMatcher::OperatorMatcher(Matcher<ExpressionOperator> * value_matcher, CaptureMatcher<ExpressionOperator> * capture_matcher)
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
		return nullptr;
	}
}
