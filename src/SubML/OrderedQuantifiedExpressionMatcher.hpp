#pragma once

#include <vector>
#include "Matcher.hpp"
#include "Quantifier.hpp"
#include "Semantic/Expression.hpp"

namespace Ravel::SubML
{
	using namespace Semantic;

	class OrderedQuantifiedExpressionMatcher : public Matcher<std::vector<Expression *> *, uint32_t, uint32_t>
	{
	public:
		OrderedQuantifiedExpressionMatcher(
			Matcher<Expression *> * expression_matcher, 
			Quantifier quantifier, 
			Matcher<Expression *> * capture_matcher);
		~OrderedQuantifiedExpressionMatcher();

		inline virtual Quantifier GetQuantifier()
		{
			return quantifier;
		}

		inline uint32_t MatchLength()
		{
			return static_cast<uint32_t>(match_idx + 1);
		}

		virtual OrderedQuantifiedExpressionMatcher * DeepCopy() override;

	protected:
		virtual void BeginInternal() override;
		virtual bool NextInternal() override;

	private:
		Matcher<Expression *> * expression_matcher;
		Quantifier quantifier;
		Matcher<Expression *> * capture_matcher;
		
		int32_t match_idx;
		std::vector<std::vector<uint64_t>> captures_stack;

		uint32_t cache_size;
		std::vector<Matcher<Expression *> *> expression_matchers_cache;
		std::vector<Matcher<Expression *> *> capture_matchers_cache;

		void ResizeCache(uint32_t new_cache_size);

		bool NextCaptures(std::vector<uint64_t> & output);

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}
