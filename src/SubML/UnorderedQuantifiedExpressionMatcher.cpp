#include "UnorderedQuantifiedExpressionMatcher.hpp"
#include "Util/Numeric.hpp"

namespace Ravel::SubML
{
	UnorderedQuantifiedExpressionMatcher::UnorderedQuantifiedExpressionMatcher(
		Matcher<Expression *> * expression_matcher, 
		Quantifier quantifier, 
		Matcher<Expression *> * capture_matcher)
		: expression_matcher(expression_matcher), quantifier(quantifier), capture_matcher(capture_matcher), 
		  cache_size(0), expr_indices(nullptr), captures_stack(nullptr)
	{
		ResizeCache(Min(10u, quantifier.high));
	}
	UnorderedQuantifiedExpressionMatcher::~UnorderedQuantifiedExpressionMatcher()
	{
		delete expression_matcher;
		delete capture_matcher;
		if (expr_indices) delete[] expr_indices;
		if (captures_stack) delete[] captures_stack;
	}

	void UnorderedQuantifiedExpressionMatcher::BeginInternal()
	{
		Expression ** exprs = MatchArgument<0>();
		uint32_t expr_count = MatchArgument<1>();
		
		if (expr_indices) delete[] expr_indices;
		expr_indices = new uint32_t [expr_count] {0};

		if (captures_stack) delete[] captures_stack;
		captures_stack = new uint64_t * [Min(expr_count, quantifier.high)];

		if (expr_count > cache_size) ResizeCache(expr_count);
		match_idx = 0;
		if (expr_count > 0) expression_matchers_cache[0]->Begin(match_captures, match_capture_count, exprs[0]);
	}

	uint64_t * UnorderedQuantifiedExpressionMatcher::NextInternal()
	{
		Expression ** exprs = MatchArgument<0>();
		uint32_t expr_count = MatchArgument<1>();
		int32_t match_idx_max = Min(expr_count, quantifier.high) - 1;

		if (match_idx_max == -1)
		{
			match_idx = -1;
			Finish();
			if (quantifier.low == 0) return match_captures;
			return nullptr;
		}

		while (match_idx >= 0)
		{
			uint64_t * next_captures;
			if (!NextCaptures(next_captures))
			{
				match_idx--;
				continue;
			}

			if (match_idx < match_idx_max)
			{
				match_idx++;
				captures_stack[match_idx] = next_captures;
				expr_indices[match_idx] = expr_indices[match_idx - 1];
			}
			if (match_idx + 1 >= quantifier.low) return next_captures;
		}

		Finish();
		if (quantifier.low == 0) return match_captures;
		return nullptr;
	}

	void UnorderedQuantifiedExpressionMatcher::ResizeCache(uint32_t new_cache_size)
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

	bool UnorderedQuantifiedExpressionMatcher::NextCaptures(uint64_t * & output)
	{
		Expression ** exprs = MatchArgument<0>();
		uint32_t expr_count = MatchArgument<1>();

		auto e_matcher = expression_matchers_cache[match_idx];
		auto c_matcher = capture_matchers_cache[match_idx];

		output = c_matcher->HasBegun() ? c_matcher->Next() : nullptr;
		while (!output)
		{
			uint64_t * expression_captures = e_matcher->HasBegun() ? e_matcher->Next() : nullptr;
			while (!expression_captures)
			{
				do { expr_indices[match_idx]++; } 
				while (expr_indices[match_idx] < expr_count && !exprs[expr_indices[match_idx]]);

				if (expr_indices[match_idx] >= expr_count) return false;

				e_matcher->Begin(captures_stack[match_idx], match_capture_count, exprs[expr_indices[match_idx]]);
				expression_captures = e_matcher->Next();
			}
			c_matcher->Begin(expression_captures, match_capture_count, exprs[match_idx]);
			output = c_matcher->Next();
		}

		return true;
	}

	UnorderedQuantifiedExpressionMatcher * UnorderedQuantifiedExpressionMatcher::DeepCopy()
	{
		return new UnorderedQuantifiedExpressionMatcher(expression_matcher->DeepCopy(), quantifier, capture_matcher->DeepCopy());
	}

	void UnorderedQuantifiedExpressionMatcher::PPrint(std::ostream & output)
	{
		output << "UnorderedQuantifiedExpressionMatcher {\n";

		std::stringstream inner;
		expression_matcher->PPrint(inner);
		inner << ",\n";
		quantifier.PPrint(inner);
		inner << ",\n";
		capture_matcher->PPrint(inner);
		output << Indent() << inner.str();
		
		output << "\n}";
	}
}
