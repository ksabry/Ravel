#pragma once

#include "ArgsMatcher.h"
#include "QuantifiedExpressionMatcher.h"

namespace Ravel
{
    namespace SubML
    {
        class OrderedArgsMatcher : public ArgsMatcher
        {
        public:
            OrderedArgsMatcher(bool ordered, QuantifiedExpressionMatcher ** matchers, size_t matcher_count);

        protected:
            virtual void BeginInternal() override;
            virtual void ** NextInternal() override;

        private:
            bool ordered;
            QuantifiedExpressionMatcher ** matchers;
            size_t matcher_count;

            bool CalculateBounds(Expression ** exprs, size_t expr_count, size_t max_length, size_t * result);
        };
    }
}
