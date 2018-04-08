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
			T value_to_capture = this->template MatchArgument<0>();

			if (match_captures[capture_idx] == 0)
			{
				auto new_captures = new uint64_t [match_capture_count];
				ArrCpy(new_captures, match_captures, match_capture_count);
				new_captures[capture_idx] = bit_cast<uint64_t>(value_to_capture);

				Finish();
				return new_captures;
			}
			else
			{
				// TODO: check types intelligently
				if (match_captures[capture_idx] == bit_cast<uint64_t>(value_to_capture)) return match_captures;
				
				Finish();
				return nullptr;
			}
		}
	};
}
