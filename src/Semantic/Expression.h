#pragma once

#include "ExpressionOperator.h"
#include "DataType.h"
#include "BitCast.h"

namespace Ravel
{
	namespace Semantic
	{
		class Expression
		{
		public:
			template<typename... TArgs>
			Expression(ExpressionOperator oper, TArgs... args)
				: oper(oper), data_type(DataType::UNKNOWN), args{ args... }
			{
			}
			template<typename... TArgs>
			Expression(char const * const oper_str, TArgs... args)
				: oper(OperatorFromString(oper_str)), args{ args... }
			{
			}
			template<typename IMM_T>
			Expression(ExpressionOperator oper, IMM_T data)
				: oper(oper)
			{
				SetData(data);
			}
			template<typename IMM_T>
			Expression(char const * const oper_str, IMM_T data)
			{
				SetData(data);
			}
			~Expression();

			inline ExpressionOperator Oper() const
			{
				return oper;
			}
			
			inline Expression ** Args()
			{
				return args;
			}

			inline uint32_t ArgCount() const
			{
				return arg_count;
			}

			inline DataType GetDataType() const
			{
				return data_type;
			}

			template<typename T>
			inline void SetData(T data)
			{
				static_assert(sizeof(T) <= 8, "Expression data must be no more than 64 bits");
				data_type = GetAssociatedDataType<T>();
				data = bit_cast<uint64_t>(data);
			}
			
			inline uint64_t GetData() const
			{
				return data;
			}

			template<typename T>
			inline T GetDataAs() const
			{
				return bit_cast<T>(data);
			}

			friend bool operator==(Expression const & left, Expression const & right);
			inline friend bool operator!=(Expression const & left, Expression const & right)
			{
				return !(left == right);
			}

		private:
			ExpressionOperator oper;
			Expression ** args;
			uint32_t arg_count;
			uint32_t arg_container_count;
			DataType data_type;
			uint64_t data;
		};
	}
}