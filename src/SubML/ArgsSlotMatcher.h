#pragma once

#include "Matcher.h"
#include "Expression.h"
#include "Quantifier.h"

namespace Ravel
{
	using namespace Semantic;

	namespace SubML
	{
		class ArgsSlotMatcher : public Matcher<Expression **, size_t>
		{
		public:
			virtual Quantifier GetQuantifier() = 0;
			virtual bool IsPlaceHolder() = 0;

		protected:
			virtual void BeginInternal() = 0;
			virtual void ** NextInternal() = 0;
		};
	}
}