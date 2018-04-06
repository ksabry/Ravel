#pragma once

#include "Assert.h"
#include "Token.h"

namespace Ravel::SubML
{
	class TokenList
	{
	public:
		TokenList(uint32_t initialCapacity = 256);
		~TokenList();

		inline uint32_t Count() const
		{
			return count;
		}

		inline uint32_t Capacity() const
		{
			return capacity;
		}

		inline Token * GetToken(uint32_t idx) const
		{
			return tokens[idx];
		}

		inline void SetToken(uint32_t idx, Token * token)
		{
			tokens[idx] = token;
		}

		void Resize(uint32_t new_capacity);
		void Add(Token * token);

		inline Token * & operator[](uint32_t idx)
		{
			assert(idx < capacity);
			return tokens[idx];
		}

	private:
		Token ** tokens;
		uint32_t count;
		uint32_t capacity;
	};
}