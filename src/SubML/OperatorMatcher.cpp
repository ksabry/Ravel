#include "OperatorMatcher.h"

namespace Ravel
{
	namespace SubML
	{
		OperatorMatcher::OperatorMatcher(ExpressionOperator oper, size_t capture_idx)
			: oper(oper), capture_idx(capture_idx)
		{
		}

		void OperatorMatcher::BeginInternal()
		{
		}

		void ** OperatorMatcher::NextInternal()
		{
			return nullptr;
		}
	}
}
