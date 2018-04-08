#pragma once

#include "Matcher.h"

namespace Ravel::SubML
{
	class ImmediateMatcher : public Matcher<Expression *>
	{
	public:
		ImmediateMatcher(DataType data_type, uint64_t data);
		~ImmediateMatcher();
	
	protected:
		virtual void BeginInternal() override;
		virtual uint64_t * NextInternal() override;

	private:
		DataType data_type;
		uint64_t data;
	};
}