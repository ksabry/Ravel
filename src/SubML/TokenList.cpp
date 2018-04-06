#include "TokenList.h"
#include "ArrCpy.h"

namespace Ravel::SubML
{
	TokenList::TokenList(uint32_t initialCapacity)
		: capacity(initialCapacity), tokens(nullptr), count(0)
	{
		tokens = new Token * [capacity];
	}

	TokenList::~TokenList()
	{
		for (uint32_t idx = 0; idx < count; idx++)
		{
			delete tokens[idx];
		}
		delete[] tokens;
	}

	void TokenList::Add(Token * token)
	{
		if (count == capacity)
		{
			Resize(capacity * 2);
		}
		tokens[count] = token;
	}

	void TokenList::Resize(uint32_t newCapacity)
	{
		Token ** newTokens = new Token * [newCapacity];
		ArrCpy(newTokens, tokens, capacity);
		delete[] tokens;
		tokens = newTokens;
	}
}