#pragma once

#include "Matcher.h"
#include "Populator.h"

namespace Ravel
{
    namespace SubML
    {
        class Substitution
        {
        public:
            Substitution();
            ~Substitution();

        private:
            Matcher * matcher;
            Populator * populator;
        };
    }
}
