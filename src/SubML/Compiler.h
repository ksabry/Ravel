#pragma once

#include <iostream>
#include <fstream>

#include "Error.h"
#include "Program.h"
#include "Tokenizer.h"
#include "Substitution.h"
#include "QuantifiedExpressionMatcher.h"
#include "OperatorMatcher.h"

namespace Ravel::SubML
{
	class Compiler
	{
	public:
		Compiler();
		~Compiler();

		Error * Compile(char const * const input_filename, Program * result);
		Error * Compile(std::istream * input, Program * result);

	private:
		char const * input_filename;
		char * line_info;

		Tokenizer* tokenizer;
		std::vector<Token> tokens;
		uint32_t token_idx;
		Program * program;

		char * LineInfo();

		Error * CompileTokens(uint32_t token_idx = 0);

		Error * ParseSubstitution(Substitution * & output);
		Error * ParseQuantifiedExpressionMatcher(QuantifiedExpressionMatcher * & output);
		Error * ParseExpressionMatcher(ExpressionMatcher * & output);
		Error * TryParseOperatorMatcher(OperatorMatcher * & output);
		Error * TryParseOperatorMatcherAlternation(OperatorValueMatcher * & output);
		Error * TryParseOperationMatcherSingle(OperatorValueMatcher * & output);
		Error * TryParseArgsMatcher(ArgsMatcher * & output);
		Error * TryParseQuantifier(Quantifier & output);

		Error * TryParseCaptureMatcher(CaptureMatcher * & output);
		
		Error * ParseMultiExpressionPopulator(Populator * & output);
	};
}
