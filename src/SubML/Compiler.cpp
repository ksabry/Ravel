#include "Compiler.hpp"

#include <vector>

#include "OrderedArgsMatcher.hpp"
#include "UnorderedArgsMatcher.hpp"
#include "ImmediateMatcher.hpp"
#include "ExpressionMatcher.hpp"
#include "OperatorMatcher.hpp"
#include "CaptureMatcher.hpp"
#include "PassMatcher.hpp"
#include "MultiExpressionPopulator.hpp"
#include "CapturePopulator.hpp"
#include "OperatorValuePopulator.hpp"
#include "ArgsPopulator.hpp"
#include "ImmediatePopulator.hpp"
#include "Util/Assert.hpp"
#include "Util/ArrCpy.hpp"

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
		for (Token * tok : tokens) delete tok; 
		if (line_info) delete[] line_info;
	}

	char * Compiler::LineInfo()
	{
		if (line_info) delete[] line_info;
		line_info = Formatted("%d : %d", Tok().line, Tok().column);
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
		substitution = output = new Substitution();

		OrderedQuantifiedExpressionMatcher * matcher;
		Error * err = ParseOrderedQuantifiedExpressionMatcher(matcher);
		if (err) return err;

		if (matcher->GetQuantifier().low != 1 || matcher->GetQuantifier().high != 1)
		{
			return new Error(ERR_PARSE, "Left side of substitution must not be quantified at %s in file %s", LineInfo(), input_filename);
		}
		if (!IsOperatorToken(Tok(), TokenOperator::DOUBLE_RIGHT_ARROW))
		{
			return new Error(ERR_PARSE, "Expected substitute token `=>` at %s in file %s", LineInfo(), input_filename);
		}

		token_idx++;
		Populator<Expression ** &, uint32_t &> * populator;
		err = ParseMultiExpressionPopulator(populator);
		if (err) return err;

		output->SetMatcher(matcher);
		output->SetPopulator(populator);
		return nullptr;
	}

	Error * Compiler::ParseQuantifiedExpressionMatcher(
		Matcher<Expression *> * & expression_matcher, 
		Quantifier & quantifier, 
		Matcher<Expression *> * & capture_matcher)
	{
		Error * err = ParseExpressionMatcher(expression_matcher);
		if (err) return err;
		
		err = TryParseQuantifier(quantifier);
		if (err) return err;

		uint32_t capture_idx = 0;
		err = TryParseCapture(capture_idx);
		if (err) return err;
		
		if (capture_idx) capture_matcher = new CaptureMatcher<Expression *>(capture_idx);
		else capture_matcher = new PassMatcher<Expression *>();

		return nullptr;
	}

	Error * Compiler::ParseOrderedQuantifiedExpressionMatcher(OrderedQuantifiedExpressionMatcher * & output)
	{
		Matcher<Expression *> * expression_matcher;
		Quantifier quantifier {1, 1};
		Matcher<Expression *> * capture_matcher;
		Error * err = ParseQuantifiedExpressionMatcher(expression_matcher, quantifier, capture_matcher);
		if (err) return err;

		output = new OrderedQuantifiedExpressionMatcher(expression_matcher, quantifier, capture_matcher);
		return nullptr;
	}

	Error * Compiler::ParseUnorderedQuantifiedExpressionMatcher(UnorderedQuantifiedExpressionMatcher * & output)
	{
		Matcher<Expression *> * expression_matcher;
		Quantifier quantifier {1, 1};
		Matcher<Expression *> * capture_matcher;
		Error * err = ParseQuantifiedExpressionMatcher(expression_matcher, quantifier, capture_matcher);
		if (err) return err;

		output = new UnorderedQuantifiedExpressionMatcher(expression_matcher, quantifier, capture_matcher);
		return nullptr;
	}

	Error * Compiler::ParseExpressionMatcher(Matcher<Expression *> * & output)
	{
		Matcher<ExpressionOperator> * oper_matcher = nullptr;
		Error * err = TryParseOperatorMatcher(oper_matcher);
		if (err) return err;
		if (!oper_matcher) oper_matcher = new PassMatcher<ExpressionOperator>();

		Matcher<Expression *> * args_matcher = nullptr;
		err = TryParseArgsMatcher(args_matcher);
		if (err) return err;
		if (!args_matcher) args_matcher = new PassMatcher<Expression *>();

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
		if (!value_matcher) value_matcher = new PassMatcher<ExpressionOperator>();

		uint32_t capture_idx = 0;
		err = TryParseCapture(capture_idx);
		if (err) return err;

		Matcher<ExpressionOperator> * capture_matcher;
		if (capture_idx) capture_matcher = new CaptureMatcher<ExpressionOperator>(capture_idx);
		else capture_matcher = new PassMatcher<ExpressionOperator>();

		OperatorMatcher * result = new OperatorMatcher(value_matcher, capture_matcher);
	
		if (token_idx >= tokens.size() || !IsOperatorToken(Tok(), TokenOperator::COLON))
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
		if (IsOperatorToken(Tok(), TokenOperator::BANG))
		{
			neg = true;
			token_idx++;
		}
		
		if (token_idx >= tokens.size() || !IsOperatorToken(Tok(), TokenOperator::LEFT_SQUARE))
		{
			token_idx = old_idx;
			return nullptr;
		}
		token_idx++;

		if (token_idx < tokens.size() && IsOperatorToken(Tok(), TokenOperator::RIGHT_SQUARE))
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
			else if (Tok().type != TokenType::IDENTIFIER)
			{
				return new Error(ERR_PARSE, "Invalid operator in alternation at %s in file %s", LineInfo(), input_filename);
			}

			IdentifierToken & iden_tok = static_cast<IdentifierToken &>(Tok());
			opers.push_back(OperatorFromString(iden_tok.string));
			token_idx++;
			
			if (token_idx >= tokens.size())
			{
				return new Error(ERR_PARSE, "Unexpected EOF while parsing operator alternation in file %s", input_filename);
			}
			
			if (IsOperatorToken(Tok(), TokenOperator::RIGHT_SQUARE))
			{
				token_idx++;
				break;
			}
			if (!IsOperatorToken(Tok(), TokenOperator::COMMA))
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
		if (IsOperatorToken(Tok(), TokenOperator::BANG))
		{
			neg = true;
			token_idx++;
		}

		if (token_idx >= tokens.size() || Tok().type != TokenType::IDENTIFIER)
		{
			token_idx = old_idx;
			return nullptr;
		}

		ExpressionOperator * arr = new ExpressionOperator[1];
		IdentifierToken & iden_tok = static_cast<IdentifierToken &>(Tok());
		arr[0] = OperatorFromString(iden_tok.string);
		output = new OperatorValueMatcher(arr, 1, neg);
		token_idx++;
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
		if (IsOperatorToken(Tok(), TokenOperator::LEFT_PAREN))
		{
			ordered = true;
			delim = TokenOperator::RIGHT_PAREN;
		}
		else if (IsOperatorToken(Tok(), TokenOperator::LEFT_CURLY))
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

		if (IsOperatorToken(Tok(), delim))
		{
			token_idx++;
			output = new OrderedArgsMatcher(nullptr, 0);
			return nullptr;
		}

		// TODO: floats? signed?
		if (Tok().type == TokenType::INTEGER)
		{
			IntegerToken & int_tok = static_cast<IntegerToken &>(Tok());
			
			token_idx++;
			if (token_idx >= tokens.size())
			{
				return new Error(ERR_PARSE, "Unexpected EOF while parsing expression arguments at %s in file %s", LineInfo(), input_filename);
			}
			if (!IsOperatorToken(Tok(), delim))
			{
				return new Error(ERR_PARSE, "May not have immediate values among multiple expression arguments at %s in file %s", LineInfo(), input_filename);
			}
			token_idx++;
			output = new ImmediateMatcher(DataType::UINT32, int_tok.value);
			return nullptr;
		}

		std::vector<OrderedQuantifiedExpressionMatcher *> ordered_matchers;
		std::vector<UnorderedQuantifiedExpressionMatcher *> unordered_matchers;
		while (true)
		{
			if (ordered)
			{
				OrderedQuantifiedExpressionMatcher * matcher;
				Error * err = ParseOrderedQuantifiedExpressionMatcher(matcher);
				if (err) return err;
				ordered_matchers.push_back(matcher);
			}
			else
			{
				UnorderedQuantifiedExpressionMatcher * matcher;
				Error * err = ParseUnorderedQuantifiedExpressionMatcher(matcher);
				if (err) return err;
				unordered_matchers.push_back(matcher);
			}

			if (token_idx >= tokens.size())
			{
				return new Error(ERR_PARSE, "Unexpected EOF while parsing expression arguments at %s in file %s", LineInfo(), input_filename);
			}
			
			if (IsOperatorToken(Tok(), delim))
			{
				token_idx++;
				break;
			}

			if (!IsOperatorToken(Tok(), TokenOperator::COMMA))
			{
				return new Error(ERR_PARSE, "Expected comma or %s at %s in file %s", ordered ? "right paren" : "right curly", LineInfo(), input_filename);
			}
			
			token_idx++;
		}

		if (ordered)
		{
			uint32_t size = ordered_matchers.size();
			auto arr = new OrderedQuantifiedExpressionMatcher * [size];
			ArrCpy(arr, &ordered_matchers[0], size);
			output = new OrderedArgsMatcher(arr, size);
		}
		else 
		{
			uint32_t size = unordered_matchers.size();
			auto arr = new UnorderedQuantifiedExpressionMatcher * [size];
			ArrCpy(arr, &unordered_matchers[0], size);
			output = new UnorderedArgsMatcher(arr, size);
		}
		return nullptr;
	}

	Error * Compiler::TryParseQuantifier(Quantifier & output)
	{
		if (token_idx >= tokens.size())
		{
			return nullptr;
		}

		if (IsOperatorToken(Tok(), TokenOperator::STAR))
		{
			token_idx++;
			output = Quantifier{0, Quantifier::infinity};
		}

		else if (IsOperatorToken(Tok(), TokenOperator::PLUS))
		{
			token_idx++;
			output = Quantifier{1, Quantifier::infinity};
		}

		else if (IsOperatorToken(Tok(), TokenOperator::QUESTION_MARK))
		{
			token_idx++;
			output = Quantifier{0, 1};
		}

		else if (IsOperatorToken(Tok(), TokenOperator::LEFT_ANGLE))
		{
			token_idx++;
			if (token_idx >= tokens.size())
			{
				return new Error(ERR_PARSE, "Unexpected EOF while parsing quantifier at %s in file %s", LineInfo(), input_filename);
			}

			int32_t low = -1, high = -1;
			if (IsOperatorToken(Tok(), TokenOperator::COMMA))
			{
				low = 0;
				token_idx++;
			}
			else if (Tok().type == TokenType::INTEGER)
			{
				low = static_cast<IntegerToken &>(Tok()).value;
				token_idx++;
				if (token_idx >= tokens.size())
				{
					return new Error(ERR_PARSE, "Unexpected EOF while parsing quantifier at %s in file %s", LineInfo(), input_filename);
				}
				
				if (IsOperatorToken(Tok(), TokenOperator::RIGHT_ANGLE))
				{
					high = low;
					token_idx++;
				}
				else
				{
					if (!IsOperatorToken(Tok(), TokenOperator::COMMA))
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

				if (IsOperatorToken(Tok(), TokenOperator::RIGHT_ANGLE))
				{
					high = Quantifier::infinity;
					token_idx++;
				}
				else if (Tok().type == TokenType::INTEGER)
				{
					high = static_cast<IntegerToken &>(Tok()).value;
					token_idx++;
					if (token_idx >= tokens.size())
					{
						return new Error(ERR_PARSE, "Unexpected EOF while parsing quantifier at %s in file %s", LineInfo(), input_filename);
					}
					if (!IsOperatorToken(Tok(), TokenOperator::RIGHT_ANGLE))
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
		}

		return nullptr;
	}

	Error * Compiler::ParseMultiExpressionPopulator(Populator<Expression ** &, uint32_t &> * & output)
	{
		if (token_idx >= tokens.size())
		{
			return new Error(ERR_PARSE, "Unexpected EOF while expecting populator at %s in file %s", LineInfo(), input_filename);
		}

		if (IsOperatorToken(Tok(), TokenOperator::TILDE))
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

		while (token_idx < tokens.size() && IsOperatorToken(Tok(), TokenOperator::COMMA))
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
		return nullptr;
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

		if (!value_populator || token_idx >= tokens.size() || !IsOperatorToken(Tok(), TokenOperator::COLON))
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
		if (token_idx >= tokens.size() || Tok().type != TokenType::IDENTIFIER)
		{
			return nullptr;
		}

		IdentifierToken & iden_tok = static_cast<IdentifierToken &>(Tok());
		ExpressionOperator oper = OperatorFromString(iden_tok.string);
		output = new OperatorValuePopulator(oper);
		token_idx++;
		return nullptr;
	}

	Error * Compiler::TryParseArgsPopulator(Populator<Expression * &> * & output)
	{
		if (token_idx >= tokens.size() || !IsOperatorToken(Tok(), TokenOperator::LEFT_PAREN))
		{
			return nullptr;
		}
		token_idx++;

		if (token_idx >= tokens.size())
		{
			return new Error(ERR_PARSE, "Unexpected EOF while parsing populator arguments at %s in file %s", LineInfo(), input_filename);
		}

		if (IsOperatorToken(Tok(), TokenOperator::RIGHT_PAREN))
		{
			token_idx++;
			output = new ArgsPopulator(nullptr, 0);
			return nullptr;
		}

		if (Tok().type == TokenType::INTEGER)
		{
			IntegerToken & int_tok = static_cast<IntegerToken &>(Tok());
			
			token_idx++;
			if (token_idx >= tokens.size())
			{
				return new Error(ERR_PARSE, "Unexpected EOF while parsing expression arguments at %s in file %s", LineInfo(), input_filename);
			}
			if (!IsOperatorToken(Tok(), TokenOperator::RIGHT_PAREN))
			{
				return new Error(ERR_PARSE, "May not have immediate values among multiple expression arguments at %s in file %s", LineInfo(), input_filename);
			}
			token_idx++;
			output = new ImmediatePopulator(DataType::UINT32, int_tok.value);
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
			
			if (IsOperatorToken(Tok(), TokenOperator::RIGHT_PAREN))
			{
				token_idx++;
				break;
			}

			if (!IsOperatorToken(Tok(), TokenOperator::COMMA))
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
		if (token_idx >= tokens.size() || !IsOperatorToken(Tok(), TokenOperator::AMPERSAT))
		{
			return nullptr;
		}
		token_idx++;

		if (token_idx >= tokens.size())
		{
			return new Error(ERR_PARSE, "Unexpected EOF after capture token at %s in file %s", LineInfo(), input_filename);
		}
		if (Tok().type != TokenType::IDENTIFIER)
		{
			return new Error(ERR_PARSE, "Expected identifier after captuer token at %s in file %s", LineInfo(), input_filename);
		}

		IdentifierToken & iden_tok = static_cast<IdentifierToken &>(Tok());
		output = substitution->CaptureFromName(iden_tok.string);
		token_idx++;
		return nullptr;
	}
}
