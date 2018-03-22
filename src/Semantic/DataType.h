#pragma once

#include <cstdint>

#include "BitCast.h"

namespace Ravel
{
    namespace Semantic
    {
        enum DataType
        {
            TYPE_UNKNOWN,
            TYPE_UINT32,
            TYPE_UINT64,
            TYPE_INT32,
            TYPE_INT64,
            TYPE_FLOAT32,
            TYPE_FLOAT64
        };
        
        static bool DataEqual(DataType type, uint64_t data_left, uint64_t data_right)
        {
            switch (type)
            {
            case TYPE_UNKNOWN: return false;
            case TYPE_UINT32:  return bit_cast<uint32_t>(data_left) == bit_cast<uint32_t>(data_right);
            case TYPE_UINT64:  return bit_cast<uint64_t>(data_left) == bit_cast<uint64_t>(data_right);
            case TYPE_INT32:   return bit_cast<int32_t>(data_left)  == bit_cast<int32_t>(data_right);
            case TYPE_INT64:   return bit_cast<int64_t>(data_left)  == bit_cast<int64_t>(data_right);
            case TYPE_FLOAT32: return bit_cast<float>(data_left)    == bit_cast<float>(data_right);
            case TYPE_FLOAT64: return bit_cast<double>(data_left)   == bit_cast<double>(data_right);
            }
        }

        template<typename T>
        static inline DataType GetAssociatedDataType()
        {
            return TYPE_UNKNOWN;
        }
        
        template<>
        inline DataType GetAssociatedDataType<uint32_t>()
        {
            return TYPE_UINT32;
        }
        template<>
        inline DataType GetAssociatedDataType<uint64_t>()
        {
            return TYPE_UINT64;
        }
        template<>
        inline DataType GetAssociatedDataType<int32_t>()
        {
            return TYPE_INT32;
        }
        template<>
        inline DataType GetAssociatedDataType<int64_t>()
        {
            return TYPE_INT64;
        }
        template<>
        inline DataType GetAssociatedDataType<float>()
        {
            return TYPE_FLOAT32;
        }
        template<>
        inline DataType GetAssociatedDataType<double>()
        {
            return TYPE_FLOAT64;
        }
    }
}
