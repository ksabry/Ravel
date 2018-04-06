#include "Tokenizer.h"

#include <sstream>

namespace Ravel
{
	namespace SubML
	{
		Tokenizer::Tokenizer()
			: infile(nullptr), input(nullptr)
		{
		}
		Tokenizer::~Tokenizer()
		{
			if (infile && infile->is_open())
			{
				infile->close();
			}
		}

		Error * Tokenizer::Tokenize(std::istream * input, std::vector<Token> * output)
		{
			this->input = input;
			line = column = 1;

			while (!input->eof())
			{
				if (ConsumeWhitespace()) continue;
				if (TryTokenizeComment(output)) continue;
				if (TryTokenizeIdentifier(output)) continue;
				if (TryTokenizeInteger(output)) continue;
				if (TryTokenizeString(output)) continue;
				if (TryTokenizeOperator(output)) continue;

				return new Error(ERR_INVALID_TOKEN, "Invalid token beginning with '%c' at %d:%d", static_cast<char>(Peek()), line, column);
			}

			return nullptr;
		}

		Error * Tokenizer::Tokenize(char const * const input_filename, std::vector<Token> * output)
		{
			infile = new std::ifstream;
			infile->open(input_filename);

			if (!infile->is_open())
			{
				return new Error(ERR_INVALID_FILE, "Could not open '%s'", input_filename);
			}

			Error * result = Tokenize(infile, output);
			infile->close();
			delete infile;

			return result;
		}

		bool Tokenizer::ConsumeWhitespace()
		{
			bool found = false;
			while (PeekIsWhitespace())
			{
				found = true;
				Next();
			}
			return found;
		}

		char Tokenizer::ConsumeEscapeChar()
		{
			int chr = Peek();
			uint8_t result;

			if ('0' <= chr && chr <= '7')
			{
				result = 0;
				for (int32_t i = 0; i < 3; i++)
				{
					if (chr < '0' || chr > '7') break;
					result = result * 8 + (static_cast<char>(chr) - '0');
					chr = Next();
				}
			}

			else if (chr == 'x')
			{
				chr = Next();
				result = 0;
				for (int32_t i = 0; i < 2; i++)
				{
					int32_t digit = ('0' <= chr && chr <= '9') ? chr - '0' :
						('a' <= chr && chr <= 'z') ? chr - 'a' + 10 :
						('A' <= chr && chr <= 'Z') ? chr - 'A' + 10 : -1;
					if (digit == -1) break;

					result = result * 16 + static_cast<uint8_t>(digit);
					chr = Next();
				}
			}
			else
			{
				if (chr == 'a') result = '\a';
				else if (chr == 'b') result = '\b';
				else if (chr == 't') result = '\t';
				else if (chr == 'n') result = '\n';
				else if (chr == 'v') result = '\v';
				else if (chr == 'f') result = '\f';
				else if (chr == 'r') result = '\r';
				else result = static_cast<char>(chr);
				Next();
			}

			return static_cast<char>(result);
		}

		bool Tokenizer::TryTokenizeComment(std::vector<Token> *)
		{
			if (Peek() != '#') return false;

			uint32_t start_line = line;
			while (line == start_line)
			{
				Next();
			}

			return true;
		}

		bool Tokenizer::TryTokenizeIdentifier(std::vector<Token> * output)
		{
			if (!PeekIsAlpha()) return false;

			uint32_t start_line = line;
			uint32_t start_column = column;

			std::stringstream identifier;
			while (PeekIsAlpha() || PeekIsNumeric())
			{
				identifier << Peek();
				Next();
			}

			output->push_back(IdentifierToken(identifier.str().c_str(), start_line, start_column));
			return true;
		}

		bool Tokenizer::TryTokenizeInteger(std::vector<Token> * output)
		{
			if (!PeekIsNumeric()) return false;

			uint32_t start_line = line;
			uint32_t start_column = column;

			int first = Peek();
			int second = Next();

			int32_t base = 10;
			if (first == '0')
			{
				if (second == 'x' || second == 'X') base = 16;
				else if (second == 'o' || second == 'O') base = 8;
				else if (second == 'b' || second == 'B') base = 2;

				if (base != 10) Next();
			}

			uint32_t value = first - '0';

			while (true)
			{
				int chr = Peek();
				int32_t digit = ('0' <= chr && chr <= '9') ? chr - '0' :
					('a' <= chr && chr <= 'z') ? chr - 'a' + 10 :
					('A' <= chr && chr <= 'Z') ? chr - 'A' + 10 : -1;

				if (digit == -1 || digit >= base) break;
				value = value * base + digit;
			}

			output->push_back(IntegerToken(value, start_line, start_column));
			return true;
		}

		bool Tokenizer::TryTokenizeString(std::vector<Token> * output)
		{
			if (Peek() != '\'' && Peek() != '"') return false;

			uint32_t start_line = line;
			uint32_t start_column = column;

			int delim = Peek();
			Next();

			std::stringstream string;
			while (true)
			{
				int chr = Peek();
				Next();
				if (chr == '\\') chr = ConsumeEscapeChar();

				if (chr == delim) break;
				string << static_cast<char>(chr);
			}

			output->push_back(StringToken(string.str().c_str(), start_line, start_column));
			return true;
		}

		bool Tokenizer::TryTokenizeOperator(std::vector<Token>* output)
		{
			static char const * op_strings[] = { "=>", "->", "<-", "$$", "(", ")", "{", "}", "[", "]", "<", ">", "@", "*", "+", "!", ",", "?", ":", "-", "=", ";", "/", "$", "~", "|" };
			static TokenOperator op_values[] = { OP_DOUBLE_RIGHT_ARROW, OP_RIGHT_ARROW, OP_LEFT_ARROW, OP_DOUBLE_DOLLAR, OP_LEFT_PAREN, OP_RIGHT_PAREN, OP_LEFT_CURLY, OP_RIGHT_CURLY,
												 OP_LEFT_SQUARE, OP_RIGHT_SQUARE, OP_LEFT_ANGLE, OP_RIGHT_ANGLE, OP_AMPERSAT, OP_STAR, OP_PLUS, OP_BANG, OP_COMMA, OP_QUESTION_MARK,
												 OP_COLON, OP_MINUS, OP_EQUALS, OP_SEMICOLON, OP_FORWARD_SLASH, OP_DOLLAR, OP_TILDE, OP_BAR };
			static size_t op_count = sizeof(op_values) / sizeof(TokenOperator);

			auto start_pos = input->tellg();
			uint32_t start_line = line;
			uint32_t start_column = column;

			TokenOperator result = OP_NONE;
			for (int32_t i = 0; i < op_count; i++)
			{
				bool matched = true;
				char const * string = op_strings[i];
				while (*string)
				{
					if (*string == Peek())
					{
						string++;
						Next();
					}
					else
					{
						matched = false;
						break;
					}
				}

				if (matched)
				{
					result = op_values[i];
					break;
				}

				input->seekg(start_pos);
				line = start_line;
				column = start_column;
			}

			if (result == OP_NONE) return false;
			output->push_back(OperatorToken(result, start_line, start_column));
			return true;
		}
	}
}
