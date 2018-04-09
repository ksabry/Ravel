#pragma once

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
		
	private:
		QuantifiedExpressionMatcher * matcher;
		Populator<Expression ** &, uint32_t &> * populator;
	};
}
