#pragma once

#include "Matcher.hpp"
#include "UnorderedQuantifiedExpressionMatcher.hpp"

namespace Ravel::SubML
{
	class UnorderedArgsMatcher : public Matcher<Expression *>
	{
	public:
		UnorderedArgsMatcher(std::vector<UnorderedQuantifiedExpressionMatcher *> matchers);
		~UnorderedArgsMatcher();

		virtual UnorderedArgsMatcher * DeepCopy() override;

	protected:
		virtual void BeginInternal() override;
		virtual bool NextInternal() override;

	private:
		std::vector<UnorderedQuantifiedExpressionMatcher *> matchers;

		struct Frame
		{
			bool initialized = false;
			std::vector<Expression *> incoming_remaining_exprs;
		};

		Frame * stack;
		int32_t stack_idx;

		void DeleteStack();

		void BeginFrame(
			uint32_t idx,
			std::vector<uint64_t> & incoming_captures,
			std::vector<Expression *> & incoming_remaining_exprs);

		void FinishFrame(uint32_t idx);

		bool IsComplete(std::vector<Expression *> & remaining_exprs);

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}