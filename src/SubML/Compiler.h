#pragma once

#include <iostream>
#include <fstream>

#include "Error.h"
#include "Program.h"
#include "Tokenizer.h"

namespace Ravel
{
	namespace SubML
	{
		class Compiler
		{
		public:
			Compiler();
			~Compiler();

			Error * Compile(char const * const input_filename, Program * result);
			Error * Compile(std::istream * input, Program * result);

		private:
			Tokenizer* tokenizer;
			TokenList tokens;

			Error * CompileTokens(Program * result);
		};
	}
}
