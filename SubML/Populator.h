#pragma once

namespace Ravel
{
    namespace SubML
    {
        class Populator
        {
        public:
            virtual Expression * Populate(std::vector<Expression *> * captures) = 0;

        };
    }
}
