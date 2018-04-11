#include "OrderedQuantifiedExpressionMatcher.hpp"
#include "Numeric.hpp"

namespace Ravel::SubML
{
	OrderedQuantifiedExpressionMatcher::OrderedQuantifiedExpressionMatcher(
		Matcher<Expression *> * expression_matcher, 
		Quantifier quantifier, 
		CaptureMatcher<Expression *> * capture_matcher)
		: expression_matcher(expression_matcher), quantifier(quantifier), capture_matcher(capture_matcher), cache_size(0)
	{
		ResizeCache(Min(10u, quantifier.high));
	}

	OrderedQuantifiedExpressionMatcher::~OrderedQuantifiedExpressionMatcher()
	{
		if (expression_matcher) delete expression_matcher;
		if (capture_matcher) delete capture_matcher;
		for (auto e_matcher : expression_matchers_cache) delete e_matcher;
		for (auto c_matcher : capture_matchers_cache) delete c_matcher;
	}

	void OrderedQuantifiedExpressionMatcher::BeginInternal()
	{
		Expression ** exprs = MatchArgument<0>();
		uint32_t expr_count = MatchArgument<1>();

		if (expr_count > cache_size) ResizeCache(expr_count);
		match_idx = 0;
		if (expr_count > 0) expression_matchers_cache[0]->Begin(match_captures, match_capture_count, exprs[0]);
	}

	uint64_t * OrderedQuantifiedExpressionMatcher::NextInternal()
	{
		Expression ** exprs = MatchArgument<0>();
		uint32_t expr_count = MatchArgument<1>();
		uint32_t match_idx_max = Min(expr_count - 1, quantifier.high);

		if (expr_count == 0)
		{
			match_idx = -1;
			Finish();
			if (quantifier.low == 0) return match_captures;
			return nullptr;
		}

		while (match_idx >= 0)
		{
			auto e_matcher = expression_matchers_cache[match_idx];
			auto c_matcher = capture_matchers_cache[match_idx];
			
			if (e_matcher->HasFinished())
			{
				match_idx--;
				continue;
			}
			
			uint64_t * capture_captures = c_matcher->Next();
			while (!capture_captures)
			{
				uint64_t * expression_captures = e_matcher->Next();
				if (!expression_captures)
				{
					match_idx--;
					continue;
				}
				c_matcher->Begin(expression_captures, match_capture_count, exprs[match_idx]);
				capture_captures = c_matcher->Next();
			}

			if (match_idx < match_idx_max)
			{
				match_idx++;
				expression_matchers_cache[match_idx]->Begin(capture_captures, match_capture_count, exprs[match_idx]);
			}

			if (match_idx + 1 >= quantifier.low) return capture_captures;
		}

		Finish();
		return nullptr;
	}

	void OrderedQuantifiedExpressionMatcher::ResizeCache(uint32_t new_cache_size)
	{
		if (new_cache_size > cache_size)
		{
			expression_matchers_cache.resize(new_cache_size);
			capture_matchers_cache.resize(new_cache_size);
			for (uint32_t i = cache_size; i < new_cache_size; i++)
			{
				expression_matchers_cache[i] = expression_matcher->DeepCopy();
				capture_matchers_cache[i] = capture_matcher->DeepCopy();
			}
		}
		else
		{
			for (uint32_t i = new_cache_size; i < cache_size; i++)
			{
				delete expression_matchers_cache[i];
				delete capture_matchers_cache[i];
			}
			expression_matchers_cache.resize(new_cache_size);
			capture_matchers_cache.resize(new_cache_size);
		}
		cache_size = new_cache_size;
	}

	OrderedQuantifiedExpressionMatcher * OrderedQuantifiedExpressionMatcher::DeepCopy()
	{
		return new OrderedQuantifiedExpressionMatcher(expression_matcher->DeepCopy(), quantifier, capture_matcher->DeepCopy());
	}

	void OrderedQuantifiedExpressionMatcher::PPrint(std::ostream & output)
	{
		output << "OrderedQuantifiedExpressionMatcher {\n";

		std::stringstream inner;
		if (expression_matcher) expression_matcher->PPrint(inner);
		else inner << "NULL";
		inner << ",\n";
		quantifier.PPrint(inner);
		inner << ",\n";
		if (capture_matcher) capture_matcher->PPrint(inner);
		else inner << "NULL";
		output << Indent() << inner.str();
		
		output << "\n}";
	}
}
