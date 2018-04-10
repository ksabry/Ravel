#pragma once

#include "Matcher.h"
#include "UnorderedQuantifiedExpressionMatcher.h"

namespace Ravel::SubML
{
	class UnorderedArgsMatcher : public Matcher<Expression *>
	{
	public:
		UnorderedArgsMatcher(UnorderedQuantifiedExpressionMatcher ** matchers, uint32_t matcher_count);
		~UnorderedArgsMatcher();

	protected:
		virtual void BeginInternal() override;
		virtual uint64_t * NextInternal() override;

	private:
		UnorderedQuantifiedExpressionMatcher ** matchers;
		uint32_t matcher_count;

		Expression ** exprs;
		uint32_t expr_count;

		struct Frame
		{
			bool initialized = false;
			Expression ** remaining_exprs = nullptr;
		};

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}