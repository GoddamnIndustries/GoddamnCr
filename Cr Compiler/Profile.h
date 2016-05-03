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

		CRAPI virtual Ast::Expression* CreateLogicUnaryExpression(Lexeme::Type const op, Ast::Expression* const expr) ;
		CRAPI virtual Ast::Expression* CreateBitwiseUnaryExpression(Lexeme::Type const op, Ast::Expression* const expr) ;
		CRAPI virtual Ast::Expression* CreateArithmeticUnaryExpression(Lexeme::Type const op, Ast::Expression* const expr) ;
		
		CRAPI virtual Ast::Expression* CreateCommaExpression(Ast::Expression* const lhs, Ast::Expression* const rhs) ;
		
		CRAPI virtual Ast::Expression* CreateLogicBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) ;
		CRAPI virtual Ast::Expression* CreateBitwiseBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) ;
		CRAPI virtual Ast::Expression* CreateArithmeticBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) ;
	
		CRAPI virtual Ast::Expression* CreateAssignmentBinaryExpression(Ast::Expression* const lhs, Ast::Expression* const rhs) ;
		CRAPI virtual Ast::Expression* CreateBitwiseAssignmentBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) ;
		CRAPI virtual Ast::Expression* CreateArithmeticAssignmentBinaryExpression(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) ;

		CRAPI virtual Ast::Expression* CreateTernaryExpression(Ast::Expression* const conditionExpr, Ast::Expression* const trueBranchExpr, Ast::Expression* const falseBranchExpr) ;

		// *************************************************************** //
		// **                     Statements parsing.                   ** //
		// *************************************************************** //

		CRAPI virtual Ast::Statement* CreateNullStatement() ;
		CRAPI virtual Ast::Statement* CreateCompoundStatement(...) ;

		CRAPI virtual Ast::Statement* CreateIfSelectionStatement(Ast::Expression* const conditionExpr, Ast::Statement* const trueBranchStatement, Ast::Statement* const falseBranchStatement) ;

		CRAPI virtual Ast::DoIterationStatement* CreateDoIterationStatement() ;
		CRAPI virtual Ast::ForIterationStatement* CreateForIterationStatement() ;
		CRAPI virtual Ast::WhileIterationStatement* CreateWhileIterationStatement() ;

		CRAPI virtual Ast::Statement* CreateBreakJumpStatement(Ast::IterationStatement* const breakTo) ;
		CRAPI virtual Ast::Statement* CreateReturnJumpStatement(Ast::Function* const function, Ast::Expression* const expr) ;
		CRAPI virtual Ast::Statement* CreateDiscardJumpStatement() ;
		CRAPI virtual Ast::Statement* CreateContinueJumpStatement(Ast::IterationStatement* const continueWith) ;
	};

}	// namespace Cr
