#pragma once

#include "Populator.h"

namespace Ravel::SubML
{
	class ExpressionPopulator : public Populator<Expression * &>
	{
	public:
		ExpressionPopulator(Populator<ExpressionOperator &> * oper, Populator<Expression * &> * args);
		~ExpressionPopulator();

		virtual Error * Populate(void ** captures, uint32_t capture_count, Expression * & output) override;

		virtual void PPrint(std::ostream & output) override;

	private:
		Populator<ExpressionOperator &> * oper;
		Populator<Expression * &> * args;
	};
}