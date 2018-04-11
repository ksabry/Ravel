#pragma once

#include <vector>
#include "CaptureMatcher.hpp"
#include "Quantifier.hpp"

namespace Ravel::SubML
{
	class OrderedQuantifiedExpressionMatcher : public Matcher<Expression **, uint32_t>
	{
	public:
		OrderedQuantifiedExpressionMatcher(
			Matcher<Expression *> * expression_matcher, 
			Quantifier quantifier, 
			CaptureMatcher<Expression *> * capture_matcher);
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
		virtual uint64_t * NextInternal() override;

	private:
		Matcher<Expression *> * expression_matcher;
		Quantifier quantifier;
		CaptureMatcher<Expression *> * capture_matcher;
		
		int32_t match_idx;
		
		uint32_t cache_size;
		std::vector<Matcher<Expression *> *> expression_matchers_cache;
		std::vector<CaptureMatcher<Expression *> *> capture_matchers_cache;

		void ResizeCache(uint32_t new_cache_size);

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}
