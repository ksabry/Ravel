#pragma once

#include <cstdint>
#include <unordered_map>

namespace Ravel
{
	namespace Semantic
	{
		typedef uint32_t ExpressionOperator;

		static ExpressionOperator ExpressionOperator_UNKOWN = 0;
		static ExpressionOperator ExpressionOperator_IMM = 1;

		ExpressionOperator OperatorFromString(char const * const string);
		char const * StringFromOperator(ExpressionOperator oper); 
	}
}
