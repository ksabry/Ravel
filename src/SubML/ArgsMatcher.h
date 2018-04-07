#pragma once

#include "Matcher.h"
#include "Expression.h"

namespace Ravel::SubML
{
	using namespace Semantic;
	
	class ArgsMatcher : public Matcher<Expression *>
	{
	public:
		ArgsMatcher();

	protected:
		virtual void BeginInternal() override;
		virtual void ** NextInternal() override;
	};
}
