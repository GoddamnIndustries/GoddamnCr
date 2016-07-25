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

// ReSharper disable CppRedundantElseKeyword
// ReSharper disable CppRedundantElseKeywordInsideCompoundStatement

#include "Lexer.hpp"
#include "../Syntax/SyntaxFactory.hpp"
#include "../Utils/String.hpp"
#include "../Utils/half.hpp"
#include "../Error.hpp"

#include <map>

/** @todo Find correct values. */
#define HLF_MAX FLT_MAX
#define HLF_MIN FLT_MIN

#define CR_INL
#undef NULL
#define fallthrough

namespace Cr
{
	typedef ASyntaxLexer AbstractSyntaxLexer;
	typedef int32_t  Int;
	typedef uint32_t UInt;
	typedef int64_t  Long;
	typedef uint64_t ULong;
	typedef half_float::half Half;
	typedef float    Float;
	typedef double   Double;

	/** Complete syntax lexer. */
	class CR_API CASyntaxLexer final : public AbstractSyntaxLexer, public std::enable_shared_from_this<CASyntaxLexer>
	{
	private:
		StringBuilder  _builder;
		CStr           _text = "  \n//\r\n  while";
		mutable size_t _textPeekedChars = 0;
		bool const     _suppressDocumentationCommentParse = true;

		CR_INL void Start()
		{
		}

		CR_INL StringSlice GetText() const
		{
			return{};
		}

		CR_INL Char PeekChar() const
		{
			return _text[_textPeekedChars = 0];
		}

		CR_INL Char PeekNextChar() const
		{
			return _text[++_textPeekedChars];
		}

		CR_INL void AdvanceChars()
		{
			_text += _textPeekedChars + 1;
			_textPeekedChars = 0;
		}

	public:

		// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
		// XX                  Public syntax lexer.                     XX //
		// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //

		/** Scans token node. */
		CR_API SyntaxLexeme Lex() override
		{
			SyntaxTokenInfo tokenInfo = {};
			SyntaxNode leading, trailing;

			ScanSyntaxTrivia(leading, false);
			Start();
			ScanSyntaxToken(tokenInfo);
			ScanSyntaxTrivia(trailing, true);

			SyntaxLexeme token;
			switch (tokenInfo.Kind)
			{
				case SyntaxKind::Identifier:
					token = SyntaxFactory::Identifier(leading, tokenInfo.Text, trailing);
					break;

				case SyntaxKind::ConstantInt:
					token = SyntaxFactory::Literal(leading, tokenInfo.ValueInt, trailing);
					break;
				case SyntaxKind::ConstantUInt:
					token = SyntaxFactory::Literal(leading, tokenInfo.ValueUInt, trailing);
					break;
				case SyntaxKind::ConstantLong:
					token = SyntaxFactory::Literal(leading, tokenInfo.ValueLong, trailing);
					break;
				case SyntaxKind::ConstantULong:
					token = SyntaxFactory::Literal(leading, tokenInfo.ValueULong, trailing);
					break;
				case SyntaxKind::ConstantDouble:
					token = SyntaxFactory::Literal(leading, tokenInfo.ValueDouble, trailing);
					break;
				case SyntaxKind::ConstantFloat:
					token = SyntaxFactory::Literal(leading, tokenInfo.ValueFloat, trailing);
					break;
				case SyntaxKind::ConstantHalf:
					token = SyntaxFactory::Literal(leading, tokenInfo.ValueHalf, trailing);
					break;
				case SyntaxKind::ConstantChar:
					token = SyntaxFactory::Literal(leading, tokenInfo.ValueChar, trailing);
					break;
				case SyntaxKind::ConstantString:
					token = SyntaxFactory::Literal(leading, tokenInfo.ValueString, trailing);
					break;

				default:
					assert(SyntaxFacts::IsPunctuationOrKeyword(tokenInfo.Kind));
					token = SyntaxFactory::Token(leading, tokenInfo.Kind, trailing);
					break;
			}

			return token;
		}

	private:

		// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
		// XX                 Internal syntax lexer.                    XX //
		// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //

		/** Raw token information. */
		CR_API struct SyntaxTokenInfo final
		{
			SyntaxKind  Kind;
			StringSlice Text;
			Double      ValueDouble;
			Float       ValueFloat;
			Half        ValueHalf;
			Int         ValueInt;
			UInt        ValueUInt;
			Long        ValueLong;
			ULong       ValueULong;
			Char        ValueChar;
			String      ValueString;
		};	// struct SyntaxTokenInfo

		/** Scans raw token information. */
		CR_INL void ScanSyntaxToken(SyntaxTokenInfo& tokenInfo)
		{
			tokenInfo.Kind = SyntaxKind::None;
			tokenInfo.Text = StringSlice::Empty;

			auto character = PeekChar();
			switch (character)
			{
				/* String literals. */
				case '\'':
				case '\"':
					ScanStringLiteral(tokenInfo);
					break;

				/* '#' or '##' */
				case '#':
					AdvanceChars();
					if (PeekChar() == '#')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpHashHash;
					}
					else
					{
						tokenInfo.Kind = SyntaxKind::OpHash;
					}
					break;

				/* '.' or '.123' */
				case '.':
					if (PeekNextChar() == '.' && PeekNextChar() == '.')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpDotDotDot;
					}
					else if (!ScanNumericLiteral(tokenInfo))
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpDot;
					}
					break;

				/* ':' or '::' */
				case ':':
					AdvanceChars();
					if (PeekChar() == ':')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpColonColon;
					}
					else
					{
						tokenInfo.Kind = SyntaxKind::OpColon;
					}
					break;

				case ',':
					AdvanceChars();
					tokenInfo.Kind = SyntaxKind::OpComma;
					break;

				case ';':
					AdvanceChars();
					tokenInfo.Kind = SyntaxKind::OpSemicolon;
					break;

				case '(':
					AdvanceChars();
					tokenInfo.Kind = SyntaxKind::OpParenOpen;
					break;

				case ')':
					AdvanceChars();
					tokenInfo.Kind = SyntaxKind::OpParenClose;
					break;

				case '{':
					AdvanceChars();
					tokenInfo.Kind = SyntaxKind::OpBraceOpen;
					break;

				case '}':
					AdvanceChars();
					tokenInfo.Kind = SyntaxKind::OpBraceClose;
					break;

				case '[':
					AdvanceChars();
					tokenInfo.Kind = SyntaxKind::OpBracketOpen;
					break;

				case ']':
					AdvanceChars();
					tokenInfo.Kind = SyntaxKind::OpBracketClose;
					break;

				case '?':
					AdvanceChars();
					tokenInfo.Kind = SyntaxKind::OpQuestion;
					break;

				/* '=' or '==' */
				case '=':
					AdvanceChars();
					if (PeekChar() == '=')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpEqualsEquals;
					}
					else
					{
						tokenInfo.Kind = SyntaxKind::OpEquals;
					}
					break;

				/* '+' or '+=' or '++' */
				case '+':
					AdvanceChars();
					if ((character = PeekChar()) == '=')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpPlusEquals;
					}
					else if (character == '+')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpPlusPlus;
					}
					else
					{
						tokenInfo.Kind = SyntaxKind::OpPlus;
					}
					break;

				/* '-' or '-=' or '--' or '->' */
				case '-':
					AdvanceChars();
					if ((character = PeekChar()) == '=')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpMinusEquals;
					}
					else if (character == '-')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpMinusMinus;
					}
					else if (character == '>')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpMinusGreater;
					}
					else
					{
						tokenInfo.Kind = SyntaxKind::OpMinus;
					}
					break;

				/* '*' or '*=' */
				case '*':
					AdvanceChars();
					if (PeekChar() == '=')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpAsteriskEquals;
					}
					else
					{
						tokenInfo.Kind = SyntaxKind::OpAsterisk;
					}
					break;

				/* '/' or '/=' */
				case '/':
					AdvanceChars();
					if (PeekChar() == '=')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpSlashEquals;
					}
					else
					{
						tokenInfo.Kind = SyntaxKind::OpSlash;
					}
					break;

				/* '%' or '%=' */
				case '%':
					AdvanceChars();
					if (PeekChar() == '=')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpPercentEquals;
					}
					else
					{
						tokenInfo.Kind = SyntaxKind::OpPercent;
					}
					break;

				/* '!' or '!=' */
				case '!':
					AdvanceChars();
					if (PeekChar() == '=')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpExclamationEquals;
					}
					else
					{
						tokenInfo.Kind = SyntaxKind::OpExclamation;
					}
					break;

				/* '>' or '>=' or '>>' or '>>=' */
				case '>':
					AdvanceChars();
					if ((character = PeekChar()) == '=')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpGreaterEquals;
					}
					else if (character == '>')
					{
						AdvanceChars();
						if (PeekChar() == '=')
						{
							AdvanceChars();
							tokenInfo.Kind = SyntaxKind::OpGreaterGreaterEquals;
						}
						else
						{
							tokenInfo.Kind = SyntaxKind::OpGreaterGreater;
						}
					}
					else
					{
						tokenInfo.Kind = SyntaxKind::OpGreater;
					}
					break;

				/* '<' or '<=' or '<<' or '<<=' */
				case '<':
					AdvanceChars();
					if ((character = PeekChar()) == '=')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpLessEquals;
					}
					else if (character == '<')
					{
						AdvanceChars();
						if (PeekChar() == '=')
						{
							AdvanceChars();
							tokenInfo.Kind = SyntaxKind::OpLessLessEquals;
						}
						else
						{
							tokenInfo.Kind = SyntaxKind::OpLessLess;
						}
					}
					else
					{
						tokenInfo.Kind = SyntaxKind::OpLess;
					}
					break;

				case '~':
					AdvanceChars();
					tokenInfo.Kind = SyntaxKind::OpTilde;
					break;

				/* '&' or '&=' or '&&' */
				case '&':
					AdvanceChars();
					if ((character = PeekChar()) == '=')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpAmpersandEquals;
					}
					else if (character == '&')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpAmpersandAmpersand;
					}
					else
					{
						tokenInfo.Kind = SyntaxKind::OpAmpersand;
					}
					break;

				/* '|' or '|=' or '||' */
				case '|':
					AdvanceChars();
					if ((character = PeekChar()) == '=')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpBarEquals;
					}
					else if (character == '|')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpBarBar;
					}
					else
					{
						tokenInfo.Kind = SyntaxKind::OpBar;
					}
					break;

				/* '^' or '^=' */
				case '^':
					AdvanceChars();
					if (PeekChar() == '=')
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::OpCaretEquals;
					}
					else
					{
						tokenInfo.Kind = SyntaxKind::OpCaret;
					}
					break;

				/* Keywords and operators. */
				case '_':
				case '$':
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'G':
				case 'H':
				case 'I':
				case 'J':
				case 'K':
				case 'L':
				case 'M':
				case 'N':
				case 'O':
				case 'P':
				case 'Q':
				case 'R':
				case 'S':
				case 'T':
				case 'U':
				case 'V':
				case 'W':
				case 'X':
				case 'Y':
				case 'Z':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
				case 'g':
				case 'h':
				case 'i':
				case 'j':
				case 'k':
				case 'l':
				case 'm':
				case 'n':
				case 'o':
				case 'p':
				case 'q':
				case 'r':
				case 's':
				case 't':
				case 'u':
				case 'v':
				case 'w':
				case 'x':
				case 'y':
				case 'z':
					ScanIdentifierOrKeywordLiteral(tokenInfo);
					break;

				/* Numeric constants. */
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					ScanNumericLiteral(tokenInfo);
					break;

				/* Invalid symbol. */
				default:
					AddError(Error::ErrorInvalidSymbol);
					break;
			}

			tokenInfo.Text = GetText();
		}

		// **************************************************************** //
		//                        Numbers proceeding.                       //
		// **************************************************************** //

		/** Scans a decimal integer without cleaning the builder. */
		template<typename TFunc = void(*)(char)>
		CR_INL bool ScanInteger(TFunc const& func = [](char){})
		{
			auto const start = _text;

			char character;
			while ((character = PeekChar()) >= '0' && character <= '9')
			{
				func(character);
				_builder.push_back(character);
				AdvanceChars();
			}

			return start < _text;
		}

		/** Scans a hexadecimal integer without cleaning the builder. */
		CR_INL bool ScanHexInteger()
		{
			auto const start = _text;

			char character;
			while (isxdigit(character = PeekChar()))
			{
				_builder.push_back(character);
				AdvanceChars();
			}

			return start < _text;
		}

		/** Scans a hexadecimal integer without cleaning the builder. */
		CR_INL bool ScanOctalInteger()
		{
			auto const start = _text;

			char character;
			while ((character = PeekChar()) >= '0' && character <= '7')
			{
				_builder.push_back(character);
				AdvanceChars();
			}

			return start < _text;
		}

		/** Parses unsigned long value from the builder. */
		CR_INL ULong ParseValueULong(int const radix = 10) const
		{
			char* endPtr = nullptr;
			auto const value = strtoull(_builder.data(), &endPtr, radix);
			if (_builder.data() == endPtr)
			{
				/* Since syntax was already parsed, the only error that we can
				 * expect is an overflow. */
				AddError(Error::ErrorIntOverflow);
			}

			return value;
		}

		/** Parses double value from the builder. */
		CR_INL Double ParseValueDouble() const
		{
			char* endPtr = nullptr;
			auto const value = strtod(_builder.data(), &endPtr);
			if (_builder.data() == endPtr)
			{
				/* Since syntax was already parsed, the only error that we can
				 * expect is an overflow. */
				AddError(Error::ErrorFloatOverflow);
			}

			return value;
		}

		/** Parses float value from the builder. */
		CR_INL Float ParseValueFloat() const
		{
			char* endPtr = nullptr;
			auto const value = strtof(_builder.data(), &endPtr);
			if (_builder.data() == endPtr)
			{
				/* Since syntax was already parsed, the only error that we can
				 * expect is an overflow. */
				AddError(Error::ErrorFloatOverflow);
			}

			return value;
		}

		/** Parses half value from the builder. */
		CR_INL Half ParseValueHalf() const
		{
			auto const valueFloat = ParseValueFloat();
			if (valueFloat < HLF_MIN && valueFloat > HLF_MAX)
			{
				/* Since syntax was already parsed, the only error that we can
				 * expect is an overflow. */
				AddError(Error::ErrorFloatOverflow);
			}

			return static_cast<Half>(valueFloat);
		}

		/** Scans a numeric literal token. */
		CR_INL bool ScanNumericLiteral(SyntaxTokenInfo& tokenInfo)
		{
			auto canBeHex    = false;
			auto canBeOctal  = false;
			auto hasFraction = false;
			auto hasExponent = false;
			auto hasUSuffix  = false;
			auto hasLSuffix  = false;

			_builder.resize(0);

			/* First of all lets determine what possible radix can this 
			 * numeric literal be using: */
			auto character = PeekChar();
			if (character == '0')
			{
				if ((character = PeekNextChar()) == 'x' || character == 'X')
				{
					/* ..either definitly hexadecimal, */
					AdvanceChars();
					canBeHex = true;
				}
				else
				{
					/* ..either possibly octal. */
					canBeOctal = true;
				}
			}

			if (canBeHex)
			{
				/* Parsing hexadecimal literals is quite simple - '0x' prefix
				 * and a sequence of hexadecimal digits. */
				ScanHexInteger();
				
				/* Parsing suffix (U, L or both)... */
				if ((character = PeekChar()) == 'l' || character == 'L')
				{
					if (character == 'l')
					{
						AddError(Error::WarningLowercaseLSuffix);
					}

					AdvanceChars();
					hasLSuffix = true;
					if ((character = PeekChar()) == 'u' || character == 'U')
					{
						AdvanceChars();
						hasUSuffix = true;
					}
				}
				else if (character == 'u' || character == 'U')
				{
					AdvanceChars();
					hasUSuffix = true;
					if ((character = PeekChar()) == 'l' || character == 'L')
					{
						AdvanceChars();
						hasLSuffix = true;
					}
				}
			}
			else
			{
				/* Parsing octal or decimals literals is harder than hexadecimal -
				 * '013' is octal int, but '013.' is decimal float. */
				auto hasOctalChars = true;
				ScanInteger([&hasOctalChars](char const ch) { hasOctalChars &= ch <= '7'; });

				/* Parsing fraction... */
				if ((character = PeekChar()) == '.')
				{
					AdvanceChars();
					if (!ScanInteger() && _builder.size() == 0)
					{
						/* No following or preceding number. */
						return false;
					}

					_builder.push_back(character);
					hasFraction = true;
				}

				/* Parsing exponent... */
				if ((character = PeekChar()) == 'e' || character == 'E')
				{
					_builder.push_back(character);
					AdvanceChars();
					hasExponent = true;

					if ((character = PeekChar()) == '-' || character == '+')
					{
						_builder.push_back(character);
						AdvanceChars();
					}

					if (!ScanInteger())
					{
						/* Adding '0' to the builder to make real parser work
						 * correctly. */
						_builder.push_back('0');
						AddError(Error::ErrorInvalidReal);
					}
				}

				/* So, literals that start with zero and have fraction or exponent are 
				 * treated as decimal float literals, e.g. '013e0' or '013.'.
				 * Literals that start with zero and have non-octal digits are treated as
				 * error literals. 
				 *
				 * Note that this behaviour differs from other complers, where '08' is 
				 * treated as two lexemes - 0 and 8. */
				canBeOctal &= !hasFraction && !hasExponent;
				if (canBeOctal && !hasOctalChars)
				{
					AddError(Error::ErrorInvalidOctalNumber);
				}

				/* Parsing suffix... */
				if ((character = PeekChar()) == 'd' || character == 'D')
				{
					/* '013d' is not a decimal double. */
					if (!canBeOctal)
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::ConstantDouble;
					}
				}
				else if (character == 'f' || character == 'F')
				{
					/* '013f' is not a decimal float. */
					if (!canBeOctal)
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::ConstantFloat;
					}
				}
				else if (character == 'h' || character == 'H')
				{
					/* '013h' is not a decimal half. */
					if (!canBeOctal)
					{
						AdvanceChars();
						tokenInfo.Kind = SyntaxKind::ConstantHalf;
					}
				}
				else if (character == 'l' || character == 'L')
				{
					if (character == 'l')
					{
						AddError(Error::WarningLowercaseLSuffix);
					}

					AdvanceChars();
					if (hasFraction || hasExponent)
					{
						/* Floating-point literals with 'l' or 'L' suffix
						 * are treated as double-precision values. */
						tokenInfo.Kind = SyntaxKind::ConstantDouble;
					}
					else
					{
						hasLSuffix = true;
						if ((character = PeekChar()) == 'u' || character == 'U')
						{
							AdvanceChars();
							hasUSuffix = true;
						}
					}
				}
				else if ((character = PeekChar()) == 'u' || character == 'U')
				{
					AdvanceChars();
					hasUSuffix = true;
					if ((character = PeekChar()) == 'l' || character == 'L')
					{
						AdvanceChars();
						hasLSuffix = true;
					}
				}
				else if (hasFraction || hasExponent)
				{
					tokenInfo.Kind = SyntaxKind::ConstantDouble;
				}
			}

			switch (tokenInfo.Kind)
			{ 
				case SyntaxKind::ConstantDouble:
					tokenInfo.ValueDouble = ParseValueDouble();
					break;
				case SyntaxKind::ConstantFloat:
					tokenInfo.ValueFloat = ParseValueFloat();
					break;
				case SyntaxKind::ConstantHalf:
					tokenInfo.ValueHalf = ParseValueHalf();
					break;

				default:
					ULong value;
					if (_builder.size() == 0)
					{
						/* Safe default value for parsing. */
						value = 0;
						AddError(Error::ErrorInvalidNumber);
					}
					else
					{
						value = ParseValueULong(canBeHex ? 16 : canBeOctal ? 8 : 10);
					}

					if (!hasUSuffix && !hasLSuffix)
					{
						/* If literal has no suffix, value is first of these types it's value 
						 * can be presented: int, uint, long, ulong. */
						if (value <= INT32_MAX)
						{
							tokenInfo.Kind = SyntaxKind::ConstantInt;
							tokenInfo.ValueInt = static_cast<Int>(value);
						}
						else if (value <= UINT32_MAX)
						{
							tokenInfo.Kind = SyntaxKind::ConstantUInt;
							tokenInfo.ValueUInt = static_cast<UInt>(value);
						}
						else if (value <= INT64_MAX)
						{
							tokenInfo.Kind = SyntaxKind::ConstantLong;
							tokenInfo.ValueLong = static_cast<Long>(value);
						}
						else
						{
							tokenInfo.Kind = SyntaxKind::ConstantULong;
							tokenInfo.ValueULong = value;
						}
					}
					else if (hasUSuffix && !hasLSuffix)
					{
						/* If literal has 'u' or 'U' suffix, value is first of these types it's value 
						 * can be presented: uint, ulong. */
						if (value <= UINT32_MAX)
						{
							tokenInfo.Kind = SyntaxKind::ConstantUInt;
							tokenInfo.ValueUInt = static_cast<UInt>(value);
						}
						else
						{
							tokenInfo.Kind = SyntaxKind::ConstantULong;
							tokenInfo.ValueULong = value;
						}
					}
					else if (!hasUSuffix && hasLSuffix)
					{
						/* If literal has 'l' or 'L' suffix, value is first of these types it's value 
						 * can be presented: long, ulong. */
						if (value <= INT64_MAX)
						{
							tokenInfo.Kind = SyntaxKind::ConstantLong;
							tokenInfo.ValueLong = static_cast<Long>(value);
						}
						else
						{
							tokenInfo.Kind = SyntaxKind::ConstantULong;
							tokenInfo.ValueULong = value;
						}
					}
					else
					{
						/* If literal has 'ul', 'uL', 'Ul', 'UL' suffix, value is of ulong type. */
						tokenInfo.Kind = SyntaxKind::ConstantULong;
						tokenInfo.ValueULong = value;
					}
					break;
			}

			return true;
		}

		// **************************************************************** //
		//                        Strings proceeding.                       //
		// **************************************************************** //

		/** Scans an escape sequence without cleaning the builder. */
		CR_INL void ScanEscapeSequence()
		{
			auto character = PeekChar();
			AdvanceChars();
			assert(character == '\\');

			/* Saving existing builder for cases where we will need to
			 * parse embedded character codes. */
			auto savedBuilder = move(_builder);

			character = PeekChar();
			switch (character)
			{
				/* This escapes translate themselves. */
				case '\\':
				case '\'':
				case '\"':
				case '\?':
					AdvanceChars();
					break;

				/* Standard C escapes. */
				case 'a':
					AdvanceChars();
					character = '\a';
					break;
				case 'b':
					AdvanceChars();
					character = '\b';
					break;
				case 'f':
					AdvanceChars();
					character = '\f';
					break;
				case 'n':
					AdvanceChars();
					character = '\n';
					break;
				case 'r':
					AdvanceChars();
					character = '\r';
					break;
				case 't':
					AdvanceChars();
					character = '\t';
					break;
				case 'v':
					AdvanceChars();
					character = '\v';
					break;
				
				/* Octal character code. */
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
					ScanOctalInteger();
					character = static_cast<char>(ParseValueULong(8));
					break;

				/* Hex character code. */
				case 'x':
					AdvanceChars();
					if (!ScanHexInteger())
					{
						AddError(Error::ErrorInvalidHexNumber);
					}
					character = static_cast<char>(ParseValueULong(16));
					break;

				/* Or some illegal escape. */
				default:
					AddError(Error::ErrorIllegalEscape);
					break;
			}

			/* Restoring the orginal builder back. */
			_builder = move(savedBuilder);
			_builder.push_back(character);
		}

		/** Scans a string or character literal token. */
		CR_INL void ScanStringLiteral(SyntaxTokenInfo& tokenInfo, bool const allowEscapes = true)
		{
			auto const quoteCharacter = PeekChar();
			AdvanceChars();
			assert(quoteCharacter == /*'\''*/0x27 || quoteCharacter == '\"');

			_builder.resize(0);

			AdvanceChars();
			while (true)
			{
				auto const character = PeekChar();
				if (character == '\\' && allowEscapes)
				{
					/* Escape sequence. */
					ScanEscapeSequence();
				}
				else if (character == quoteCharacter)
				{
					/* End of string. */
					AdvanceChars();
					break;
				}
				else if (character == '\n' || character == EOF)
				{
					/* Unexpected end of line or file. */
					AddError(Error::ErrorNewlineInConst);
				}
				else
				{
					/* Regular string character. */
					_builder.push_back(character);
					AdvanceChars();
				}
			}

			if (quoteCharacter == '\'')
			{
				tokenInfo.Kind = SyntaxKind::ConstantChar;
				if (_builder.size() != 1)
				{
					AddError(_builder.size() != 0 ? Error::ErrorTooManyCharsInConst : Error::ErrorEmptyCharConst);
				}
			}
			else
			{
				tokenInfo.Kind = SyntaxKind::ConstantString;
			}
			tokenInfo.ValueString = _builder;
			tokenInfo.ValueChar = _builder.size() > 0 ? _builder[0] : EOF;
		}

		// **************************************************************** //
		//                       Keywords proceeding.                       //
		// **************************************************************** //

		/** Scans an identifier without cleaning the builder. */
		CR_INL bool ScanIdentifier()
		{
			auto const start = _text;

			while (true)
			{
				auto const character = PeekChar();
				switch (character)
				{
					/* Valid identifier characters. */
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						if (start == _text)
						{
							/* Identifier starts with a digit.
							 * How did we actually got here? */
							return false;
						}
						fallthrough;

					case '_':
					case '$':
					case 'A':
					case 'B':
					case 'C':
					case 'D':
					case 'E':
					case 'F':
					case 'G':
					case 'H':
					case 'I':
					case 'J':
					case 'K':
					case 'L':
					case 'M':
					case 'N':
					case 'O':
					case 'P':
					case 'Q':
					case 'R':
					case 'S':
					case 'T':
					case 'U':
					case 'V':
					case 'W':
					case 'X':
					case 'Y':
					case 'Z':
					case 'a':
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':
					case 'g':
					case 'h':
					case 'i':
					case 'j':
					case 'k':
					case 'l':
					case 'm':
					case 'n':
					case 'o':
					case 'p':
					case 'q':
					case 'r':
					case 's':
					case 't':
					case 'u':
					case 'v':
					case 'w':
					case 'x':
					case 'y':
					case 'z':
						AdvanceChars();
						_builder.push_back(character);
						break;

					/* Invalid identifier characters - end of the identifier. */
					default:
						return start < _text;
				}
			}
		}

		/** Scans a identifier or keyword literal token. */
		CR_INL void ScanIdentifierOrKeywordLiteral(SyntaxTokenInfo& tokenInfo)
		{
			if (ScanIdentifier())
			{
				///////////////////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////////////////
				/// @todo Implement this without using any STL.
				static const std::map<std::string, SyntaxKind> keywordsMap = 
				{
#define KEYWORD(kw, ...) { #kw, SyntaxKind::Kw_ ## kw },
#define PPKEYWORD(kw, ...) { #kw, SyntaxKind::KwPp_ ## kw },
#include "LexerKeywords.hxx"
				};
				if (keywordsMap.count(_builder))
				{
					tokenInfo.Kind = keywordsMap.at(_builder);
				}
				else
				{
					tokenInfo.Kind = SyntaxKind::Identifier;
				}
				///////////////////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////////////////
			}
			else
			{
				/* Do not know why this branch even exists - Roslyn has it
				 * for some purpose. */
				assert(false);
				tokenInfo.Kind = SyntaxKind::None;
			}
		}

		// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
		// XX                 Internal trivia lexer.                    XX //
		// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //

		/** Adds trivia to the end of the builder list. */
		CR_INL static void AddTrivia(SyntaxNode& triviaBuilder, SyntaxNode const trivia)
		{
		//	assert(trivia != nullptr);

			if (triviaBuilder != nullptr)
			{
				trivia->_prevNode = triviaBuilder;
				triviaBuilder->_nextNode = trivia;
			}
			triviaBuilder = trivia;
		}

		/** Scans syntax trivia information. */
		CR_INL void ScanSyntaxTrivia(SyntaxNode& triviaBuilder, bool isTrailing)
		{
			while (true)
			{
				auto character = PeekChar();
				switch (character)
				{
					/* Whitespace. */
					case ' ':
					case '\t':
					case '\v':
					case '\f':
						AddTrivia(triviaBuilder, ScanWhitespace());
						break;

					/* Line break. */
					case '\r':
					case '\n':
						AddTrivia(triviaBuilder, ScanEndOfLine());
						if (isTrailing)
						{
							return;
						}
						break;

					/* Possible comments. */
					case '/':
						if ((character = PeekNextChar()) == '/')
						{
							if (!_suppressDocumentationCommentParse && PeekNextChar() == '/' && 
								PeekNextChar() != '/')
							{
								/* Documentation comments should not be a part of the
								 * trailing trivia. */
								if (isTrailing)
								{
									return;
								}

								AdvanceChars();
								AddTrivia(triviaBuilder, ScanXmlDocComment());
								break;
							}

							/* Normal single line comment. */
							AdvanceChars();
							ScanToEndOfLine();
							AddTrivia(triviaBuilder, SyntaxFactory::Comment(GetText()));
							break;
						}
						if (character == '*')
						{
							if (!_suppressDocumentationCommentParse && PeekNextChar() == '*' && 
								(character = PeekNextChar()) != '*' && character != '/')
							{
								/* Documentation comments should not be a part of the
								 * trailing trivia. */
								if (isTrailing)
								{
									return;
								}

								AdvanceChars();
								AddTrivia(triviaBuilder, ScanXmlDocComment());
								break;
							}

							/* Normal multiline line comment. */
							auto isTerminated = false;
							ScanMultiLineComment(isTerminated);
							if (!isTerminated)
							{
								AddError(Error::ErrorOpenEndedComment);
							}

							AddTrivia(triviaBuilder, SyntaxFactory::Comment(GetText()));
							break;
						}
						/* Not a trivia. */
						return;

					/* End of trivia. */
					default:
						return;
				}
			}
		}

		/** Scans whitespace trivia information. */
		CR_INL SyntaxTrivia ScanWhitespace()
		{
			while (true)
			{
				auto character = PeekChar();
				switch (character)
				{
					/* Whitespace. */
					case ' ':
					case '\t':
					case '\v':
					case '\f':
						AdvanceChars();
						break;

					/* Other character. */
					default:
						return SyntaxFactory::Whitespace();
				}
			}
		}

		/** Skips all characters until the end of line. */
		CR_INL void ScanToEndOfLine()
		{
			char character;
			while ((character = PeekChar()) != '\n' && character != EOF)
			{
				AdvanceChars();
			}
		}

		/** Scans end of line trivia information. */
		CR_INL SyntaxTrivia ScanEndOfLine()
		{
			auto const character = PeekChar();
			assert(character == '\r' || character == '\n');

			if (character == '\r')
			{
				AdvanceChars();
				if (PeekChar() == '\n')
				{
					AdvanceChars();
					return SyntaxFactory::CarriageReturnLineFeed();
				}
				return SyntaxFactory::CarriageReturn();
			}
			else
			{
				AdvanceChars();
				return SyntaxFactory::LineFeed();
			}
		}

		/** Scans multiline information. */
		CR_INL void ScanMultiLineComment(bool& isTerminated)
		{
			auto character = PeekChar();
			auto nextCharacter = PeekNextChar();
			assert(character == '/' && nextCharacter == '*');
			AdvanceChars();

			while (true)
			{
				if ((character = PeekChar()) == EOF)
				{
					isTerminated = false;
					break;
				}
				else if (character == '*' && PeekNextChar() == '/')
				{
					AdvanceChars();
					isTerminated = true;
					break;
				}
				else
				{
					AdvanceChars();
				}
			}
		}

		//////////////////////////////////////////////
		//////////////////////////////////////////////
		//////////////////////////////////////////////
		//////////////////////////////////////////////
		//////////////////////////////////////////////
		CR_INL SyntaxTrivia ScanXmlDocComment(...)
		{
			assert(false);
			return nullptr;
		}

	};	// class CASyntaxLexer

}	// namespace Cr

int main()
{
	auto const l = new Cr::CASyntaxLexer();
	auto t = l->Lex();
	return 0;
}

// ReSharper restore CppRedundantElseKeyword
// ReSharper restore CppRedundantElseKeywordInsideCompoundStatement
