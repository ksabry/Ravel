#pragma once

#include "Matcher.h"
#include "ExpressionOperator.h"

namespace Ravel::SubML
{
	using namespace Semantic;

	class OperatorValueMatcher : public Matcher<ExpressionOperator>
	{
	public:
		OperatorValueMatcher(ExpressionOperator * opers, uint32_t oper_count, bool neg);
		~OperatorValueMatcher();

	protected:
		virtual void BeginInternal() override;
		virtual uint64_t * NextInternal() override;
	
	private:
		ExpressionOperator * opers;
		uint32_t oper_count;
		bool neg;
	};
}