#pragma once

#include <cstdint>

namespace Ravel
{
    namespace SubML
    {
        struct Quantifier
        {
            uint32_t low;
            uint32_t high;

            static const uint32_t infinity = 100000000;
        };
    }
}
