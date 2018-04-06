#pragma once

#include <fstream>

#include "Token.h"
#include "Error.h"
#include "DynamicArray.h"

namespace Ravel::SubML
{
	using TokenList = DynamicArray<Token>;

	class Tokenizer
	{
	public:
		Tokenizer();
		~Tokenizer();

		Error * Tokenize(std::istream * input, TokenList * output);
		Error * Tokenize(char const * const input_filename, TokenList * output);

	private:
		static char const * const whitespace_chars;

		std::ifstream * infile;
		std::istream * input;

		uint32_t line;
		uint32_t column;

		inline int Peek()
		{
			return input->peek();
		}

		inline int Next()
		{
			if (input->get() == '\n')
			{
				line++;
				column = 1;
			}
			else
			{
				column++;
			}

			return input->peek();
		}

		inline bool PeekIsWhitespace()
		{
			int chr = Peek();
			return (chr != EOF && chr == ' ' || chr == '\t' || chr == '\r' || chr == '\n' || chr == '\v' || chr == '\f');
		}

		inline bool PeekIsAlpha()
		{
			int chr = Peek();
			return (chr != EOF && ('a' <= chr && chr <= 'z' || 'A' <= chr && chr <= 'Z' || chr == '_'));
		}

		inline bool PeekIsNumeric()
		{
			int chr = Peek();
			return (chr != EOF && ('0' <= chr && chr <= '9'));
		}

		bool ConsumeWhitespace();
		char ConsumeEscapeChar();

		bool TryTokenizeComment(TokenList * output);
		bool TryTokenizeIdentifier(TokenList * output);
		bool TryTokenizeInteger(TokenList * output);
		bool TryTokenizeString(TokenList * output);
		bool TryTokenizeOperator(TokenList * output);
		bool TryTokenizeEval(TokenList * output);
	};
}
