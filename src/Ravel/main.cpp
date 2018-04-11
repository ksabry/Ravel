#include <iostream>

#include "Tokenizer.hpp"
#include "Assert.hpp"
#include "Compiler.hpp"
#include "Program.hpp"

using namespace Ravel;

int main(int argc, const char * argv[])
{
	SubML::Compiler compiler;
	SubML::Program program;

	auto err = compiler.Compile("bin/test.subml", &program);
	if (err)
	{
		std::cout << *err << std::endl;
		delete err;
	}
	else
	{
		program.PPrint(std::cout);
		std::cout << std::endl;
	}

	std::cout << "Success";
	return 0;
}
