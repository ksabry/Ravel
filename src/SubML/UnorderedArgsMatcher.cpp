#include "UnorderedArgsMatcher.h"

namespace Ravel::SubML
{
	UnorderedArgsMatcher::UnorderedArgsMatcher(QuantifiedExpressionMatcher ** matchers, uint32_t matcher_count)
		: matchers(matchers), matcher_count(matcher_count)
	{
	}
	UnorderedArgsMatcher::~UnorderedArgsMatcher()
	{
		for (uint32_t idx = 0; idx < matcher_count; idx++)
		{
			delete matchers[idx];
		}
		delete[] matchers;
	}

	void UnorderedArgsMatcher::BeginInternal()
	{
	}

	uint64_t * UnorderedArgsMatcher::NextInternal()
	{
		return nullptr;
	}
}