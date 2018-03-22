#include "CaptureMatcher.h"

#include <cstring>

namespace Ravel
{
    namespace SubML
    {
        CaptureMatcher::CaptureMatcher(size_t capture_idx)
            : capture_idx(capture_idx)
        {
        }
        CaptureMatcher::~CaptureMatcher()
        {
        }

        void CaptureMatcher::BeginInternal()
        {
        }

        void ** CaptureMatcher::NextInternal()
        {
            if (match_captures[capture_idx] == nullptr)
            {
                auto new_captures = new void* [match_capture_count];
                memcpy(new_captures, match_captures, match_capture_count * sizeof(void*));
                new_captures[capture_idx] = MatchArgument<0>();

                Finish();
                return new_captures;
            }
            else
            {
                if (match_captures[capture_idx] == MatchArgument<0>()) return match_captures;
                
                Finish();
                return nullptr;
            }
        }
    }
}

