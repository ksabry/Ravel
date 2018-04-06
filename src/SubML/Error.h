#pragma once

#include <iostream>
#include <cstring>

#include "String.h"

namespace Ravel
{
    namespace SubML
    {
        enum ErrorType
        {
            ERR_INVALID_FILE,
            ERR_INVALID_TOKEN,
        };

        std::ostream & operator<<(std::ostream & output, ErrorType error_type);

        class Error
        {
        public:
            template<typename... TArgs>
            Error(ErrorType type, char const * const format, TArgs... args)
                : Error(type)
            {
                this->message = Formatted(format, args...);
            }

            Error(ErrorType type);
            ~Error();

            friend std::ostream & operator<<(std::ostream & output, const Error & error);

            ErrorType type;
            char * message;
        };
    }
}
