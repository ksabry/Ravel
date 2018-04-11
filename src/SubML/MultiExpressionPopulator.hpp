#pragma once

#include "Populator.hpp"
#include "ExpressionPopulator.hpp"

namespace Ravel::SubML
{
	class MultiExpressionPopulator : public Populator<Expression ** &, uint32_t &>
	{
	public:
		MultiExpressionPopulator(Populator<Expression * &> ** populators, uint32_t populator_count);
		~MultiExpressionPopulator();

		virtual Error * Populate(void ** captures, uint32_t capture_count, Expression ** & exprs, uint32_t & expr_count);

	private:
		Populator<Expression * &> ** populators;
		uint32_t populator_count;

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}