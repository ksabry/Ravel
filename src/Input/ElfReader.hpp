#pragma once

#include <elf.h>
#include <iostream>
#include <fstream>
#include "ElfExecutable.hpp"
#include "Util/Error.hpp"
#include "Util/Assert.hpp"

namespace Ravel::Input
{
	class ElfReader
	{
	public:
		ElfReader();
		~ElfReader();

		Error * Read(char const * input_filename);
		Error * Read(std::istream * input);

	private:
		char const * input_filename;
		std::ifstream * input_file;
		std::istream * input;
		ElfExecutable * output;

		uint32_t offset;
		union {
			Elf32_Ehdr cls_32;
			Elf64_Ehdr cls_64;
		} header;

		inline int Get()
		{
			offset++;
			return input->get();
		}
		inline int Peek()
		{
			return input->peek();
		}

		Error * Skip(uint32_t count);
		Error * GoTo(uint32_t offset);

		Error * ParseByte(uint8_t * output);
		Error * ParseBytes(uint8_t * output, uint32_t count);

		Error * ParseData(uint64_t * dest, uint32_t bytes);
		Error * ParseData(uint8_t * dest);
		Error * ParseData(uint16_t * dest);
		Error * ParseData(uint32_t * dest);
		Error * ParseData(uint64_t * dest);

		Error * ParseHeader();
		bool CheckMagicNumber();

		template<typename... TArgs>
		struct _Proxy_ParseDataMultiple { inline static Error * Call(ElfReader * self, TArgs... dests) 
		{ 
			Assert(false); 
		} };

		template<typename... TArgs>
		inline Error * ParseDataMultiple(TArgs... dests)
		{
			_Proxy_ParseDataMultiple<TArgs...>::Call(this, dests...);
		}
	};

	template<typename TFirst, typename... TRest>
	struct ElfReader::_Proxy_ParseDataMultiple<TFirst, TRest...> { inline static Error * Call(ElfReader * self, TFirst first, TRest... rest)
	{ 
		Error * err = self->ParseData(first);
		if (err) return err;
		return self->ParseDataMultiple(rest...);
	} };
	template<>
	struct ElfReader::_Proxy_ParseDataMultiple<> { inline static Error * Call(ElfReader * self)
	{ 
		return nullptr;
	} };
}