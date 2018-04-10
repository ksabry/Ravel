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

	uint64_t * ImmediateMatcher::NextInternal()
	{
		uint64_t * result = nullptr;
		Expression * expr = MatchArgument<0>();
		if (expr->GetDataType() == data_type && DataEqual(data_type, expr->GetData(), data)) 
		{
			result = match_captures;
		}
		Finish();
		return result;
	}

	void ImmediateMatcher::PPrint(std::ostream & output)
	{
		output << "ImmediateMatcher(" << data << ")";
	}
}