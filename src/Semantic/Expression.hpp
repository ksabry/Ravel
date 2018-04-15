#pragma once

#include "ExpressionOperator.hpp"
#include "DataType.hpp"
#include "Util/BitCast.hpp"

namespace Ravel
{
	namespace Semantic
	{
		class Expression
		{
		public:
			template<typename... TArgs>
			Expression(ExpressionOperator oper, TArgs... args)
				: oper(oper), data_type(DataType::UNKNOWN)
			{
				arg_count = sizeof...(TArgs);
				this->args = new Expression * [arg_count];
				InitArgs<0>(args...);
			}
			template<typename IMM_T>
			Expression(ExpressionOperator oper, IMM_T data)
				: oper(oper), arg_count(0), args(nullptr)
			{
				SetData(data);
			}
			template<typename... TArgs>
			Expression(char const * const oper_str, TArgs... args)
				: Expression(OperatorFromString(oper_str), args...)
			{
			}
			Expression(ExpressionOperator oper);
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
				this->data = bit_cast<uint64_t, T>(data);
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
			DataType data_type;
			uint64_t data;

			template<uint32_t idx, typename... TArgs>
			inline void InitArgs(Expression * head, TArgs... tail)
			{
				args[idx] = head;
				InitArgs<idx + 1>(tail...);
			}
			template<uint32_t idx>
			inline void InitArgs(Expression * head)
			{
				args[idx] = head;
			}
		};
	}
}