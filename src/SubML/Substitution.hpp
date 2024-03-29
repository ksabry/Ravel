#pragma once

#include <ostream>
#include <vector>

#include "OrderedQuantifiedExpressionMatcher.hpp"
#include "Populator.hpp"
#include "Semantic/Expression.hpp"

namespace Ravel::SubML
{
	using namespace Semantic;

	class Substitution
	{
	public:
		Substitution(OrderedQuantifiedExpressionMatcher * matcher = nullptr, Populator<Expression ** &, uint32_t &> * populator = nullptr);
		~Substitution();
		
		inline OrderedQuantifiedExpressionMatcher * GetMatcher()
		{
			return matcher;
		}
		inline void SetMatcher(OrderedQuantifiedExpressionMatcher * matcher)
		{
			if (this->matcher) delete this->matcher;
			this->matcher = matcher;
		}

		inline Populator<Expression ** &, uint32_t &> * GetPopulator()
		{
			return populator;
		}
		inline void SetPopulator(Populator<Expression ** &, uint32_t &> * populator)
		{
			if (this->populator) delete this->populator;
			this->populator = populator;
		}

		uint32_t CaptureFromName(char const * const name);

		inline uint32_t CaptureCount()
		{
			return capture_count;
		}

	private:
		OrderedQuantifiedExpressionMatcher * matcher;
		Populator<Expression ** &, uint32_t &> * populator;

		std::vector<char const *> capture_names;
		uint32_t capture_count;

	public:
		void PPrint(std::ostream & output);
	
	};
}
