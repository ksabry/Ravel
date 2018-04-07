#pragma once

#include "ArgsMatcher.h"

namespace Ravel::SubML
{
	class ImmediateMatcher : public ArgsMatcher
	{
	public:
		ImmediateMatcher(DataType data_type, uint64_t data);
		~ImmediateMatcher();
	
	protected:
		virtual void BeginInternal() override;
		virtual void ** NextInternal() override;

	private:
		DataType data_type;
		uint64_t data;
	};
}