#include "catch.hpp"
#include <sstream>
#include "SubML/Tokenizer.hpp"

using namespace Ravel::SubML;

TEST_CASE( "Tokenizes identifiers", "[tokenize]" )
{
	std::stringstream input_stream;
	input_stream.str("identifiers  with_underscore    numbers1234 _underscore_start");
	std::vector<char const *> to_match {"identifiers", "with_underscore", "numbers1234", "_underscore_start"};

	Tokenizer tokenizer;
	std::vector<Token *> output;
	tokenizer.Tokenize(&input_stream, &output);

	REQUIRE( output.size() == to_match.size() );
	for (uint32_t i = 0; i < output.size(); i++)
	{
		auto tok = output[i];
		REQUIRE(tok->type == TokenType::IDENTIFIER);
		IdentifierToken & iden_tok = static_cast<IdentifierToken &>(*tok);
		REQUIRE(strcmp(iden_tok.string, to_match[i]) == 0);
	}
}

TEST_CASE( "Tokenizes integers", "[tokenize]" )
{
	std::stringstream input_stream;
	input_stream.str("1234 01234 0x12AB 0o456 0b1010");
	std::vector<uint32_t> to_match {1234, 1234, 0x12AB, 0456, 0b1010};

	Tokenizer tokenizer;
	std::vector<Token *> output;
	tokenizer.Tokenize(&input_stream, &output);

	REQUIRE( output.size() == to_match.size() );
	for (uint32_t i = 0; i < output.size(); i++)
	{
		auto tok = output[i];
		REQUIRE(tok->type == TokenType::INTEGER);
		IntegerToken & int_tok = static_cast<IntegerToken &>(*tok);
		REQUIRE(int_tok.value == to_match[i]);
	}
}

TEST_CASE( "Tokenizes strings", "[tokenize]" )
{
	std::stringstream input_stream;
	input_stream.str("\"abc\" 'abc' \"escaped quote \\\"\" '\\n \\b \\xAB \\012'");
	std::vector<char const *> to_match {"abc", "abc", "escaped quote \"", "\n \b \xAB \012"};

	Tokenizer tokenizer;
	std::vector<Token *> output;
	tokenizer.Tokenize(&input_stream, &output);

	REQUIRE( output.size() == to_match.size() );
	for (uint32_t i = 0; i < output.size(); i++)
	{
		auto tok = output[i];
		REQUIRE(tok->type == TokenType::STRING);
		StringToken & str_tok = static_cast<StringToken &>(*tok);
		REQUIRE(strcmp(str_tok.string, to_match[i]) == 0);
	}
}