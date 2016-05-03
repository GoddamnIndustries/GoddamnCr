// *************************************************************** //
// Goddamn "C for Rendering" project.                              //
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.     //
//                                                                 //
// This software or any its part is distributed under terms of     //
// Goddamn Industries End User License Agreement. By downloading   //
// or using this software or any its part you agree with           //
// terms of Goddamn Industries End User License Agreement.         //
// *************************************************************** //

#include <iostream>
#include "Parser.h"
#include "Profile.h"
#include "Utils.h"
#include <vector>

namespace Cr
{

	CRINL void Parser::ReadNextLexeme()
	{
		m_Lexeme = m_Scanner->GetNextLexeme();
	}

	CRINL void Parser::ExpectLexemeAndReadNext(Lexeme::Type const type)
	{
		if (m_Lexeme != type)
		{
			throw ParserException("something was expected.");
		}
		ReadNextLexeme();
	}

	CRAPI void Parser::ParseProgram() throw(ParserException)
	{
		m_Profile = new Profile();
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
			if (m_Lexeme == Lexeme::Type::Null)
			{
				throw ParserException("'}' expected.");
			}
			if (m_Lexeme == Lexeme::Type::OpScopeEnd)
			{
				break;
			}

			auto s = Parse_Statement();
			s = nullptr;
		}
	}

	// *************************************************************** //
	// **                    Expressions parsing.                   ** //
	// *************************************************************** //

	/**
	 * Parses all kinds of expressions.
	 * { Internally parses: <expression> [, <expression>] }
	 */
	CRAPI Ast::Expression* Parser::Parse_Expression() throw(ParserException)
	{
		auto expression = Parse_Expression_Assignments();
		while (m_Lexeme == Lexeme::Type::OpComma)
		{
			CrLog(1, ",");
			ReadNextLexeme();
			expression = m_Profile->CreateCommaExpression(expression, Parse_Expression_Assignments());
		}
		return expression;
	}

	/**
	 * Parses all kinds of assignment expressions.
	 * { Internally parses: <expression> [ @= <expression> ] }
	 */
	CRAPI Ast::Expression* Parser::Parse_Expression_Assignments() throw(ParserException)
	{
		auto expression = Parse_Expression_Ternary();
		while (true)
		{
			switch (m_Lexeme.GetType())
			{
				case Lexeme::Type::OpAssignment:
					CrLog(2, "=");
					expression = m_Profile->CreateAssignmentBinaryExpression(expression, Parse_Expression_Ternary());
					break;

				case Lexeme::Type::OpAddAssign: case Lexeme::Type::OpSubtractAssign:
				case Lexeme::Type::OpMultiplyAssign: case Lexeme::Type::OpDivideAssign: case Lexeme::Type::OpModuloAssign:
					CrLog(2, "+=_-=_*=_/=_%%=");
					expression = m_Profile->CreateArithmeticAssignmentBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_Ternary());
					break;

				case Lexeme::Type::OpBitwiseAndAssign: case Lexeme::Type::OpBitwiseOrAssign: case Lexeme::Type::OpBitwiseXorAssign:
				case Lexeme::Type::OpBitwiseLeftShiftAssign: case Lexeme::Type::OpBitwiseRightShiftAssign:
					CrLog(2, "&=_|=_^=_<<=_>>=");
					expression = m_Profile->CreateBitwiseAssignmentBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_Ternary());
					break;

				default:
					return expression;
			}
		}
	}

	// --------------------------------------------------------------- //
	// --                Ternary expression parsing.                -- //
	// --------------------------------------------------------------- //

	/**
	 * Parses all kinds of ternary expressions.
	 * { Internally parses: <expression> [ ? <expression> : <expression> ] }
	 */
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_Ternary() throw(ParserException)
	{
		auto expression = Parse_Expression_Or();
		while (m_Lexeme == Lexeme::Type::OpTernary)
		{
			CrLog(3, "?:");
			auto const trueBranchExpression = Parse_Expression();
			if (m_Lexeme.GetType() != Lexeme::Type::OpColon)
			{
				throw ParserException("':' expected in ternary operator.");
			}

			ReadNextLexeme();
			auto const falseBranchExpression = Parse_Expression();
			expression = m_Profile->CreateTernaryExpression(expression, trueBranchExpression, falseBranchExpression);
		}
		return expression;
	}

	// --------------------------------------------------------------- //
	// --                 Binary expression parsing.                -- //
	// --------------------------------------------------------------- //

	/**
	 * Parses all kinds of binary expressions.
	 * { Internally parses: <expression> [ @ <expression>] }
	 */
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_Or() throw(ParserException)
	{
		auto expression = Parse_Expression_And();
		while (m_Lexeme == Lexeme::Type::OpOr)
		{
			CrLog(4, "||");
			ReadNextLexeme();
			expression = m_Profile->CreateLogicBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_And());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_And() throw(ParserException)
	{
		auto expression = Parse_Expression_BitwiseOr();
		while (m_Lexeme == Lexeme::Type::OpAnd)
		{
			CrLog(5, "&&");
			ReadNextLexeme();
			expression = m_Profile->CreateLogicBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_BitwiseOr());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_BitwiseOr() throw(ParserException)
	{
		auto expression = Parse_Expression_BitwiseXor();
		while (m_Lexeme == Lexeme::Type::OpBitwiseOr)
		{
			CrLog(6, "|");
			ReadNextLexeme();
			expression = m_Profile->CreateBitwiseBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_BitwiseXor());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_BitwiseXor() throw(ParserException)
	{
		auto expression = Parse_Expression_BitwiseAnd();
		while (m_Lexeme == Lexeme::Type::OpBitwiseXor)
		{
			CrLog(7, "^");
			ReadNextLexeme();
			expression = m_Profile->CreateBitwiseBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_BitwiseAnd());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_BitwiseAnd() throw(ParserException)
	{
		auto expression = Parse_Expression_Equals_OR_NotEquals();
		while (m_Lexeme == Lexeme::Type::OpBitwiseAnd)
		{
			CrLog(8, "&");
			ReadNextLexeme();
			expression = m_Profile->CreateBitwiseBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_Equals_OR_NotEquals());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_Equals_OR_NotEquals() throw(ParserException)
	{
		auto expression = Parse_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals();
		while (m_Lexeme == Lexeme::Type::OpEquals || m_Lexeme == Lexeme::Type::OpNotEquals)
		{
			CrLog(9, "&&_||");
			ReadNextLexeme();
			expression = m_Profile->CreateLogicBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals() throw(ParserException)
	{
		auto expression = Parse_Expression_BitwiseLeftShift_OR_BitwiseRightShift();
		while (m_Lexeme == Lexeme::Type::OpLess || m_Lexeme == Lexeme::Type::OpLessEquals
			|| m_Lexeme == Lexeme::Type::OpGreater || m_Lexeme == Lexeme::Type::OpGreaterEquals)
		{
			CrLog(10, "<_<=_>_>=");
			ReadNextLexeme();
			expression = m_Profile->CreateLogicBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_BitwiseLeftShift_OR_BitwiseRightShift());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_BitwiseLeftShift_OR_BitwiseRightShift() throw(ParserException)
	{
		auto expression = Parse_Expression_Add_OR_Subtract();
		while (m_Lexeme == Lexeme::Type::OpBitwiseLeftShift || m_Lexeme == Lexeme::Type::OpBitwiseRightShift)
		{
			CrLog(11, "<<_>>");
			ReadNextLexeme();
			expression = m_Profile->CreateBitwiseBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_Add_OR_Subtract());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_Add_OR_Subtract() throw(ParserException)
	{
		auto expression = Parse_Expression_Multiply_OR_Divide_OR_Modulo();
		while (m_Lexeme == Lexeme::Type::OpAdd || m_Lexeme == Lexeme::Type::OpSubtract)
		{
			CrLog(12, "+_-");
			ReadNextLexeme();
			expression = m_Profile->CreateArithmeticBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_Multiply_OR_Divide_OR_Modulo());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_Multiply_OR_Divide_OR_Modulo() throw(ParserException)
	{
		auto expression = Parse_Expression_PrefixUnary();
		while (m_Lexeme == Lexeme::Type::OpMultiply || m_Lexeme == Lexeme::Type::OpDivide || m_Lexeme == Lexeme::Type::OpModulo)
		{
			CrLog(13, "*_/_%%");
			ReadNextLexeme();
			expression = m_Profile->CreateArithmeticBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_PrefixUnary());
		}
		return expression;
	}

	// --------------------------------------------------------------- //
	// --                  Unary expression parsing.                -- //
	// --------------------------------------------------------------- //

	/**
	 * Parses all kinds of unary expressions.
	 * { Internally parses: [@]<expression> }
	 */
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_PrefixUnary() throw(ParserException)
	{
		//! @todo Add cast expressions.
		//! @todo Add 'sizeof' expressions.
		switch (m_Lexeme.GetType())
		{
			case Lexeme::Type::OpAdd:
			case Lexeme::Type::OpSubtract:
				CrLog(14, "+_-");
				ReadNextLexeme();
				return m_Profile->CreateArithmeticUnaryExpression(m_Lexeme.GetType(), Parse_Expression_OPERAND());

			case Lexeme::Type::OpInc:
			case Lexeme::Type::OpDec:
				CrLog(14, "++_--");
				CrAssert(0);
				ReadNextLexeme();
				return m_Profile->CreateArithmeticUnaryExpression(m_Lexeme.GetType(), Parse_Expression_OPERAND());

			case Lexeme::Type::OpNot:
				CrLog(14, "!");
				ReadNextLexeme();
				return m_Profile->CreateLogicUnaryExpression(m_Lexeme.GetType(), Parse_Expression_OPERAND());

			case Lexeme::Type::OpBitwiseNot:
				CrLog(14, "~");
				ReadNextLexeme();
				return m_Profile->CreateBitwiseUnaryExpression(m_Lexeme.GetType(), Parse_Expression_OPERAND());

			default:
				return Parse_Expression_OPERAND();
		}
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_OPERAND() throw(ParserException)
	{
		switch (m_Lexeme.GetType())
		{
			case Lexeme::Type::IdIdentifier:
				CrLog(15, "ID");
				ReadNextLexeme();
				break;

			case Lexeme::Type::CtInteger:
			case Lexeme::Type::CtReal:
			case Lexeme::Type::CtString:
				CrLog(15, "ct"); // prog.put_lex (Lex (LEX_ID, c_val));
				ReadNextLexeme();
				return /*new Ast::OperandExpression()*/nullptr;

			case Lexeme::Type::OpParenthesesBegin:
				{
					ReadNextLexeme();
					auto const parenthesesExpr = Parse_Expression();
					ExpectLexemeAndReadNext(Lexeme::Type::OpParenthesesEnd);
					return parenthesesExpr;
				}

			default:
				throw ParserException("Unexpected lexeme in the stream.");
		}
	}

	// *************************************************************** //
	// **                     Statements parsing.                   ** //
	// *************************************************************** //

	/**
	 * Parses all kinds of statements.
	 */
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement() throw(ParserException)
	{
		switch (m_Lexeme.GetType())
		{
			// Null statement.
			// *************************************************************** //
			case Lexeme::Type::OpSemicolon:
				return Parse_Statement_Null();

			// Compound statement.
			// *************************************************************** //
			case Lexeme::Type::OpScopeBegin:
				ReadNextLexeme();
				return Parse_Statement_Compound();

			// Selection statement.
			// *************************************************************** //
			case Lexeme::Type::KwIf:
				ReadNextLexeme();
				return Parse_Statement_Selection_If();

			// Iteration statement.
			// *************************************************************** //
			case Lexeme::Type::KwDo:
				ReadNextLexeme();
				return Parse_Statement_Iteration_Do();
			case Lexeme::Type::KwWhile:
				ReadNextLexeme();
				return Parse_Statement_Iteration_While();
			case Lexeme::Type::KwFor:
				ReadNextLexeme();
				return Parse_Statement_Iteration_For();

			// Jump statement.
			// *************************************************************** //
			case Lexeme::Type::KwBreak:
				ReadNextLexeme();
				return Parse_Statement_Jump_Break();
			case Lexeme::Type::KwContinue:
				ReadNextLexeme();
				return Parse_Statement_Jump_Continue();
			case Lexeme::Type::KwReturn:
				ReadNextLexeme();
				return Parse_Statement_Jump_Return();
			case Lexeme::Type::KwDiscard:
				ReadNextLexeme();
				return Parse_Statement_Jump_Return();

			// Declaration or Expression statement.
			// *************************************************************** //
			case Lexeme::Type::KwTypedef:
				ReadNextLexeme();
				return Parse_Statement_Declaration_Typedef();
			case Lexeme::Type::KwStruct:
				ReadNextLexeme();
				return Parse_Statement_Declaration_Struct();
			default:
				return Parse_Statement_Declaration_OR_Expression();
		}
	}

	/**
	 * Parses null statements.
	 * { Examples: ; }
	 */
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Null() throw(ParserException)
	{
		ReadNextLexeme();
		return m_Profile->CreateNullStatement();
	}

	// --------------------------------------------------------------- //
	// --                Compound statement parsing.                -- //
	// --------------------------------------------------------------- //

	/**
	 * Parses compound statements.
	 * { Examples: { [statement] [statement] ... } }
	 */
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Compound() throw(ParserException)
	{
		m_ScopedIdentifiers.emplace_back();
		CrAssert(0);

		std::vector<std::unique_ptr<Ast::Statement>> statements;
		while (m_Lexeme != Lexeme::Type::OpScopeEnd)
		{
			statements.emplace_back(Parse_Statement());
		}

		ReadNextLexeme();
		m_ScopedIdentifiers.pop_back();
		return m_Profile->CreateCompoundStatement(std::move(statements));
	}

	// --------------------------------------------------------------- //
	// --               Selection statement parsing.                -- //
	// --------------------------------------------------------------- //

	/**
	 * Parses compound statements.
	 * { Examples: if (<expression>) <statement> [else <statement>] }
	 */
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Selection_If() throw(ParserException)
	{
		ExpectLexemeAndReadNext(Lexeme::Type::OpParenthesesBegin);

		// { if (<expression>) <statement> [else <statement>] }
		//   ----~~~~~~~~~~~~--------------------------------
		auto const conditionExpr = Parse_Expression();

		ExpectLexemeAndReadNext(Lexeme::Type::OpParenthesesEnd);

		// { if (<expression>) <statement> [else <statement>] }
		//   ------------------~~~~~~~~~~~-------------------
		auto const trueBranchStatement = Parse_Statement();

		// { if (<expression>) <statement> else [<statement>] }
		//   -----------------------------------~~~~~~~~~~~~~
		Ast::Statement* falseBranchStatement = nullptr;
		if (m_Lexeme == Lexeme::Type::KwElse)
		{
			ReadNextLexeme();
			falseBranchStatement = Parse_Statement();
		}
		return m_Profile->CreateIfSelectionStatement(conditionExpr, trueBranchStatement, falseBranchStatement);
	}

	// --------------------------------------------------------------- //
	// --               Iteration statement parsing.                -- //
	// --------------------------------------------------------------- //

	/**
	 * Parses iteration statements.
	 * / Examples: do <statement> while (<expression>);                         \
	 * |           for (<statement> [<expression>]; [<expression>]) <statement> |
	 * \           while (<expression>) <statement>                             /
	 */
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Iteration_While() throw(ParserException)
	{
		auto const whileIterationStatement = m_Profile->CreateWhileIterationStatement();
		m_IterationStatements.push_back(whileIterationStatement);

		ExpectLexemeAndReadNext(Lexeme::Type::OpParenthesesBegin);
		
		// while (<expression>) <statement>
		// -------~~~~~~~~~~~~-------------
		auto const conditionExpr = Parse_Expression();
		
		ExpectLexemeAndReadNext(Lexeme::Type::OpParenthesesEnd);

		// while (<expression>) <statement>
		// ---------------------~~~~~~~~~~~
		auto const loopStatement = Parse_Statement();

		m_IterationStatements.pop_back();
		whileIterationStatement->Initialize(conditionExpr, loopStatement);
		return whileIterationStatement;
	}
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Iteration_Do() throw(ParserException)
	{
		auto const doWhileIterationStatement = m_Profile->CreateDoIterationStatement();
		m_IterationStatements.push_back(doWhileIterationStatement);

		// do <statement> while (<expression>);
		// ---~~~~~~~~~~~----------------------
		auto const loopStatement = Parse_Statement();

		ExpectLexemeAndReadNext(Lexeme::Type::KwWhile);
		ExpectLexemeAndReadNext(Lexeme::Type::OpParenthesesBegin);
		
		// do <statement> while (<expression>);
		// ----------------------~~~~~~~~~~~~--
		auto const conditionExpr = Parse_Expression();
		
		ExpectLexemeAndReadNext(Lexeme::Type::OpParenthesesEnd);
		ExpectLexemeAndReadNext(Lexeme::Type::OpSemicolon);
		
		m_IterationStatements.pop_back();
		doWhileIterationStatement->Initialize(loopStatement, conditionExpr);
		return doWhileIterationStatement;
	}
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Iteration_For() throw(ParserException)
	{
		auto const forIterationStatement = m_Profile->CreateForIterationStatement();
		m_IterationStatements.push_back(forIterationStatement);
		m_ScopedIdentifiers.emplace_back();

		ExpectLexemeAndReadNext(Lexeme::Type::OpParenthesesBegin);
		
		// for (<statement> [<expression>]; [<expression>]) <statement>
		// -----~~~~~~~~~~~-< null, declaration or expression. >-------
		auto const initStatement = Parse_Statement();
		if (dynamic_cast<Ast::NullStatement*>(initStatement) == nullptr &&
			dynamic_cast<Ast::DeclarationStatement*>(initStatement) == nullptr &&
			dynamic_cast<Ast::ExpressionStatement*>(initStatement) == nullptr)
		{
			throw ParserException("Declaration or expression statement is expected in initialization of 'for' statement.");
		}
		
		// for (<statement> [<expression>]; [<expression>]) <statement>
		// -----------------~~~~~~~~~~~~~~-----------------------------
		Ast::Expression* conditionExpr = nullptr;
		if (m_Lexeme != Lexeme::Type::OpSemicolon)
		{
			conditionExpr = Parse_Expression();
			ExpectLexemeAndReadNext(Lexeme::Type::OpSemicolon);
		}
		else
		{
			ReadNextLexeme();
		}

		// for (<statement> [<expression>]; [<expression>]) <statement>
		// ---------------------------------~~~~~~~~~~~~~~-------------
		Ast::Expression* iterationExpr = nullptr;
		if (m_Lexeme != Lexeme::Type::OpParenthesesEnd)
		{
			iterationExpr = Parse_Expression();
			ExpectLexemeAndReadNext(Lexeme::Type::OpParenthesesEnd);
		}
		else
		{
			ReadNextLexeme();
		}

		// for (<statement> [<expression>]; [<expression>]) <statement>
		// -------------------------------------------------~~~~~~~~~~~
		auto const loopStatement = Parse_Statement();
		
		m_ScopedIdentifiers.pop_back();
		m_IterationStatements.pop_back();
		forIterationStatement->Initialize(initStatement, conditionExpr, iterationExpr, loopStatement);
		return forIterationStatement;
	}
	
	// --------------------------------------------------------------- //
	// --                  Jump statement parsing.                  -- //
	// --------------------------------------------------------------- //

	/**
	 * Parses jump statements.
	 * / Examples: break;                 \
	 * |           discard;               |
	 * |           continue;              |
	 * \           return [<expression>]; /
	 */
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Jump_Break() throw(ParserException)
	{
		if (m_IterationStatements.empty())
		{
			throw ParserException("'break' statement outside of any loop.");
		}
		ExpectLexemeAndReadNext(Lexeme::Type::OpSemicolon);
		return m_Profile->CreateBreakJumpStatement(m_IterationStatements.back());
	}
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Jump_Continue() throw(ParserException)
	{
		if (m_IterationStatements.empty())
		{
			throw ParserException("'continue' statement outside of any loop.");
		}
		ExpectLexemeAndReadNext(Lexeme::Type::OpSemicolon);
		return m_Profile->CreateContinueJumpStatement(m_IterationStatements.back());
	}
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Jump_Return() throw(ParserException)
	{
		// return [<expression>]; 
		// -------~~~~~~~~~~~~~~-
		Ast::Expression* expression = nullptr;
		if (m_Lexeme != Lexeme::Type::OpSemicolon)
		{
			expression = Parse_Expression();
			ExpectLexemeAndReadNext(Lexeme::Type::OpSemicolon);
		}
		else
		{
			ReadNextLexeme();
		}
		return m_Profile->CreateReturnJumpStatement(m_Function, expression);
	}
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Jump_Discard() throw(ParserException)
	{
		ExpectLexemeAndReadNext(Lexeme::Type::OpSemicolon);
		return m_Profile->CreateDiscardJumpStatement();
	}
	
	// --------------------------------------------------------------- //
	// --        Declaration & Expression statement parsing.        -- //
	// --------------------------------------------------------------- //

	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Declaration_OR_Expression() throw(ParserException)
	{
		CrAssert(0);
		return nullptr;
	}
	
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Declaration() throw(ParserException)
	{
		CrAssert(0);
		return nullptr;
	}
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Declaration_Typedef() throw(ParserException)
	{
		CrAssert(0);
		return nullptr;
	}
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Declaration_Struct() throw(ParserException)
	{
		CrAssert(0);
		return nullptr;
	}

	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Expression() throw(ParserException)
	{
		CrAssert(0);
		return nullptr;
	}

	// *************************************************************** //
	// *************************************************************** //

#if 0
	CRAPI Ast::Statement* Parser::Parse_Statement_Definition() throw(ParserException)
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
				{
					auto expression = Parse_Expression();
					expression->EvaluateType();
					return;
				}

			default:
				throw ParserException("';', ',' or '= [expression]' expected.");
		}
	}
#endif

	CrUnitTest(ScannerEmptyStream)
	{
		Parser parser(new Scanner(std::make_shared<IO::StringInputStream>("program { for(if (1); 3 < 2; ~0) continue; }")));
		parser.ParseProgram();
		int i = 0;
	};

}	// namespace Cr
