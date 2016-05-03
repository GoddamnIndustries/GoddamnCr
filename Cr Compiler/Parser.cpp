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
#include <algorithm>

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

	CRINL Ast::Identifier* Parser::FindIdentifier(std::string const& name) const
	{
		for (auto iter = m_ScopedIdentifiers.crbegin(); iter != m_ScopedIdentifiers.crend(); ++iter)
		{
			if (iter->count(name) != 0)
			{
				return iter->at(name);
			}
		}
		return nullptr;
	}

	CRAPI void Parser::ParseProgram()
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
	CRAPI Ast::Expression* Parser::Parse_Expression()
	{
		auto expression = Parse_Expression_Assignments();
		while (m_Lexeme == Lexeme::Type::OpComma)
		{
			ReadNextLexeme();
			expression = m_Profile->CreateCommaExpression(expression, Parse_Expression_Assignments());
		}
		return expression;
	}

	/**
	 * Parses all kinds of assignment expressions.
	 * { Internally parses: <expression> [ @= <expression> ] }
	 */
	CRAPI Ast::Expression* Parser::Parse_Expression_Assignments()
	{
		auto expression = Parse_Expression_Ternary();
		while (true)
		{
			switch (m_Lexeme.GetType())
			{
				case Lexeme::Type::OpAssignment:
					expression = m_Profile->CreateAssignmentBinaryExpression(expression, Parse_Expression_Ternary());
					break;

				case Lexeme::Type::OpAddAssign: case Lexeme::Type::OpSubtractAssign:
				case Lexeme::Type::OpMultiplyAssign: case Lexeme::Type::OpDivideAssign: case Lexeme::Type::OpModuloAssign:
					expression = m_Profile->CreateArithmeticAssignmentBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_Ternary());
					break;

				case Lexeme::Type::OpBitwiseAndAssign: case Lexeme::Type::OpBitwiseOrAssign: case Lexeme::Type::OpBitwiseXorAssign:
				case Lexeme::Type::OpBitwiseLeftShiftAssign: case Lexeme::Type::OpBitwiseRightShiftAssign:
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
	CRAPI Ast::Expression* Parser::Parse_Expression_Ternary()
	{
		auto expression = Parse_Expression_Or();
		while (m_Lexeme == Lexeme::Type::OpTernary)
		{
			// <expression> ? <expression> : <expression>
			// ---------------~~~~~~~~~~~~---------------
			auto const trueBranchExpression = Parse_Expression();

			ExpectLexemeAndReadNext(Lexeme::Type::OpColon);

			// <expression> ? <expression> : <expression>
			// ------------------------------~~~~~~~~~~~~
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
	CRAPI Ast::Expression* Parser::Parse_Expression_Or()
	{
		auto expression = Parse_Expression_And();
		while (m_Lexeme == Lexeme::Type::OpOr)
		{
			ReadNextLexeme();
			expression = m_Profile->CreateLogicBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_And());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_And()
	{
		auto expression = Parse_Expression_BitwiseOr();
		while (m_Lexeme == Lexeme::Type::OpAnd)
		{
			ReadNextLexeme();
			expression = m_Profile->CreateLogicBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_BitwiseOr());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_BitwiseOr()
	{
		auto expression = Parse_Expression_BitwiseXor();
		while (m_Lexeme == Lexeme::Type::OpBitwiseOr)
		{
			ReadNextLexeme();
			expression = m_Profile->CreateBitwiseBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_BitwiseXor());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_BitwiseXor()
	{
		auto expression = Parse_Expression_BitwiseAnd();
		while (m_Lexeme == Lexeme::Type::OpBitwiseXor)
		{
			ReadNextLexeme();
			expression = m_Profile->CreateBitwiseBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_BitwiseAnd());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_BitwiseAnd()
	{
		auto expression = Parse_Expression_Equals_OR_NotEquals();
		while (m_Lexeme == Lexeme::Type::OpBitwiseAnd)
		{
			ReadNextLexeme();
			expression = m_Profile->CreateBitwiseBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_Equals_OR_NotEquals());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_Equals_OR_NotEquals()
	{
		auto expression = Parse_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals();
		while (m_Lexeme == Lexeme::Type::OpEquals || m_Lexeme == Lexeme::Type::OpNotEquals)
		{
			ReadNextLexeme();
			expression = m_Profile->CreateLogicBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals()
	{
		auto expression = Parse_Expression_BitwiseLeftShift_OR_BitwiseRightShift();
		while (m_Lexeme == Lexeme::Type::OpLess || m_Lexeme == Lexeme::Type::OpLessEquals
			|| m_Lexeme == Lexeme::Type::OpGreater || m_Lexeme == Lexeme::Type::OpGreaterEquals)
		{
			ReadNextLexeme();
			expression = m_Profile->CreateLogicBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_BitwiseLeftShift_OR_BitwiseRightShift());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_BitwiseLeftShift_OR_BitwiseRightShift()
	{
		auto expression = Parse_Expression_Add_OR_Subtract();
		while (m_Lexeme == Lexeme::Type::OpBitwiseLeftShift || m_Lexeme == Lexeme::Type::OpBitwiseRightShift)
		{
			ReadNextLexeme();
			expression = m_Profile->CreateBitwiseBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_Add_OR_Subtract());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_Add_OR_Subtract()
	{
		auto expression = Parse_Expression_Multiply_OR_Divide_OR_Modulo();
		while (m_Lexeme == Lexeme::Type::OpAdd || m_Lexeme == Lexeme::Type::OpSubtract)
		{
			ReadNextLexeme();
			expression = m_Profile->CreateArithmeticBinaryExpression(m_Lexeme.GetType(), expression, Parse_Expression_Multiply_OR_Divide_OR_Modulo());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_Multiply_OR_Divide_OR_Modulo()
	{
		auto expression = Parse_Expression_PrefixUnary();
		while (m_Lexeme == Lexeme::Type::OpMultiply || m_Lexeme == Lexeme::Type::OpDivide || m_Lexeme == Lexeme::Type::OpModulo)
		{
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
	CRAPI Ast::Expression* Parser::Parse_Expression_PrefixUnary()
	{
		//! @todo Add cast expressions.
		//! @todo Add 'sizeof' expressions.
		switch (m_Lexeme.GetType())
		{
			case Lexeme::Type::OpAdd:
			case Lexeme::Type::OpSubtract:
				ReadNextLexeme();
				return m_Profile->CreateArithmeticUnaryExpression(m_Lexeme.GetType(), Parse_Expression_OPERAND());

			case Lexeme::Type::OpInc:
			case Lexeme::Type::OpDec:
				CrAssert(0);
				ReadNextLexeme();
				return m_Profile->CreateArithmeticUnaryExpression(m_Lexeme.GetType(), Parse_Expression_OPERAND());

			case Lexeme::Type::OpNot:
				ReadNextLexeme();
				return m_Profile->CreateLogicUnaryExpression(m_Lexeme.GetType(), Parse_Expression_OPERAND());

			case Lexeme::Type::OpBitwiseNot:
				ReadNextLexeme();
				return m_Profile->CreateBitwiseUnaryExpression(m_Lexeme.GetType(), Parse_Expression_OPERAND());

			default:
				return Parse_Expression_OPERAND();
		}
	}
	// *************************************************************** //
	CRAPI Ast::Expression* Parser::Parse_Expression_OPERAND()
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
	CRAPI Ast::Statement* Parser::Parse_Statement()
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
			default:
				return Parse_Statement_Declaration_OR_Expression();
		}
	}

	/**
	 * Parses NULL statements.
	 * { NULL ::= ; }
	 */
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Null()
	{
		ReadNextLexeme();
		return m_Profile->CreateNullStatement();
	}

	/**
	 * Parses COUMPOUND statements.
	 * { COUMPOUND ::= { [statement] [statement] ... } }
	 */
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Compound()
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
	 * Parses IF-ELSE compound statement.
	 * { IF-ELSE ::= if (<expression>) <statement> [else <statement>] }
	 */
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Selection_If()
	{
		ExpectLexemeAndReadNext(Lexeme::Type::OpParenthesesBegin);

		// if (<expression>) <statement> [else <statement>]
		// ----~~~~~~~~~~~~--------------------------------
		auto const conditionExpr = Parse_Expression();

		ExpectLexemeAndReadNext(Lexeme::Type::OpParenthesesEnd);

		// if (<expression>) <statement> [else <statement>]
		// ------------------~~~~~~~~~~~-------------------
		auto const trueBranchStatement = Parse_Statement();

		// if (<expression>) <statement> [else <statement>]
		// ------------------------------------~~~~~~~~~~~-
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
	 * Parses WHILE iteration statement.
	 * { WHILE ::= while (<expression>) <statement> }
	 */
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Iteration_While()
	{
		auto const whileIterationStatement = m_Profile->CreateWhileIterationStatement();
		m_IterationStatements.push(whileIterationStatement);

		ExpectLexemeAndReadNext(Lexeme::Type::OpParenthesesBegin);
		
		// while (<expression>) <statement>
		// -------~~~~~~~~~~~~-------------
		auto const conditionExpr = Parse_Expression();
		
		ExpectLexemeAndReadNext(Lexeme::Type::OpParenthesesEnd);

		// while (<expression>) <statement>
		// ---------------------~~~~~~~~~~~
		auto const loopStatement = Parse_Statement();

		m_IterationStatements.pop();
		whileIterationStatement->Initialize(conditionExpr, loopStatement);
		return whileIterationStatement;
	}

	/**
	 * Parses DO-WHILE iteration statement.
	 * { DO-WHILE ::= do <statement> while (<expression>); }
	 */
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Iteration_Do()
	{
		auto const doWhileIterationStatement = m_Profile->CreateDoIterationStatement();
		m_IterationStatements.push(doWhileIterationStatement);

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
		
		m_IterationStatements.pop();
		doWhileIterationStatement->Initialize(loopStatement, conditionExpr);
		return doWhileIterationStatement;
	}

	/**
	 * Parses FOR iteration statement.
	 * { FOR ::= for (<statement> [<expression>]; [<expression>]) <statement> }
	 */
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Iteration_For()
	{
		auto const forIterationStatement = m_Profile->CreateForIterationStatement();
		m_IterationStatements.push(forIterationStatement);
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
		m_IterationStatements.pop();
		forIterationStatement->Initialize(initStatement, conditionExpr, iterationExpr, loopStatement);
		return forIterationStatement;
	}
	
	// --------------------------------------------------------------- //
	// --                  Jump statement parsing.                  -- //
	// --------------------------------------------------------------- //

	/**
	 * Parses BREAK jump statements.
	 * { BREAK ::= break; }
	 */
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Jump_Break()
	{
		if (m_IterationStatements.empty())
		{
			throw ParserException("'break' statement outside of any loop.");
		}
		ExpectLexemeAndReadNext(Lexeme::Type::OpSemicolon);
		return m_Profile->CreateBreakJumpStatement(m_IterationStatements.top());
	}

	/**
	 * Parses CONTINUE jump statements.
	 * { CONTINUE ::= continue; }
	 */
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Jump_Continue()
	{
		if (m_IterationStatements.empty())
		{
			throw ParserException("'continue' statement outside of any loop.");
		}
		ExpectLexemeAndReadNext(Lexeme::Type::OpSemicolon);
		return m_Profile->CreateContinueJumpStatement(m_IterationStatements.top());
	}

	/**
	 * Parses RETURN jump statements.
	 * { RETURN ::= return [<expression>]; }
	 */
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Jump_Return()
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

	/**
	 * Parses DISCARD jump statements.
	 * { DISCARD ::= discard; }
	 */
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Jump_Discard()
	{
		ExpectLexemeAndReadNext(Lexeme::Type::OpSemicolon);
		return m_Profile->CreateDiscardJumpStatement();
	}
	
	// --------------------------------------------------------------- //
	// --        Declaration & Expression statement parsing.        -- //
	// --------------------------------------------------------------- //

	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Declaration_OR_Expression()
	{
		CrAssert(0);
		return nullptr;
	}
	
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Declaration()
	{
		CrAssert(0);
		return nullptr;
	}
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Declaration_Typedef()
	{
		CrAssert(0);
		return nullptr;
	}
	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Declaration_Struct()
	{
		CrAssert(0);
		return nullptr;
	}

	// *************************************************************** //
	CRAPI Ast::Statement* Parser::Parse_Statement_Expression()
	{
		CrAssert(0);
		return nullptr;
	}

	// *************************************************************** //
	// *************************************************************** //

#if 0
	CRAPI Ast::Statement* Parser::Parse_Statement_Definition()
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
