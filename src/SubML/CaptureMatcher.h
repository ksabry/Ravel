#pragma once

#include "Matcher.h"
#include "Expression.h"

namespace Ravel::SubML
{
	using namespace Semantic;

	class CaptureMatcher : public Matcher<Expression *>
	{
	public:
		CaptureMatcher(uint32_t capture_idx);
		~CaptureMatcher();

	protected:
		uint32_t capture_idx;
		virtual void BeginInternal() override;
		virtual void ** NextInternal() override;
	};
}
