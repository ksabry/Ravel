#include "Compiler.h"

#include <vector>

#include "Assert.h"
#include "ArrCpy.h"
#include "OrderedArgsMatcher.h"
#include "UnorderedArgsMatcher.h"
#include "ImmediateMatcher.h"

namespace Ravel::SubML
{
	Compiler::Compiler()
		: input_filename("<INPUT>"), line_info(nullptr)
	{
		tokenizer = new Tokenizer();
	}
	Compiler::~Compiler()
	{
		delete tokenizer;
		if (line_info) delete[] line_info;
	}

	char * Compiler::LineInfo()
	{
		if (line_info) delete[] line_info;
		line_info = Formatted("%d : %d", tokens[token_idx].line, tokens[token_idx].column);
		return line_info;
	}

	Error * Compiler::Compile(char const * const input_filename, Program * result)
	{
		this->input_filename = input_filename;
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
		while (token_idx < tokens.size())
		{
			Substitution * output;
			Error * err = ParseSubstitution(output);
			if (err) return err;

			program->substitutions.push_back(output);
		}

		return nullptr;
	}

	Error * Compiler::ParseSubstitution(Substitution * & output)
	{
		uint32_t old_idx;

		QuantifiedExpressionMatcher * matcher;
		Error * err = ParseQuantifiedExpressionMatcher(matcher);
		if (err) return err;

		if (matcher->GetQuantifier().low != 1 || matcher->GetQuantifier().high != 1)
		{
			return new Error(ERR_PARSE, "Left side of substitution must not be quantified at %s in file %s", LineInfo(), input_filename);
		}
		if (!IsOperatorToken(tokens[token_idx], TokenOperator::DOUBLE_RIGHT_ARROW))
		{
			return new Error(ERR_PARSE, "Expected substitute token `=>` at %s in file %s", LineInfo(), input_filename);
		}

		token_idx++;
		Populator * populator;
		err = ParseMultiExpressionPopulator(populator);
		if (err) return err;

		output = new Substitution(matcher, populator);
		return nullptr;
	}

	Error * Compiler::ParseQuantifiedExpressionMatcher(QuantifiedExpressionMatcher * & output)
	{
		ExpressionMatcher * expression_matcher;
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

		OperatorValueMatcher * value_matcher = nullptr;
		Error * err = TryParseOperatorMatcherAlternation(value_matcher);
		if (err) return err;

		if (!value_matcher)
		{
			err = TryParseOperationMatcherSingle(value_matcher);
			if (err) return err;
		}

		CaptureMatcher * capture_matcher = nullptr;
		err = TryParseCaptureMatcher(capture_matcher);
		if (err) return err;

		OperatorMatcher * result = new OperatorMatcher(value_matcher, capture_matcher);
	
		if (token_idx >= tokens.size() || !IsOperatorToken(tokens[token_idx], TokenOperator::COLON))
		{
			delete result;
			token_idx = old_idx;
			return nullptr;
		}

		token_idx++;
		output = result;
		return nullptr;
	}

	Error * Compiler::TryParseOperatorMatcherAlternation(OperatorValueMatcher * & output)
	{
		uint32_t old_idx = token_idx;

		bool neg = false;
		if (IsOperatorToken(tokens[token_idx], TokenOperator::BANG))
		{
			neg = true;
			token_idx++;
		}
		
		if (token_idx >= tokens.size() || !IsOperatorToken(tokens[token_idx], TokenOperator::LEFT_SQUARE))
		{
			token_idx = old_idx;
			return nullptr;
		}
		token_idx++;

		if (token_idx < tokens.size() && IsOperatorToken(tokens[token_idx], TokenOperator::RIGHT_SQUARE))
		{
			token_idx++;
			output = new OperatorValueMatcher(nullptr, 0, neg);
			return nullptr;
		}

		std::vector<ExpressionOperator> opers;
		while (true)
		{
			if (token_idx >= tokens.size())
			{
				return new Error(ERR_PARSE, "Unexpected EOF while parsing operator alternation in file %s", input_filename);
			}
			else if (tokens[token_idx].type != TokenType::IDENTIFIER)
			{
				return new Error(ERR_PARSE, "Invalid operator in alternation at %s in file %s", LineInfo(), input_filename);
			}

			IdentifierToken & iden_tok = static_cast<IdentifierToken &>(tokens[token_idx]);
			opers.push_back(OperatorFromString(iden_tok.string));
			token_idx++;
			
			if (token_idx >= tokens.size())
			{
				return new Error(ERR_PARSE, "Unexpected EOF while parsing operator alternation in file %s", input_filename);
			}
			
			if (IsOperatorToken(tokens[token_idx], TokenOperator::RIGHT_SQUARE))
			{
				token_idx++;
				break;
			}
			if (!IsOperatorToken(tokens[token_idx], TokenOperator::COMMA))
			{
				return new Error(ERR_PARSE, "Expected comma or right square bracket at %s in file %s", LineInfo(), input_filename);
			}

			token_idx++;
		}
		
		ExpressionOperator * arr = new ExpressionOperator[opers.size()];
		ArrCpy(arr, &opers[0], opers.size());
		output = new OperatorValueMatcher(arr, opers.size(), neg);
		return nullptr;
	}

	Error * Compiler::TryParseOperationMatcherSingle(OperatorValueMatcher * & output)
	{
		uint32_t old_idx = token_idx;
		
		bool neg = false;
		if (IsOperatorToken(tokens[token_idx], TokenOperator::BANG))
		{
			neg = true;
			token_idx++;
		}

		if (token_idx >= tokens.size() || tokens[token_idx].type != TokenType::IDENTIFIER)
		{
			token_idx = old_idx;
			return nullptr;
		}

		ExpressionOperator * arr = new ExpressionOperator[1];
		IdentifierToken & iden_tok = static_cast<IdentifierToken &>(tokens[token_idx]);
		arr[0] = OperatorFromString(iden_tok.string);
		output = new OperatorValueMatcher(arr, 1, neg);
		return nullptr;
	}

	Error * Compiler::TryParseArgsMatcher(ArgsMatcher * & output)
	{
		bool ordered;
		TokenOperator delim;
		if (IsOperatorToken(tokens[token_idx], TokenOperator::LEFT_PAREN))
		{
			ordered = true;
			delim = TokenOperator::RIGHT_PAREN;
		}
		else if (IsOperatorToken(tokens[token_idx], TokenOperator::LEFT_CURLY))
		{
			ordered = false;
			delim = TokenOperator::RIGHT_CURLY;
		}
		else
		{
			return nullptr;
		}
		
		token_idx++;
		if (token_idx >= tokens.size())
		{
			return new Error(ERR_PARSE, "Unexpected EOF while parsing expression arguments at %s in file %s", LineInfo(), input_filename);
		}

		if (IsOperatorToken(tokens[token_idx], delim))
		{
			token_idx++;
			output = new OrderedArgsMatcher(nullptr, 0);
			return nullptr;
		}

		// TODO: floats? signed?
		if (tokens[token_idx].type == TokenType::INTEGER)
		{
			token_idx++;
			if (token_idx >= tokens.size())
			{
				return new Error(ERR_PARSE, "Unexpected EOF while parsing expression arguments at %s in file %s", LineInfo(), input_filename);
			}
			if (!IsOperatorToken(tokens[token_idx], delim))
			{
				return new Error(ERR_PARSE, "May not have immediate values among multiple expression arguments at %s in file %s", LineInfo(), input_filename);
			}
			
			token_idx++;
			IntegerToken & int_tok = static_cast<IntegerToken &>(tokens[token_idx]);
			output = new ImmediateMatcher(DataType::UINT32, int_tok.value);
			return nullptr;
		}

		std::vector<QuantifiedExpressionMatcher *> matchers;
		while (true)
		{
			QuantifiedExpressionMatcher * matcher;
			Error * err = ParseQuantifiedExpressionMatcher(matcher);
			if (err) return err;
			matchers.push_back(matcher);

			if (token_idx >= tokens.size())
			{
				return new Error(ERR_PARSE, "Unexpected EOF while parsing expression arguments at %s in file %s", LineInfo(), input_filename);
			}
			
			if (IsOperatorToken(tokens[token_idx], delim))
			{
				token_idx++;
				break;
			}

			if (!IsOperatorToken(tokens[token_idx], TokenOperator::COMMA))
			{
				return new Error(ERR_PARSE, "Expected comma or %s at %s in file %s", ordered ? "right paren" : "right curly", LineInfo(), input_filename);
			}
			
			token_idx++;
		}

		QuantifiedExpressionMatcher ** arr = new QuantifiedExpressionMatcher * [matchers.size()];
		ArrCpy(arr, &matchers[0], matchers.size());
		if (ordered) output = new OrderedArgsMatcher(arr, matchers.size());
		else output = new UnorderedArgsMatcher(arr, matchers.size());
		return nullptr;
	}

	
}
