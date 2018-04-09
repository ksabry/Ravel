#include "ExpressionOperator.h"

namespace Ravel
{
	namespace Semantic
	{
		std::unordered_map<std::string, ExpressionOperator> __operator_strings = { { "unknown", ExpressionOperator_UNKOWN }, { "imm", ExpressionOperator_IMM } };

		ExpressionOperator OperatorFromString(char const * const string)
		{
			std::string str = std::string(string);

			auto result = __operator_strings.find(str);
			if (result != __operator_strings.end())
			{
				return result->second;
			}
			ExpressionOperator next = __operator_strings.size();
			__operator_strings[str] = next;
			return next;
		}

		char const * StringFromOperator(ExpressionOperator oper)
		{
			for (auto pair : __operator_strings)
			{
				if (pair.second == oper) return pair.first.c_str();
			}
			return "<NONE>";
		}
	}
}
