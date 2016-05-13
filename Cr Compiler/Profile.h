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

#pragma once

#include "Utils.h"
#include "Lexeme.h"
#include <vector>

namespace Cr
{
	namespace Ast
	{
		struct Type;
		union Value;
		class BitwiseNotExpression;
		class NotExpression;
		class NegateExpression;
		class ArithmeticAssignmentBinaryExpression;
		class BitwiseAssignmentBinaryExpression;
		class AssignmentBinaryExpression;
		class BitwiseBinaryExpression;
		class ArithmeticBinaryExpression;
		class LogicBinaryExpression;
		class ConstantExpression;
		class TernaryExpression;
		class CommaExpression;
		class SwitchDefaultCase;
		class SwitchSection;
		class CompoundStatement;
		class ExpressionStatement;
		class DiscardJumpStatement;
		class ReturnJumpStatement;
		class ContinueJumpStatement;
		class BreakJumpStatement;
		class IfSelectionStatement;
		class Statement;
		class SwitchSelectionStatement;
		class IterationStatement;
		class DoWhileIterationStatement;
		class ForIterationStatement;
		class WhileIterationStatement;
		class Expression;
		class Function;
	}	// namespace Ast

	class Profile
	{
	public:

		// *************************************************************** //
		// **                     Expressions parsing.                  ** //
		// *************************************************************** //

		CR_API virtual Ast::NotExpression* CreateNotExpression(Ast::Expression* const expr) ;
		CR_API virtual Ast::BitwiseNotExpression* CreateBitwiseNotExpression(Ast::Expression* const expr) ;
		CR_API virtual Ast::NegateExpression* CreateNegateExpression(Ast::Expression* const expr) ;
		
		CR_API virtual Ast::CommaExpression* CreateCommaExpression();
		
		CR_API virtual Ast::LogicBinaryExpression* CreateLogicBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) ;
		CR_API virtual Ast::BitwiseBinaryExpression* CreateBitwiseBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) ;
		CR_API virtual Ast::ArithmeticBinaryExpression* CreateArithmeticBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) ;
	
		CR_API virtual Ast::AssignmentBinaryExpression* CreateAssignmentBinaryExpression(Ast::Expression* const lhs, Ast::Expression* const rhs) ;
		CR_API virtual Ast::BitwiseAssignmentBinaryExpression* CreateBitwiseAssignmentBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) ;
		CR_API virtual Ast::ArithmeticAssignmentBinaryExpression* CreateArithmeticAssignmentBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) ;

		CR_API virtual Ast::TernaryExpression* CreateTernaryExpression(Ast::Expression* const condExpr, Ast::Expression* const thenExpr, Ast::Expression* const elseExpr);

		CR_API virtual Ast::Expression* CreateValueExpression(...) {return nullptr;}
		CR_API virtual Ast::ConstantExpression* CreateConstExpression(Ast::Value const& value, Ast::Type const& type);

		// *************************************************************** //
		// **                     Statements parsing.                   ** //
		// *************************************************************** //

		CR_API virtual Ast::CompoundStatement* CreateCompoundStatement();
		CR_API virtual Ast::CompoundStatement* CreateCompoundStatement(std::vector<std::unique_ptr<Ast::Statement>>&& stmts);

		CR_API virtual Ast::IfSelectionStatement* CreateIfSelectionStatement() ;
		CR_API virtual Ast::SwitchSelectionStatement* CreateSwitchSelectionStatement();
		CR_API virtual Ast::SwitchSection* CreateSwitchSection();

		CR_API virtual Ast::DoWhileIterationStatement* CreateDoIterationStatement() ;
		CR_API virtual Ast::ForIterationStatement* CreateForIterationStatement() ;
		CR_API virtual Ast::WhileIterationStatement* CreateWhileIterationStatement() ;

		CR_API virtual Ast::BreakJumpStatement* CreateBreakJumpStatement();
		CR_API virtual Ast::ContinueJumpStatement* CreateContinueJumpStatement();
		CR_API virtual Ast::ReturnJumpStatement* CreateReturnJumpStatement() ;
		CR_API virtual Ast::DiscardJumpStatement* CreateDiscardJumpStatement();

		CR_API virtual Ast::ExpressionStatement* CreateExpressionStatement();
	};

}	// namespace Cr
