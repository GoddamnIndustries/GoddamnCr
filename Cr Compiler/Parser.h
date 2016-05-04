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

#include "Lexeme.h"
#include "Utils.h"
#include "Scanner.h"

#include <vector>

namespace Cr
{
	namespace Ast
	{
		enum class BaseType
		{
			Void,
			Int,
			UInt,
			Dword = UInt,
			Half,
			Float,
			Double,
		};	// enum class ExpressionBaseType

		struct Type
		{
		private:
			BaseType m_BaseType;
			uint8_t m_Rows = 1;
			uint8_t m_Columns = 1;

		public:
			CRINL explicit Type(BaseType baseType = BaseType::Int) 
				: m_BaseType(baseType) 
			{}

			bool MatchDimensions(Type const& other) const
			{
				return m_Rows == other.m_Rows && m_Columns == other.m_Columns;
			}

			bool CanBeCastedToBool() const
			{
			}
		};

		struct Identifier
		{
			CRINL virtual ~Identifier() = default;
		};

		struct Function
		{
		};

		// *************************************************************** //
		// **                     Expressions parsing.                  ** //
		// *************************************************************** //

		/**
		 * Base class for all expressions.
		 */
		// *************************************************************** //
		class Expression
		{
		public:
			CRINL Expression() = default;
			CRINL Expression(Expression const&) = delete;
			CRINL Expression& operator= (Expression const&) = delete;
			CRINL virtual ~Expression() = default;

		public:
			CR_API virtual Type GetType() const { return Type(); }
		};	// class Expression

		class OperandExpression : public Expression
		{
		public:
			OperandExpression(...) {}
		};

		class ConstantOperandExpression : public Expression
		{
		public:
			ConstantOperandExpression(...) {}
		};

		// --------------------------------------------------------------- //
		// --                  Unary expression parsing.                -- //
		// --------------------------------------------------------------- //

		/**
		 * Base class for all unary expressions.
		 */
		class UnaryExpression : public Expression
		{
		protected:
			Lexeme::Type m_Op;
			std::unique_ptr<Expression> m_Expr;

		public:
			CRINL UnaryExpression(Lexeme::Type const op, Expression* const expr)
				: m_Op(op), m_Expr(expr)
			{ }

		};	// class UnaryExpression

		/**
		 * Logic unary expression class.
		 * Sub-expression should have convertible to boolean type.
		 */
		class LogicUnaryExpression : public UnaryExpression
		{
		public:
			CR_API LogicUnaryExpression(Lexeme::Type const op, Expression* const expr)
				: UnaryExpression(op, expr)
			{ }
		};	// class LogicUnaryExpression

		/**
		 * Bitwise unary expression class.
		 * Sub-expression should have convertible to integral type.
		 */
		class BitwiseUnaryExpression : public UnaryExpression
		{
		public:
			CR_API BitwiseUnaryExpression(Lexeme::Type const op, Expression* const expr)
				: UnaryExpression(op, expr)
			{ }
		};	// class BitwiseUnaryExpression

		/**
		 * Per-component arithmetic unary expression class.
		 */
		class ArithmeticUnaryExpression : public UnaryExpression
		{
		public:
			CR_API ArithmeticUnaryExpression(Lexeme::Type const op, Expression* const expr)
				: UnaryExpression(op, expr)
			{ }
		};	// class ArithmeticUnaryExpression

		//! @todo Add cast operation.
		//! @todo Add value node.

		// --------------------------------------------------------------- //
		// --                 Binary expression parsing.                -- //
		// --------------------------------------------------------------- //

		/**
		 * Base class for all binary expressions.
		 */
		class BinaryExpression : public Expression
		{
		protected:
			Lexeme::Type m_Op;
			std::unique_ptr<Expression> const m_Lhs;
			std::unique_ptr<Expression> const m_Rhs;

		public:
			CRINL BinaryExpression(Lexeme::Type op, Expression* const lhs, Expression* const rhs)
				: m_Op(op), m_Lhs(lhs), m_Rhs(rhs)
			{ }

		};	// class BinaryExpression

		/**
		 * Comma-separated expressions.
		 * No limitations on left and right sub-expressions.
		 */
		class CommaExpression : public BinaryExpression
		{
		public:
			CRINL CommaExpression(Expression* const lhs, Expression* const rhs) 
				: BinaryExpression(Lexeme::Type::OpComma, lhs, rhs) 
			{ }

		};	// class CommaExpression

		/**
		 * Per-component logic binary expression class.
		 * Left and right sub-expressions should have convertible to boolean types of same dimensions.
		 */
		class LogicBinaryExpression : public BinaryExpression
		{
		public:
			CR_API LogicBinaryExpression(Lexeme::Type const op, Expression* const lhs, Expression* const rhs)
				: BinaryExpression(op, lhs, rhs)
			{ }
		};	// class LogicBinaryExpression

		/**
		 * Per-component bitwise binary expression class.
		 * Left and right sub-expressions should have convertible to integral types of same dimensions.
		 */
		class BitwiseBinaryExpression : public BinaryExpression
		{
		public:
			CR_API BitwiseBinaryExpression(Lexeme::Type const op, Expression* const lhs, Expression* const rhs)
				: BinaryExpression(op, lhs, rhs)
			{ }
		};	// class BitwiseBinaryExpression

		/**
		 * Per-component arithmetic binary expression class.
		 * Left and right sub-expressions should have same or convertible types of same dimensions.
		 */
		class ArithmeticBinaryExpression : public BinaryExpression
		{
		public:
			CR_API ArithmeticBinaryExpression(Lexeme::Type const op, Expression* const lhs, Expression* const rhs)
				: BinaryExpression(op, lhs, rhs)
			{ }
		};	// class ArithmeticBinaryExpression

		/**
		 * Per-component assignment binary expression class.
		 * Left sub-expression should be valid l-value.
		 * Left and right sub-expressions should have same or convertible types.
		 */
		class AssignmentBinaryExpression : public BinaryExpression
		{
		protected:
			CR_API AssignmentBinaryExpression(Lexeme::Type const op, Expression* const lhs, Expression* const rhs)
				: BinaryExpression(op, lhs, rhs)
			{ }

		public:
			CR_API AssignmentBinaryExpression(Expression* const lhs, Expression* const rhs)
				: BinaryExpression(Lexeme::Type::OpAssignment, lhs, rhs)
			{ }

		};	// AssignmentBinaryExpression

		/**
		 * Per-component bitwise-assignment binary expression class.
		 * Left sub-expression should be valid l-value.
		 * Left and right sub-expressions should have convertible to integral types of same dimensions.
		 */
		class BitwiseAssignmentBinaryExpression : public AssignmentBinaryExpression
		{
		public:
			CR_API BitwiseAssignmentBinaryExpression(Lexeme::Type const op, Expression* const lhs, Expression* const rhs)
				: AssignmentBinaryExpression(op, lhs, rhs)
			{ }

		};	// BitwiseAssignmentBinaryExpression

		/**
		 * Per-component arithmetic-assignment binary expression class.
		 * Left sub-expression should be valid l-value.
		 * Left and right sub-expressions should have same or convertible types of same dimensions.
		 */
		class ArithmeticAssignmentBinaryExpression : public AssignmentBinaryExpression
		{
		public:
			CR_API ArithmeticAssignmentBinaryExpression(Lexeme::Type const op, Expression* const lhs, Expression* const rhs)
				: AssignmentBinaryExpression(op, lhs, rhs)
			{ }

		};	// ArithmeticAssignmentBinaryExpression

		// --------------------------------------------------------------- //
		// --                Ternary expression parsing.                -- //
		// --------------------------------------------------------------- //

		/**
		 * Ternary expression class.
		 * Condition sub-expression should be convertible to boolean expression.
		 * Branch sub-expressions should have same or convertible types of same dimensions.
		 */
		class TernaryExpression : public Expression
		{
		private:
			std::unique_ptr<Expression> m_CondExpr;
			std::unique_ptr<Expression> m_TrueExpr;
			std::unique_ptr<Expression> m_FalseExpr;

		public:
			CR_API TernaryExpression(Expression* const condExpr, Expression* const trueExpr, Expression* const falseExpr)
				: m_CondExpr(condExpr), m_TrueExpr(trueExpr), m_FalseExpr(falseExpr)
			{ }
		};	// class TernaryExpression

		// *************************************************************** //
		// **                     Statements parsing.                   ** //
		// *************************************************************** //

		/**
		 * Base class for all statements.
		 */
		class Statement
		{
		public:
			CRINL Statement() = default;
			CRINL Statement(Statement const&) = delete;
			CRINL Statement& operator= (Statement const&) = delete;
			CRINL virtual ~Statement() = default;

		};	// class Statement

		class NullStatement : public Statement
		{
		};	// class NullStatement

		class CompoundStatement : public Statement
		{
		};	// class CompoundStatement

		// --------------------------------------------------------------- //
		// --               Selection statement parsing.                -- //
		// --------------------------------------------------------------- //

		class SelectionStatement : public Statement
		{
		};	// class SelectionStatement

		class IfSelectionStatement : public SelectionStatement
		{
		private:
			std::unique_ptr<Expression> m_CondExpr;
			std::unique_ptr<Statement> m_TrueStmt;
			std::unique_ptr<Statement> m_FalseStmt;

		public:
			CR_API IfSelectionStatement(Expression* const condExpr, Statement* const trueStmt, Statement* const falseStmt)
				: m_CondExpr(condExpr), m_TrueStmt(trueStmt), m_FalseStmt(falseStmt)
			{ }
		};	// class IfSelectionStatement

		// --------------------------------------------------------------- //
		// --               Iteration statement parsing.                -- //
		// --------------------------------------------------------------- //

		class IterationStatement : public Statement
		{
		};	// class IterationStatement

		class DoIterationStatement : public IterationStatement
		{
		private:
			std::unique_ptr<Statement> m_LoopStmt;
			std::unique_ptr<Expression> m_CondExpr;

		public:
			CR_API virtual void Initialize(Statement* const loopStmt, Expression* const condExpr)
			{
				m_LoopStmt.reset(loopStmt);
				m_CondExpr.reset(condExpr);
			}
		};	// class DoIterationStatement

		class ForIterationStatement : public IterationStatement
		{
			std::unique_ptr<Statement> m_InitStmt;
			std::unique_ptr<Expression> m_CondExpr;
			std::unique_ptr<Expression> m_IterExpr;
			std::unique_ptr<Statement> m_LoopStmt;

		public:
			CR_API void Initialize(Statement* const initStmt, Expression* const condExpr, Expression* const iterExpr, Statement* const loopStmt)
			{
				m_InitStmt.reset(initStmt);
				m_CondExpr.reset(condExpr);
				m_IterExpr.reset(iterExpr);
				m_LoopStmt.reset(loopStmt);
			}
		};	// class ForIterationStatement

		class WhileIterationStatement : public IterationStatement
		{
			std::unique_ptr<Expression> m_CondExpr;
			std::unique_ptr<Statement> m_LoopStmt;

		public:
			CR_API void Initialize(Expression* const condExpr, Statement* const loopStmt)
			{
				m_CondExpr.reset(condExpr);
				m_LoopStmt.reset(loopStmt);
			}
		};	// class WhileIterationStatement

		// --------------------------------------------------------------- //
		// --                  Jump statement parsing.                  -- //
		// --------------------------------------------------------------- //

		class JumpStatement : public Statement
		{
		};	// class JumpStatement

		class BreakJumpStatement : public JumpStatement
		{
			//! @todo
		};	// class BreakJumpStatement

		class ReturnJumpStatement : public JumpStatement
		{
			//! @todo
		};	// class ReturnJumpStatement

		class DiscardJumpStatement : public JumpStatement
		{
			//! @todo
		};	// class DiscardJumpStatement

		class ContinueJumpStatement : public JumpStatement
		{
			//! @todo
		};	// class ContinueJumpStatement

		// --------------------------------------------------------------- //
		// --        Declaration & Expression statement parsing.        -- //
		// --------------------------------------------------------------- //

		// *************************************************************** //
		class ExpressionStatement : public Statement
		{
		private:
			std::unique_ptr<Expression> m_Expr;

		public:
			CR_API explicit ExpressionStatement(Expression* const expr) 
				: m_Expr(expr)
			{}
		};	// class ExpressionStatement

		// *************************************************************** //
		class DeclarationStatement : public Statement
		{
		public:
			//! @todo
			CR_API DeclarationStatement(Expression* const expr) {}
		};	// class DeclarationStatement

	}	// namespace Ast

	CrDefineExceptionBase(ParserException, WorkflowException);

	class Profile;

	class Parser
	{
		typedef AutoReset<Ast::IterationStatement*> AutoResetIterationStatement;
		typedef AutoResize<std::shared_ptr<Ast::Identifier>> AutoResetIdentifiers;

	private:
		Profile* m_Profile;
		Scanner* m_Scanner;
		
		Lexeme m_Lexeme;

	private:
		Ast::Function* m_Function;
		Ast::IterationStatement* m_TopIterStmt;
		std::vector<std::shared_ptr<Ast::Identifier>> m_Idents; 

	private:
		CRINL std::shared_ptr<Ast::Identifier> FindIdentifier(std::string const& name) const;
		CRINL std::shared_ptr<Ast::Identifier> DeclareVariable(Ast::Type const&, std::string const&)
		{
			return std::shared_ptr<Ast::Identifier>(nullptr);
		}

		CRINT void ReadNextLexeme();
		CRINL void ReadNextAndExpectLexeme(Lexeme::Type const type);
		CRINL void ExpectAndReadNextLexeme(Lexeme::Type const type);

		CR_API Ast::Statement* Parse_Statement();
		CR_API Ast::Statement* Parse_Statement_Null();
		CR_API Ast::Statement* Parse_Statement_Compound();
		CR_API Ast::Statement* Parse_Statement_Selection_If();
		CR_API Ast::Statement* Parse_Statement_Iter_Do();
		CR_API Ast::Statement* Parse_Statement_Iter_For();
		CR_API Ast::Statement* Parse_Statement_Iter_While();
		CR_API Ast::Statement* Parse_Statement_Jump_Break();
		CR_API Ast::Statement* Parse_Statement_Jump_Return();
		CR_API Ast::Statement* Parse_Statement_Jump_Discard();
		CR_API Ast::Statement* Parse_Statement_Jump_Continue();
		CR_API Ast::Statement* Parse_Statement_Declaration_OR_Expression();
		CR_API Ast::Statement* Parse_Statement_Declaration_Variable_OR_Function();
		CR_API Ast::Statement* Parse_Statement_Declaration_Typedef();
		CR_API Ast::Statement* Parse_Statement_Declaration_Struct();
		CR_API Ast::Statement* Parse_Statement_Expression();

		CR_API Ast::Expression* Parse_Expression_Condition();

		CR_API Ast::Expression* Parse_Expression();
		CR_API Ast::Expression* Parse_Expression_Assignments();
		CR_API Ast::Expression* Parse_Expression_Ternary();
		CR_API Ast::Expression* Parse_Expression_Or();
		CR_API Ast::Expression* Parse_Expression_And();
		CR_API Ast::Expression* Parse_Expression_BitwiseOr();
		CR_API Ast::Expression* Parse_Expression_BitwiseXor();
		CR_API Ast::Expression* Parse_Expression_BitwiseAnd();
		CR_API Ast::Expression* Parse_Expression_Equals_OR_NotEquals();
		CR_API Ast::Expression* Parse_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals();
		CR_API Ast::Expression* Parse_Expression_BitwiseLeftShift_OR_BitwiseRightShift();
		CR_API Ast::Expression* Parse_Expression_Add_OR_Subtract();
		CR_API Ast::Expression* Parse_Expression_Multiply_OR_Divide_OR_Modulo();
		CR_API Ast::Expression* Parse_Expression_PrefixUnary();

	public:
		CR_API void ParseProgram();

		Parser(Scanner* scanner) : m_Scanner(scanner) { ReadNextLexeme(); }
		CRINL Parser(Parser const&) = delete;
		CRINL Parser& operator= (Parser const&) = delete;

	};	// class Parser

}	// namespace Cr
