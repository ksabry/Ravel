#pragma once

#include "Populator.hpp"

namespace Ravel::SubML
{
	template<typename T>
	class CapturePopulator : public Populator<T>
	{
	public:
		CapturePopulator(uint32_t capture_idx)
			: capture_idx(capture_idx)
		{
		}
		~CapturePopulator()
		{
		}

		virtual Error * Populate(void ** captures, uint32_t capture_count, T arg) override
		{
			return nullptr;
		}

	private:
		uint32_t capture_idx;
	
	public:
		virtual void PPrint(std::ostream & output) override
		{
			output << "CapturePopulator<" << typeid(T).name() << ">(" << capture_idx << ")";
		}
	};
}