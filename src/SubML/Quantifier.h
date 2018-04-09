#pragma once

#include <cstdint>
#include <ostream>

namespace Ravel::SubML
{
	struct Quantifier
	{
		uint32_t low;
		uint32_t high;

		static const uint32_t infinity = 100000000;

		void PPrint(std::ostream & output);
	};
}
