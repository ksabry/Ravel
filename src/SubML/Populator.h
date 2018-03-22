#pragma once

#include "Expression.h"

namespace Ravel
{
    using namespace Semantic;

    namespace SubML
    {
        class Populator
        {
        public:
            virtual Expression * Populate(void ** captures, size_t capture_count) = 0;

        };
    }
}
