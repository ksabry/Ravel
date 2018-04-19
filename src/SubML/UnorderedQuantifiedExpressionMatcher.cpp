#include "UnorderedQuantifiedExpressionMatcher.hpp"
#include "Util/Numeric.hpp"

namespace Ravel::SubML
{
	UnorderedQuantifiedExpressionMatcher::UnorderedQuantifiedExpressionMatcher(
		Matcher<Expression *> * expression_matcher, 
		Quantifier quantifier, 
		Matcher<Expression *> * capture_matcher)
		: expression_matcher(expression_matcher), quantifier(quantifier), capture_matcher(capture_matcher), 
		  cache_size(0)
	{
		ResizeCache(Min(10u, quantifier.high));
	}
	UnorderedQuantifiedExpressionMatcher::~UnorderedQuantifiedExpressionMatcher()
	{
		delete expression_matcher;
		delete capture_matcher;
	}

	void UnorderedQuantifiedExpressionMatcher::BeginInternal()
	{
		auto exprs = MatchArgument<0>();
		
		expr_indices.resize(exprs->size());
		expr_indices[0] = -1;
		expr_indices_count = 0;

		captures_stack.resize(Min(exprs->size(), quantifier.high));
		captures_stack[0] = input_captures;

		if (exprs->size() > cache_size) ResizeCache(exprs->size());
		for (uint32_t i = 0; i < cache_size; i++)
		{
			expression_matchers_cache[i]->Reset();
			capture_matchers_cache[i]->Reset();
		}
		
		match_idx = 0;
	}

	bool UnorderedQuantifiedExpressionMatcher::NextInternal()
	{
		auto exprs = MatchArgument<0>();

		if (captures_stack.size() == 0)
		{
			if (quantifier.low == 0)
			{
				output_captures = input_captures;
				Finish(); return true;
			}
			return false;
		}

		while (match_idx >= 0)
		{
			std::vector<uint64_t> next_captures;
			if (!NextCaptures(next_captures))
			{
				expression_matchers_cache[match_idx]->Reset();
				capture_matchers_cache[match_idx]->Reset();

				match_idx--;
				continue;
			}

			bool returning = (match_idx + 1 >= quantifier.low);
			expr_indices_count = match_idx + 1;
			if (match_idx < captures_stack.size() - 1)
			{
				match_idx++;
				captures_stack[match_idx] = next_captures;
				expr_indices[match_idx] = expr_indices[match_idx - 1];
			}
			if (returning)
			{
				output_captures =  next_captures;
				return true;
			}
		}

		if (quantifier.low == 0) 
		{
			output_captures = input_captures;
			Finish(); return true;
		}
		return false;
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

	bool UnorderedQuantifiedExpressionMatcher::NextCaptures(std::vector<uint64_t> & output)
	{
		auto exprs = MatchArgument<0>();
		
		auto e_matcher = expression_matchers_cache[match_idx];
		auto c_matcher = capture_matchers_cache[match_idx];

		bool c_found = c_matcher->HasBegun() ? c_matcher->Next() : false;
		while (!c_found)
		{
			bool e_found = e_matcher->HasBegun() ? e_matcher->Next() : false;
			while (!e_found)
			{
				do { expr_indices[match_idx]++; }
				while (expr_indices[match_idx] < exprs->size() && !(*exprs)[expr_indices[match_idx]]);

				if (expr_indices[match_idx] >= exprs->size()) return false;

				e_matcher->Begin(captures_stack[match_idx], (*exprs)[expr_indices[match_idx]]);
				e_found = e_matcher->Next();
			}
			c_matcher->Begin(e_matcher->OutputCaptures(), (*exprs)[match_idx]);
			c_found = c_matcher->Next();
		}

		output = c_matcher->OutputCaptures();
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
