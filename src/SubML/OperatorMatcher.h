#pragma once

#include "Matcher.h"
#include "OperatorValueMatcher.h"
#include "CaptureMatcher.h"

namespace Ravel::SubML
{
	class OperatorMatcher : public Matcher<ExpressionOperator>
	{
	public:
		OperatorMatcher(Matcher<ExpressionOperator> * value_matcher, CaptureMatcher<ExpressionOperator> * capture_matcher);
		~OperatorMatcher();

	protected:
		virtual void BeginInternal() override;
		virtual uint64_t * NextInternal() override;

	private:
		Matcher<ExpressionOperator> * value_matcher;
		CaptureMatcher<ExpressionOperator> * capture_matcher;
	};
}
