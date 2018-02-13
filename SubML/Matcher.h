#pragma once

#include <vector>
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
                : begun(false), finished(false)
            {
            }

            void Begin(void ** captures, size_t capture_count, TArgs... values)
            {
                begun = true;
                SetMatchArguments(values...);
                match_captures = captures;
                match_capture_count = capture_count;
                BeginInternal();
            }

            void ** Next()
            {
                if (finished) return nullptr;
                return NextInternal();
            }

            inline void Finish()
            {
                finished = true;
            }

            template<size_t idx>
            inline type_pack_element<idx, TArgs...> MatchArgument()
            {
                return static_cast<type_pack_element<idx, TArgs...>>(match_arguments[idx]);
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

        private:
            template<typename Head, typename... Tail>
            inline void SetMatchArguments(size_t idx, Head head, Tail... tail)
            {
                static_assert(sizeof(Head) <= sizeof(void*), "Matcher arguments must not be larger than the size of a pointer (likely 4 bytes)");
                match_arguments[idx] = static_cast<void*>(head);
                SetMatchArguments(idx + 1, tail...);
            }
            inline void SetMatchArguments<>(size_t idx)
            {
            }
        };
    }
}
