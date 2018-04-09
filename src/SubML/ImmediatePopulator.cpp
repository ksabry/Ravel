#include "ImmediatePopulator.h"

namespace Ravel::SubML
{
	ImmediatePopulator::ImmediatePopulator(DataType data_type, uint64_t data)
		: data_type(data_type), data(data)
	{
	}
	ImmediatePopulator::~ImmediatePopulator()
	{
	}

	Error * ImmediatePopulator::Populate(void ** captures, uint32_t capture_count, Expression * & output)
	{
		return nullptr;
	}

	void ImmediatePopulator::PPrint(std::ostream & output)
	{
		output << "ImmediatePopulator(" << data << ")";
	}
}