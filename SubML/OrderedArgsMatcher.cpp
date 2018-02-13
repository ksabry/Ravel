#include "OrderedArgsMatcher.h"

namespace Ravel
{
    namespace SubML
    {
        OrderedArgsMatcher::OrderedArgsMatcher(bool ordered, QuantifiedExpressionMatcher ** matchers, size_t matcher_count)
            : ordered(ordered), matchers(matchers), matcher_count(matcher_count)
        {
        }

        void OrderedArgsMatcher::BeginInternal()
        {

        }

        void ** OrderedArgsMatcher::NextInternal()
        {
            Expression ** exprs = MatchArgument<0>();
            size_t expr_count = MatchArgument<1>();

            bool any = false;
            for (int32_t i = 0; i < expr_count; i++)
            {
                if (exprs[i] != nullptr)
                {
                    any = true;
                    break;
                }
            }
            if (!any)
            {
                Finish();
                return match_captures;
            }

            //
        }
        
        bool OrderedArgsMatcher::CalculateBounds(Expression ** exprs, size_t expr_count, size_t max_length, size_t * result)
        {
            size_t start_low, start_high;
            size_t * ltr_bounds = new size_t[2 * expr_count];
            size_t * rtl_bounds = new size_t[2 * expr_count];

            start_low = start_high = 0;
            for (int32_t i = 0; i < expr_count; i++)
            {
                auto quantifier = matchers[i]->GetQuantifier();
               
                ltr_bounds[2 * i] = start_low;
                ltr_bounds[2 * i + 1] = start_high;
                
                start_low += quantifier.low;
                start_high += quantifier.high;
                start_high = start_high > max_length ? max_length : start_high;
            }

            start_low = start_high = max_length;
            for (int32_t i = expr_count - 1; i >= 0; i++)
            {
                auto quantifier = matchers[i]->GetQuantifier();

                start_low -= quantifier.high;
                start_high -= quantifier.low;
                start_low = start_low < 0 ? 0 : start_low;

                rtl_bounds[2 * i] = start_low;
                rtl_bounds[2 * i + 1] = start_high;
            }

            for (int32_t i = 0; i < expr_count; i++)
            {
                result[2 * i]     = ltr_bounds[2 * i]     > rtl_bounds[2 * i]     ? ltr_bounds[2 * i]     : rtl_bounds[2 * i];
                result[2 * i + 1] = ltr_bounds[2 * i + 1] < rtl_bounds[2 * i + 1] ? ltr_bounds[2 * i + 1] : rtl_bounds[2 * i + 1];

                if (result[2 * i] > result[2 * i + 1]) return false;
            }

            return true;
        }
    }
}
