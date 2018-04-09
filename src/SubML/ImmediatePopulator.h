#pragma once

#include "Populator.h"

namespace Ravel::SubML
{
	class ImmediatePopulator : public Populator<Expression * &>
	{
	public:
		ImmediatePopulator(DataType data_type, uint64_t data);
		~ImmediatePopulator();

		virtual Error * Populate(void ** captures, uint32_t capture_count, Expression * & output);

		virtual void PPrint(std::ostream & output) override;

	private:
		DataType data_type;
		uint64_t data;
	};
}