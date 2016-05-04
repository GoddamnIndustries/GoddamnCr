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

#include "Profile.h"
#include "Parser.h"

namespace Cr
{

	// *************************************************************** //
	// **                     Expressions parsing.                  ** //
	// *************************************************************** //

	CR_API Ast::Expression* Profile::CreateLogicUnaryExpression(Lexeme::Type const op, Ast::Expression* const expr)
	{
		return new Ast::LogicUnaryExpression(op, expr);
	}
	CR_API Ast::Expression* Profile::CreateBitwiseUnaryExpression(Lexeme::Type const op, Ast::Expression* const expr)
	{
		return new Ast::BitwiseUnaryExpression(op, expr);
	}
	CR_API Ast::Expression* Profile::CreateArithmeticUnaryExpression(Lexeme::Type const op, Ast::Expression* const expr)
	{
		return new Ast::ArithmeticUnaryExpression(op, expr);
	}

	CR_API Ast::Expression* Profile::CreateCommaExpression(Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::CommaExpression(lhs, rhs);
	}

	CR_API Ast::Expression* Profile::CreateLogicBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::LogicBinaryExpression(op, lhs, rhs);
	}
	CR_API Ast::Expression* Profile::CreateBitwiseBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::BitwiseBinaryExpression(op, lhs, rhs);
	}
	CR_API Ast::Expression* Profile::CreateArithmeticBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::ArithmeticBinaryExpression(op, lhs, rhs);
	}

	CR_API Ast::Expression* Profile::CreateAssignmentBinaryExpression(Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::AssignmentBinaryExpression(lhs, rhs);
	}
	CR_API Ast::Expression* Profile::CreateBitwiseAssignmentBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::BitwiseAssignmentBinaryExpression(op, lhs, rhs);
	}
	CR_API Ast::Expression* Profile::CreateArithmeticAssignmentBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::ArithmeticAssignmentBinaryExpression(op, lhs, rhs);
	}

	CR_API Ast::Expression* Profile::CreateTernaryExpression(Ast::Expression* const condExpr, Ast::Expression* const trueExpr, Ast::Expression* const falseExpr)
	{
		return new Ast::TernaryExpression(condExpr, trueExpr, falseExpr);
	}

	// *************************************************************** //
	// **                     Statements parsing.                   ** //
	// *************************************************************** //

	CR_API Ast::Statement* Profile::CreateNullStatement()
	{
		return new Ast::NullStatement();
	}
	CR_API Ast::Statement* Profile::CreateCompoundStatement(...)
	{
		CrAssert(0);
		return new Ast::CompoundStatement();
	}

	CR_API Ast::Statement* Profile::CreateIfSelectionStatement(Ast::Expression* const condExpr, Ast::Statement* const trueStmt, Ast::Statement* const falseStmt)
	{
		return new Ast::IfSelectionStatement(condExpr, trueStmt, falseStmt);
	}

	CR_API Ast::DoIterationStatement* Profile::CreateDoIterationStatement()
	{
		return new Ast::DoIterationStatement();
	}
	CR_API Ast::ForIterationStatement* Profile::CreateForIterationStatement()
	{
		return new Ast::ForIterationStatement();
	}
	CR_API Ast::WhileIterationStatement* Profile::CreateWhileIterationStatement()
	{
		return new Ast::WhileIterationStatement();
	}

	CR_API Ast::Statement* Profile::CreateBreakJumpStatement(Ast::IterationStatement* const breakTo)
	{
		return new Ast::BreakJumpStatement(/*breakTo*/);
	}
	CR_API Ast::Statement* Profile::CreateContinueJumpStatement(Ast::IterationStatement* const continueWith)
	{
		return new Ast::ContinueJumpStatement(/*continueWith*/);
	}
	CR_API Ast::Statement* Profile::CreateReturnJumpStatement(Ast::Function* const function, Ast::Expression* const expr)
	{
		return new Ast::ReturnJumpStatement(/*function, expr*/);
	}
	CR_API Ast::Statement* Profile::CreateDiscardJumpStatement()
	{
		return new Ast::DiscardJumpStatement();
	}

	CR_API Ast::Statement* Profile::CreateExpressionStatement(Ast::Expression* const expression)
	{
		return new Ast::ExpressionStatement(expression);
	}

}	// namespace Cr
