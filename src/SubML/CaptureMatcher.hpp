#pragma once

#include <cstring>

#include "Matcher.hpp"
#include "Semantic/Expression.hpp"
#include "Util/ArrCpy.hpp"

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

		virtual CaptureMatcher<T> * DeepCopy() override
		{
			return new CaptureMatcher<T>(capture_idx);
		}

	protected:
		uint32_t capture_idx;
		
		using Matcher<T>::match_captures;
		using Matcher<T>::match_capture_count;
		using Matcher<T>::Finish;

		virtual void BeginInternal() override
		{
		}

		virtual bool NextInternal() override
		{
			T value_to_capture = this->template MatchArgument<0>();

			if (match_captures[capture_idx] == 0)
			{
				output_captures = input_captures;
				output_captures[capture_idx] = bit_cast<uint64_t>(value_to_capture);
				Finish(); return true;
			}
			// TODO: check types intelligently
			else if (match_captures[capture_idx] == bit_cast<uint64_t>(value_to_capture)) 
			{
				output_captures = input_captures;
				Finish(); return true;
			}
			return false;
		}
	
	public:
		virtual void PPrint(std::ostream & output) override
		{
			output << "CaptureMatcher<" << typeid(T).name() << ">(" << capture_idx << ")";
		}
	};
}
