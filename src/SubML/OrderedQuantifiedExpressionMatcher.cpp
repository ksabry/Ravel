#include "OrderedQuantifiedExpressionMatcher.hpp"
#include "Util/Numeric.hpp"

namespace Ravel::SubML
{
	OrderedQuantifiedExpressionMatcher::OrderedQuantifiedExpressionMatcher(
		Matcher<Expression *> * expression_matcher, 
		Quantifier quantifier, 
		Matcher<Expression *> * capture_matcher)
		: expression_matcher(expression_matcher), quantifier(quantifier), capture_matcher(capture_matcher), 
		  cache_size(0)
	{
		ResizeCache(Min(10u, quantifier.high));
	}

	OrderedQuantifiedExpressionMatcher::~OrderedQuantifiedExpressionMatcher()
	{
		delete expression_matcher;
		delete capture_matcher;
		for (auto e_matcher : expression_matchers_cache) delete e_matcher;
		for (auto c_matcher : capture_matchers_cache) delete c_matcher;
	}

	void OrderedQuantifiedExpressionMatcher::BeginInternal()
	{
		auto exprs = MatchArgument<0>();
		uint32_t expr_start_idx = MatchArgument<1>();
		uint32_t expr_end_idx = MatchArgument<2>();
		uint32_t expr_count = expr_end_idx - expr_start_idx;

		captures_stack.resize(Min(expr_count, quantifier.high));

		if (expr_count > cache_size) ResizeCache(expr_count);
		for (uint32_t i = 0; i < cache_size; i++)
		{
			expression_matchers_cache[i]->Reset();
			capture_matchers_cache[i]->Reset();
		}

		match_idx = 0;
		captures_stack[0] = input_captures;
	}

	bool OrderedQuantifiedExpressionMatcher::NextInternal()
	{
		auto exprs = MatchArgument<0>();
		uint32_t expr_start_idx = MatchArgument<1>();
		uint32_t expr_end_idx = MatchArgument<2>();

		if (captures_stack.size() == 0)
		{
			match_idx = -1;
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
				match_idx--;
				continue;
			}

			if (match_idx < captures_stack.size() - 1)
			{
				captures_stack[++match_idx] = next_captures;
			}
			if (match_idx + 1 >= quantifier.low)
			{
				output_captures = next_captures;
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

	bool OrderedQuantifiedExpressionMatcher::NextCaptures(std::vector<uint64_t> & output)
	{
		auto exprs = MatchArgument<0>();

		auto e_matcher = expression_matchers_cache[match_idx];
		auto c_matcher = capture_matchers_cache[match_idx];

		bool c_found = c_matcher->HasBegun() ? c_matcher->Next() : false;
		while (!c_found)
		{
			if (!e_matcher->HasBegun()) e_matcher->Begin(captures_stack[match_idx], (*exprs)[match_idx]);
			bool e_found = e_matcher->Next();
			if (!e_found) return false;
			
			c_matcher->Begin(e_matcher->OutputCaptures(), (*exprs)[match_idx]);
			c_found = c_matcher->Next();
		}
		
		output = c_matcher->OutputCaptures();
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
