#include "Compiler.h"

namespace Ravel
{
    namespace SubML
    {
        Compiler::Compiler()
        {
            tokenizer = new Tokenizer();
        }
        Compiler::~Compiler()
        {
            delete tokenizer;
        }

        Error * Compiler::Compile(char const * const input_filename, Program * result)
        {
            Error * err = tokenizer->Tokenize(input_filename, &tokens);
            if (err) return err;

            return CompileTokens(result);
        }
        Error * Compiler::Compile(std::istream * input, Program * result)
        {
            Error * err = tokenizer->Tokenize(input, &tokens);
            if (err) return err;

            return CompileTokens(result);
        }

        Error * Compiler::CompileTokens(Program * result)
        {

        }
    }
}
