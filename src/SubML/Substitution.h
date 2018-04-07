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
		Substitution(QuantifiedExpressionMatcher * matcher, Populator * populator);
		~Substitution();

	private:
		QuantifiedExpressionMatcher * matcher;
		Populator * populator;
	};
}
