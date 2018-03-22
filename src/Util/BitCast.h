#pragma once

namespace Ravel
{
    template<typename To, typename From>
    static inline To bit_cast(From data)
    {
        return *reinterpret_cast<To*>(&data);
    }
}
