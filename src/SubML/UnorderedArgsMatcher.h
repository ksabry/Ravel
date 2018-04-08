#pragma once

#include "ArgsMatcher.h"
#include "QuantifiedExpressionMatcher.h"

namespace Ravel::SubML
{
	class UnorderedArgsMatcher : public ArgsMatcher
	{
	public:
		UnorderedArgsMatcher(QuantifiedExpressionMatcher ** matchers, uint32_t matcher_count);
		~UnorderedArgsMatcher();

	protected:
		virtual void BeginInternal() override;
		virtual void ** NextInternal() override;

	private:
		QuantifiedExpressionMatcher ** matchers;
		uint32_t matcher_count;
	};
}