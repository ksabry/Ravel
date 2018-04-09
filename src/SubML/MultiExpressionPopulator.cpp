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
		return nullptr;
	}

	void MultiExpressionPopulator::PPrint(std::ostream & output)
	{
		output << "MultiExpressionPopulator {\n";

		std::stringstream inner;
		if (populator_count > 0) populators[0]->PPrint(inner);
		for (uint32_t i = 1; i < populator_count; i++)
		{
			inner << ",\n";
			populators[i]->PPrint(inner);
		}
		output << Indent() << inner.str();

		output << "\n}";
	}
}