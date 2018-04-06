#pragma once

#include "Expression.h"

namespace Ravel::SubML
{
	using namespace Semantic;

	class Populator
	{
	public:
		virtual Expression * Populate(void ** captures, uint32_t capture_count) = 0;

	};
}
