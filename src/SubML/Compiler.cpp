#include "Compiler.h"
#include "Assert.h"

namespace Ravel::SubML
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
		program = result;

		Error * err = tokenizer->Tokenize(input_filename, &tokens);
		if (err) return err;

		return CompileTokens();
	}
	Error * Compiler::Compile(std::istream * input, Program * result)
	{
		program = result;

		Error * err = tokenizer->Tokenize(input, &tokens);
		if (err) return err;

		return CompileTokens();
	}

	Error * Compiler::CompileTokens(uint32_t initial_token_idx)
	{
		token_idx = initial_token_idx;
		while (token_idx < tokens.Count())
		{
			Substitution * output = nullptr;
			Error * err = ParseSubstitution(output);
			if (err) return err;

			program->substitutions.Add(output);
		}

		return nullptr;
	}

	Error * Compiler::ParseSubstitution(Substitution * & output)
	{
		QuantifiedExpressionMatcher * matcher = nullptr;
		Error * err = ParseQuantifiedExpressionMatcher(matcher);
		if (err) return err;

		assert(matcher->GetQuantifier().low == 1 && matcher->GetQuantifier().high == 1);
		assert(tokens[token_idx]->type == TokenType::OPERATOR);
		assert(IsOperatorToken(*tokens[token_idx], TokenOperator::DOUBLE_RIGHT_ARROW));

		token_idx++;
		Populator * populator = nullptr;
		err = ParseMultiExpressionPopulator(populator);
		if (err) return err;

		output = new Substitution(matcher, populator);
		return nullptr;
	}

	Error * Compiler::ParseQuantifiedExpressionMatcher(QuantifiedExpressionMatcher * & output)
	{
		ExpressionMatcher * expression_matcher = nullptr;
		Error * err = ParseExpressionMatcher(expression_matcher);
		if (err) return err;
		
		Quantifier quantifier;
		err = TryParseQuantifier(quantifier);
		if (err) return err;

		CaptureMatcher * capture_matcher = nullptr;
		err = TryParseCaptureMatcher(capture_matcher);
		if (err) return err;
		
		output = new QuantifiedExpressionMatcher(expression_matcher, quantifier, capture_matcher);
		return nullptr;
	}

	Error * Compiler::ParseExpressionMatcher(ExpressionMatcher * & output)
	{
		OperatorMatcher * oper_matcher = nullptr;
		Error * err = TryParseOperatorMatcher(oper_matcher);
		if (err) return err;

		ArgsMatcher * args_matcher = nullptr;
		err = TryParseArgsMatcher(args_matcher);
		if (err) return err;

		output = new ExpressionMatcher(oper_matcher, args_matcher);
		return nullptr;
	}

	Error * Compiler::TryParseOperatorMatcher(OperatorMatcher * & output)
	{
		uint32_t old_idx = token_idx;

		OperatorMatcher * result = nullptr;
		Error * err = TryParseOperatorMatcherValue(result);
		if (err) return err;

		if (token_idx > tokens.Count() || !IsOperatorToken(*tokens[token_idx], TokenOperator::COLON))
		{
			token_idx = old_idx;
			return nullptr;
		}

		token_idx++;
		output = result;
		return nullptr;
	}

	Error * Compiler::TryParseOperatorMatcherValue(OperatorMatcher * & output)
	{
		
	}
}
