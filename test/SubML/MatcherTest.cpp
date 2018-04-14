#include "catch.hpp"
#include <sstream>
#include "SubML/Compiler.hpp"
#include "Semantic/Expression.hpp"

using namespace Ravel;
using namespace Ravel::Semantic;
using namespace Ravel::SubML;

TEST_CASE( "Matcher", "[matcher]" )
{
	Compiler compiler;
	Program program;
	Error * err = compiler.Compile("example/subml/test.subml", &program); // Probably should have this in plaintext

	REQUIRE(err == nullptr);

	Expression expr ("add",
		new Expression("imm", 0u),
		new Expression("imm", 1u),
		new Expression("imm", 0u),
		new Expression("mul",
			new Expression("imm", 2u),
			new Expression("imm", 3u)
		),
		new Expression("imm", 4u),
		new Expression("imm", 0u),
		new Expression("imm", 5u)
	);

	// [add, orb, xorb, orl]@t:{imm:(0)+, *@r} => @t:(@r)
	auto test_matcher = program.substitutions[3]->GetMatcher();
	
	uint32_t capture_count = program.substitutions[3]->CaptureCount();
	REQUIRE(capture_count == 3);
	
	uint64_t captures[3] = {};
	Expression * exprs[1] = {&expr};
	test_matcher->Begin(captures, capture_count, exprs, 1);
	uint64_t * result = test_matcher->Next();

	std::cout << result << std::endl;
}
