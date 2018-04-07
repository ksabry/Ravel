#pragma once

#include "Matcher.h"
#include "Expression.h"

namespace Ravel::SubML
{
	using namespace Semantic;

	class EvalMatcher : public Matcher<Expression *>
	{
	};
}