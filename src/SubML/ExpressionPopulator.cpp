#include "ExpressionPopulator.hpp"

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

	void ExpressionPopulator::PPrint(std::ostream & output)
	{
		output << "ExpressionPopulator {\n";
		
		std::stringstream inner;
		oper->PPrint(inner);
		inner << ",\n";
		args->PPrint(inner);
		output << Indent() << inner.str();

		output << "\n}";
	}
}