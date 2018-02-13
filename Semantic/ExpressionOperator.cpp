#include "ExpressionOperator.h"

#include <unordered_map>

namespace Ravel
{
    namespace Semantic
    {
        ExpressionOperator OperatorFromString(char const * const string)
        {
            static std::unordered_map<char const * const, ExpressionOperator> cache;
            
            auto result = cache.find(string);
            if (result != cache.end())
            {
                return result->second;
            }
            ExpressionOperator next = cache.size();
            cache[string] = next;
            return next;
        }
    }
}
