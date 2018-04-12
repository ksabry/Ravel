#include "catch.hpp"
#include <sstream>
#include "SubML/Tokenizer.hpp"

using namespace Ravel::SubML;

// TEST_CASE( "Tokenizes identifiers", "[tokenize]" )
// {
// 	std::stringstream input_stream;
// 	input_stream.str("identifiers with_underscore numbers1234 _underscore_start");

// 	Tokenizer tokenizer;
// 	std::vector<Token *> output;
// 	tokenizer.Tokenize(&input_stream, &output);

// 	REQUIRE( output.size() == 4 );
// }

TEST_CASE( "SimpleTest", "[simple]")
{
	REQUIRE(true);
}