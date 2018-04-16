#include "ImmediateMatcher.hpp"

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

	bool ImmediateMatcher::NextInternal()
	{
		Expression * expr = MatchArgument<0>();
		if (expr->GetDataType() == data_type && DataEqual(data_type, expr->GetData(), data)) 
		{
			output_captures = input_captures;
			Finish(); return true;
		}
		return false;
	}

	ImmediateMatcher * ImmediateMatcher::DeepCopy()
	{
		return new ImmediateMatcher(data_type, data);
	}

	void ImmediateMatcher::PPrint(std::ostream & output)
	{
		output << "ImmediateMatcher(" << data << ")";
	}
}