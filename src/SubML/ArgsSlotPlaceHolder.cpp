#include "ArgsSlotPlaceHolder.h"

#include <assert.h>

namespace Ravel
{
    namespace SubML
    {
        ArgsSlotPlaceHolder::ArgsSlotPlaceHolder(size_t size)
            : size(size)
        {
        }

        void ArgsSlotPlaceHolder::BeginInternal()
        {
            // This should never be reached
            assert(false);
        }

        void ** ArgsSlotPlaceHolder::NextInternal()
        {
            // This should never be reached
            return nullptr;
        }
    }
}
