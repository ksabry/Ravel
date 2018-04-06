#pragma once

#include "ExpressionMatcher.h"
#include "CaptureMatcher.h"
#include "Quantifier.h"

namespace Ravel
{
	namespace SubML
	{
		class QuantifiedExpressionMatcher : public Matcher<Expression **, size_t>
		{
		public:
			QuantifiedExpressionMatcher(ExpressionMatcher * expression, Quantifier quantifier, CaptureMatcher * capture);
			
			inline virtual Quantifier GetQuantifier()
			{
				return quantifier;
			}

			inline size_t MatchLength()
			{
				return match_length;
			}

		protected:
			virtual void BeginInternal() override;
			virtual void ** NextInternal() override;

		private:
			ExpressionMatcher * expression;
			Quantifier quantifier;
			CaptureMatcher * capture;
			size_t match_length;
		};
	}
}
