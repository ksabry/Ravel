#pragma once

#include "Matcher.h"
#include "Expression.h"

namespace Ravel
{
    using namespace Semantic;

    namespace SubML
    {
        class CaptureMatcher : public Matcher<Expression *>
        {
        public:
            CaptureMatcher(size_t capture_idx);
            ~CaptureMatcher();

        protected:
            size_t capture_idx;
            virtual void BeginInternal() override;
            virtual void ** NextInternal() override;
        };
    }
}
