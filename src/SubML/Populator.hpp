#pragma once

#include "Expression.hpp"
#include "Error.hpp"
#include "String.hpp"

namespace Ravel::SubML
{
	using namespace Semantic;

	template<typename... TArgs>
	class Populator
	{
	public:
		virtual Error * Populate(void ** captures, uint32_t capture_count, TArgs... args) = 0;

		virtual void PPrint(std::ostream & output)
		{
			output << "Populator<<" << this << ">>";
		}
	};
}
