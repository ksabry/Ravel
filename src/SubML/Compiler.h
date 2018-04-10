#pragma once

#include <iostream>
#include <fstream>

#include "Error.h"
#include "Program.h"
#include "Tokenizer.h"
#include "Substitution.h"
#include "Matcher.h"
#include "Populator.h"
#include "QuantifiedExpressionMatcher.h"

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

		Tokenizer * tokenizer;
		std::vector<Token *> tokens;
		uint32_t token_idx;
		Program * program;
		Substitution * substitution;

		char * LineInfo();
		inline Token & Tok()
		{
			return *tokens[token_idx];
		}

		Error * CompileTokens(uint32_t token_idx = 0);

		Error * ParseSubstitution(Substitution * & output);
		Error * ParseQuantifiedExpressionMatcher(QuantifiedExpressionMatcher * & output);
		Error * ParseExpressionMatcher(Matcher<Expression *> * & output);
		Error * TryParseOperatorMatcher(Matcher<ExpressionOperator> * & output);
		Error * TryParseOperatorMatcherAlternation(Matcher<ExpressionOperator> * & output);
		Error * TryParseOperatorMatcherSingle(Matcher<ExpressionOperator> * & output);
		Error * TryParseArgsMatcher(Matcher<Expression *> * & output);
		Error * TryParseQuantifier(Quantifier & output);
		Error * ParseMultiExpressionPopulator(Populator<Expression ** &, uint32_t &> * & output);
		Error * ParseExpressionPopulator(Populator<Expression * &> * & output);
		Error * TryParseOperatorPopulator(Populator<ExpressionOperator &> * & output);
		Error * TryParseOperationPopulatorSingle(Populator<ExpressionOperator &> * & output);
		Error * TryParseArgsPopulator(Populator<Expression * &> * & output);
		Error * TryParseCapture(uint32_t & output);
	};
}
