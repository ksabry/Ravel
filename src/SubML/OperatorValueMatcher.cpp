#include "OperatorValueMatcher.hpp"
#include "Util/ArrCpy.hpp"

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
		ExpressionOperator to_match = MatchArgument<0>();
		for (uint32_t i = 0; i < oper_count; i++)
		{
			if (opers[i] == to_match)
			{
				Finish();
				return neg ? nullptr : match_captures;
			}
		}
		Finish();
		return neg ? match_captures : nullptr;
	}

	OperatorValueMatcher * OperatorValueMatcher::DeepCopy()
	{
		auto new_opers = new ExpressionOperator [oper_count];
		ArrCpy(new_opers, opers, oper_count);
		return new OperatorValueMatcher(new_opers, oper_count, neg);
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