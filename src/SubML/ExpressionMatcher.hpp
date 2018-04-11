#pragma once

#include "Matcher.h"
#include "Expression.h"

namespace Ravel::SubML
{
	using namespace Semantic;

	class ExpressionMatcher : public Matcher<Expression *>
	{
	public:
		ExpressionMatcher(Matcher<ExpressionOperator> * oper_matcher, Matcher<Expression *> * args_matcher);
		~ExpressionMatcher();
		
		virtual ExpressionMatcher * DeepCopy() override;

	protected:
		virtual void BeginInternal() override;
		virtual uint64_t * NextInternal() override;
	
	private:
		Matcher<ExpressionOperator> * oper_matcher;
		Matcher<Expression *> * args_matcher;

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}
