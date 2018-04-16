#pragma once

#include "Matcher.hpp"
#include "Semantic/ExpressionOperator.hpp"

namespace Ravel::SubML
{
	using namespace Semantic;

	class OperatorValueMatcher : public Matcher<ExpressionOperator>
	{
	public:
		OperatorValueMatcher(ExpressionOperator * opers, uint32_t oper_count, bool neg);
		~OperatorValueMatcher();

		virtual OperatorValueMatcher * DeepCopy() override;

	protected:
		virtual void BeginInternal() override;
		virtual bool NextInternal() override;
	
	private:
		ExpressionOperator * opers;
		uint32_t oper_count;
		bool neg;

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}