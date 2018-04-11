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
			Expression ** incoming_remaining_exprs = nullptr;
		};

		Frame * stack;
		int32_t stack_idx;

		void BeginFrame(
			uint32_t idx,
			uint64_t * incoming_captures,
			Expression ** new_remaining_exprs);

		void FinishFrame(uint32_t idx);

		bool IsComplete(Expression ** exprs);

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}