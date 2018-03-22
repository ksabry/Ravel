#include "Tokenizer.h"
#include <iostream>

using namespace Ravel;

int main(int argc, const char * argv[])
{
    SubML::Tokenizer * tokenizer = new SubML::Tokenizer();

    std::vector<SubML::Token> output;
    auto err = tokenizer->Tokenize("test.subml", &output);
    if (err)
    {
        std::cout << *err;
        delete err;
    }

    return 0;
}
    