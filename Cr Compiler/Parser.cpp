// *************************************************************** //
// Goddamn "C for Rendering" project.                              //
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.     //
//                                                                 //
// This software or any its part is distributed under terms of     //
// Goddamn Industries End User License Agreement. By downloading   //
// or using this software or any its part you agree with           //
// terms of Goddamn Industries End User License Agreement.         //
// *************************************************************** //

#include "Parser.h"
#include "Utils.h"

namespace Cr
{

	CRINL void Parser::ReadNextLexeme()
	{
		m_Lexeme = m_Scanner->GetNextLexeme();
	}

	CRAPI void Parser::ParseProgram() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);

		ReadNextLexeme();
		if (m_Lexeme.GetType() != Lexeme::Type::KwProgram)
		{
			throw ParserException("'program' expected.");
		}

		ReadNextLexeme();
		if (m_Lexeme.GetType() != Lexeme::Type::OpScopeBegin)
		{
			throw ParserException("'{' expected.");
		}

		while (true)
		{
			ReadNextLexeme();
			if (m_Lexeme.GetType() == Lexeme::Type::Null)
			{
				throw ParserException("'}' expected.");
			}
			if (m_Lexeme.GetType() == Lexeme::Type::OpScopeEnd)
			{
				break;
			}

			Parse_Statement();
		}
	}

	CRAPI void Parser::Parse_Statement() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		switch (m_Lexeme.GetType())
		{
			// *************************************************************** //
			case Lexeme::Type::OpSemicolon:
				ReadNextLexeme();
				Parse_Statement_Null();
				break;

			// *************************************************************** //
			case Lexeme::Type::KwInt:
			case Lexeme::Type::KwString:
			case Lexeme::Type::KwReal:
				ReadNextLexeme();
				Parse_Statement_Definition();
				break;

			default:
				CrAssert(0);
				break;
		}
	}

	CRAPI void Parser::Parse_Statement_Null() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
	}

	CRAPI void Parser::Parse_Statement_Definition() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		if (m_Lexeme.GetType() != Lexeme::Type::IdIdentifier)
		{
			throw ParserException("Identifier expected.");
		}

		auto const definitionName = m_Lexeme.GetValueID();
		CrLog(1, "declared: %s", definitionName.c_str());

		ReadNextLexeme();
		switch (m_Lexeme.GetType())
		{
			// Ending this definition.
			// *************************************************************** //
			case Lexeme::Type::OpSemicolon:
				return;

			// Reading another definition.
			// *************************************************************** //
			case Lexeme::Type::OpComma:
				ReadNextLexeme();
				Parse_Statement_Definition();
				return;

			// Reading initial value of this definition.
			// *************************************************************** //
			case Lexeme::Type::OpAssignment:
				ReadNextLexeme();
				Parse_Statement_Expression();
				return;

		default:
			throw ParserException("';', ',' or '= [expression]' expected.");
		}
	}

	// *************************************************************** //
	// **                    Expressions parsing.                   ** //
	// *************************************************************** //

	/**
	 * Parses all kinds of expressions.
	 * { Internally parses: <expression> [, <expression>] }
	 */
	CRAPI Parser::ExpressionType Parser::Parse_Statement_Expression() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		auto expressionType = Parse_Statement_Expression_Assignments();
		while (m_Lexeme.GetType() == Lexeme::Type::OpComma)
		{
			CrLog(1, ",");
			ReadNextLexeme();
			expressionType = Parse_Statement_Expression_Assignments();
		}
		return expressionType;
	}

	/**
	 * Parses all kinds of assigment expressions.
	 * { Internally parses: <expression> [ @= <expression> ] }
	 */
	CRAPI Parser::ExpressionType Parser::Parse_Statement_Expression_Assignments() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		Parse_Statement_Expression_Ternary();
		while (m_Lexeme.GetType() == Lexeme::Type::OpAssignment
			|| m_Lexeme.GetType() == Lexeme::Type::OpAddAssign || m_Lexeme.GetType() == Lexeme::Type::OpSubtractAssign
			|| m_Lexeme.GetType() == Lexeme::Type::OpMultiplyAssign || m_Lexeme.GetType() == Lexeme::Type::OpDivideAssign || m_Lexeme.GetType() == Lexeme::Type::OpModuloAssign
			|| m_Lexeme.GetType() == Lexeme::Type::OpBitwiseAndAssign || m_Lexeme.GetType() == Lexeme::Type::OpBitwiseOrAssign || m_Lexeme.GetType() == Lexeme::Type::OpBitwiseXorAssign
			|| m_Lexeme.GetType() == Lexeme::Type::OpBitwiseLeftShiftAssign || m_Lexeme.GetType() == Lexeme::Type::OpBitwiseRightShiftAssign)
		{
			CrLog(2, "@=");
			ReadNextLexeme();
			Parse_Statement_Expression_Ternary();
		}
	}

	/**
	 * Parses all kinds of ternary expressions.
	 * { Internally parses: <expression> [ ? <expression> : <expression> ] }
	 */
	CRAPI Parser::ExpressionType Parser::Parse_Statement_Expression_Ternary() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		Parse_Statement_Expression_Or();
		while (m_Lexeme.GetType() == Lexeme::Type::OpTernary)
		{
			CrLog(3, "?:");
			Parse_Statement_Expression();
			if (m_Lexeme.GetType() != Lexeme::Type::OpColon)
			{
				throw ParserException("':' expected in ternary operator.");
			}

			ReadNextLexeme();
			Parse_Statement_Expression();
		}
	}

	/**
	 * Parses all kinds of binary expressions.
	 * { Internally parses: <expression> [ @ <expression>] }
	 */
	CRAPI Parser::ExpressionType Parser::Parse_Statement_Expression_Or() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		Parse_Statement_Expression_And();
		while (m_Lexeme.GetType() == Lexeme::Type::OpOr)
		{
			CrLog(4, "||");
			ReadNextLexeme();
			Parse_Statement_Expression_And();
		}
	}
	// *************************************************************** //
	CRAPI Parser::ExpressionType Parser::Parse_Statement_Expression_And() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		Parse_Statement_Expression_BitwiseOr();
		while (m_Lexeme.GetType() == Lexeme::Type::OpAnd)
		{
			CrLog(5, "&&");
			ReadNextLexeme();
			Parse_Statement_Expression_BitwiseOr();
		}
	}
	// *************************************************************** //
	CRAPI Parser::ExpressionType Parser::Parse_Statement_Expression_BitwiseOr() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		Parse_Statement_Expression_BitwiseXor();
		while (m_Lexeme.GetType() == Lexeme::Type::OpBitwiseOr)
		{
			CrLog(6, "|");
			ReadNextLexeme();
			Parse_Statement_Expression_BitwiseXor();
		}
	}
	// *************************************************************** //
	CRAPI Parser::ExpressionType Parser::Parse_Statement_Expression_BitwiseXor() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		Parse_Statement_Expression_BitwiseAnd();
		while (m_Lexeme.GetType() == Lexeme::Type::OpBitwiseXor)
		{
			CrLog(7, "^");
			ReadNextLexeme();
			Parse_Statement_Expression_BitwiseAnd();
		}
	}
	// *************************************************************** //
	CRAPI Parser::ExpressionType Parser::Parse_Statement_Expression_BitwiseAnd() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		Parse_Statement_Expression_Equals_OR_NotEquals();
		while (m_Lexeme.GetType() == Lexeme::Type::OpBitwiseAnd)
		{
			CrLog(8, "&");
			ReadNextLexeme();
			Parse_Statement_Expression_Equals_OR_NotEquals();
		}
	}
	// *************************************************************** //
	CRAPI Parser::ExpressionType Parser::Parse_Statement_Expression_Equals_OR_NotEquals() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		Parse_Statement_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals();
		while (m_Lexeme.GetType() == Lexeme::Type::OpEquals || m_Lexeme.GetType() == Lexeme::Type::OpNotEquals)
		{
			CrLog(9, "&&_||");
			ReadNextLexeme();
			Parse_Statement_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals();
		}
	}
	// *************************************************************** //
	CRAPI Parser::ExpressionType Parser::Parse_Statement_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		Parse_Statement_Expression_BitwiseLeftShift_OR_BitwiseRightShift();
		while (m_Lexeme.GetType() == Lexeme::Type::OpLess || m_Lexeme.GetType() == Lexeme::Type::OpBitwiseRightShift
			|| m_Lexeme.GetType() == Lexeme::Type::OpGreater || m_Lexeme.GetType() == Lexeme::Type::OpGreaterEquals)
		{
			CrLog(10, "<_<=_>_>=");
			ReadNextLexeme();
			Parse_Statement_Expression_BitwiseLeftShift_OR_BitwiseRightShift();
		}
	}
	// *************************************************************** //
	CRAPI Parser::ExpressionType Parser::Parse_Statement_Expression_BitwiseLeftShift_OR_BitwiseRightShift() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		Parse_Statement_Expression_Add_OR_Subtract();
		while (m_Lexeme.GetType() == Lexeme::Type::OpBitwiseLeftShift || m_Lexeme.GetType() == Lexeme::Type::OpBitwiseRightShift)
		{
			CrLog(11, "<<_>>");
			ReadNextLexeme();
			Parse_Statement_Expression_Add_OR_Subtract();
		}
	}
	// *************************************************************** //
	CRAPI Parser::ExpressionType Parser::Parse_Statement_Expression_Add_OR_Subtract() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		auto expressionType = Parse_Statement_Expression_Multiply_OR_Divide_OR_Modulo();
		while (m_Lexeme.GetType() == Lexeme::Type::OpAdd 
			|| m_Lexeme.GetType() == Lexeme::Type::OpSubtract)
		{
			CrLog(12, "+_-");
			ReadNextLexeme();
		
			// We can add or subtract variables of same dimensions only.
			auto const rhsExpressionType = Parse_Statement_Expression_Multiply_OR_Divide_OR_Modulo();
			if (!expressionType.MatchDimensions(rhsExpressionType))
			{
				throw ParserException("Dimensions mismatch in '+-' operators.");
			}

			expressionType = rhsExpressionType;
		}
		return expressionType;
	}
	// *************************************************************** //
	CRAPI Parser::ExpressionType Parser::Parse_Statement_Expression_Multiply_OR_Divide_OR_Modulo() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		auto expressionType = Parse_Statement_Expression_UNARY();
		while (m_Lexeme.GetType() == Lexeme::Type::OpMultiply || m_Lexeme.GetType() == Lexeme::Type::OpDivide
			|| m_Lexeme.GetType() == Lexeme::Type::OpModulo)
		{
			CrLog(13, "*_/_%%");
			ReadNextLexeme();

			// We can multiply matrices with coulumn amount of first equal to 
			auto const rhsExpressionType = Parse_Statement_Expression_UNARY();
		}
		return expressionType;
	}

	/**
	 * Parses all kinds of unary expressions.
	 * { Internally parses: [@]<expression> }
	 */
	CRAPI Parser::ExpressionType Parser::Parse_Statement_Expression_UNARY() throw(ParserException)
	{
		//! @todo Fix up everything here.
		//! @todo Add cast expressions.
		CrLog(0, __FUNCSIG__);
		return Parse_Statement_Expression_OPERAND();
		/*while (m_Lexeme.GetType() == Lexeme::Type::OpAdd || m_Lexeme.GetType() == Lexeme::Type::OpSubtract
			|| m_Lexeme.GetType() == Lexeme::Type::OpNot || m_Lexeme.GetType() == Lexeme::Type::OpBitwiseNot
			|| m_Lexeme.GetType() == Lexeme::Type::OpInc || m_Lexeme.GetType() == Lexeme::Type::OpDec)
		{
			CrLog(14, "+_-_!_~_++_--");
			ReadNextLexeme();
			Parse_Statement_Expression_OPERAND();
			break;
		}*/
	}

	CRAPI Parser::ExpressionType Parser::Parse_Statement_Expression_OPERAND() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		switch (m_Lexeme.GetType())
		{
			case Lexeme::Type::IdIdentifier:
				CrAssert(0);
				ValidateIdentifier();
				CrLog(1, "id"); // prog.put_lex (Lex (LEX_ID, c_val));
				ReadNextLexeme();
				break;

			case Lexeme::Type::CtInteger:
			case Lexeme::Type::CtReal:
			case Lexeme::Type::CtString:
				CrLog(1, "ct"); // prog.put_lex (Lex (LEX_ID, c_val));
				ReadNextLexeme();
				return ExpressionType((ExpressionBaseType)m_Lexeme.GetType());

			case Lexeme::Type::OpParenthesesBegin:
				{
					ReadNextLexeme();
					auto const parenthessedExpression = Parse_Statement_Expression();
					if (m_Lexeme.GetType() != Lexeme::Type::OpParenthesesEnd)
					{
						throw ParserException("')' expected");
					}
					ReadNextLexeme();
					return parenthessedExpression;
				}

			default:
				throw ParserException("Unexpected lexeme in the stream.");
		}
	}

	// *************************************************************** //
	// *************************************************************** //

	CrUnitTest(ScannerEmptyStream)
	{
		Parser parser(new Scanner(std::make_shared<IO::StringInputStream>("program { int a = (3 - \"aa\") * b += 1; real b, c; }")));
		parser.ParseProgram();
		int i = 0;
	};

}	// namespace Cr
