#pragma once

#include <cstring>

template<typename T, uint32_t COUNT>
inline void ArrCpy(T * dest, T (&src)[COUNT])
{
	memccpy(dest, src, COUNT * sizeof(T));
}

template<typename T>
inline void ArrCpy(T * dest, T const * src, uint32_t count)
{
	memcpy(dest, src, count * sizeof(T));
}