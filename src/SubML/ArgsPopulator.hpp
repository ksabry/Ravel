#pragma once

#include "Populator.hpp"

namespace Ravel::SubML
{
	class ArgsPopulator : public Populator<Expression * &>
	{
	public:
		ArgsPopulator(Populator<Expression * &> ** populators, uint32_t populator_count);
		~ArgsPopulator();

		virtual Error * Populate(void ** captures, uint32_t capture_count, Expression * & output);

	private:
		Populator<Expression * &> ** populators;
		uint32_t populator_count;

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}