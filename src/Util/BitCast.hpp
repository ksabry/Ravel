#pragma once

namespace Ravel
{
	template<typename To, typename From>
	inline To bit_cast(From data)
	{
		if (sizeof(To) > sizeof(From))
		{
			To result = static_cast<To>(0);
			*reinterpret_cast<From*>(&result) = data;
			return result;
		}
		else 
		{
			return *reinterpret_cast<To*>(&data);
		}
	}
}
