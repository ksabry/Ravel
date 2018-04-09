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

	void OperatorValueMatcher::PPrint(std::ostream & output)
	{
		output << "OperatorValueMatcher(";

		if (neg) output << "! ";

		if (oper_count > 0) output << StringFromOperator(opers[0]);
		for (uint32_t i = 1; i < oper_count; i++)
		{
			output << ", " << StringFromOperator(opers[i]);
		}

		output << ")";
	}
}