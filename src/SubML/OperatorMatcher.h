#pragma once

#include "Matcher.h"
#include "OperatorValueMatcher.h"
#include "CaptureMatcher.h"

namespace Ravel::SubML
{
	class OperatorMatcher : public Matcher<ExpressionOperator>
	{
	public:
		OperatorMatcher(OperatorValueMatcher * value_matcher, CaptureMatcher * capture);
		~OperatorMatcher();

	protected:
		virtual void BeginInternal() override;
		virtual void ** NextInternal() override;

	private:
		OperatorValueMatcher * value_matcher;
		CaptureMatcher * capture;
	};
}
