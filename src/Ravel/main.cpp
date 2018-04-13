#include <iostream>

#include "Util/Assert.hpp"
#include "SubML/Tokenizer.hpp"
#include "SubML/Compiler.hpp"
#include "SubML/Program.hpp"
#include "Input/ElfReader.hpp"

using namespace Ravel;

int main(int argc, const char * argv[])
{
	// Input::ElfReader reader;
	// Error *err = reader.Read("example/exec/main");
	// if (err)
	// {
	// 	std::cout << *err << std::endl;
	// 	delete err;
	// }

	SubML::Compiler compiler;
	SubML::Program program;

	auto err = compiler.Compile("example/subml/test.subml", &program);
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
