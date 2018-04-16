#include "Semantic/Expression.hpp"

namespace Ravel
{
	namespace Semantic
	{
		Expression::Expression(ExpressionOperator oper)
			: oper(oper), data_type(DataType::UNKNOWN)
		{
		}
		Expression::~Expression()
		{
			for (auto arg : args) delete arg;
		}

		bool operator==(Expression const & left, Expression const & right)
		{
			if (left.oper != right.oper || left.args.size() != right.args.size() || left.GetDataType() != right.GetDataType())
			{
				return false;
			}

			if (left.GetDataType() != DataType::UNKNOWN)
			{
				if (!DataEqual(left.GetDataType(), left.GetData(), right.GetData())) return false;
			}
			else
			{
				for (uint32_t i = 0; i < left.args.size(); i++)
				{
					if (left.args[i] == nullptr && right.args[i] == nullptr) continue;
					if (left.args[i] == nullptr || right.args[i] == nullptr) return false;
					if (*(left.args[i]) != *(right.args[i])) return false;
				}
			}

			return true;
		}
	}
}