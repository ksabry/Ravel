#include "OrderedQuantifiedExpressionMatcher.hpp"
#include "Util/Numeric.hpp"

namespace Ravel::SubML
{
	OrderedQuantifiedExpressionMatcher::OrderedQuantifiedExpressionMatcher(
		Matcher<Expression *> * expression_matcher, 
		Quantifier quantifier, 
		Matcher<Expression *> * capture_matcher)
		: expression_matcher(expression_matcher), quantifier(quantifier), capture_matcher(capture_matcher), cache_size(0), captures_stack(nullptr)
	{
		ResizeCache(Min(10u, quantifier.high));
	}

	OrderedQuantifiedExpressionMatcher::~OrderedQuantifiedExpressionMatcher()
	{
		delete expression_matcher;
		delete capture_matcher;
		if (captures_stack) delete[] captures_stack;
		for (auto e_matcher : expression_matchers_cache) delete e_matcher;
		for (auto c_matcher : capture_matchers_cache) delete c_matcher;
	}

	void OrderedQuantifiedExpressionMatcher::BeginInternal()
	{
		Expression ** exprs = MatchArgument<0>();
		uint32_t expr_count = MatchArgument<1>();

		if (captures_stack) delete[] captures_stack;
		captures_stack = new uint64_t * [Min(expr_count, quantifier.high)];

		if (expr_count > cache_size) ResizeCache(expr_count);
		match_idx = 0;
		captures_stack[0] = match_captures;
	}

	uint64_t * OrderedQuantifiedExpressionMatcher::NextInternal()
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
			}
			if (match_idx + 1 >= quantifier.low) return next_captures;
		}

		Finish();
		if (quantifier.low == 0) return match_captures;
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

	bool OrderedQuantifiedExpressionMatcher::NextCaptures(uint64_t * & output)
	{
		Expression ** exprs = MatchArgument<0>();

		auto e_matcher = expression_matchers_cache[match_idx];
		auto c_matcher = capture_matchers_cache[match_idx];

		output = c_matcher->HasBegun() ? c_matcher->Next() : nullptr;
		while (!output)
		{
			if (!e_matcher->HasBegun())
			{
				e_matcher->Begin(captures_stack[match_idx], match_capture_count, exprs[match_idx]);
			}
			uint64_t * expression_captures = e_matcher->Next();
			if (!expression_captures) return false;
			
			c_matcher->Begin(expression_captures, match_capture_count, exprs[match_idx]);
			output = c_matcher->Next();
		}

		return true;
	}

	OrderedQuantifiedExpressionMatcher * OrderedQuantifiedExpressionMatcher::DeepCopy()
	{
		return new OrderedQuantifiedExpressionMatcher(expression_matcher->DeepCopy(), quantifier, capture_matcher->DeepCopy());
	}

	void OrderedQuantifiedExpressionMatcher::PPrint(std::ostream & output)
	{
		output << "OrderedQuantifiedExpressionMatcher {\n";

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
