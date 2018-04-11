#pragma once

#include <ostream>
#include "TypePackElement.h"
#include "BitCast.h"
#include "String.h"

namespace Ravel::SubML
{
	template<typename... TArgs>
	class Matcher
	{
	public:
		Matcher()
			: begun(false), finished(false), match_captures(nullptr), child_match_captures(nullptr)
		{
		}
		~Matcher()
		{
			if (child_match_captures && child_match_captures != match_captures) delete[] child_match_captures;
		}

		virtual void Begin(uint64_t * captures, uint32_t capture_count, TArgs... values)
		{
			begun = true;
			finished = false;
			SetMatchArguments(0, values...);
			match_captures = captures;
			match_capture_count = capture_count;
			BeginInternal();
		}

		virtual uint64_t * Next()
		{
			if (child_match_captures && child_match_captures != match_captures) delete[] child_match_captures; 
			if (finished) return nullptr;
			child_match_captures = NextInternal();
			return child_match_captures;
		}

		inline void Finish()
		{
			finished = true;
		}

		template<uint32_t idx>
		inline type_pack_element<idx, TArgs...> MatchArgument()
		{
			return bit_cast<type_pack_element<idx, TArgs...>>(match_arguments[idx]);
		}

		inline bool HasBegun()
		{
			return begun;
		}

		inline bool HasFinished()
		{
			return finished;
		}

		virtual void PPrint(std::ostream & output)
		{
			output << "Matcher<<" << this << ">>";
		}

	protected:
		uint64_t match_arguments[sizeof...(TArgs)];
		uint64_t * match_captures;
		uint32_t match_capture_count;
		uint64_t * child_match_captures;
		
		bool begun;
		bool finished;

		virtual void BeginInternal() = 0;
		virtual uint64_t * NextInternal() = 0;

	private:
		template<typename Head, typename... Tail>
		inline void SetMatchArguments(uint32_t idx, Head head, Tail... tail)
		{
			static_assert(sizeof(Head) <= sizeof(uint64_t), "Matcher arguments must not be larger than 4 bytes");
			match_arguments[idx] = bit_cast<uint64_t>(head);
			SetMatchArguments(idx + 1, tail...);
		}
		template<typename Head>
		inline void SetMatchArguments(uint32_t idx, Head head)
		{
			static_assert(sizeof(Head) <= sizeof(uint64_t), "Matcher arguments must not be larger than 4 bytes");
			match_arguments[idx] = bit_cast<uint64_t>(head);
		}
	};
}
