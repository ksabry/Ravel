#pragma once

#include "Matcher.h"
#include "ExpressionArgsMatcher.h"
#include "OperatorMatcher.h"

namespace Ravel
{
    namespace SubML
    {
        class ExpressionMatcher : public Matcher<Expression *>
        {
        public:
            ExpressionMatcher(OperatorMatcher * oper, ExpressionArgsMatcher * groupMatcher);
            
        protected:
            virtual void BeginInternal() override;
            virtual void ** NextInternal() override;

        private:
            OperatorMatcher * oper;
            ExpressionArgsMatcher * groupMatcher;
        };
    }
}
