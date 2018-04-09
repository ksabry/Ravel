#pragma once

#include <ostream>
#include <vector>

#include "Substitution.h"

namespace Ravel::SubML
{
	class Program
	{
	public:
		Program();
		~Program();

		std::vector<Substitution *> substitutions;

		void PPrint(std::ostream & output);
	};
}
