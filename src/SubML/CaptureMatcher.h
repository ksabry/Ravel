#pragma once

#include <cstring>

#include "Matcher.h"
#include "Expression.h"
#include "ArrCpy.h"

namespace Ravel::SubML
{
	using namespace Semantic;

	template<typename T>
	class CaptureMatcher : public Matcher<T>
	{
	public:
		CaptureMatcher(uint32_t capture_idx)
			: capture_idx(capture_idx)
		{
		}
		~CaptureMatcher()
		{
		}

		virtual void PPrint(std::ostream & output) override
		{
			output << "CaptureMatcher<" << typeid(T).name() << ">(" << capture_idx << ")";
		}

	protected:
		uint32_t capture_idx;
		
		using Matcher<T>::match_captures;
		using Matcher<T>::match_capture_count;
		using Matcher<T>::Finish;

		virtual void BeginInternal() override
		{
		}

		virtual uint64_t * NextInternal() override
		{
			uint64_t * result = nullptr;
			T value_to_capture = this->template MatchArgument<0>();

			if (match_captures[capture_idx] == 0)
			{
				result = new uint64_t [match_capture_count];
				ArrCpy(result, match_captures, match_capture_count);
				result[capture_idx] = bit_cast<uint64_t>(value_to_capture);
			}
			// TODO: check types intelligently
			else if (match_captures[capture_idx] == bit_cast<uint64_t>(value_to_capture)) 
			{
				result = match_captures;
			}

			Finish();
			return result;
		}
	};
}
