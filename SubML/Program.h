#pragma once

#include <vector>

#include "Substitution.h"

namespace Ravel
{
    namespace SubML
    {
        class Program
        {
        public:
            Program();
            ~Program();

        private:
            std::vector<Substitution *> substitutions;
            std::vector<const char * const> capture_names;

        };
    }
}
