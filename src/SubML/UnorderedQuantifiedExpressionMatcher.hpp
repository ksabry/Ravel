#pragma once

#include <vector>
#include "CaptureMatcher.h"
#include "Quantifier.h"

namespace Ravel::SubML
{
	class UnorderedQuantifiedExpressionMatcher : public Matcher<Expression **, uint32_t>
	{
	public:
		UnorderedQuantifiedExpressionMatcher(
			Matcher<Expression *> * expression_matcher, 
			Quantifier quantifier, 
			CaptureMatcher<Expression *> * capture_matcher);
		~UnorderedQuantifiedExpressionMatcher();

		inline void GetUsedIndices(uint32_t * & indices, uint32_t & index_count)
		{
			indices = this->expr_indices;
			index_count = this->match_length;
		}

		virtual UnorderedQuantifiedExpressionMatcher * DeepCopy() override;

	protected:
		virtual void BeginInternal() override;
		virtual uint64_t * NextInternal() override;

	private:
		Matcher<Expression *> * expression_matcher;
		Quantifier quantifier;
		CaptureMatcher<Expression *> * capture_matcher;

		uint32_t * expr_indices;
		uint32_t match_length;

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}
