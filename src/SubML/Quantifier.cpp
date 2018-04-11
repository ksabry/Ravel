#include "Quantifier.hpp"

namespace Ravel::SubML
{
	void Quantifier::PPrint(std::ostream & output)
	{
		output << "Quantifier(" << low << ", " << high << ")";
	}
}