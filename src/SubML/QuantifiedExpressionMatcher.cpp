#include "QuantifiedExpressionMatcher.h"

namespace Ravel
{
	namespace SubML
	{
		QuantifiedExpressionMatcher::QuantifiedExpressionMatcher(ExpressionMatcher * expression, Quantifier quantifier, CaptureMatcher * capture)
			: expression(expression), quantifier(quantifier), capture(capture)
		{
		}

		void QuantifiedExpressionMatcher::BeginInternal()
		{
		}

		void ** QuantifiedExpressionMatcher::NextInternal()
		{
			return nullptr;
		}
	}
}

