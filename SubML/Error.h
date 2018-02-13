#pragma once

#include <iostream>

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
                char buffer[1024];
                sprintf_s(buffer, format, args...);

                size_t length = strlen(buffer);
                this->message = new char[length + 1];
                strcpy_s(this->message, length + 1, buffer);
            }

            Error(ErrorType type);
            ~Error();

            friend std::ostream & operator<<(std::ostream & output, const Error & error);

            ErrorType type;
            char * message;
        };
    }
}
