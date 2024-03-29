#include "Util/Error.hpp"

namespace Ravel
{
	Error::Error(ErrorType type)
		: type(type), message(nullptr)
	{
	}
	Error::~Error()
	{
		delete[] message;
	}

	std::ostream & operator<<(std::ostream & output, ErrorType error_type)
	{
		switch (error_type)
		{
		case ERR_INVALID_FILE:
			output << "ERR_INVALID_FILE";
			break;
		case ERR_INVALID_TOKEN:
			output << "ERR_INVALID_TOKEN";
			break;
		case ERR_PARSE:
			output << "ERR_PARSE";
		}
		return output;
	}

	std::ostream & operator<<(std::ostream & output, const Error & error) {
		output << error.type;
		if (error.message) output << ": " << error.message;
		return output;
	}
}
