#include "OperatorMatcher.h"

namespace Ravel::SubML
{
	OperatorMatcher::OperatorMatcher(ExpressionOperator oper, uint32_t capture_idx)
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
