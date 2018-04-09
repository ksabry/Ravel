#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>

namespace Ravel::SubML
{
	enum class TokenType
	{
		IDENTIFIER,
		INTEGER,
		STRING,
		OPERATOR,
		EVAL
	};

	enum class TokenOperator
	{
		NONE,
		DOUBLE_RIGHT_ARROW,
		RIGHT_ARROW,
		LEFT_ARROW,
		DOUBLE_DOLLAR,
		LEFT_PAREN,
		RIGHT_PAREN,
		LEFT_CURLY,
		RIGHT_CURLY,
		LEFT_SQUARE,
		RIGHT_SQUARE,
		LEFT_ANGLE,
		RIGHT_ANGLE,
		AMPERSAT,
		STAR,
		PLUS,
		BANG,
		COMMA,
		QUESTION_MARK,
		COLON,
		MINUS,
		EQUALS,
		SEMICOLON,
		FORWARD_SLASH,
		DOLLAR,
		TILDE,
		BAR
	};

	struct Token
	{
		Token(TokenType type, uint32_t line, uint32_t column)
			: type(type), line(line), column(column)
		{
		}

		TokenType type;

		uint32_t line;
		uint32_t column;
	};

	struct IdentifierToken : public Token
	{
		IdentifierToken(char const * const string, uint32_t line, uint32_t column)
			: Token(TokenType::IDENTIFIER, line, column)
		{
			uint32_t length = strlen(string);
			this->string = new char[length + 1];
			strcpy(this->string, string);
		}

		~IdentifierToken()
		{
			delete[] string;
		}

		char * string;
	};

	struct IntegerToken : public Token
	{
		IntegerToken(uint32_t value, uint32_t line, uint32_t column)
			: Token(TokenType::INTEGER, line, column), value(value)
		{
		}

		uint32_t value;
	};

	struct StringToken : public Token
	{
		StringToken(char const * const string, uint32_t line, uint32_t column)
			: Token(TokenType::STRING, line, column)
		{
			uint32_t length = strlen(string);
			this->string = new char[length + 1];
			strcpy(this->string, string);
		}

		~StringToken()
		{
			delete[] string;
		}

		char * string;
	};

	struct OperatorToken : public Token
	{
		OperatorToken(TokenOperator oper, uint32_t line, uint32_t column)
			: Token(TokenType::OPERATOR, line, column), oper(oper)
		{
		}

		TokenOperator oper;
	};

	inline bool IsOperatorToken (const Token & token, TokenOperator oper)
	{
		return token.type == TokenType::OPERATOR && static_cast<const OperatorToken &>(token).oper == oper;
	}

	std::ostream & operator<<(std::ostream & os, TokenType const & token_type);
	std::ostream & operator<<(std::ostream & os, TokenOperator const & oper);
	std::ostream & operator<<(std::ostream & os, Token const & token);
}
