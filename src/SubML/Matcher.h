#pragma once

#include "TypePackElement.h"
#include "BitCast.h"

namespace Ravel::SubML
{
	template<typename... TArgs>
	class Matcher
	{
	public:
		Matcher()
			: begun(false), finished(false), result_queue_idx(0), result_queue_add_idx(0)
		{
		}
		~Matcher()
		{
		}

		void Begin(uint64_t * captures, uint32_t capture_count, TArgs... values)
		{
			begun = true;
			SetMatchArguments(0, values...);
			match_captures = captures;
			match_capture_count = capture_count;
			BeginInternal();
		}

		uint64_t * Next()
		{
			if (result_queue[result_queue_idx])
			{
				uint64_t * result = result_queue[result_queue_idx];
				result_queue[result_queue_idx] = nullptr;
				result_queue_idx = (result_queue_idx + 1) % result_queue_size;
				return result;
			}
			else
			{
				if (finished) return nullptr;
				return NextInternal();
			}
		}

		bool AddToResultQueue(uint64_t * result)
		{
			if (result[result_queue_add_idx]) return false;
			
			result[result_queue_add_idx] = result;
			result_queue_add_idx = (result_queue_add_idx + 1) % result_queue_size;
			return true;
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

	protected:
		uint64_t match_arguments[sizeof...(TArgs)];
		uint64_t * match_captures;
		uint32_t match_capture_count;
		
		bool begun;
		bool finished;

		virtual void BeginInternal() = 0;
		virtual uint64_t * NextInternal() = 0;

		static const uint32_t result_queue_size = 8;
		uint64_t * result_queue[result_queue_size];
		uint32_t result_queue_idx;
		uint32_t result_queue_add_idx;

	private:
		template<typename Head, typename... Tail>
		inline void SetMatchArguments(uint32_t idx, Head head, Tail... tail)
		{
			static_assert(sizeof(Head) <= sizeof(uint64_t), "Matcher arguments must not be larger than the size of a pointer (likely 4 bytes)");
			match_arguments[idx] = bit_cast<uint64_t>(head);
			SetMatchArguments(idx + 1, tail...);
		}
		template<typename Head>
		inline void SetMatchArguments(uint32_t idx, Head head)
		{
			static_assert(sizeof(Head) <= sizeof(uint64_t), "Matcher arguments must not be larger than the size of a pointer (likely 4 bytes)");
			match_arguments[idx] = bit_cast<uint64_t>(head);
		}
	};
}
