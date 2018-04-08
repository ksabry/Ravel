#include "Compiler.h"

#include <vector>

#include "Assert.h"
#include "ArrCpy.h"
#include "OrderedArgsMatcher.h"
#include "UnorderedArgsMatcher.h"
#include "ImmediateMatcher.h"
#include "ExpressionMatcher.h"
#include "OperatorMatcher.h"
#include "MultiExpressionPopulator.h"
#include "CapturePopulator.h"
#include "OperatorValuePopulator.h"
#include "ArgsPopulator.h"

//TODO: ensure things are cleaned up even when errors are thrown

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
		Populator<Expression ** &, uint32_t &> * populator;
		err = ParseMultiExpressionPopulator(populator);
		if (err) return err;

		output = new Substitution(matcher, populator);
		return nullptr;
	}

	Error * Compiler::ParseQuantifiedExpressionMatcher(QuantifiedExpressionMatcher * & output)
	{
		Matcher<Expression *> * expression_matcher;
		Error * err = ParseExpressionMatcher(expression_matcher);
		if (err) return err;
		
		Quantifier quantifier;
		err = TryParseQuantifier(quantifier);
		if (err) return err;

		uint32_t capture_idx;
		err = TryParseCapture(capture_idx);
		if (err) return err;
		CaptureMatcher<Expression *> * capture_matcher = new CaptureMatcher<Expression *>(capture_idx);
		
		output = new QuantifiedExpressionMatcher(expression_matcher, quantifier, capture_matcher);
		return nullptr;
	}

	Error * Compiler::ParseExpressionMatcher(Matcher<Expression *> * & output)
	{
		Matcher<ExpressionOperator> * oper_matcher = nullptr;
		Error * err = TryParseOperatorMatcher(oper_matcher);
		if (err) return err;

		Matcher<Expression *> * args_matcher = nullptr;
		err = TryParseArgsMatcher(args_matcher);
		if (err) return err;

		output = new ExpressionMatcher(oper_matcher, args_matcher);
		return nullptr;
	}

	Error * Compiler::TryParseOperatorMatcher(Matcher<ExpressionOperator> * & output)
	{
		uint32_t old_idx = token_idx;

		Matcher<ExpressionOperator> * value_matcher = nullptr;
		Error * err = TryParseOperatorMatcherAlternation(value_matcher);
		if (err) return err;

		if (!value_matcher)
		{
			err = TryParseOperatorMatcherSingle(value_matcher);
			if (err) return err;
		}

		uint32_t capture_idx;
		err = TryParseCapture(capture_idx);
		if (err) return err;
		CaptureMatcher<ExpressionOperator> * capture_matcher = new CaptureMatcher<ExpressionOperator>(capture_idx);

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

	Error * Compiler::TryParseOperatorMatcherAlternation(Matcher<ExpressionOperator> * & output)
	{
		if (token_idx >= tokens.size())
		{
			return nullptr;
		}

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

	Error * Compiler::TryParseOperatorMatcherSingle(Matcher<ExpressionOperator> * & output)
	{
		if (token_idx >= tokens.size())
		{
			return nullptr;
		}

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

	Error * Compiler::TryParseArgsMatcher(Matcher<Expression *> * & output)
	{
		if (token_idx >= tokens.size())
		{
			return nullptr;
		}

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

	Error * Compiler::TryParseQuantifier(Quantifier & output)
	{
		if (token_idx >= tokens.size())
		{
			return nullptr;
		}

		if (IsOperatorToken(tokens[token_idx], TokenOperator::STAR))
		{
			token_idx++;
			output = Quantifier{0, Quantifier::infinity};
			return nullptr;
		}

		if (IsOperatorToken(tokens[token_idx], TokenOperator::PLUS))
		{
			token_idx++;
			output = Quantifier{1, Quantifier::infinity};
			return nullptr;
		}

		if (IsOperatorToken(tokens[token_idx], TokenOperator::QUESTION_MARK))
		{
			token_idx++;
			output = Quantifier{0, 1};
			return nullptr;
		}

		if (IsOperatorToken(tokens[token_idx], TokenOperator::LEFT_ANGLE))
		{
			token_idx++;
			if (token_idx >= tokens.size())
			{
				return new Error(ERR_PARSE, "Unexpected EOF while parsing quantifier at %s in file %s", LineInfo(), input_filename);
			}

			int32_t low = -1, high = -1;
			if (IsOperatorToken(tokens[token_idx], TokenOperator::COMMA))
			{
				low = 0;
				token_idx++;
			}
			else if (tokens[token_idx].type == TokenType::INTEGER)
			{
				low = static_cast<IntegerToken &>(tokens[token_idx]).value;
				token_idx++;
				if (token_idx >= tokens.size())
				{
					return new Error(ERR_PARSE, "Unexpected EOF while parsing quantifier at %s in file %s", LineInfo(), input_filename);
				}
				
				if (IsOperatorToken(tokens[token_idx], TokenOperator::RIGHT_ANGLE))
				{
					high = low;
					token_idx++;
				}
				else
				{
					if (!IsOperatorToken(tokens[token_idx], TokenOperator::COMMA))
					{
						return new Error(ERR_PARSE, "Expected comma or right angle bracket at %s in file %s", LineInfo(), input_filename);
					}
					token_idx++;
				}
			}
			else
			{
				return new Error(ERR_PARSE, "Expected integer or comma at %s in file %s", LineInfo(), input_filename);
			}

			if (high == -1)
			{
				if (token_idx >= tokens.size())
				{
					return new Error(ERR_PARSE, "Unexpected EOF while parsing quantifier at %s in file %s", LineInfo(), input_filename);
				}

				if (IsOperatorToken(tokens[token_idx], TokenOperator::RIGHT_ANGLE))
				{
					high = Quantifier::infinity;
					token_idx++;
				}
				else if (tokens[token_idx].type == TokenType::INTEGER)
				{
					high = static_cast<IntegerToken &>(tokens[token_idx]).value;
					token_idx++;
					if (token_idx >= tokens.size())
					{
						return new Error(ERR_PARSE, "Unexpected EOF while parsing quantifier at %s in file %s", LineInfo(), input_filename);
					}
					if (!IsOperatorToken(tokens[token_idx], TokenOperator::RIGHT_ANGLE))
					{
						return new Error(ERR_PARSE, "Expected right angle bracket at %s in file %s", LineInfo(), input_filename);
					}
					token_idx++;
				}
				else
				{
					return new Error(ERR_PARSE, "Expected integer or right angle bracket at %s in file %s", LineInfo(), input_filename);
				}
			}

			output = Quantifier{static_cast<uint32_t>(low), static_cast<uint32_t>(high)};
			return nullptr;
		}
	}

	Error * Compiler::ParseMultiExpressionPopulator(Populator<Expression ** &, uint32_t &> * & output)
	{
		if (token_idx >= tokens.size())
		{
			return new Error(ERR_PARSE, "Unexpected EOF while expecting populator at %s in file %s", LineInfo(), input_filename);
		}

		if (IsOperatorToken(tokens[token_idx], TokenOperator::TILDE))
		{
			token_idx++;
			output = new MultiExpressionPopulator(nullptr, 0);
			return nullptr;
		}

		std::vector<Populator<Expression * &> *> populators;

		Populator<Expression * &> * expr_populator;
		Error * err = ParseExpressionPopulator(expr_populator);
		if (err) return err;
		populators.push_back(expr_populator);

		while (token_idx < tokens.size() && IsOperatorToken(tokens[token_idx], TokenOperator::COMMA))
		{
			token_idx++;
			err = ParseExpressionPopulator(expr_populator);
			if (err) return err;
			populators.push_back(expr_populator);
		}
		
		auto arr = new Populator<Expression * &> * [populators.size()];
		ArrCpy<>(arr, &populators[0], populators.size());
		output = new MultiExpressionPopulator(arr, populators.size());
		return nullptr;
	}

	Error * Compiler::ParseExpressionPopulator(Populator<Expression * &> * & output)
	{
		Populator<ExpressionOperator &> * oper_populator = nullptr;
		Error * err = TryParseOperatorPopulator(oper_populator);
		if (err) return err;

		Populator<Expression * &> * args_populator = nullptr;
		err = TryParseArgsPopulator(args_populator);
		if (err) return err;

		if (!oper_populator && !args_populator)
		{
			uint32_t capture_idx = 0;
			err = TryParseCapture(capture_idx);
			if (err) return err;

			if (!capture_idx)
			{
				return new Error(ERR_PARSE, "Expected populator at %s in file %s", LineInfo(), input_filename);
			}
			auto capture_populator = new CapturePopulator<Expression * &>(capture_idx);
			output = capture_populator;
			return nullptr;
		}

		output = new ExpressionPopulator(oper_populator, args_populator);
	}

	Error * Compiler::TryParseOperatorPopulator(Populator<ExpressionOperator &> * & output)
	{
		uint32_t old_idx = token_idx;
		
		Populator<ExpressionOperator &> * value_populator = nullptr;
		Error * err = TryParseOperationPopulatorSingle(value_populator);
		if (err) return err;

		if (!value_populator)
		{
			uint32_t capture_idx = 0;
			err = TryParseCapture(capture_idx);
			if (err) return err;

			if (capture_idx)
			{
				value_populator = new CapturePopulator<ExpressionOperator &>(capture_idx);
			}
		}

		if (!value_populator || token_idx >= tokens.size() || !IsOperatorToken(tokens[token_idx], TokenOperator::COLON))
		{
			if (value_populator) delete value_populator;
			token_idx = old_idx;
			return nullptr;
		}

		token_idx++;
		output = value_populator;
		return nullptr;
	}

	Error * Compiler::TryParseOperationPopulatorSingle(Populator<ExpressionOperator &> * & output)
	{
		if (token_idx >= tokens.size() || tokens[token_idx].type != TokenType::IDENTIFIER)
		{
			return nullptr;
		}

		IdentifierToken & iden_tok = static_cast<IdentifierToken &>(tokens[token_idx]);
		ExpressionOperator oper = OperatorFromString(iden_tok.string);
		output = new OperatorValuePopulator(oper);
		return nullptr;
	}

	Error * Compiler::TryParseArgsPopulator(Populator<Expression * &> * & output)
	{
		if (token_idx >= tokens.size() || !IsOperatorToken(tokens[token_idx], TokenOperator::LEFT_PAREN))
		{
			return nullptr;
		}
		token_idx++;

		if (token_idx >= tokens.size())
		{
			return new Error(ERR_PARSE, "Unexpected EOF while parsing populator arguments at %s in file %s", LineInfo(), input_filename);
		}

		if (IsOperatorToken(tokens[token_idx], TokenOperator::RIGHT_PAREN))
		{
			token_idx++;
			output = new ArgsPopulator(nullptr, 0);
			return nullptr;
		}

		std::vector<Populator<Expression * &> *> populators;
		while (true)
		{
			Populator<Expression * &> * populator;
			Error * err = ParseExpressionPopulator(populator);
			if (err) return err;
			populators.push_back(populator);

			if (token_idx >= tokens.size())
			{
				return new Error(ERR_PARSE, "Unexpected EOF while parsing populator arguments at %s in file %s", LineInfo(), input_filename);
			}
			
			if (IsOperatorToken(tokens[token_idx], TokenOperator::RIGHT_PAREN))
			{
				token_idx++;
				break;
			}

			if (!IsOperatorToken(tokens[token_idx], TokenOperator::COMMA))
			{
				return new Error(ERR_PARSE, "Expected comma or right paren at %s in file %s", LineInfo(), input_filename);
			}

			token_idx++;
		}

		auto ** arr = new Populator<Expression * &> * [populators.size()];
		ArrCpy(arr, &populators[0], populators.size());
		output = new ArgsPopulator(arr, populators.size());
		return nullptr;
	}

	
	Error * Compiler::TryParseCapture(uint32_t & output)
	{
		if (token_idx >= tokens.size() || !IsOperatorToken(tokens[token_idx], TokenOperator::AMPERSAT))
		{
			return nullptr;
		}
		token_idx++;

		if (token_idx >= tokens.size())
		{
			return new Error(ERR_PARSE, "Unexpected EOF after capture token at %s in file %s", LineInfo(), input_filename);
		}
		if (tokens[token_idx].type != TokenType::IDENTIFIER)
		{
			return new Error(ERR_PARSE, "Expected identifier after captuer token at %s in file %s", LineInfo(), input_filename);
		}

		IdentifierToken & iden_tok = static_cast<IdentifierToken &>(tokens[token_idx]);
		output = OperatorFromString(iden_tok.string);
		return nullptr;
	}
}
