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

namespace Cr
{
	namespace Ast
	{
		class Statement;
		class IterationStatement;
		class DoIterationStatement;
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

		CR_API virtual Ast::Expression* CreateLogicUnaryExpression(Lexeme::Type const op, Ast::Expression* const expr) ;
		CR_API virtual Ast::Expression* CreateBitwiseUnaryExpression(Lexeme::Type const op, Ast::Expression* const expr) ;
		CR_API virtual Ast::Expression* CreateArithmeticUnaryExpression(Lexeme::Type const op, Ast::Expression* const expr) ;
		
		CR_API virtual Ast::Expression* CreateCommaExpression(Ast::Expression* const lhs, Ast::Expression* const rhs) ;
		
		CR_API virtual Ast::Expression* CreateLogicBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) ;
		CR_API virtual Ast::Expression* CreateBitwiseBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) ;
		CR_API virtual Ast::Expression* CreateArithmeticBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) ;
	
		CR_API virtual Ast::Expression* CreateAssignmentBinaryExpression(Ast::Expression* const lhs, Ast::Expression* const rhs) ;
		CR_API virtual Ast::Expression* CreateBitwiseAssignmentBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) ;
		CR_API virtual Ast::Expression* CreateArithmeticAssignmentBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) ;

		CR_API virtual Ast::Expression* CreateTernaryExpression(Ast::Expression* const condExpr, Ast::Expression* const trueExpr, Ast::Expression* const falseExpr);

		CR_API virtual Ast::Expression* CreateValueExpression(...);
		CR_API virtual Ast::Expression* CreateConstExpression(bool const valueBool);
		CR_API virtual Ast::Expression* CreateConstExpression(int32_t const valueInt);
		CR_API virtual Ast::Expression* CreateConstExpression(uint32_t const valueUInt);
		CR_API virtual Ast::Expression* CreateConstExpression(float const valueFloat);
		CR_API virtual Ast::Expression* CreateConstExpression(double const valueDouble);

		// *************************************************************** //
		// **                     Statements parsing.                   ** //
		// *************************************************************** //

		CR_API virtual Ast::Statement* CreateNullStatement() ;
		CR_API virtual Ast::Statement* CreateCompoundStatement(...) ;

		CR_API virtual Ast::Statement* CreateIfSelectionStatement(Ast::Expression* const condExpr, Ast::Statement* const trueStmt, Ast::Statement* const falseStmt) ;

		CR_API virtual Ast::DoIterationStatement* CreateDoIterationStatement() ;
		CR_API virtual Ast::ForIterationStatement* CreateForIterationStatement() ;
		CR_API virtual Ast::WhileIterationStatement* CreateWhileIterationStatement() ;

		CR_API virtual Ast::Statement* CreateBreakJumpStatement(Ast::IterationStatement* const breakTo) ;
		CR_API virtual Ast::Statement* CreateReturnJumpStatement(Ast::Function* const function, Ast::Expression* const expr) ;
		CR_API virtual Ast::Statement* CreateDiscardJumpStatement() ;
		CR_API virtual Ast::Statement* CreateContinueJumpStatement(Ast::IterationStatement* const continueWith) ;

		CR_API virtual Ast::Statement* CreateExpressionStatement(Ast::Expression* const expression);
	};

}	// namespace Cr
