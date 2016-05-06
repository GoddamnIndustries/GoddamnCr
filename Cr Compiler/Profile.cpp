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

	CR_API Ast::NotExpression* Profile::CreateNotExpression(Ast::Expression* const expr)
	{
		return new Ast::NotExpression(expr);
	}
	CR_API Ast::BitwiseNotExpression* Profile::CreateBitwiseNotExpression(Ast::Expression* const expr)
	{
		return new Ast::BitwiseNotExpression(expr);
	}
	CR_API Ast::NegateExpression* Profile::CreateNegateExpression(Ast::Expression* const expr)
	{
		return new Ast::NegateExpression(expr);
	}

	CR_API Ast::CommaExpression* Profile::CreateCommaExpression()
	{
		return new Ast::CommaExpression();
	}

	CR_API Ast::LogicBinaryExpression* Profile::CreateLogicBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::LogicBinaryExpression(op, lhs, rhs);
	}
	CR_API Ast::BitwiseBinaryExpression* Profile::CreateBitwiseBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::BitwiseBinaryExpression(op, lhs, rhs);
	}
	CR_API Ast::ArithmeticBinaryExpression* Profile::CreateArithmeticBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::ArithmeticBinaryExpression(op, lhs, rhs);
	}

	CR_API Ast::AssignmentBinaryExpression* Profile::CreateAssignmentBinaryExpression(Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::AssignmentBinaryExpression(lhs, rhs);
	}
	CR_API Ast::BitwiseAssignmentBinaryExpression* Profile::CreateBitwiseAssignmentBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::BitwiseAssignmentBinaryExpression(op, lhs, rhs);
	}
	CR_API Ast::ArithmeticAssignmentBinaryExpression* Profile::CreateArithmeticAssignmentBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs)
	{
		return new Ast::ArithmeticAssignmentBinaryExpression(op, lhs, rhs);
	}

	CR_API Ast::TernaryExpression* Profile::CreateTernaryExpression(Ast::Expression* const condExpr, Ast::Expression* const thenExpr, Ast::Expression* const elseExpr)
	{
		return new Ast::TernaryExpression(condExpr, thenExpr, elseExpr);
	}

	CR_API Ast::ConstantExpression* Profile::CreateConstExpression()
	{
		return new Ast::ConstantExpression();
	}
	CR_API Ast::Expression* Profile::CreateConstExpression(bool const valueBool)
	{
		return nullptr;
	//	return new Ast::ConstantExpression(valueBool);
	}
	CR_API Ast::Expression* Profile::CreateConstExpression(int32_t const valueInt)
	{
		return nullptr;
		//	return new Ast::ConstantExpression(valueInt);
	}
	CR_API Ast::Expression* Profile::CreateConstExpression(uint32_t const valueUInt)
	{
		return nullptr;
		//	return new Ast::ConstantExpression(valueUInt);
	}
	CR_API Ast::Expression* Profile::CreateConstExpression(float const valueFloat)
	{
		return nullptr;
		//	return new Ast::ConstantExpression(valueFloat);
	}
	CR_API Ast::Expression* Profile::CreateConstExpression(double const valueDouble)
	{
		return nullptr;
		//	return new Ast::ConstantExpression(valueDouble);
	}

	// *************************************************************** //
	// **                     Statements parsing.                   ** //
	// *************************************************************** //

	CR_API Ast::CompoundStatement* Profile::CreateCompoundStatement()
	{
		return new Ast::CompoundStatement();
	}
	CR_API Ast::CompoundStatement* Profile::CreateCompoundStatement(std::vector<std::unique_ptr<Ast::Statement>>&& stmts)
	{
		return new Ast::CompoundStatement(std::forward<std::vector<std::unique_ptr<Ast::Statement>>>(stmts));
	}

	CR_API Ast::IfSelectionStatement* Profile::CreateIfSelectionStatement()
	{
		return new Ast::IfSelectionStatement();
	}
	CR_API Ast::SwitchSelectionStatement* Profile::CreateSwitchSelectionStatement()
	{
		return new Ast::SwitchSelectionStatement();
	}
	CR_API Ast::SwitchCase* Profile::CreateSwitchCase()
	{
		return new Ast::SwitchCase();
	}
	CR_API Ast::SwitchDefaultCase* Profile::CreateSwitchDefaultCase()
	{
		return new Ast::SwitchDefaultCase();
	}

	CR_API Ast::DoWhileIterationStatement* Profile::CreateDoIterationStatement()
	{
		return new Ast::DoWhileIterationStatement();
	}
	CR_API Ast::ForIterationStatement* Profile::CreateForIterationStatement()
	{
		return new Ast::ForIterationStatement();
	}
	CR_API Ast::WhileIterationStatement* Profile::CreateWhileIterationStatement()
	{
		return new Ast::WhileIterationStatement();
	}

	CR_API Ast::BreakJumpStatement* Profile::CreateBreakJumpStatement()
	{
		return new Ast::BreakJumpStatement();
	}
	CR_API Ast::ContinueJumpStatement* Profile::CreateContinueJumpStatement()
	{
		return new Ast::ContinueJumpStatement();
	}
	CR_API Ast::ReturnJumpStatement* Profile::CreateReturnJumpStatement()
	{
		return new Ast::ReturnJumpStatement();
	}
	CR_API Ast::DiscardJumpStatement* Profile::CreateDiscardJumpStatement()
	{
		return new Ast::DiscardJumpStatement();
	}

	CR_API Ast::ExpressionStatement* Profile::CreateExpressionStatement()
	{
		return new Ast::ExpressionStatement();
	}

}	// namespace Cr
