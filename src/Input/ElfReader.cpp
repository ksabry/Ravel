#include "ElfReader.hpp"

namespace Ravel::Input
{
	ElfReader::ElfReader()
		: input_filename("<NONE>"), input(nullptr), input_file(nullptr)
	{
	}
	ElfReader::~ElfReader()
	{
		if (input_file)
		{
			if (input_file->is_open()) input_file->close();
			delete input_file;
		}
	}

	Error * ElfReader::Read(char const * input_filename)
	{
		this->input_filename = input_filename;
		if (input_file)
		{
			if (input_file->is_open()) input_file->close();
			delete input_file;
		}
		input_file = new std::ifstream;
		input_file->open(input_filename);


		if (!input_file->is_open())
		{
			return new Error(ERR_INVALID_FILE, "Could not open '%s'", input_filename);
		}

		Error * result = Read(input_file);
		input_file->close();
		return result;
	}

	Error * ElfReader::Read(std::istream * input)
	{
		this->input = input;
		Error * err = ParseHeader();
		if (err) return err;
		return nullptr;
	}

	Error * ElfReader::ParseByte(uint8_t * output)
	{
		int byte = Get();
		if (byte == -1) return new Error(ERR_PARSE, "Invalid elf file: `%s` (unexpected eof)", input_filename);
		*output = static_cast<uint16_t>(byte);
		return nullptr;
	}

	Error * ElfReader::ParseBytes(uint8_t * output, uint32_t count)
	{
		for (uint32_t i = 0; i < count; i++)
		{
			Error * err = ParseByte(output + i);
			if (err) return err;
		}
		return nullptr;
	}

	Error * ElfReader::ParseData(uint64_t * dest, uint32_t bytes)
	{
		*dest = 0;
		for (uint32_t i = 0; i < bytes; i++)
		{
			uint8_t byte;
			Error * err = ParseByte(&byte);
			if (err) return err;

			uint8_t endian = header.cls_32.e_ident[EI_DATA];
			if      (endian == ELFDATA2LSB) *dest |= byte << (8 * i);
			else if (endian == ELFDATA2MSB) *dest |= byte << (8 * (bytes - i - 1));
		}
		return nullptr;
	}
	Error * ElfReader::ParseData(uint8_t * dest)
	{
		uint64_t result;
		Error * err = ParseData(&result, 1);
		if (err) return err;
		*dest = static_cast<uint8_t>(result);
		return nullptr;
	}
	Error * ElfReader::ParseData(uint16_t * dest)
	{
		uint64_t result;
		Error * err = ParseData(&result, 2);
		if (err) return err;
		*dest = static_cast<uint16_t>(result);
		return nullptr;
	}
	Error * ElfReader::ParseData(uint32_t * dest)
	{
		uint64_t result;
		Error * err = ParseData(&result, 4);
		if (err) return err;
		*dest = static_cast<uint32_t>(result);
		return nullptr;
	}
	Error * ElfReader::ParseData(uint64_t * dest)
	{
		uint64_t result;
		Error * err = ParseData(&result, 8);
		if (err) return err;
		*dest = static_cast<uint64_t>(result);
		return nullptr;
	}

	Error * ElfReader::ParseHeader()
	{
		Error * err = ParseBytes(header.cls_32.e_ident, 0x10);
		if (err) return err;

		if (!CheckMagicNumber())
		{
			return new Error(ERR_PARSE, "Invalid elf file: `%s` (incorrect magic number)", input_filename);
		}

		if (header.cls_32.e_ident[EI_CLASS] == ELFCLASS32)
		{
			err = ParseDataMultiple(
				&header.cls_32.e_type,
				&header.cls_32.e_machine,
				&header.cls_32.e_version,
				&header.cls_32.e_entry,
				&header.cls_32.e_phoff,
				&header.cls_32.e_shoff,
				&header.cls_32.e_flags,
				&header.cls_32.e_ehsize,
				&header.cls_32.e_phentsize,
				&header.cls_32.e_phnum,
				&header.cls_32.e_shentsize,
				&header.cls_32.e_shnum,
				&header.cls_32.e_shstrndx
			);
			if (err) return err;
		}
		else if (header.cls_32.e_ident[EI_CLASS] == ELFCLASS64)
		{
			err = ParseDataMultiple(
				&header.cls_64.e_type,
				&header.cls_64.e_machine,
				&header.cls_64.e_version,
				&header.cls_64.e_entry,
				&header.cls_64.e_phoff,
				&header.cls_64.e_shoff,
				&header.cls_64.e_flags,
				&header.cls_64.e_ehsize,
				&header.cls_64.e_phentsize,
				&header.cls_64.e_phnum,
				&header.cls_64.e_shentsize,
				&header.cls_64.e_shnum,
				&header.cls_64.e_shstrndx
			);
			if (err) return err;
		}
		return nullptr;
	}

	bool ElfReader::CheckMagicNumber()
	{
		return (
			header.cls_32.e_ident[EI_MAG0] == 0x7F && 
			header.cls_32.e_ident[EI_MAG1] == 0x45 && 
			header.cls_32.e_ident[EI_MAG2] == 0x4C && 
			header.cls_32.e_ident[EI_MAG3] == 0x46
		);
	}
}