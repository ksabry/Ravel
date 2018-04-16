#pragma once

#include <ostream>
#include <vector>
#include "Util/TypePackElement.hpp"
#include "Util/BitCast.hpp"
#include "Util/String.hpp"

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
		}

		virtual void Begin(std::vector<uint64_t> & captures, uint32_t capture_count, TArgs... values)
		{
			begun = true;
			finished = false;
			SetMatchArguments(0, values...);
			input_captures = captures;
			capture_count = capture_count;
			BeginInternal();
		}

		virtual std::vector<uint64_t> * Next()
		{
			if (finished) return nullptr;
			if (!NextInternal())
			{
				Finish();
				return nullptr;
			}
			return &result_captures;
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

		inline void Reset()
		{
			begun = false;
			finished = false;
		}

		virtual Matcher<TArgs...> * DeepCopy() = 0;

		virtual void PPrint(std::ostream & output)
		{
			output << "Matcher<<" << this << ">>";
		}

	protected:
		uint64_t match_arguments[sizeof...(TArgs)];
		std::vector<uint64_t> & input_captures;
		std::vector<uint64_t> output_captures;
		uint32_t capture_count;
		
		bool begun;
		bool finished;

		virtual void BeginInternal() = 0;
		virtual bool NextInternal() = 0;

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
