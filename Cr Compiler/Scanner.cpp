// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#include "Scanner.h"

namespace Cr
{
	/**
	 * Overload of the global function to support '_' and '$' as alphabetic characters.
	 */
	static int isalpha(int const c)
	{
		return c == '_' || c == '$' || ::isalpha(c);
	}

	/**
	 * Returns true if specified character is octal digit.
	 */
	static int isodigit(int const c)
	{
		return c >= '0' && c <= '7';
	}

	// *************************************************************** //
	// **               Scanner class implementation.               ** //
	// *************************************************************** //

	enum class ScannerState
	{
		Undefined,

		Parsing_Identifier_OR_Keyword,

		Parsing_DECIMAL_ConstantInteger_OR_ConstantFloat_OR_ConstantDouble, Parsing_DECIMAL_ConstantFloat_OR_ConstantDouble_FractionPart,
		Parsing_OCTAL_OR_HEXADECIMAL_ConstantInteger, Parsing_OCTAL_ConstantInteger, Parsing_HEXADECIMAL_ConstantInteger,
			
		Parsing_ConstantString,

		Parsing_OpAdd_OR_OpAddAssign_OR_OpInc,
		Parsing_OpSubtract_OR_OpSubtractAssign_OR_OpDec,
		Parsing_OpMultiply_OR_OpMultiplyAssign,
		Parsing_OpDivide_OR_OpDivideAssign_OR_SignleLineComment_OR_MultiLineComment,
		Parsing_SingleLineComment,
		Parsing_MultiLineComment,
		Parsing_OpModulo_OR_OpModuloAssign,
		Parsing_OpAssignment_OR_OpEquals,
		Parsing_OpNot_OR_OpNotEquals,
		Parsing_OpGreater_OR_OpGreaterEquals_OR_OpBitwiseRightShift_OR_OpBitwiseRightShiftAssign,
		Parsing_OpBitwiseRightShift_OR_OpBitwiseRightShiftAssign,
		Parsing_OpLess_OR_OpLessEquals_OR_OpBitwiseLeftShift_OR_OpBitwiseLeftShiftAssign,
		Parsing_OpBitwiseLeftShift_OR_OpBitwiseLeftShiftAssign,
		Parsing_OpAnd_OR_OpBitwiseAnd_OR_OpBitwiseAndAssign,
		Parsing_OpOr_OR_OpBitwiseOr_OR_OpBitwiseOrAssign,
		Parsing_OpBitwiseXor_OR_OpBitwiseXorAssign,
		Parsing_OpDot_OR_DECIMAL_ConstantFloat_OR_ConstantDouble_FractionPart,
		Parsing_OpPreprocessor_OR_OpPreprocessorGlue,

	};	// enum class State

	/**
	 * Reads next character from the specified stream.
	 */
	void Scanner::ReadNextChar()
	{
		m_PrevChar = m_Char;
		m_Char = m_InputStream->ReadNextChar();
	}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"

	/**
	 * Reads next lexem from the specified stream.
	 */
	Lexeme Scanner::GetNextLexeme()
	{
		// Resetting state of the scanner.
		std::string bufferedString;
		auto bufferedInt = 0;
		auto bufferedReal = 0.0;
		auto bufferedRealExponent = 1.0;
		
		// Executing the state machine to chop next lexeme.
		for (auto state = ScannerState::Undefined;;)
		{
			switch (state)
			{
				// *************************************************************** //
				case ScannerState::Undefined:
					if (m_Char == EOF)
					{
						return Lexeme(Lexeme::Type::Null);
					}
					if (isspace(m_Char))
					{
						// Skipping all unnecessary spaces.
						ReadNextChar();
					}
					else
					{	
						// Proceeding alphabetical characters or digits.
						if (isalpha(m_Char))
						{
							state = ScannerState::Parsing_Identifier_OR_Keyword;
							bufferedString.push_back(m_Char);
						}
						else if (isdigit(m_Char))
						{
							if (m_Char != '0')
							{
								// This numeric constant is octal or hexadecimal.
								state = ScannerState::Parsing_DECIMAL_ConstantInteger_OR_ConstantFloat_OR_ConstantDouble;
								bufferedInt = m_Char - '0';
							}
							else
							{
								// This numeric constant is decimal.
								state = ScannerState::Parsing_OCTAL_OR_HEXADECIMAL_ConstantInteger;
							}
						}
						else switch (m_Char)
						{	
							case '"': state = ScannerState::Parsing_ConstantString; break;

							// Or determining operator: possible composite operator...
							case '+': state = ScannerState::Parsing_OpAdd_OR_OpAddAssign_OR_OpInc; break;
							case '-': state = ScannerState::Parsing_OpSubtract_OR_OpSubtractAssign_OR_OpDec; break;
							case '*': state = ScannerState::Parsing_OpMultiply_OR_OpMultiplyAssign; break;
							case '/': state = ScannerState::Parsing_OpDivide_OR_OpDivideAssign_OR_SignleLineComment_OR_MultiLineComment; break;
							case '%': state = ScannerState::Parsing_OpModulo_OR_OpModuloAssign; break;
							case '=': state = ScannerState::Parsing_OpAssignment_OR_OpEquals; break;
							case '!': state = ScannerState::Parsing_OpNot_OR_OpNotEquals; break;
							case '<': state = ScannerState::Parsing_OpGreater_OR_OpGreaterEquals_OR_OpBitwiseRightShift_OR_OpBitwiseRightShiftAssign; break;
							case '>': state = ScannerState::Parsing_OpLess_OR_OpLessEquals_OR_OpBitwiseLeftShift_OR_OpBitwiseLeftShiftAssign; break;
							case '&': state = ScannerState::Parsing_OpAnd_OR_OpBitwiseAnd_OR_OpBitwiseAndAssign; break;
							case '|': state = ScannerState::Parsing_OpOr_OR_OpBitwiseOr_OR_OpBitwiseOrAssign; break;
							case '^': state = ScannerState::Parsing_OpBitwiseXor_OR_OpBitwiseXorAssign; break;
							case '.': state = ScannerState::Parsing_OpDot_OR_DECIMAL_ConstantFloat_OR_ConstantDouble_FractionPart; break;
							case '#': state = ScannerState::Parsing_OpPreprocessor_OR_OpPreprocessorGlue; break;

							// ...or single-character one. 
							default:
								ReadNextChar();
								switch (m_PrevChar)
								{
									case '~': return Lexeme(Lexeme::Type::OpBitwiseNot);
									case ';': return Lexeme(Lexeme::Type::OpSemicolon);
									case ':': return Lexeme(Lexeme::Type::OpColon);
									case ',': return Lexeme(Lexeme::Type::OpComma);
									case '{': return Lexeme(Lexeme::Type::OpScopeBegin);
									case '}': return Lexeme(Lexeme::Type::OpScopeEnd);
									case '[': return Lexeme(Lexeme::Type::OpSubindexBegin);
									case ']': return Lexeme(Lexeme::Type::OpSubindexEnd);
									case '(': return Lexeme(Lexeme::Type::OpParenthesesBegin);
									case ')': return Lexeme(Lexeme::Type::OpParenthesesEnd);
									default:
										throw ScannerException("Unsupported character in the input stream.");
								}	// switch (m_PrevChar)

						}	// switch (m_Char)
						ReadNextChar();
					}
					break;

				// *************************************************************** //
				case ScannerState::Parsing_Identifier_OR_Keyword:
					if (isalpha(m_Char) || isdigit(m_Char))
					{
						bufferedString.push_back(m_Char);
						ReadNextChar();
					}
					else
					{
						if (Lexeme::s_KeywordsTable.count(bufferedString) != 0)
						{
							return Lexeme(Lexeme::s_KeywordsTable.at(bufferedString));
						}
						PIdentifier identifier = new Identifier { bufferedString };
						if (m_IdentifierTable != nullptr)
						{
						//	identifier = new Identifier();
							(*m_IdentifierTable)[bufferedString].reset(identifier);
						}
						return Lexeme(identifier);
					}
					break;

				// *************************************************************** //
				case ScannerState::Parsing_DECIMAL_ConstantInteger_OR_ConstantFloat_OR_ConstantDouble:
					if (m_Char == '\'')
					{
						ReadNextChar();
					}
					else if (isdigit(m_Char))
					{
						bufferedInt *= 10;
						bufferedInt += m_Char - '0';
						ReadNextChar();
					}
					else if (m_Char == '.')
					{
						bufferedReal = static_cast<double>(bufferedInt);
						state = ScannerState::Parsing_DECIMAL_ConstantFloat_OR_ConstantDouble_FractionPart;
						ReadNextChar();
					}
					else
					{
						return Lexeme(bufferedInt);
					}
					break;
				case ScannerState::Parsing_DECIMAL_ConstantFloat_OR_ConstantDouble_FractionPart:
				//	assert(0 && "Not implemented.");
					if (isdigit(m_Char))
					{
						bufferedRealExponent /= 10.0f;
						bufferedReal += (m_Char - '0') * bufferedRealExponent;
						ReadNextChar();
					}
					else
					{
						return Lexeme(bufferedReal);
					}
					break;

				// *************************************************************** //
				case ScannerState::Parsing_OCTAL_OR_HEXADECIMAL_ConstantInteger:
					if (m_Char == 'x' || m_Char == 'X')
					{
						state = ScannerState::Parsing_HEXADECIMAL_ConstantInteger;
						ReadNextChar();
						break;
					}
					state = ScannerState::Parsing_OCTAL_ConstantInteger;
					break;
				case ScannerState::Parsing_OCTAL_ConstantInteger:
					if (m_Char == '\'')
					{
						ReadNextChar();
					}
					if (isodigit(m_Char))
					{
						bufferedInt *= 8;
						bufferedInt += m_Char - '0';
						ReadNextChar();
					}
					else
					{
						return Lexeme(bufferedInt);
					}
					break;
				case ScannerState::Parsing_HEXADECIMAL_ConstantInteger:
					if (m_Char == '\'')
					{
						ReadNextChar();
					}
					if (isxdigit(m_Char))
					{
						bufferedInt *= 16;
						if (isdigit(m_Char))
						{
							bufferedInt += m_Char - '0';
						}
						else
						{
							bufferedInt += tolower(m_Char) - 'a';
						}
						ReadNextChar();
					}
					else
					{
						return Lexeme(bufferedInt);
					}
					break;
				
				//! @todo Do we actually need string constants?
				// *************************************************************** //
				case ScannerState::Parsing_ConstantString:
					if (m_Char == EOF)
					{
						throw ScannerException("Unexpected end of stream while scanning string constant.");
					}
					if (m_Char != '"')
					{
						bufferedString.push_back(m_Char);
						ReadNextChar();
						break;
					}
					ReadNextChar();
					return Lexeme(bufferedString);

				// *************************************************************** //
				case ScannerState::Parsing_OpAdd_OR_OpAddAssign_OR_OpInc:
					if (m_Char == '+')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpInc);
					}
					if (m_Char == '=')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpAddAssign);
					}
					return Lexeme(Lexeme::Type::OpAdd);

				// *************************************************************** //
				case ScannerState::Parsing_OpSubtract_OR_OpSubtractAssign_OR_OpDec:
					if (m_Char == '-')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpDec);
					}
					if (m_Char == '=')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpSubtractAssign);
					}
					return Lexeme(Lexeme::Type::OpSubtract);

				// *************************************************************** //
				case ScannerState::Parsing_OpMultiply_OR_OpMultiplyAssign: 
					if (m_Char == '=')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpMultiplyAssign);
					}
					return Lexeme(Lexeme::Type::OpMultiply);

				// *************************************************************** //
				case ScannerState::Parsing_OpDivide_OR_OpDivideAssign_OR_SignleLineComment_OR_MultiLineComment: 
					if (m_Char == '/')
					{
						ReadNextChar();
						state = ScannerState::Parsing_SingleLineComment;
						break;
					}
					if (m_Char == '*')
					{
						ReadNextChar();
						state = ScannerState::Parsing_MultiLineComment;
						//! @todo: Surpass this dirty hack somehow.
						m_PrevChar = 0;
						break;
					}
					if (m_Char == '=')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpDivideAssign);
					}
					return Lexeme(Lexeme::Type::OpDivide);
				case ScannerState::Parsing_SingleLineComment:
					if (m_Char == '\n' || m_Char == EOF)
					{
						ReadNextChar();
						state = ScannerState::Undefined;
						break;
					}
					ReadNextChar();
					break;
				case ScannerState::Parsing_MultiLineComment: 
					if (m_PrevChar == '*' && m_Char == '/')
					{
						ReadNextChar();
						state = ScannerState::Undefined;
						break;
					}
					if (m_Char == EOF)
					{
						throw ScannerException("Unexpected end of stream while scanning multiline comment.");
					}
					ReadNextChar();
					break;

				// *************************************************************** //
				case ScannerState::Parsing_OpModulo_OR_OpModuloAssign:
					if (m_Char == '=')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpModuloAssign);
					}
					return Lexeme(Lexeme::Type::OpModulo);

				// *************************************************************** //
				case ScannerState::Parsing_OpAssignment_OR_OpEquals: 
					if (m_Char == '=')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpEquals);
					}
					return Lexeme(Lexeme::Type::OpAssignment);

				// *************************************************************** //
				case ScannerState::Parsing_OpNot_OR_OpNotEquals: 
					if (m_Char == '=')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpNotEquals);
					}
					return Lexeme(Lexeme::Type::OpNot);

				// *************************************************************** //
				case ScannerState::Parsing_OpGreater_OR_OpGreaterEquals_OR_OpBitwiseRightShift_OR_OpBitwiseRightShiftAssign: 
					if (m_Char == '>')
					{
						ReadNextChar();
						state = ScannerState::Parsing_OpBitwiseRightShift_OR_OpBitwiseRightShiftAssign;
						break;
					}
					if (m_Char == '=')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpGreaterEquals);
					}
					return Lexeme(Lexeme::Type::OpGreater);
				case ScannerState::Parsing_OpBitwiseRightShift_OR_OpBitwiseRightShiftAssign: 
					if (m_Char == '=')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpBitwiseRightShiftAssign);
					}
					return Lexeme(Lexeme::Type::OpBitwiseRightShift);

				// *************************************************************** //
				case ScannerState::Parsing_OpLess_OR_OpLessEquals_OR_OpBitwiseLeftShift_OR_OpBitwiseLeftShiftAssign: 
					if (m_Char == '<')
					{
						ReadNextChar();
						state = ScannerState::Parsing_OpBitwiseLeftShift_OR_OpBitwiseLeftShiftAssign;
						break;
					}
					if (m_Char == '=')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpLessEquals);
					}
					return Lexeme(Lexeme::Type::OpLess);
				case ScannerState::Parsing_OpBitwiseLeftShift_OR_OpBitwiseLeftShiftAssign: 
					if (m_Char == '=')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpBitwiseLeftShiftAssign);
					}
					return Lexeme(Lexeme::Type::OpBitwiseLeftShift);

				// *************************************************************** //
				case ScannerState::Parsing_OpAnd_OR_OpBitwiseAnd_OR_OpBitwiseAndAssign:
					if (m_Char == '&')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpAnd);
					}
					if (m_Char == '=')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpBitwiseAndAssign);
					}
					return Lexeme(Lexeme::Type::OpBitwiseAnd);

				// *************************************************************** //
				case ScannerState::Parsing_OpOr_OR_OpBitwiseOr_OR_OpBitwiseOrAssign: 
					if (m_Char == '|')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpOr);
					}
					if (m_Char == '=')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpBitwiseOrAssign);
					}
					return Lexeme(Lexeme::Type::OpBitwiseOr);

				// *************************************************************** //
				case ScannerState::Parsing_OpBitwiseXor_OR_OpBitwiseXorAssign: 
					if (m_Char == '=')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpBitwiseXorAssign);
					}
					return Lexeme(Lexeme::Type::OpBitwiseXor);
				
				// *************************************************************** //
				case ScannerState::Parsing_OpDot_OR_DECIMAL_ConstantFloat_OR_ConstantDouble_FractionPart:
					if (isdigit(m_Char))
					{
						state = ScannerState::Parsing_DECIMAL_ConstantFloat_OR_ConstantDouble_FractionPart;
						ReadNextChar();
						break;
					}
					return Lexeme(Lexeme::Type::OpDot);

				// *************************************************************** //
				case ScannerState::Parsing_OpPreprocessor_OR_OpPreprocessorGlue:
					if (m_Char == '#')
					{
						ReadNextChar();
						return Lexeme(Lexeme::Type::OpPreprocessorGlue);
					}
					return Lexeme(Lexeme::Type::OpPreprocessor);

				default:
					assert(0);

			}	// switch (state)
		}	// for (state;;)
	}

#pragma clang diagnostic pop

	// *************************************************************** //
	// **                 Scanner class unit tests.                 ** //
	// *************************************************************** //

	CrUnitTest(ScannerEmptyStream)
	{
		auto inputStream = std::make_shared<IO::StringInputStream>("   ");
		Scanner scanner(inputStream);

		auto lexeme = scanner.GetNextLexeme();
		assert(lexeme.GetType() == Lexeme::Type::Null);
	};

	CrUnitTest(ScannerNumeric)
	{
		auto inputStream = std::make_shared<IO::StringInputStream>("1'000 -1.2003");
		Scanner scanner(inputStream);

		auto lexeme = scanner.GetNextLexeme();
		assert(lexeme.GetValueInt() == 1000);

		lexeme = scanner.GetNextLexeme();
		assert(lexeme.GetType() == Lexeme::Type::OpSubtract);

		lexeme = scanner.GetNextLexeme();
		assert(lexeme.GetValueReal() == 1.2003);

		lexeme = scanner.GetNextLexeme();
		assert(lexeme.GetType() == Lexeme::Type::Null);
	};

	CrUnitTest(ScannerStrings)
	{
		auto inputStream = std::make_shared<IO::StringInputStream>("\"aaa\" \"bbb");
		Scanner scanner(inputStream);

		auto lexeme = scanner.GetNextLexeme();
		assert(lexeme.GetValueString() == "aaa");

		try
		{
			scanner.GetNextLexeme();
			assert(0);
		}
		catch (ScannerException const&)
		{ }

		lexeme = scanner.GetNextLexeme();
		assert(lexeme.GetType() == Lexeme::Type::Null);
	};

	CrUnitTest(ScannerKeywordsAndIDs)
	{
		auto inputStream = std::make_shared<IO::StringInputStream>("if ifa");
		Scanner scanner(inputStream);

		auto lexeme = scanner.GetNextLexeme();
		assert(lexeme.GetType() == Lexeme::Type::KwIf);

		lexeme = scanner.GetNextLexeme();
		assert(lexeme.GetType() == Lexeme::Type::IdIdentifier);

		lexeme = scanner.GetNextLexeme();
		assert(lexeme.GetType() == Lexeme::Type::Null);
	};

	CrUnitTest(ScannerCorrectComments)
	{
		auto inputStream = std::make_shared<IO::StringInputStream>("// \n/*/**/");
		Scanner scanner(inputStream);

		auto lexeme = scanner.GetNextLexeme();
		assert(lexeme.GetType() == Lexeme::Type::Null);
	};

	CrUnitTest(ScannerBrokenComments)
	{
		auto inputStream = std::make_shared<IO::StringInputStream>("// \n/* /*");
		Scanner scanner(inputStream);

		try
		{
			scanner.GetNextLexeme();
			assert(0);
		}
		catch (ScannerException const&)
		{ }
	};

}	// namespace Cr
