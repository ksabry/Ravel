#include "Substitution.h"

namespace Ravel::SubML
{
	Substitution::Substitution(QuantifiedExpressionMatcher * matcher, Populator * populator)
		: matcher(matcher), populator(populator)
	{
	}
	Substitution::~Substitution()
	{
	}
}
