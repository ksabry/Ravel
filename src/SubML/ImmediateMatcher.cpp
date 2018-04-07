#include "ImmediateMatcher.h"

namespace Ravel::SubML
{
	ImmediateMatcher::ImmediateMatcher(DataType data_type, uint64_t data)
		: data_type(data_type), data(data)
	{
	}

	ImmediateMatcher::~ImmediateMatcher()
	{
	}

	void ImmediateMatcher::BeginInternal()
	{
	}

	void ** ImmediateMatcher::NextInternal()
	{
		Expression * expr = MatchArgument<0>();
		if (expr->GetDataType() != data_type || !DataEqual(data_type, expr->GetData(), data)) 
		{
			Finish();
			return nullptr;
		}
		return match_captures;
	}
}