#pragma once

namespace Ravel
{
	template<typename T, typename U>
	T Min(T a, U b)
	{
		static_assert(std::is_convertible<U, T>::value);
		return a < b ? a : static_cast<T>(b);
	}

	template<typename T, typename U>
	T Max(T a, U b)
	{
		static_assert(std::is_convertible<U, T>::value);
		return a > b ? a : static_cast<T>(b);
	}
}