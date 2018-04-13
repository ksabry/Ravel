#pragma once

#include <elf.h>

namespace Ravel::Input
{
	class ElfExecutable
	{
	public:
		ElfExecutable();
		~ElfExecutable();

		union {
			Elf32_Ehdr class32;
			Elf64_Ehdr class64;
		} header;
	};
}