// $$***************************************************************$$ //
//                                                                     //
//                  Goddamn "C for Rendering" project                  //
//     Copyright (C) Goddamn Industries 2016. All Rights Reserved.     //
//          ( https://github.com/GoddamnIndustries/GoddamnCr )         //
//                                                                     //
//    This software or any its part is distributed under the terms of  //
//   Goddamn Industries End User License Agreement. By downloading or  //
//   using this software or any its part you agree with the terms of   //
//   Goddamn Industries End User License Agreement.                    //
//                                                                     //
// $$***************************************************************$$ //

/// @todo Implement functional macros.
/// @todo Implement correct nested macro substitution.
/// @todo Implement "##" and @#" operators for macros expansions.
#include "Preprocessor.h"

namespace Cr
{
	// *************************************************************** //
	// **            Preprocessor class implementation.             ** //
	// *************************************************************** //

	/**
	 * Checks whether current lexeme is of expected type and
	 * reads next lexeme.
	 */
	CRINL void Preprocessor::ExpectLexeme(Lexeme::Type const type) const
	{
		if (m_Lexeme != type)
		{
			throw PreprocessorException("something was expected.");
		}
	}

	/**
	 * Reads next lexeme from the specified stream.
	 */
	CRINL void Preprocessor::ReadNextLexeme()
	{
		if (m_LinePipe.empty())
		{
			// Reading line from file.
			std::string line;
			auto c = m_InputStream->ReadNextChar();
			for (; c != '\n' && c != EOF; c = m_InputStream->ReadNextChar())
			{
				line.push_back(static_cast<char>(c));
			}
			auto const isEof = c == EOF;

			// Lexing it.
			Scanner scanner(std::make_shared<IO::StringInputStream>(line.c_str()));
			for (auto lex = scanner.GetNextLexeme(); lex != Lexeme::Type::Null; lex = scanner.GetNextLexeme())
			{
				m_LinePipe.push_back(lex);
			}
			m_LinePipe.push_back(Lexeme(isEof ? Lexeme::Type::Null : Lexeme::Type::NewLine));
		}
		m_Lexeme = m_LinePipe.front();
		m_LinePipe.pop_front();
	}
	CRINL void Preprocessor::ReadNextLexeme(Lexeme::Type const type)
	{
		ExpectLexeme(type);
		ReadNextLexeme();
	}

	// --------------------------------------------------------------- //
	// --                        Preprocessing.                     -- //
	// --------------------------------------------------------------- //

	// { PP-BLOCK ::= [<PP-DIRECTIVE-OR-LINE> .. <PP-DIRECTIVE-OR-LINE>] <eof>
	//              | [<PP-DIRECTIVE-OR-LINE> .. <PP-DIRECTIVE-OR-LINE>] #elif
	//              | [<PP-DIRECTIVE-OR-LINE> .. <PP-DIRECTIVE-OR-LINE>] #else
	//              | [<PP-DIRECTIVE-OR-LINE> .. <PP-DIRECTIVE-OR-LINE>] #endif
	//   PP-DIRECTIVE-OR-LINE ::= #<PP-DIRECTIVE>|<PP-LINE>
	//   PP-DIRECTIVE ::= ... }
	// *************************************************************** //
	CR_INTERNAL void Preprocessor::Parse_Block(bool const isGlobalScope /*= false*/)
	{
		do
		{
			if (m_Lexeme == Lexeme::Type::Null)
			{
				// End of file reached.
				if (!isGlobalScope)
				{
					throw PreprocessorException("Unexpected end of file while parsing inset preprocessor scope.");
				}
				return;
			}
			if (m_Lexeme != Lexeme::Type::OpPreprocessor)
			{
				// Parsing an ordinary line of code.
				Parse_OrdinaryLine();
				continue;
			}

			// Parsing some preprocessor directive.
			ReadNextLexeme();
			switch (m_Lexeme.GetType())
			{
				// Compiler control processing.
				/// @todo Implement this.
				// ---------------------------------------------------
				case Lexeme::Type::KwPpPragma:
					ReadNextLexeme();
					CrAssert(0);
					break;
				case Lexeme::Type::KwPpError:
					ReadNextLexeme();
					CrAssert(0);
					break;

				// Macro preprocessing.
				// ---------------------------------------------------
				case Lexeme::Type::KwPpDefine:
					ReadNextLexeme();
					Parse_Directive_Define();
					break;
				case Lexeme::Type::KwPpUndef:
					ReadNextLexeme();
					Parse_Directive_Undef();
					break;

				// Conditional preprocessing.
				// ---------------------------------------------------
				case Lexeme::Type::KwIf:
					ReadNextLexeme();
					Parse_Directive_If();
					break;
				case Lexeme::Type::KwPpIfdef:
					ReadNextLexeme();
					Parse_Directive_Ifdef();
					break;
				case Lexeme::Type::KwPpIfndef:
					ReadNextLexeme();
					Parse_Directive_Ifndef();
					break;

				// Scope breaking directives.
				// ---------------------------------------------------
				case Lexeme::Type::KwPpElif:
					if (isGlobalScope)
					{
						throw PreprocessorException("Unexpected 'elif' directive.");
					}
					return;
				case Lexeme::Type::KwPpElse:
					if (isGlobalScope)
					{
						throw PreprocessorException("Unexpected 'else' directive.");
					}
					return;
				case Lexeme::Type::KwPpEndif:
					if (isGlobalScope)
					{
						throw PreprocessorException("Unexpected 'endif' directive.");
					}
					return;

				// Other directives.
				// ---------------------------------------------------
				default:
					throw PreprocessorException("Unknown preprocessor directive.");
			}
		} while (!isGlobalScope);
	}

	/// @todo Implement this correctly
	// *************************************************************** //
	CR_INTERNAL void Preprocessor::Parse_OrdinaryLine()
	{
		//std::deque<Lexeme> substitutedLexemes;
		//while (true)
		//{
		//	auto anyMacroSubstituted = false;
		//	while (m_Lexeme != Lexeme::Type::NewLine)
		//	{
		//		if (m_DoWriteLexemes)
		//		{
		//			if (m_Lexeme == Lexeme::Type::IdIdentifier && m_Macros.count(m_Lexeme.GetValueID()) != 0)
		//			{
		//				// This is a macro substitution.
		//				auto& macro = m_Macros[m_Lexeme.GetValueID()];
		//				substitutedLexemes.insert(m_LexemesPipe.end(), macro.m_Lexemes.begin(), macro.m_Lexemes.end());
		//				anyMacroSubstituted = true;
		//			}
		//			else
		//			{
		//				// This is an ordinary lexeme.
		//				substitutedLexemes.push_back(m_Lexeme);
		//			}
		//		}
		//		ReadNextLexeme();
		//	}

		//	if (!anyMacroSubstituted)
		//	{
		//		break;
		//	}
		//	m_LinePipe = std::move(substitutedLexemes);
		//}
		//ReadNextLexeme();

		while (m_Lexeme != Lexeme::Type::NewLine)
		{
			if (m_DoWriteLexemes)
			{
				if (m_Lexeme == Lexeme::Type::IdIdentifier && m_Macros.count(m_Lexeme.GetValueID()) != 0)
				{
					// This is a macro substitution.
					auto& macro = m_Macros[m_Lexeme.GetValueID()];
					m_LexemesPipe.insert(m_LexemesPipe.end(), macro.m_Lexemes.begin(), macro.m_Lexemes.end());
				}
				else
				{
					// This is an ordinary lexeme.
					m_LexemesPipe.push_back(m_Lexeme);
				}
			}
			ReadNextLexeme();
		}
		ReadNextLexeme();
	}

	// --------------------------------------------------------------- //
	// --                     Macro preprocessing.                  -- //
	// --------------------------------------------------------------- //

	// { PP-DIR-DEFINE ::= #define <ident> [<lexeme> .. <lexeme>] <newline>
	//                   | #define <ident>([<ident>, .. <ident>,][<ident>][[,]...]) [<lexeme> .. <lexeme>] <newline> }
	// *************************************************************** //
	CR_INTERNAL void Preprocessor::Parse_Directive_Define()
	{
		ExpectLexeme(Lexeme::Type::IdIdentifier);
		if (m_Macros.count(m_Lexeme.GetValueID()) != 0)
		{
			/// @todo Uncomment warning.
		//	throw PreprocessorException("Macro redefinition detected.");
		}
		auto& macro = m_Macros[m_Lexeme.GetValueID()];
		
		// Macros with parameters are still not implemented.
		/// @todo Implement correct macro evaluation.
		macro.m_Lexemes = std::move(m_LinePipe);
		macro.m_Lexemes.pop_back();
		ReadNextLexeme();
	}

	// { PP-DIR-UNDEF ::= #undef <ident> <newline> }
	// *************************************************************** //
	CR_INTERNAL void Preprocessor::Parse_Directive_Undef()
	{
		ExpectLexeme(Lexeme::Type::IdIdentifier);
		m_Macros.erase(m_Lexeme.GetValueID());
		ReadNextLexeme();
		ReadNextLexeme(Lexeme::Type::NewLine);
	}

	// --------------------------------------------------------------- //
	// --                  Conditional preprocessing.               -- //
	// --------------------------------------------------------------- //

	// { PP-DIR-IF ::= #if <expression> <newline> <PP-DIR-IF-BODY> }
	// *************************************************************** //
	CR_INTERNAL void Preprocessor::Parse_Directive_If()
	{
		auto const condValue = EvaluateExpression() != 0;
		ReadNextLexeme(Lexeme::Type::NewLine);

		// Parsing sections of the conditional directive.
		Parse_Directive_ElifElseEndif_Section(condValue);
	}

	// { PP-DIR-IFDEF ::= #ifdef <ident> <newline> <PP-DIR-IF-BODY> }
	// *************************************************************** //
	CR_INTERNAL void Preprocessor::Parse_Directive_Ifdef()
	{
		ExpectLexeme(Lexeme::Type::IdIdentifier);
		auto const condValue = m_Macros.count(m_Lexeme.GetValueID()) != 0;
		ReadNextLexeme();
		ReadNextLexeme(Lexeme::Type::NewLine);

		// Parsing sections of the conditional directive.
		Parse_Directive_ElifElseEndif_Section(condValue);
	}

	// { PP-DIR-IFNDEF ::= #ifndef <ident> <newline> <PP-DIR-IF-SECTION> }
	// *************************************************************** //
	CR_INTERNAL void Preprocessor::Parse_Directive_Ifndef()
	{
		ExpectLexeme(Lexeme::Type::IdIdentifier);
		auto const condValue = m_Macros.count(m_Lexeme.GetValueID()) == 0;
		ReadNextLexeme();
		ReadNextLexeme(Lexeme::Type::NewLine);

		// Parsing sections of the conditional directive.
		Parse_Directive_ElifElseEndif_Section(condValue);
	}

	// { PP-DIR-IF-SECTION ::= <PP-BLOCK> #elif <newline> <PP-DIR-IF-SECTION> 
	//                       | <PP-BLOCK> #else <newline> <PP-DIR-IF-SECTION> 
	//                       | <PP-BLOCK> #endif <newline> }
	// *************************************************************** //
	CR_INTERNAL void Preprocessor::Parse_Directive_ElifElseEndif_Section(bool const cond
		, bool const allowElifOrElse /*= true*/)
	{
		// Parsing section lexemes.
		auto const oldDoWriteLexemes = m_DoWriteLexemes;
		m_DoWriteLexemes &= cond;
		Parse_Block();
		m_DoWriteLexemes = oldDoWriteLexemes;

		// Determining what to parse next..
		switch (m_Lexeme.GetType())
		{
			// .. 'elif'/'else' sections of the conditional directive.
			// ---------------------------------------------------
			case Lexeme::Type::KwPpElif:
				{
					if (!allowElifOrElse)
					{
						throw PreprocessorException("Unexpected '#elif' directive following '#else' directive. ");
					}

					ReadNextLexeme();
					auto const condValue = EvaluateExpression() != 0;
					ReadNextLexeme(Lexeme::Type::NewLine);

					Parse_Directive_ElifElseEndif_Section(!cond && condValue);
					break;
				}

			// .. 'else' sections of the conditional directive.
			// ---------------------------------------------------
			case Lexeme::Type::KwPpElse:
				{
					if (!allowElifOrElse)
					{
						throw PreprocessorException("Unexpected '#else' directive following '#else' directive. ");
					}

					ReadNextLexeme();
					ReadNextLexeme(Lexeme::Type::NewLine);

					Parse_Directive_ElifElseEndif_Section(!cond, false);
					break;
				}

			// .. or to finish parsing conditional directive.
			// ---------------------------------------------------
			case Lexeme::Type::KwPpEndif:
				{
					ReadNextLexeme();
					ReadNextLexeme(Lexeme::Type::NewLine);
					break;
				}

			// ---------------------------------------------------
			default:
				CrAssert(0);
				break;
		}
	}

	// Recursive descent expression parser.
	// { expression ::= ..see comments in Parser class.. }
	// *************************************************************** //
	CR_INTERNAL int64_t Preprocessor::EvaluateExpression()
	{
		/// @todo Preprocess condition expression.
		return EvaluateExpression_Or();
	}
	// *************************************************************** //
	CR_INTERNAL int64_t Preprocessor::EvaluateExpression_Or()
	{
		auto value = EvaluateExpression_And();
		while (m_Lexeme == Lexeme::Type::OpOr)
		{
			ReadNextLexeme();
			value = value || EvaluateExpression_And();
		}
		return value;
	}
	// *************************************************************** //
	CR_INTERNAL int64_t Preprocessor::EvaluateExpression_And()
	{
		auto value = EvaluateExpression_BitwiseOr();
		while (m_Lexeme == Lexeme::Type::OpAnd)
		{
			ReadNextLexeme();
			value = value && EvaluateExpression_BitwiseOr();
		}
		return value;
	}
	// *************************************************************** //
	CR_INTERNAL int64_t Preprocessor::EvaluateExpression_BitwiseOr()
	{
		auto value = EvaluateExpression_BitwiseXor();
		while (m_Lexeme == Lexeme::Type::OpBitwiseOr)
		{
			ReadNextLexeme();
			value |= EvaluateExpression_BitwiseXor();
		}
		return value;
	}
	// *************************************************************** //
	CR_INTERNAL int64_t Preprocessor::EvaluateExpression_BitwiseXor()
	{
		auto value = EvaluateExpression_BitwiseAnd();
		while (m_Lexeme == Lexeme::Type::OpBitwiseXor)
		{
			ReadNextLexeme();
			value ^= EvaluateExpression_BitwiseAnd();
		}
		return value;
	}
	// *************************************************************** //
	CR_INTERNAL int64_t Preprocessor::EvaluateExpression_BitwiseAnd()
	{
		auto value = EvaluateExpression_Equals_OR_NotEquals();
		while (m_Lexeme == Lexeme::Type::OpBitwiseAnd)
		{
			ReadNextLexeme();
			value &= EvaluateExpression_Equals_OR_NotEquals();
		}
		return value;
	}
	// *************************************************************** //
	CR_INTERNAL int64_t Preprocessor::EvaluateExpression_Equals_OR_NotEquals()
	{
		auto value = EvaluateExpression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals();
		while (true)
		{
			switch (m_Lexeme.GetType())
			{
				case Lexeme::Type::OpEquals:
					ReadNextLexeme();
					value = value == EvaluateExpression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals();
					break;
				case Lexeme::Type::OpNotEquals:
					ReadNextLexeme();
					value = value != EvaluateExpression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals();
					break;
				default:
					return value;
			}
		}
	}
	// *************************************************************** //
	CR_INTERNAL int64_t Preprocessor::EvaluateExpression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals()
	{
		auto value = EvaluateExpression_BitwiseLeftShift_OR_BitwiseRightShift();
		while (true)
		{
			switch (m_Lexeme.GetType())
			{
				case Lexeme::Type::OpLess:
					ReadNextLexeme();
					value = value < EvaluateExpression_BitwiseLeftShift_OR_BitwiseRightShift();
					break;
				case Lexeme::Type::OpLessEquals:
					ReadNextLexeme();
					value = value <= EvaluateExpression_BitwiseLeftShift_OR_BitwiseRightShift();
					break;
				case Lexeme::Type::OpGreater:
					ReadNextLexeme();
					value = value > EvaluateExpression_BitwiseLeftShift_OR_BitwiseRightShift();
					break;
				case Lexeme::Type::OpGreaterEquals:
					ReadNextLexeme();
					value = value >= EvaluateExpression_BitwiseLeftShift_OR_BitwiseRightShift();
					break;
				default:
					return value;
			}
		}
	}
	// *************************************************************** //
	CR_INTERNAL int64_t Preprocessor::EvaluateExpression_BitwiseLeftShift_OR_BitwiseRightShift()
	{
		auto value = EvaluateExpression_Add_OR_Subtract();
		while (true)
		{
			switch (m_Lexeme.GetType())
			{
				case Lexeme::Type::OpBitwiseLeftShift:
					ReadNextLexeme();
					value <<= EvaluateExpression_Add_OR_Subtract();
					break;
				case Lexeme::Type::OpBitwiseRightShift:
					ReadNextLexeme();
					value >>= EvaluateExpression_Add_OR_Subtract();
					break;
				default:
					return value;
			}
		}
	}
	// *************************************************************** //
	CR_INTERNAL int64_t Preprocessor::EvaluateExpression_Add_OR_Subtract()
	{
		auto value = EvaluateExpression_Multiply_OR_Divide_OR_Modulo();
		while (true)
		{
			switch (m_Lexeme.GetType())
			{
				case Lexeme::Type::OpAdd:
					ReadNextLexeme();
					value += EvaluateExpression_Multiply_OR_Divide_OR_Modulo();
					break;
				case Lexeme::Type::OpSubtract:
					ReadNextLexeme();
					value -= EvaluateExpression_Multiply_OR_Divide_OR_Modulo();
					break;
				default:
					return value;
			}
		}
	}
	// *************************************************************** //
	CR_INTERNAL int64_t Preprocessor::EvaluateExpression_Multiply_OR_Divide_OR_Modulo()
	{
		auto value = EvaluateExpression_Unary();
		while (true)
		{
			switch (m_Lexeme.GetType())
			{
				case Lexeme::Type::OpMultiply:
					ReadNextLexeme();
					value *= EvaluateExpression_Unary();
					break;
				case Lexeme::Type::OpDivide:
					{
						ReadNextLexeme();
						auto const rightValue = EvaluateExpression_Unary();
						if (rightValue == 0)
						{
							throw PreprocessorException("Division by zero occurred while evaluating preprocessor "
									                    "expression.");
						}
						value /= rightValue;
						break;
					}
				case Lexeme::Type::OpModulo:
					{
						ReadNextLexeme();
						auto const rightValue = EvaluateExpression_Unary();
						if (rightValue == 0)
						{
							throw PreprocessorException("Division by zero occurred while evaluating preprocessor "
									                    "expression.");
						}
						value %= rightValue;
						break;
					}
				default:
					return value;
			}
		}
	}
	// *************************************************************** //
	CR_INTERNAL int64_t Preprocessor::EvaluateExpression_Unary()
	{
		switch (m_Lexeme.GetType())
		{
			case Lexeme::Type::OpNot:
				ReadNextLexeme();
				return !EvaluateExpression_Unary();
			case Lexeme::Type::OpBitwiseNot:
				ReadNextLexeme();
				return ~EvaluateExpression_Unary();
			case Lexeme::Type::OpAdd:
				ReadNextLexeme();
				return +EvaluateExpression_Unary();
			case Lexeme::Type::OpSubtract:
				ReadNextLexeme();
				return -EvaluateExpression_Unary();
			default:
				return EvaluateExpression_Primary();
		}
	}
	// *************************************************************** //
	CR_INTERNAL int64_t Preprocessor::EvaluateExpression_Primary()
	{
		switch (m_Lexeme.GetType())
		{
			// Literals in the macro expression.
			// ---------------------------------------------------
			case Lexeme::Type::KwTrue:
				ReadNextLexeme();
				return 1;
			case Lexeme::Type::KwFalse:
				ReadNextLexeme();
				return 0;
			case Lexeme::Type::CtInt:
				{
					auto const value = m_Lexeme.GetValueInt();
					ReadNextLexeme();
					return value;
				}

			// 'defined' operator.
			// ---------------------------------------------------
			case Lexeme::Type::KwPpDefined:
				{
					ReadNextLexeme();
					if (m_Lexeme == Lexeme::Type::OpParenOpen)
					{
						// { defined(<ident>) } syntax.
						ReadNextLexeme();
						ExpectLexeme(Lexeme::Type::IdIdentifier);
						auto const value = m_Macros.count(m_Lexeme.GetValueID()) != 0;
						ReadNextLexeme();
						ReadNextLexeme(Lexeme::Type::OpParenClose);
						return value;
					}
					// { defined <ident> } syntax.
					ExpectLexeme(Lexeme::Type::IdIdentifier);
					auto const value = m_Macros.count(m_Lexeme.GetValueID()) != 0;
					ReadNextLexeme();
					return value;
				}
			
			// Undefined identifier in the expression evaluating.
			// ---------------------------------------------------
			case Lexeme::Type::IdIdentifier:
				ReadNextLexeme();
				return 0;

			// Parentheses expression.
			// ---------------------------------------------------
			case Lexeme::Type::OpParenOpen:
				{
					ReadNextLexeme();
					auto const parenExpr = EvaluateExpression();
					ReadNextLexeme(Lexeme::Type::OpParenClose);
					return parenExpr;
				}

			// ---------------------------------------------------
			default:
				throw PreprocessorException("Unexpected lexeme while parsing preprocessor expression.");
		}
	}

	// *************************************************************** //
	// *************************************************************** //
	// *************************************************************** //
	// *************************************************************** //
	// *************************************************************** //
	// *************************************************************** //
	// *************************************************************** //
	// *************************************************************** //
	// *************************************************************** //
	// *************************************************************** //
	// *************************************************************** //
	// *************************************************************** //
	// *************************************************************** //
	CR_API Lexeme Preprocessor::GetNextLexeme()
	{
		while (m_LexemesPipe.empty())
		{
			Parse_Block(true);
		}
		auto const bufferedLexeme = m_LexemesPipe.front();
		m_LexemesPipe.pop_front();
		return bufferedLexeme;
	}

	// *************************************************************** //
	// **              Preprocessor class unit tests.               ** //
	// *************************************************************** //

	/*CrUnitTest(PreprocessorEmptyStream)
	{
		auto inputStream = std::make_shared<IO::StringInputStream>(R"(#define KEK 1 ## 2
KEK
)");
		Preprocessor preprocessor(inputStream);
		preprocessor.Parse_Block(true);
		preprocessor.Parse_Block(true);
		int i = 1;
	};*/

	/*CrUnitTest(PreprocessorEmptyStream)
	{
		auto inputStream = std::make_shared<IO::StringInputStream>(R"(#define TRUE 1
#if 0
1
#elif TRUE
	#undef TRUE
	#if -((1 << 1) * 35) + 3 + TRUE == -67 && !defined(KEK)
		2
	#endif
	22
#else 
3
#endif
4
)");
		Preprocessor preprocessor(inputStream);
		preprocessor.Parse_Block(true);
		preprocessor.Parse_Block(true);
		preprocessor.Parse_Block(true);
		int i = 1;
	};*/

}	// namespace Cr
