#include "Expression.h"

namespace Ravel
{
	namespace Semantic
	{
		Expression::~Expression()
		{
		}

		bool operator==(Expression const & left, Expression const & right)
		{
			if (left.oper != right.oper || left.ArgCount() != right.ArgCount() || left.GetDataType() != right.GetDataType())
			{
				return false;
			}

			if (left.GetDataType() != TYPE_UNKNOWN)
			{
				if (!DataEqual(left.GetDataType(), left.GetData(), right.GetData())) return false;
			}
			else
			{
				for (uint32_t i = 0; i < left.ArgCount(); i++)
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