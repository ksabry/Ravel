#include "ArgsSlotPlaceHolder.h"
#include "Assert.h"

namespace Ravel::SubML
{
	ArgsSlotPlaceHolder::ArgsSlotPlaceHolder(size_t size)
		: size(size)
	{
	}

	void ArgsSlotPlaceHolder::BeginInternal()
	{
		// This should never be reached
		assert(false);
	}

	void ** ArgsSlotPlaceHolder::NextInternal()
	{
		// This should never be reached
		return nullptr;
	}
}
