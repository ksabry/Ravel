#pragma once

#include "Matcher.h"
#include "CaptureMatcher.h"

namespace Ravel::SubML
{
	class OperatorMatcher : public Matcher<ExpressionOperator>
	{
	public:
		OperatorMatcher(ExpressionOperator oper, uint32_t capture_idx);

	protected:
		virtual void BeginInternal() override;
		virtual void ** NextInternal() override;

	private:
		ExpressionOperator oper;
		uint32_t capture_idx;
	};
}
