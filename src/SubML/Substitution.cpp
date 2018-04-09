#include "Substitution.h"

namespace Ravel::SubML
{
	Substitution::Substitution(QuantifiedExpressionMatcher * matcher, Populator<Expression ** &, uint32_t &> * populator)
		: matcher(matcher), populator(populator), capture_count(0)
	{
	}
	Substitution::~Substitution()
	{
	}

	void Substitution::PPrint(std::ostream & output)
	{
		output << "Substitution {\n";
		
		std::stringstream inner;
		matcher->PPrint(inner);
		inner << ",\n";
		populator->PPrint(inner);
		output << Indent() << inner.str();

		output << "\n}";
	}
}
