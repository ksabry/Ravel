#pragma once

#include "Expression.h"
#include "Matcher.h"
#include "Populator.h"

namespace Ravel::SubML
{
	using namespace Semantic;

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
