#include "OperatorValuePopulator.h"

namespace Ravel::SubML
{
	OperatorValuePopulator::OperatorValuePopulator(ExpressionOperator oper)
		: oper(oper)
	{
	}
	OperatorValuePopulator::~OperatorValuePopulator()
	{
	}

	Error * OperatorValuePopulator::Populate(void ** captures, uint32_t capture_count, ExpressionOperator & output)
	{
		output = oper;
		return nullptr;
	}
}