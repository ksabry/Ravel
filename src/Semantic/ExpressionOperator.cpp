#include "ExpressionOperator.h"

#include <unordered_map>

namespace Ravel
{
	namespace Semantic
	{
		ExpressionOperator OperatorFromString(char const * const string)
		{
			static std::unordered_map<std::string, ExpressionOperator> cache;
			std::string str = std::string(string);

			auto result = cache.find(str);
			if (result != cache.end())
			{
				return result->second;
			}
			ExpressionOperator next = cache.size();
			cache[str] = next;
			return next;
		}
	}
}
