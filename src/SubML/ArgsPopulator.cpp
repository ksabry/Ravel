#include "ArgsPopulator.h"

namespace Ravel::SubML
{
	ArgsPopulator::ArgsPopulator(Populator<Expression * &> ** populators, uint32_t populator_count)
		: populators(populators), populator_count(populator_count)
	{
	}
	ArgsPopulator::~ArgsPopulator()
	{
		for (uint32_t i = 0; i < populator_count; i++)
		{
			delete populators[i];
		}
		delete[] populators;
	}

	Error * ArgsPopulator::Populate(void ** captures, uint32_t capture_count, Expression * & output)
	{
		return nullptr;
	}
}