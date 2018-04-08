#include "ExpressionPopulator.h"

namespace Ravel::SubML
{
	ExpressionPopulator::ExpressionPopulator(Populator<ExpressionOperator &> * oper, Populator<Expression * &> * args)
		: oper(oper), args(args)
	{
	}
	ExpressionPopulator::~ExpressionPopulator()
	{
		delete oper;
		delete args;
	}

	Error * ExpressionPopulator::Populate(void ** captures, uint32_t capture_count, Expression * & output)
	{
		return nullptr;
	}
}