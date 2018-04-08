#include "OperatorMatcher.h"

namespace Ravel::SubML
{
	OperatorMatcher::OperatorMatcher(OperatorValueMatcher * value_matcher, CaptureMatcher * capture)
		: value_matcher(value_matcher), capture(capture)
	{
	}
	OperatorMatcher::~OperatorMatcher()
	{
		delete value_matcher;
		delete capture;
	}

	void OperatorMatcher::BeginInternal()
	{
	}

	void ** OperatorMatcher::NextInternal()
	{
		return nullptr;
	}
}
