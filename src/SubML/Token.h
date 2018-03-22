#pragma once

#include <cstdint>
#include <cstring>

namespace Ravel
{
    namespace SubML
    {
        enum TokenType
        {
            TOK_IDENTIFIER,
            TOK_INTEGER,
            TOK_STRING,
            TOK_OPERATOR,
            TOK_EVAL
        };

        enum TokenOperator
        {
            OP_NONE,
            OP_DOUBLE_RIGHT_ARROW,
            OP_RIGHT_ARROW,
            OP_LEFT_ARROW,
            OP_DOUBLE_DOLLAR,
            OP_LEFT_PAREN,
            OP_RIGHT_PAREN,
            OP_LEFT_CURLY,
            OP_RIGHT_CURLY,
            OP_LEFT_SQUARE,
            OP_RIGHT_SQUARE,
            OP_LEFT_ANGLE,
            OP_RIGHT_ANGLE,
            OP_AMPERSAT,
            OP_STAR,
            OP_PLUS,
            OP_BANG,
            OP_COMMA,
            OP_QUESTION_MARK,
            OP_COLON,
            OP_MINUS,
            OP_EQUALS,
            OP_SEMICOLON,
            OP_FORWARD_SLASH,
            OP_DOLLAR,
            OP_TILDE,
            OP_BAR
        };

        struct Token
        {
            Token(TokenType type, size_t line, size_t column)
                : type(type), line(line), column(column)
            {
            }

            TokenType type;

            size_t line;
            size_t column;
        };

        struct IdentifierToken : public Token
        {
            IdentifierToken(char const * const string, size_t line, size_t column)
                : Token(TOK_IDENTIFIER, line, column)
            {
                size_t length = strlen(string);
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
            IntegerToken(uint32_t value, size_t line, size_t column)
                : Token(TOK_INTEGER, line, column), value(value)
            {
            }

            uint32_t value;
        };

        struct StringToken : public Token
        {
            StringToken(char const * const string, size_t line, size_t column)
                : Token(TOK_STRING, line, column)
            {
                size_t length = strlen(string);
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
            OperatorToken(TokenOperator oper, size_t line, size_t column)
                : Token(TOK_OPERATOR, line, column), oper(oper)
            {
            }

            TokenOperator oper;
        };
    }
}
