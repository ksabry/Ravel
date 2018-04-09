#include <iostream>

#include "Tokenizer.h"
#include "Assert.h"
#include "Compiler.h"
#include "Program.h"

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
		std::cout << std::endl;
	}

	return 0;
}
