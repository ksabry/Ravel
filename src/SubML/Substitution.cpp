#include "Substitution.h"

namespace Ravel::SubML
{
	Substitution::Substitution(QuantifiedExpressionMatcher * matcher, Populator<Expression ** &, uint32_t &> * populator)
		: matcher(matcher), populator(populator)
	{
	}
	Substitution::~Substitution()
	{
	}
}
