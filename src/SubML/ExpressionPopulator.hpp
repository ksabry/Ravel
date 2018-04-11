#pragma once

#include "Populator.hpp"

namespace Ravel::SubML
{
	class ExpressionPopulator : public Populator<Expression * &>
	{
	public:
		ExpressionPopulator(Populator<ExpressionOperator &> * oper, Populator<Expression * &> * args);
		~ExpressionPopulator();

		virtual Error * Populate(void ** captures, uint32_t capture_count, Expression * & output) override;

	private:
		Populator<ExpressionOperator &> * oper;
		Populator<Expression * &> * args;

	public:
		virtual void PPrint(std::ostream & output) override;		
	};
}