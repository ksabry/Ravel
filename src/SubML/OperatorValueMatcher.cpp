#include "OperatorValueMatcher.h"

namespace Ravel::SubML
{
	OperatorValueMatcher::OperatorValueMatcher(ExpressionOperator * opers, uint32_t oper_count, bool neg)
		: opers(opers), oper_count(oper_count), neg(neg)
	{
	}

	OperatorValueMatcher::~OperatorValueMatcher()
	{
		delete[] opers;
	}

	void OperatorValueMatcher::BeginInternal()
	{
	}

	uint64_t * OperatorValueMatcher::NextInternal()
	{
		return nullptr;
	}
}