#pragma once

#include "Matcher.h"
#include "Expression.h"

namespace Ravel
{
	using namespace Semantic;

	namespace SubML
	{
		class ArgsMatcher : public Matcher<Expression **, size_t>
		{
		public:
			ArgsMatcher();

		protected:
			virtual void BeginInternal() override;
			virtual void ** NextInternal() override;
		};
	}
}
