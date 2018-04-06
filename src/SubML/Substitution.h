#pragma once

#include "Expression.h"
#include "Matcher.h"
#include "Populator.h"

namespace Ravel
{
	using namespace Semantic;

	namespace SubML
	{
		class Substitution
		{
		public:
			Substitution();
			~Substitution();

		private:
			Matcher<Expression *> * matcher;
			Populator * populator;
		};
	}
}
