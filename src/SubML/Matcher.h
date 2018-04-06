#pragma once

#include "TypePackElement.h"

namespace Ravel
{
	namespace SubML
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

			void Begin(void ** captures, size_t capture_count, TArgs... values)
			{
				begun = true;
				SetMatchArguments(0, values...);
				match_captures = captures;
				match_capture_count = capture_count;
				BeginInternal();
			}

			void ** Next()
			{
				if (result_queue[result_queue_idx])
				{
					void ** result = result_queue[result_queue_idx];
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

			bool AddToResultQueue(void ** result)
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

			template<size_t idx>
			inline type_pack_element<idx, TArgs...> MatchArgument()
			{
				return reinterpret_cast<type_pack_element<idx, TArgs...>>(match_arguments[idx]);
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
			void * match_arguments[sizeof...(TArgs)];
			void ** match_captures;
			size_t match_capture_count;
			
			bool begun;
			bool finished;

			virtual void BeginInternal() = 0;
			virtual void ** NextInternal() = 0;

			static const size_t result_queue_size = 8;
			void ** result_queue[result_queue_size];
			size_t result_queue_idx;
			size_t result_queue_add_idx;

		private:
			template<typename Head, typename... Tail>
			inline void SetMatchArguments(size_t idx, Head head, Tail... tail)
			{
				static_assert(sizeof(Head) <= sizeof(void *), "Matcher arguments must not be larger than the size of a pointer (likely 4 bytes)");
				match_arguments[idx] = reinterpret_cast<void *>(head);
				SetMatchArguments(idx + 1, tail...);
			}
			template<typename Head>
			inline void SetMatchArguments(size_t idx, Head head)
			{
				static_assert(sizeof(Head) <= sizeof(void *), "Matcher arguments must not be larger than the size of a pointer (likely 4 bytes)");
				match_arguments[idx] = reinterpret_cast<void *>(head);
			}
		};
	}
}
