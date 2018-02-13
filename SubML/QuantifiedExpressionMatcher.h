#pragma once

#include "Matcher.h"
#include "ExpressionMatcher.h"
#include "CaptureMatcher.h"
#include "Quantifier.h"

namespace Ravel
{
    namespace SubML
    {
        class QuantifiedExpressionMatcher : public Matcher<>
        {
        public:
            QuantifiedExpressionMatcher(ExpressionMatcher * expression, Quantifier quantifier, CaptureMatcher * capture);
            
            inline Quantifier GetQuantifier()
            {
                return quantifier;
            }

        protected:
            virtual void BeginInternal() override;
            virtual void ** NextInternal() override;

        private:
            ExpressionMatcher * expression;
            Quantifier quantifier;
            CaptureMatcher * capture;
        };
    }
}
