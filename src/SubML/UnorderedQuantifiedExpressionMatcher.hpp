#pragma once

#include <vector>
#include "CaptureMatcher.hpp"
#include "Quantifier.hpp"

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
			index_count = static_cast<uint32_t>(this->match_idx + 1);
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
		int32_t match_idx;
		uint64_t ** captures_stack;

		uint32_t cache_size;
		std::vector<Matcher<Expression *> *> expression_matchers_cache;
		std::vector<CaptureMatcher<Expression *> *> capture_matchers_cache;

		void ResizeCache(uint32_t new_cache_size);

		bool NextCaptures(uint64_t * & output);

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}
