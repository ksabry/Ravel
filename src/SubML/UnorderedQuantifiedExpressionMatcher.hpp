#pragma once

#include <vector>
#include "Matcher.hpp"
#include "Quantifier.hpp"
#include "Semantic/Expression.hpp"

namespace Ravel::SubML
{
	using namespace Semantic;

	class UnorderedQuantifiedExpressionMatcher : public Matcher<Expression **, uint32_t>
	{
	public:
		UnorderedQuantifiedExpressionMatcher(
			Matcher<Expression *> * expression_matcher, 
			Quantifier quantifier, 
			Matcher<Expression *> * capture_matcher);
		~UnorderedQuantifiedExpressionMatcher();

		inline void GetUsedIndices(uint32_t * & indices, uint32_t & index_count)
		{
			indices = this->expr_indices;
			index_count = this->expr_indices_count;
		}

		virtual UnorderedQuantifiedExpressionMatcher * DeepCopy() override;

	protected:
		virtual void BeginInternal() override;
		virtual uint64_t * NextInternal() override;

	private:
		Matcher<Expression *> * expression_matcher;
		Quantifier quantifier;
		Matcher<Expression *> * capture_matcher;

		uint32_t * expr_indices;
		uint32_t expr_indices_count;
		int32_t match_idx;
		uint64_t ** captures_stack;

		uint32_t cache_size;
		std::vector<Matcher<Expression *> *> expression_matchers_cache;
		std::vector<Matcher<Expression *> *> capture_matchers_cache;

		void ResizeCache(uint32_t new_cache_size);

		bool NextCaptures(uint64_t * & output);

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}
