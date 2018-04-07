#pragma once

#include "Substitution.h"
#include "DynamicArray.h"

namespace Ravel::SubML
{
	class Program
	{
	public:
		Program();
		~Program();

		DynamicArray<Substitution> substitutions;
		DynamicArray<char *> capture_names;
		uint32_t capture_count;
	};
}
