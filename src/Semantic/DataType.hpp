#pragma once

#include <cstdint>

#include "BitCast.hpp"

namespace Ravel
{
	namespace Semantic
	{
		enum class DataType
		{
			UNKNOWN,
			UINT32,
			UINT64,
			INT32,
			INT64,
			FLOAT32,
			FLOAT64
		};
		
		static bool DataEqual(DataType type, uint64_t data_left, uint64_t data_right)
		{
			switch (type)
			{
			case DataType::UNKNOWN: return false;
			case DataType::UINT32:  return bit_cast<uint32_t>(data_left) == bit_cast<uint32_t>(data_right);
			case DataType::UINT64:  return bit_cast<uint64_t>(data_left) == bit_cast<uint64_t>(data_right);
			case DataType::INT32:   return bit_cast<int32_t>(data_left)  == bit_cast<int32_t>(data_right);
			case DataType::INT64:   return bit_cast<int64_t>(data_left)  == bit_cast<int64_t>(data_right);
			case DataType::FLOAT32: return bit_cast<float>(data_left)	== bit_cast<float>(data_right);
			case DataType::FLOAT64: return bit_cast<double>(data_left)   == bit_cast<double>(data_right);
			}
		}

		template<typename T>
		static inline DataType GetAssociatedDataType()
		{
			return DataType::UNKNOWN;
		}
		
		template<>
		inline DataType GetAssociatedDataType<uint32_t>()
		{
			return DataType::UINT32;
		}
		template<>
		inline DataType GetAssociatedDataType<uint64_t>()
		{
			return DataType::UINT64;
		}
		template<>
		inline DataType GetAssociatedDataType<int32_t>()
		{
			return DataType::INT32;
		}
		template<>
		inline DataType GetAssociatedDataType<int64_t>()
		{
			return DataType::INT64;
		}
		template<>
		inline DataType GetAssociatedDataType<float>()
		{
			return DataType::FLOAT32;
		}
		template<>
		inline DataType GetAssociatedDataType<double>()
		{
			return DataType::FLOAT64;
		}
	}
}
