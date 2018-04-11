#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>

namespace Ravel
{
	template<typename... TArgs>
	char * Formatted(char const * format, TArgs... args)
	{
		static const uint32_t default_size = 256;
		
		char * result = new char[default_size];
		int32_t real_size = snprintf(result, default_size, format, args...);
		if (real_size >= default_size)
		{
			delete[] result;
			result = new char[real_size + 1];
			snprintf(result, default_size, format, args...);
		}
		return result;
	}

	class IndentProxy;

	class IndentOStream : public std::ostream
	{
	private:
		IndentOStream(std::ostream & internal, uint32_t size);
	
	public:
		std::ostream & internal;
		uint32_t size;

		template<typename T>
		friend std::ostream & operator<<(IndentOStream & os, T const & right)
		{
			std::stringstream buff;
			buff << right;

			os.internal << std::string(os.size, ' ');
			for (char & c : buff.str())
			{
				os.internal << c;
				if (c == '\n') os.internal << std::string(os.size, ' ');
			}
			delete &os;
			return os.internal;
		}

		static IndentOStream & Create(std::ostream & internal, IndentProxy & indent);
	};

	class IndentProxy
	{
	private:
		IndentProxy(uint32_t size);
	
	public:
		uint32_t size;

		friend IndentOStream & operator<<(std::ostream & os, IndentProxy & self);
		static IndentProxy & Create(uint32_t size);
	};

	static const uint32_t DEFAULT_INDENT = 4;
	IndentProxy & Indent(uint32_t size = DEFAULT_INDENT);
}