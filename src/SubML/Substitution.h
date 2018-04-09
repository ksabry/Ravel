#pragma once

#include <ostream>
#include <vector>

#include "Expression.h"
#include "QuantifiedExpressionMatcher.h"
#include "Populator.h"

namespace Ravel::SubML
{
	using namespace Semantic;

	class Substitution
	{
	public:
		Substitution(QuantifiedExpressionMatcher * matcher, Populator<Expression ** &, uint32_t &> * populator);
		~Substitution();
		
		void PPrint(std::ostream & output);

	private:
		QuantifiedExpressionMatcher * matcher;
		Populator<Expression ** &, uint32_t &> * populator;

		std::vector<char *> capture_names;
		uint32_t capture_count;
	};
}
