#pragma once

#include "Populator.hpp"

namespace Ravel::SubML
{
	class OperatorValuePopulator : public Populator<ExpressionOperator &>
	{
	public:
		OperatorValuePopulator(ExpressionOperator oper);
		~OperatorValuePopulator();

		virtual Error * Populate(void ** captures, uint32_t capture_count, ExpressionOperator & output) override;

	private:
		ExpressionOperator oper;

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}