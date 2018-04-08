#include "MultiExpressionPopulator.h"

namespace Ravel::SubML
{
	MultiExpressionPopulator::MultiExpressionPopulator(Populator<Expression * &> ** populators, uint32_t populator_count)
		: populators(populators), populator_count(populator_count)
	{
	}

	MultiExpressionPopulator::~MultiExpressionPopulator()
	{
		delete[] populators;
	}

	Error * MultiExpressionPopulator::Populate(void ** captures, uint32_t capture_count, Expression ** & exprs, uint32_t & expr_count)
	{

	}
}