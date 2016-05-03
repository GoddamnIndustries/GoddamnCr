#include "Profile.h"
#include "Parser.h"

namespace Cr
{

	// *************************************************************** //
	// **                     Expressions parsing.                  ** //
	// *************************************************************** //

	CRAPI Ast::Expression* Profile::CreateLogicUnaryExpression(Lexeme::Type const op, Ast::Expression* const expr)
	{
		return new Ast::LogicUnaryExpression(op, expr);
	}
	CRAPI Ast::Expression* Profile::CreateBitwiseUnaryExpression(Lexeme::Type const op, Ast::Expression* const expr)
	{
		return new Ast::BitwiseUnaryExpression(op, expr);
	}
	CRAPI Ast::Expression* Profile::CreateArithmeticUnaryExpression(Lexeme::Type const op, Ast::Expression* const expr)
	{
		return new Ast::ArithmeticUnaryExpression(op, expr);
	}

	CRAPI Ast::Expression* Profile::CreateCommaExpression(Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::CommaExpression(lhs, rhs);
	}

	CRAPI Ast::Expression* Profile::CreateLogicBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::LogicBinaryExpression(op, lhs, rhs);
	}
	CRAPI Ast::Expression* Profile::CreateBitwiseBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::BitwiseBinaryExpression(op, lhs, rhs);
	}
	CRAPI Ast::Expression* Profile::CreateArithmeticBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::ArithmeticBinaryExpression(op, lhs, rhs);
	}

	CRAPI Ast::Expression* Profile::CreateAssignmentBinaryExpression(Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::AssignmentBinaryExpression(lhs, rhs);
	}
	CRAPI Ast::Expression* Profile::CreateBitwiseAssignmentBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::BitwiseAssignmentBinaryExpression(op, lhs, rhs);
	}
	CRAPI Ast::Expression* Profile::CreateArithmeticAssignmentBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::ArithmeticAssignmentBinaryExpression(op, lhs, rhs);
	}

	CRAPI Ast::Expression* Profile::CreateTernaryExpression(Ast::Expression* const conditionExpr, Ast::Expression* const trueBranchExpr, Ast::Expression* const falseBranchExpr)
	{
		return new Ast::TernaryExpression(conditionExpr, trueBranchExpr, falseBranchExpr);
	}

	// *************************************************************** //
	// **                     Statements parsing.                   ** //
	// *************************************************************** //

	CRAPI Ast::Statement* Profile::CreateNullStatement()
	{
		return new Ast::NullStatement();
	}
	CRAPI Ast::Statement* Profile::CreateCompoundStatement(...)
	{
		CrAssert(0);
		return new Ast::CompoundStatement();
	}

	CRAPI Ast::Statement* Profile::CreateIfSelectionStatement(Ast::Expression* const conditionExpr, Ast::Statement* const trueBranchStatement, Ast::Statement* const falseBranchStatement)
	{
		return new Ast::IfSelectionStatement(conditionExpr, trueBranchStatement, falseBranchStatement);
	}

	CRAPI Ast::DoIterationStatement* Profile::CreateDoIterationStatement()
	{
		return new Ast::DoIterationStatement();
	}
	CRAPI Ast::ForIterationStatement* Profile::CreateForIterationStatement()
	{
		return new Ast::ForIterationStatement();
	}
	CRAPI Ast::WhileIterationStatement* Profile::CreateWhileIterationStatement()
	{
		return new Ast::WhileIterationStatement();
	}

	CRAPI Ast::Statement* Profile::CreateBreakJumpStatement(Ast::IterationStatement* const breakTo)
	{
		return new Ast::BreakJumpStatement(/*breakTo*/);
	}
	CRAPI Ast::Statement* Profile::CreateContinueJumpStatement(Ast::IterationStatement* const continueWith)
	{
		return new Ast::ContinueJumpStatement(/*continueWith*/);
	}
	CRAPI Ast::Statement* Profile::CreateReturnJumpStatement(Ast::Function* const function, Ast::Expression* const expr)
	{
		return new Ast::ReturnJumpStatement(/*function, expr*/);
	}
	CRAPI Ast::Statement* Profile::CreateDiscardJumpStatement()
	{
		return new Ast::DiscardJumpStatement();
	}

}	// namespace Cr
