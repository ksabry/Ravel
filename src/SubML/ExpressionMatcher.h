#pragma once

#include "Matcher.h"
#include "ArgsMatcher.h"
#include "OperatorMatcher.h"

namespace Ravel::SubML
{
	class ExpressionMatcher : public Matcher<Expression *>
	{
	public:
		ExpressionMatcher(OperatorMatcher * oper, ArgsMatcher * groupMatcher);
		
	protected:
		virtual void BeginInternal() override;
		virtual void ** NextInternal() override;

	private:
		OperatorMatcher * oper;
		ArgsMatcher * groupMatcher;
	};
}
