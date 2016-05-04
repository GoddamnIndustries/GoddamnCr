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

#include "Parser.h"
#include "Profile.h"
#include "Utils.h"

#include <vector>
#include <algorithm>

namespace Cr
{
	// *************************************************************** //
	// **                Parser class implementation.               ** //
	// *************************************************************** //

	/**
	 * Reads next lexeme from the specified scanner.
	 */
	CRINL void Parser::ReadNextLexeme()
	{
		m_Lexeme = m_Scanner->GetNextLexeme();
	}

	/**
	 * Reads next lexeme from the specified scanner and checks whether it
	 * is of expected type.
	 */
	//! @todo Do we need this method actually?
	CRINL void Parser::ReadNextAndExpectLexeme(Lexeme::Type const type)
	{
		ReadNextLexeme();
		if (m_Lexeme != type)
		{
			throw ParserException("something was expected.");
		}
	}

	/**
	 * Checks whether current lexeme is of expected type and
	 * reads next lexeme.
	 */
	CRINL void Parser::ExpectAndReadNextLexeme(Lexeme::Type const type)
	{
		if (m_Lexeme != type)
		{
			throw ParserException("something was expected.");
		}
		ReadNextLexeme();
	}

	CRINL std::shared_ptr<Ast::Identifier> Parser::FindIdentifier(std::string const& name) const
	{
		//! @todo Implement this.
		//auto const iter = std::find(m_Idents.crbegin(), m_Idents.crend(), name);
		return /*iter != m_Idents.crend() ? *iter :*/ nullptr;
	}

	/**
	 * Parses 'simplified' version of complete program.
	 */
	//! @todo Remove this trash.
	// *************************************************************** //
	CR_API void Parser::ParseProgram()
	{
		m_Profile = new Profile();
		CrLog(0, __FUNCSIG__);

		ExpectAndReadNextLexeme(Lexeme::Type::KwProgram);
		ExpectAndReadNextLexeme(Lexeme::Type::OpScopeBegin);

		while (true)
		{
			if (m_Lexeme == Lexeme::Type::Null)
			{
				throw ParserException("'}' expected.");
			}
			if (m_Lexeme == Lexeme::Type::OpScopeEnd)
			{
				break;
			}

			Parse_Statement();
		}
	}

	// *************************************************************** //
	// **                     Statements parsing.                   ** //
	// *************************************************************** //

	/**
	 * Parses all kinds of statements.
	 */
	// *************************************************************** //
	CR_API Ast::Statement* Parser::Parse_Statement()
	{
		switch (m_Lexeme.GetType())
		{
			// Null statements.
			// *************************************************************** //
			case Lexeme::Type::OpSemicolon:
				return Parse_Statement_Null();

			// Compound statements.
			// *************************************************************** //
			case Lexeme::Type::OpScopeBegin:
				ReadNextLexeme();
				return Parse_Statement_Compound();

			// Selection statements.
			// *************************************************************** //
			case Lexeme::Type::KwIf:
				ReadNextLexeme();
				return Parse_Statement_Selection_If();

			// Iteration statements.
			// *************************************************************** //
			case Lexeme::Type::KwDo:
				ReadNextLexeme();
				return Parse_Statement_Iter_Do();
			case Lexeme::Type::KwWhile:
				ReadNextLexeme();
				return Parse_Statement_Iter_While();
			case Lexeme::Type::KwFor:
				ReadNextLexeme();
				return Parse_Statement_Iter_For();

			// Jump statements.
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
				return Parse_Statement_Jump_Discard();

			// Declaration or expression statements.
			// *************************************************************** //
			default:
				return Parse_Statement_Declaration_OR_Expression();
		}
	}

	/**
	 * Parses NULL statements.
	 * { NULL-STMT ::= ; }
	 */
	// *************************************************************** //
	CR_API Ast::Statement* Parser::Parse_Statement_Null()
	{
		ReadNextLexeme();
		return m_Profile->CreateNullStatement();
	}

	/**
	 * Parses COMPOUND statements.
	 * { COMPOUND-STMT ::= { [statement] [statement] ... } }
	 */
	// *************************************************************** //
	CR_API Ast::Statement* Parser::Parse_Statement_Compound()
	{
		//! @todo Implement COMPOUND statements.
		AutoResetIdentifiers const resetIdents(m_Idents);

		CrAssert(0);
		std::vector<std::unique_ptr<Ast::Statement>> statements;
		while (m_Lexeme != Lexeme::Type::OpScopeEnd)
		{
			statements.emplace_back(Parse_Statement());
		}
		ReadNextLexeme();
		return m_Profile->CreateCompoundStatement(/*std::move(statements)*/);
	}

	// --------------------------------------------------------------- //
	// --               Selection statement parsing.                -- //
	// --------------------------------------------------------------- //

	/**
	 * Helper that parses CONDITIONs.
	 * { CONDITION-EXPR ::= <TYPE> <ID> = <expression> | <expression> }
	 */
	// *************************************************************** //
	CR_API Ast::Expression* Parser::Parse_Expression_Condition()
	{
		Ast::Expression* condExpr;
		switch (m_Lexeme.GetType())
		{
			//! @todo Add single variable declaration parsing.
			//case CrCaseType(Lexeme::Type::KwBool):
			//case CrCaseType(Lexeme::Type::KwInt):
			//case CrCaseType(Lexeme::Type::KwUInt):
			//case CrCaseType(Lexeme::Type::KwDword):
			//case CrCaseType(Lexeme::Type::KwHalf):
			//case CrCaseType(Lexeme::Type::KwFloat):
			//case CrCaseType(Lexeme::Type::KwDouble):
			//	{
			//		auto const declType = 1;	//! @todo

			//		ReadNextAndExpectLexeme(Lexeme::Type::IdIdentifier);

			//		auto const declName = m_Lexeme.GetValueID();

			//		ReadNextAndExpectLexeme(Lexeme::Type::OpEquals);

			//		auto const declInit = Parse_Expression();

			//		return m_Profile->CreateAssignmentBinaryExpression(m_Profile->CreateVariableExpression(DeclareVariable(declType, declName)), declInit);
			//	}

			default:
				condExpr = Parse_Expression();
				break;
		}

		if (!condExpr->GetType().CanBeCastedToBool())
		{
			throw ParserException("Condition expression could not be casted to boolean type.");
		}
		return condExpr;
	}

	/**
	 * Parses IF-ELSE compound statement.
	 * { IF-ELSE-STMT ::= if (<condition>) <statement> [else <statement>] }
	 */
	// *************************************************************** //
	CR_API Ast::Statement* Parser::Parse_Statement_Selection_If()
	{
		AutoResetIdentifiers const resetIdents(m_Idents);

		ExpectAndReadNextLexeme(Lexeme::Type::OpParenthesesBegin);

		auto const condExpr = Parse_Expression_Condition();

		ExpectAndReadNextLexeme(Lexeme::Type::OpParenthesesEnd);

		auto const trueStmt = Parse_Statement();

		Ast::Statement* falseStmt = nullptr;
		if (m_Lexeme == Lexeme::Type::KwElse)
		{
			ReadNextLexeme();
			falseStmt = Parse_Statement();
		}

		return m_Profile->CreateIfSelectionStatement(condExpr, trueStmt, falseStmt);
	}

	// --------------------------------------------------------------- //
	// --               Iteration statement parsing.                -- //
	// --------------------------------------------------------------- //

	/**
	 * Parses WHILE iteration statement.
	 * { WHILE-STMT ::= while (<condition>) <statement> }
	 */
	// *************************************************************** //
	CR_API Ast::Statement* Parser::Parse_Statement_Iter_While()
	{
		auto const whileStmt = m_Profile->CreateWhileIterationStatement();
		AutoResetIterationStatement const resetIterStmt(m_TopIterStmt, whileStmt);
		AutoResetIdentifiers const resetIdents(m_Idents);	// <condition> provides a possible scope.

		ExpectAndReadNextLexeme(Lexeme::Type::OpParenthesesBegin);

		auto const condExpr = Parse_Expression_Condition();

		ExpectAndReadNextLexeme(Lexeme::Type::OpParenthesesEnd);

		auto const loopStmt = Parse_Statement();

		whileStmt->Initialize(condExpr, loopStmt);
		return whileStmt;
	}

	/**
	 * Parses DO-WHILE iteration statement.
	 * { DO-WHILE-STMT ::= do <statement> while (<expression>); }
	 */
	// *************************************************************** //
	CR_API Ast::Statement* Parser::Parse_Statement_Iter_Do()
	{
		auto const doWhileStmt = m_Profile->CreateDoIterationStatement();
		AutoResetIterationStatement const resetIterStmt(m_TopIterStmt, doWhileStmt);

		auto const loopStmt = Parse_Statement();

		ExpectAndReadNextLexeme(Lexeme::Type::KwWhile);
		ExpectAndReadNextLexeme(Lexeme::Type::OpParenthesesBegin);

		// Note that we have an ordinary expression here.
		//! @todo Check type.
		auto const condExpr = Parse_Expression_Condition();

		ExpectAndReadNextLexeme(Lexeme::Type::OpParenthesesEnd);
		ExpectAndReadNextLexeme(Lexeme::Type::OpSemicolon);

		doWhileStmt->Initialize(loopStmt, condExpr);
		return doWhileStmt;
	}

	/**
	 * Parses FOR iteration statement.
	 * { FOR-STMT ::= for (<statement> [<condition>]; [<expression>]) <statement> }
	 */
	// *************************************************************** //
	CR_API Ast::Statement* Parser::Parse_Statement_Iter_For()
	{
		auto const forStmt = m_Profile->CreateForIterationStatement();
		AutoResetIterationStatement const resetIterStmt(m_TopIterStmt, forStmt);
		AutoResetIdentifiers const resetIdents(m_Idents);	// <condition> provides a possible scope.

		ExpectAndReadNextLexeme(Lexeme::Type::OpParenthesesBegin);

		Ast::Statement* initStmt = nullptr;
		if (m_Lexeme != Lexeme::Type::OpSemicolon)
		{
			initStmt = Parse_Statement();
			if (dynamic_cast<Ast::DeclarationStatement*>(initStmt) == nullptr &&
				dynamic_cast<Ast::ExpressionStatement*>(initStmt) == nullptr)
			{
				throw ParserException("Declaration or expression is expected in initialization section"
								      " of 'for' statement.");
			}
		}
		else
		{
			ReadNextLexeme();
		}

		Ast::Expression* condExpr = nullptr;
		if (m_Lexeme != Lexeme::Type::OpSemicolon)
		{
			condExpr = Parse_Expression_Condition();
			ExpectAndReadNextLexeme(Lexeme::Type::OpSemicolon);
		}
		else
		{
			ReadNextLexeme();
		}

		Ast::Expression* iterExpr = nullptr;
		if (m_Lexeme != Lexeme::Type::OpParenthesesEnd)
		{
			iterExpr = Parse_Expression();
			ExpectAndReadNextLexeme(Lexeme::Type::OpParenthesesEnd);
		}
		else
		{
			ReadNextLexeme();
		}

		auto const loopStmt = Parse_Statement();

		forStmt->Initialize(initStmt, condExpr, iterExpr, loopStmt);
		return forStmt;
	}
	
	// --------------------------------------------------------------- //
	// --                  Jump statement parsing.                  -- //
	// --------------------------------------------------------------- //

	/**
	 * Parses BREAK jump statements.
	 * { BREAK-STMT ::= break; }
	 */
	// *************************************************************** //
	CR_API Ast::Statement* Parser::Parse_Statement_Jump_Break()
	{
		if (m_TopIterStmt == nullptr)
		{
			throw ParserException("'break' statement outside of any loop.");
		}
		ExpectAndReadNextLexeme(Lexeme::Type::OpSemicolon);
		return m_Profile->CreateBreakJumpStatement(m_TopIterStmt);
	}

	/**
	 * Parses CONTINUE jump statements.
	 * { CONTINUE-STMT ::= continue; }
	 */
	// *************************************************************** //
	CR_API Ast::Statement* Parser::Parse_Statement_Jump_Continue()
	{
		if (m_TopIterStmt == nullptr)
		{
			throw ParserException("'continue' statement outside of any loop.");
		}
		ExpectAndReadNextLexeme(Lexeme::Type::OpSemicolon);
		return m_Profile->CreateContinueJumpStatement(m_TopIterStmt);
	}

	/**
	 * Parses RETURN jump statements.
	 * { RETURN-STMT ::= return [<expression>]; }
	 */
	// *************************************************************** //
	CR_API Ast::Statement* Parser::Parse_Statement_Jump_Return()
	{
		Ast::Expression* expr = nullptr;
		if (m_Lexeme != Lexeme::Type::OpSemicolon)
		{
			expr = Parse_Expression();
			ExpectAndReadNextLexeme(Lexeme::Type::OpSemicolon);
		}
		else
		{
			ReadNextLexeme();
		}
		return m_Profile->CreateReturnJumpStatement(m_Function, expr);
	}

	/**
	 * Parses DISCARD jump statements.
	 * { DISCARD-STMT ::= discard; }
	 */
	// *************************************************************** //
	CR_API Ast::Statement* Parser::Parse_Statement_Jump_Discard()
	{
		ExpectAndReadNextLexeme(Lexeme::Type::OpSemicolon);
		return m_Profile->CreateDiscardJumpStatement();
	}
	
	// --------------------------------------------------------------- //
	// --        Declaration & Expression statement parsing.        -- //
	// --------------------------------------------------------------- //

	/**
	 * Parses DECLARATION or EXPRESSION statements.
	 * { DECLARATION-STMT ::= boolMxN | .. | doubleMxN <identifier> [ ( [<DECLARATION>, ...] ) ] }
	 */
	// *************************************************************** //
	CR_API Ast::Statement* Parser::Parse_Statement_Declaration_OR_Expression()
	{
		switch (m_Lexeme.GetType())
		{
			//! @todo Add DECLARATION or EXPRESSION statements parsing.
			case CrCaseType(Lexeme::Type::KwBool):
			case CrCaseType(Lexeme::Type::KwInt):
			case CrCaseType(Lexeme::Type::KwUInt):
			case CrCaseType(Lexeme::Type::KwDword):
			case CrCaseType(Lexeme::Type::KwHalf):
			case CrCaseType(Lexeme::Type::KwFloat):
			case CrCaseType(Lexeme::Type::KwDouble):
				return Parse_Statement_Declaration_Variable_OR_Function();
			default:
				return Parse_Statement_Expression();
		}
	}
	
	/**
	 * Parses variable or function DECLARATION statements.
	 * { DECLARATION-STMT ::= TypeMxN <identifier> ; }
	 */
	// *************************************************************** //
	CR_API Ast::Statement* Parser::Parse_Statement_Declaration_Variable_OR_Function()
	{
		auto const declStmt = nullptr;

		auto const firstIdentType = Ast::Type(/*(Ast::ExpressionBaseType)m_Lexeme.GetType()*/Ast::BaseType::Int);
		
		ReadNextAndExpectLexeme(Lexeme::Type::IdIdentifier);

		auto const firstIdentName = m_Lexeme.GetValueID();

		ReadNextLexeme();
		switch (m_Lexeme.GetType())
		{
			case Lexeme::Type::OpSemicolon:

			// Function declaration.
			// *************************************************************** //
			case Lexeme::Type::OpParenthesesBegin:
				CrAssert(0);
				break;

		}

		return nullptr;
	}
	// *************************************************************** //
	CR_API Ast::Statement* Parser::Parse_Statement_Declaration_Typedef()
	{
		CrAssert(0);
		return nullptr;
	}
	// *************************************************************** //
	CR_API Ast::Statement* Parser::Parse_Statement_Declaration_Struct()
	{
		CrAssert(0);
		return nullptr;
	}

	// *************************************************************** //
	// **                    Expressions parsing.                   ** //
	// *************************************************************** //

	/** 
	 * Parses EXPRESSION-STMT statements.
	 * { EXPRESSION-STMT ::= <expression>; }
	 */
	// *************************************************************** //
	CR_API Ast::Statement* Parser::Parse_Statement_Expression()
	{
		auto const expr = Parse_Expression();
		ExpectAndReadNextLexeme(Lexeme::Type::OpSemicolon);
		return m_Profile->CreateExpressionStatement(expr);
	}

	/**
	 * Parses all kinds of expressions.
	 * { Internally parses: <expression> [, <expression>] }
	 */
	// *************************************************************** //
	CR_API Ast::Expression* Parser::Parse_Expression()
	{
		auto expr = Parse_Expression_Assignments();
		while (m_Lexeme == Lexeme::Type::OpComma)
		{
			ReadNextLexeme();
			expr = m_Profile->CreateCommaExpression(expr, Parse_Expression_Assignments());
		}
		return expr;
	}

	/**
	 * Parses all kinds of assignment expressions.
	 * { Internally parses: <expression> [ @= <expression> ] }
	 */
	// *************************************************************** //
	CR_API Ast::Expression* Parser::Parse_Expression_Assignments()
	{
		auto expr = Parse_Expression_Ternary();
		while (true)
		{
			switch (m_Lexeme.GetType())
			{
				case Lexeme::Type::OpAssignment:
					//! @todo Check type.
					ReadNextLexeme();
					expr = m_Profile->CreateAssignmentBinaryExpression(expr, Parse_Expression_Ternary());
					break;

				case Lexeme::Type::OpAddAssign: case Lexeme::Type::OpSubtractAssign:
				case Lexeme::Type::OpMultiplyAssign: case Lexeme::Type::OpDivideAssign: case Lexeme::Type::OpModuloAssign:
					//! @todo Check type.
					ReadNextLexeme();
					expr = m_Profile->CreateArithmeticAssignmentBinaryExpression(m_Lexeme.GetType(), expr, Parse_Expression_Ternary());
					break;

				case Lexeme::Type::OpBitwiseAndAssign: case Lexeme::Type::OpBitwiseOrAssign: case Lexeme::Type::OpBitwiseXorAssign:
				case Lexeme::Type::OpBitwiseLeftShiftAssign: case Lexeme::Type::OpBitwiseRightShiftAssign:
					//! @todo Check type.
					ReadNextLexeme();
					expr = m_Profile->CreateBitwiseAssignmentBinaryExpression(m_Lexeme.GetType(), expr, Parse_Expression_Ternary());
					break;

				default:
					return expr;
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
	CR_API Ast::Expression* Parser::Parse_Expression_Ternary()
	{
		auto expr = Parse_Expression_Or();
		while (m_Lexeme == Lexeme::Type::OpTernary)
		{
			auto const trueExpr = Parse_Expression();

			ExpectAndReadNextLexeme(Lexeme::Type::OpColon);

			//! @todo Check type.
			auto const falseExpr = Parse_Expression();

			expr = m_Profile->CreateTernaryExpression(expr, trueExpr, falseExpr);
		}
		return expr;
	}

	// --------------------------------------------------------------- //
	// --                 Binary expression parsing.                -- //
	// --------------------------------------------------------------- //

	/**
	 * Parses all kinds of binary expressions.
	 * { Internally parses: <expression> [ @ <expression>] }
	 */
	// *************************************************************** //
	CR_API Ast::Expression* Parser::Parse_Expression_Or()
	{
		auto expr = Parse_Expression_And();
		while (m_Lexeme == Lexeme::Type::OpOr)
		{
			//! @todo Check type.
			ReadNextLexeme();
			expr = m_Profile->CreateLogicBinaryExpression(m_Lexeme.GetType(), expr, Parse_Expression_And());
		}
		return expr;
	}
	// *************************************************************** //
	CR_API Ast::Expression* Parser::Parse_Expression_And()
	{
		auto expr = Parse_Expression_BitwiseOr();
		while (m_Lexeme == Lexeme::Type::OpAnd)
		{
			//! @todo Check type.
			ReadNextLexeme();
			expr = m_Profile->CreateLogicBinaryExpression(m_Lexeme.GetType(), expr, Parse_Expression_BitwiseOr());
		}
		return expr;
	}
	// *************************************************************** //
	CR_API Ast::Expression* Parser::Parse_Expression_BitwiseOr()
	{
		auto expr = Parse_Expression_BitwiseXor();
		while (m_Lexeme == Lexeme::Type::OpBitwiseOr)
		{
			//! @todo Check type.
			ReadNextLexeme();
			expr = m_Profile->CreateBitwiseBinaryExpression(m_Lexeme.GetType(), expr, Parse_Expression_BitwiseXor());
		}
		return expr;
	}
	// *************************************************************** //
	CR_API Ast::Expression* Parser::Parse_Expression_BitwiseXor()
	{
		auto expr = Parse_Expression_BitwiseAnd();
		while (m_Lexeme == Lexeme::Type::OpBitwiseXor)
		{
			//! @todo Check type.
			ReadNextLexeme();
			expr = m_Profile->CreateBitwiseBinaryExpression(m_Lexeme.GetType(), expr, Parse_Expression_BitwiseAnd());
		}
		return expr;
	}
	// *************************************************************** //
	CR_API Ast::Expression* Parser::Parse_Expression_BitwiseAnd()
	{
		auto expr = Parse_Expression_Equals_OR_NotEquals();
		while (m_Lexeme == Lexeme::Type::OpBitwiseAnd)
		{
			//! @todo Check type.
			ReadNextLexeme();
			expr = m_Profile->CreateBitwiseBinaryExpression(m_Lexeme.GetType(), expr, Parse_Expression_Equals_OR_NotEquals());
		}
		return expr;
	}
	// *************************************************************** //
	CR_API Ast::Expression* Parser::Parse_Expression_Equals_OR_NotEquals()
	{
		auto expr = Parse_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals();
		while (m_Lexeme == Lexeme::Type::OpEquals || m_Lexeme == Lexeme::Type::OpNotEquals)
		{
			//! @todo Check type.
			ReadNextLexeme();
			expr = m_Profile->CreateLogicBinaryExpression(m_Lexeme.GetType(), expr, Parse_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals());
		}
		return expr;
	}
	// *************************************************************** //
	CR_API Ast::Expression* Parser::Parse_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals()
	{
		auto expr = Parse_Expression_BitwiseLeftShift_OR_BitwiseRightShift();
		while (m_Lexeme == Lexeme::Type::OpLess || m_Lexeme == Lexeme::Type::OpLessEquals
			|| m_Lexeme == Lexeme::Type::OpGreater || m_Lexeme == Lexeme::Type::OpGreaterEquals)
		{
			//! @todo Check type.
			ReadNextLexeme();
			expr = m_Profile->CreateLogicBinaryExpression(m_Lexeme.GetType(), expr, Parse_Expression_BitwiseLeftShift_OR_BitwiseRightShift());
		}
		return expr;
	}
	// *************************************************************** //
	CR_API Ast::Expression* Parser::Parse_Expression_BitwiseLeftShift_OR_BitwiseRightShift()
	{
		auto expr = Parse_Expression_Add_OR_Subtract();
		while (m_Lexeme == Lexeme::Type::OpBitwiseLeftShift || m_Lexeme == Lexeme::Type::OpBitwiseRightShift)
		{
			//! @todo Check type.
			ReadNextLexeme();
			expr = m_Profile->CreateBitwiseBinaryExpression(m_Lexeme.GetType(), expr, Parse_Expression_Add_OR_Subtract());
		}
		return expr;
	}
	// *************************************************************** //
	CR_API Ast::Expression* Parser::Parse_Expression_Add_OR_Subtract()
	{
		auto expr = Parse_Expression_Multiply_OR_Divide_OR_Modulo();
		while (m_Lexeme == Lexeme::Type::OpAdd || m_Lexeme == Lexeme::Type::OpSubtract)
		{
			//! @todo Check type.
			ReadNextLexeme();
			expr = m_Profile->CreateArithmeticBinaryExpression(m_Lexeme.GetType(), expr, Parse_Expression_Multiply_OR_Divide_OR_Modulo());
		}
		return expr;
	}
	// *************************************************************** //
	CR_API Ast::Expression* Parser::Parse_Expression_Multiply_OR_Divide_OR_Modulo()
	{
		auto expr = Parse_Expression_PrefixUnary();
		while (m_Lexeme == Lexeme::Type::OpMultiply || m_Lexeme == Lexeme::Type::OpDivide || m_Lexeme == Lexeme::Type::OpModulo)
		{
			//! @todo Check type.
			ReadNextLexeme();
			expr = m_Profile->CreateArithmeticBinaryExpression(m_Lexeme.GetType(), expr, Parse_Expression_PrefixUnary());
		}
		return expr;
	}

	// --------------------------------------------------------------- //
	// --                  Unary expression parsing.                -- //
	// --------------------------------------------------------------- //

	/**
	 * Parses all kinds of unary expressions.
	 * { Internally parses: [@]<expression> }
	 */
	// *************************************************************** //
	CR_API Ast::Expression* Parser::Parse_Expression_PrefixUnary()
	{
		switch (m_Lexeme.GetType())
		{
			// Ordinary unary operations.
			// *************************************************************** //
			case Lexeme::Type::OpAdd:
			case Lexeme::Type::OpSubtract:
				//! @todo Check type.
				ReadNextLexeme();
				return m_Profile->CreateArithmeticUnaryExpression(m_Lexeme.GetType(), Parse_Expression_PrefixUnary());
			case Lexeme::Type::OpInc:
			case Lexeme::Type::OpDec:
				CrAssert(0);
				//! @todo Check type.
				ReadNextLexeme();
				return m_Profile->CreateArithmeticUnaryExpression(m_Lexeme.GetType(), Parse_Expression_PrefixUnary());
			case Lexeme::Type::OpNot:
				//! @todo Check type.
				ReadNextLexeme();
				return m_Profile->CreateLogicUnaryExpression(m_Lexeme.GetType(), Parse_Expression_PrefixUnary());
			case Lexeme::Type::OpBitwiseNot:
				//! @todo Check type.
				ReadNextLexeme();
				return m_Profile->CreateBitwiseUnaryExpression(m_Lexeme.GetType(), Parse_Expression_PrefixUnary());

			// Parentheses or cast operations.
			// *************************************************************** //
			case Lexeme::Type::OpParenthesesBegin:
			{
				ReadNextLexeme();
				switch (m_Lexeme.GetType())
				{
					//! @todo Add cast operation support.
					case CrCaseType(Lexeme::Type::KwBool):
					case CrCaseType(Lexeme::Type::KwInt):
					case CrCaseType(Lexeme::Type::KwUInt):
					case CrCaseType(Lexeme::Type::KwDword):
					case CrCaseType(Lexeme::Type::KwHalf):
					case CrCaseType(Lexeme::Type::KwFloat):
					case CrCaseType(Lexeme::Type::KwDouble):
						CrAssert(0);
						break;
				}

				auto const parenthesesExpr = Parse_Expression();
				ExpectAndReadNextLexeme(Lexeme::Type::OpParenthesesEnd);
				return parenthesesExpr;
			}

			case Lexeme::Type::IdIdentifier:
				CrLog(15, "ID");
				ReadNextLexeme();
				break;

			// Compile-time constants.
			// *************************************************************** //
			case Lexeme::Type::KwTrue:
				ReadNextLexeme();
				return m_Profile->CreateConstExpression(true);
			case Lexeme::Type::KwFalse:
				ReadNextLexeme();
				return m_Profile->CreateConstExpression(false);
			case Lexeme::Type::CtInt:
				ReadNextLexeme();
				return m_Profile->CreateConstExpression(m_Lexeme.GetValueInt());
				//! @todo Add other constant types.
			/*case Lexeme::Type::CtUInt:	
				ReadNextLexeme();
				return m_Profile->CreateConstExpression(m_Lexeme.GetValueUInt());
			case Lexeme::Type::CtHalf:
				ReadNextLexeme();
				return m_Profile->CreateConstExpression(m_Lexeme.GetValueHalf());
			case Lexeme::Type::CtFloat:
				ReadNextLexeme();
				return m_Profile->CreateConstExpression(m_Lexeme.GetValueFloat());*/
			case Lexeme::Type::CtDouble:
				ReadNextLexeme();
				return m_Profile->CreateConstExpression(m_Lexeme.GetValueDouble());

			default:
				throw ParserException("Unexpected lexeme in the stream.");
		}
	}

	// *************************************************************** //
	// *************************************************************** //

#if 0
	CR_API Ast::Statement* Parser::Parse_Statement_Definition()
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

	CrUnitTest(ParserEmptyStream)
	{
		Parser parser(new Scanner(std::make_shared<IO::StringInputStream>(R"(
program 
{
	int b;
}
)")));
		parser.ParseProgram();
		int i = 0;
	};

}	// namespace Cr
