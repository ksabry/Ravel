#include "Token.hpp"

namespace Ravel::SubML
{
	std::ostream & operator<<(std::ostream & os, TokenType const & token_type)
	{
		switch(token_type)
		{
			case TokenType::IDENTIFIER: os << "IDN"; break;
			case TokenType::INTEGER:    os << "INT"; break;
			case TokenType::STRING:     os << "STR"; break;
			case TokenType::OPERATOR:   os << "OPR"; break;
			case TokenType::EVAL:       os << "EVL"; break;
		}

		return os;
	}

	std::ostream & operator<<(std::ostream & os, TokenOperator const & oper)
	{
		switch(oper)
		{
			case TokenOperator::NONE :               os << "NONE";               break;
			case TokenOperator::DOUBLE_RIGHT_ARROW : os << "DOUBLE_RIGHT_ARROW"; break;
			case TokenOperator::RIGHT_ARROW :        os << "RIGHT_ARROW";        break;
			case TokenOperator::LEFT_ARROW :         os << "LEFT_ARROW";         break;
			case TokenOperator::DOUBLE_DOLLAR :      os << "DOUBLE_DOLLAR";      break;
			case TokenOperator::LEFT_PAREN :         os << "LEFT_PAREN";         break;
			case TokenOperator::RIGHT_PAREN :        os << "RIGHT_PAREN";        break;
			case TokenOperator::LEFT_CURLY :         os << "LEFT_CURLY";         break;
			case TokenOperator::RIGHT_CURLY :        os << "RIGHT_CURLY";        break;
			case TokenOperator::LEFT_SQUARE :        os << "LEFT_SQUARE";        break;
			case TokenOperator::RIGHT_SQUARE :       os << "RIGHT_SQUARE";       break;
			case TokenOperator::LEFT_ANGLE :         os << "LEFT_ANGLE";         break;
			case TokenOperator::RIGHT_ANGLE :        os << "RIGHT_ANGLE";        break;
			case TokenOperator::AMPERSAT :           os << "AMPERSAT";           break;
			case TokenOperator::STAR :               os << "STAR";               break;
			case TokenOperator::PLUS :               os << "PLUS";               break;
			case TokenOperator::BANG :               os << "BANG";               break;
			case TokenOperator::COMMA :              os << "COMMA";              break;
			case TokenOperator::QUESTION_MARK :      os << "QUESTION_MARK";      break;
			case TokenOperator::COLON :              os << "COLON";              break;
			case TokenOperator::MINUS :              os << "MINUS";              break;
			case TokenOperator::EQUALS :             os << "EQUALS";             break;
			case TokenOperator::SEMICOLON :          os << "SEMICOLON";          break;
			case TokenOperator::FORWARD_SLASH :      os << "FORWARD_SLASH";      break;
			case TokenOperator::DOLLAR :             os << "DOLLAR";             break;
			case TokenOperator::TILDE :              os << "TILDE";              break;
			case TokenOperator::BAR :                os << "BAR";                break;
		}

		return os;
	}

	std::ostream & operator<<(std::ostream & os, Token const & token)
	{
		os << token.type << "(";
		
		if (token.type == TokenType::IDENTIFIER)
		{
			auto dtoken = static_cast<const IdentifierToken &>(token);
			os << dtoken.string;
		}
		else if (token.type == TokenType::INTEGER)
		{
			auto dtoken = static_cast<const IntegerToken &>(token);
			os << dtoken.value;
		}
		else if (token.type == TokenType::STRING)
		{
			auto dtoken = static_cast<const StringToken &>(token);
			os << "\"" << dtoken.string << "\"";
		}
		else if (token.type == TokenType::OPERATOR)
		{
			auto dtoken = static_cast<const OperatorToken &>(token);
			os << dtoken.oper;
		}

		return os << ")";
	}
}
