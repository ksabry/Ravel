#include <cstring>

template<typename T, uint32_t COUNT>
void ArrCpy(T * dest, T (&src)[COUNT])
{
	memccpy(dest, src, COUNT * sizeof(T));
}

template<typename T>
void ArrCpy(T * dest, T * src, uint32_t count)
{
	memcpy(dest, src, count * sizeof(T));
}