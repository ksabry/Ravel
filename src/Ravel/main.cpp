#include <iostream>

#include "Tokenizer.h"
#include "Assert.h"

using namespace Ravel;

int main(int argc, const char * argv[])
{
	SubML::Tokenizer * tokenizer = new SubML::Tokenizer();

	SubML::TokenList output;
	auto err = tokenizer->Tokenize("bin/test.subml", &output);
	if (err)
	{
		std::cout << *err << std::endl;
		delete err;
	}
	else
	{
		std::cout << "Successful" << std::endl;
	}

	return 0;
}
