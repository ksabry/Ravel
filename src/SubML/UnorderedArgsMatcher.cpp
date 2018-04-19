#include "UnorderedArgsMatcher.hpp"
#include "Util/Assert.hpp"
#include "Util/ArrCpy.hpp"

namespace Ravel::SubML
{
	UnorderedArgsMatcher::UnorderedArgsMatcher(std::vector<UnorderedQuantifiedExpressionMatcher *> matchers)
		: matchers(matchers), stack(nullptr), stack_idx(0)
	{
	}
	UnorderedArgsMatcher::~UnorderedArgsMatcher()
	{
		for (uint32_t i = 0; i < matchers.size(); i++)
		{
			if (stack[i].initialized) FinishFrame(i);
		}
		delete[] stack;
		
		for (auto matcher : matchers)
		{
			delete matcher;
		}
	}

	void UnorderedArgsMatcher::BeginInternal()
	{
		Expression * parent = MatchArgument<0>();

		if (stack) DeleteStack();
		stack = new Frame[matchers.size()];
		stack_idx = 0;
		
		if (matchers.size() > 0) BeginFrame(0, input_captures, parent->Args());
	}

	bool UnorderedArgsMatcher::NextInternal()
	{
		Expression * parent = MatchArgument<0>();

		if (matchers.size() == 0)
		{
			if (parent->Args().size() == 0)
			{
				output_captures = input_captures;
				Finish(); return true;
			}
			return false;
		}

		while (stack_idx >= 0)
		{
			Assert(stack[stack_idx].initialized);

			auto frame_captures = matchers[stack_idx]->Next();
			if (!frame_captures)
			{
				FinishFrame(stack_idx);
				stack_idx--;
				continue;
			}

			int32_t * used_indices;
			uint32_t used_index_count;
			matchers[stack_idx]->GetUsedIndices(used_indices, used_index_count);
			
			auto new_remaining_exprs = stack[stack_idx].incoming_remaining_exprs;
			for (uint32_t i = 0; i < used_index_count; i++)
			{
				new_remaining_exprs[used_indices[i]] = nullptr;
			}

			if (stack_idx == matchers.size() - 1)
			{
				if (IsComplete(new_remaining_exprs))
				{
					output_captures = *frame_captures;
					return true;
				}
			}
			else
			{
				BeginFrame(
					++stack_idx,
					*frame_captures,
					new_remaining_exprs
				);
			}
		}

		return false;
	}

	void UnorderedArgsMatcher::DeleteStack()
	{
		for (uint32_t i = 0; i < matchers.size(); i++)
		{
			if (stack[i].initialized) FinishFrame(i);
		}
		delete[] stack;
	}

	void UnorderedArgsMatcher::BeginFrame(
		uint32_t idx,
		std::vector<uint64_t> & incoming_captures,
		std::vector<Expression *> & incoming_remaining_exprs)
	{
		Assert(idx < matchers.size());

		matchers[idx]->Begin(incoming_captures, &incoming_remaining_exprs);
		
		stack[idx].incoming_remaining_exprs = incoming_remaining_exprs;
		stack[idx].initialized = true;
	}

	void UnorderedArgsMatcher::FinishFrame(uint32_t idx)
	{
		Assert(stack[idx].initialized);
		stack[idx].initialized = false;
	}

	bool UnorderedArgsMatcher::IsComplete(std::vector<Expression *> & remaining_exprs)
	{
		for (auto expr : remaining_exprs)
		{
			if (expr) return false;
		}
		return true;
	}

	UnorderedArgsMatcher * UnorderedArgsMatcher::DeepCopy()
	{
		auto new_matchers = new std::vector<UnorderedQuantifiedExpressionMatcher *>();
		for (auto & matcher : matchers)
		{
			new_matchers->push_back(matcher->DeepCopy());
		}
		return new UnorderedArgsMatcher(*new_matchers);
	}

	void UnorderedArgsMatcher::PPrint(std::ostream & output)
	{
		output << "UnorderedArgsMatcher {\n";
		
		std::stringstream inner;
		if (matchers.size() > 0) matchers[0]->PPrint(inner);
		for (auto & matcher : matchers)
		{
			inner << ",\n";
			matcher->PPrint(inner);
		}
		output << Indent() << inner.str();

		output << "\n}";
	}
}