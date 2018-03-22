#pragma once

#include "Matcher.h"
#include "CaptureMatcher.h"

namespace Ravel
{
    namespace SubML
    {
        class OperatorMatcher : public Matcher<ExpressionOperator>
        {
        public:
            OperatorMatcher(ExpressionOperator oper, size_t capture_idx);

        protected:
            virtual void BeginInternal() override;
            virtual void ** NextInternal() override;

        private:
            ExpressionOperator oper;
            size_t capture_idx;
        };
    }
}
