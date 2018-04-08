#pragma once

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
		std::vector<char *> capture_names;
		uint32_t capture_count;
	};
}
