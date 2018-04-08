#pragma once

#include "Expression.h"
#include "Error.h"

namespace Ravel::SubML
{
	using namespace Semantic;

	template<typename... TArgs>
	class Populator
	{
	public:
		virtual Error * Populate(void ** captures, uint32_t capture_count, TArgs... args) = 0;
	};
}
