#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>

namespace Ravel
{
	template<typename... TArgs>
	char * Formatted(char const * format, TArgs... args)
	{
		static const uint32_t default_size = 256;
		
		char * result = new char[default_size];
		int32_t real_size = snprintf(result, default_size, format, args...);
		if (real_size >= default_size)
		{
			delete[] result;
			result = new char[real_size + 1];
			snprintf(result, default_size, format, args...);
		}
		return result;
	}
}