#pragma once

#include "Matcher.h"
#include "QuantifiedExpressionMatcher.h"

namespace Ravel::SubML
{
	class UnorderedArgsMatcher : public Matcher<Expression *>
	{
	public:
		UnorderedArgsMatcher(QuantifiedExpressionMatcher ** matchers, uint32_t matcher_count);
		~UnorderedArgsMatcher();

		virtual void PPrint(std::ostream & output) override;

	protected:
		virtual void BeginInternal() override;
		virtual uint64_t * NextInternal() override;

	private:
		QuantifiedExpressionMatcher ** matchers;
		uint32_t matcher_count;
	};
}