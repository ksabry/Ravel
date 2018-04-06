#include "Assert.h"
#include "ArrCpy.h"
#include "Numeric.h"

namespace Ravel::SubML
{
	template<typename T>
	class DynamicArray
	{
	public:
		DynamicArray(uint32_t initialCapacity = 256)
			: capacity(initialCapacity), arr(nullptr), count(0)
		{
			arr = new T * [capacity];
		}

		~DynamicArray()
		{
			for (uint32_t idx = 0; idx < count; idx++)
			{
				delete arr[idx];
			}
			delete[] arr;
		}

		inline uint32_t Count() const
		{
			return count;
		}

		inline uint32_t Capacity() const
		{
			return capacity;
		}

		inline T * Get(uint32_t idx) const
		{
			return arr[idx];
		}

		inline void Set(uint32_t idx, T * element)
		{
			arr[idx] = element;
		}

		void Resize(uint32_t newCapacity)
		{
			T ** newArr = new T * [newCapacity];
			ArrCpy(newArr, arr, Min(capacity, newCapacity));
			delete[] arr;
			arr = newArr;
			capacity = newCapacity;
		}

		void Add(T * element)
		{
			if (count >= capacity - 1)
			{
				Resize(capacity * 2);
			}
			arr[count] = element;
		}

		inline T * & operator[](uint32_t idx)
		{
			assert(idx < capacity);
			return arr[idx];
		}

	private:
		T ** arr;
		uint32_t count;
		uint32_t capacity;
	};
}