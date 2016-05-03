// *************************************************************** //
// Goddamn "C for Rendering" project.                              //
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.     //
//                                                                 //
// This software or any its part is distributed under terms of     //
// Goddamn Industries End User License Agreement. By downloading   //
// or using this software or any its part you agree with           //
// terms of Goddamn Industries End User License Agreement.         //
// *************************************************************** //

#pragma once

#include "Lexeme.h"
#include "Utils.h"
#include "Scanner.h"

#include <map>
#include <deque>
#include <stack>

namespace Cr
{
	namespace Ast
	{
		enum class ExpressionBaseType
		{
			Void,
			Int = (int)Lexeme::Type::CtInteger,
			Real = (int)Lexeme::Type::CtReal,
			String = (int)Lexeme::Type::CtString,
		};	// enum class ExpressionBaseType

		struct ExpressionType
		{
			ExpressionBaseType m_BaseType;
			char m_Rows = 1, m_Columns = 1;
			explicit ExpressionType(ExpressionBaseType m_BaseType) : m_BaseType(m_BaseType) {}

			bool MatchDimensions(ExpressionType const& other) const
			{
				return m_Rows == other.m_Rows && m_Columns == other.m_Columns;
			}
		};

		struct Identifier
		{
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
		//	CRAPI virtual ExpressionType EvaluateType() = 0;
		};	// class Expression

		class OperandExpression : public Expression
		{
		public:
			OperandExpression(...) {}
		};

		// --------------------------------------------------------------- //
		// --                  Unary expression parsing.                -- //
		// --------------------------------------------------------------- //

		/**
		 * Base class for all unary expressions.
		 */
		// *************************************************************** //
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
		// *************************************************************** //
		class LogicUnaryExpression : public UnaryExpression
		{
		public:
			CRAPI LogicUnaryExpression(Lexeme::Type const op, Expression* const expr)
				: UnaryExpression(op, expr)
			{ }
		};	// class LogicUnaryExpression

		/**
		 * Bitwise unary expression class.
		 * Sub-expression should have convertible to integral type.
		 */
		// *************************************************************** //
		class BitwiseUnaryExpression : public UnaryExpression
		{
		public:
			CRAPI BitwiseUnaryExpression(Lexeme::Type const op, Expression* const expr)
				: UnaryExpression(op, expr)
			{ }
		};	// class BitwiseUnaryExpression

		/**
		 * Per-component arithmetic unary expression class.
		 */
		// *************************************************************** //
		class ArithmeticUnaryExpression : public UnaryExpression
		{
		public:
			CRAPI ArithmeticUnaryExpression(Lexeme::Type const op, Expression* const expr)
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
		// *************************************************************** //
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
		// *************************************************************** //
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
		// *************************************************************** //
		class LogicBinaryExpression : public BinaryExpression
		{
		public:
			CRAPI LogicBinaryExpression(Lexeme::Type const op, Expression* const lhs, Expression* const rhs)
				: BinaryExpression(op, lhs, rhs)
			{ }
		};	// class LogicBinaryExpression

		/**
		 * Per-component bitwise binary expression class.
		 * Left and right sub-expressions should have convertible to integral types of same dimensions.
		 */
		// *************************************************************** //
		class BitwiseBinaryExpression : public BinaryExpression
		{
		public:
			CRAPI BitwiseBinaryExpression(Lexeme::Type const op, Expression* const lhs, Expression* const rhs)
				: BinaryExpression(op, lhs, rhs)
			{ }
		};	// class BitwiseBinaryExpression

		/**
		 * Per-component arithmetic binary expression class.
		 * Left and right sub-expressions should have same or convertible types of same dimensions.
		 */
		// *************************************************************** //
		class ArithmeticBinaryExpression : public BinaryExpression
		{
		public:
			CRAPI ArithmeticBinaryExpression(Lexeme::Type const op, Expression* const lhs, Expression* const rhs)
				: BinaryExpression(op, lhs, rhs)
			{ }
		};	// class ArithmeticBinaryExpression

		/**
		 * Per-component assignment binary expression class.
		 * Left sub-expression should be valid l-value.
		 * Left and right sub-expressions should have same or convertible types.
		 */
		// *************************************************************** //
		class AssignmentBinaryExpression : public BinaryExpression
		{
		protected:
			CRAPI AssignmentBinaryExpression(Lexeme::Type const op, Expression* const lhs, Expression* const rhs)
				: BinaryExpression(op, lhs, rhs)
			{ }

		public:
			CRAPI AssignmentBinaryExpression(Expression* const lhs, Expression* const rhs)
				: BinaryExpression(Lexeme::Type::OpAssignment, lhs, rhs)
			{ }

		};	// AssignmentBinaryExpression

		/**
		 * Per-component bitwise-assignment binary expression class.
		 * Left sub-expression should be valid l-value.
		 * Left and right sub-expressions should have convertible to integral types of same dimensions.
		 */
		// *************************************************************** //
		class BitwiseAssignmentBinaryExpression : public AssignmentBinaryExpression
		{
		public:
			CRAPI BitwiseAssignmentBinaryExpression(Lexeme::Type const op, Expression* const lhs, Expression* const rhs)
				: AssignmentBinaryExpression(op, lhs, rhs)
			{ }

		};	// BitwiseAssignmentBinaryExpression

		/**
		 * Per-component arithmetic-assignment binary expression class.
		 * Left sub-expression should be valid l-value.
		 * Left and right sub-expressions should have same or convertible types of same dimensions.
		 */
		// *************************************************************** //
		class ArithmeticAssignmentBinaryExpression : public AssignmentBinaryExpression
		{
		public:
			CRAPI ArithmeticAssignmentBinaryExpression(Lexeme::Type const op, Expression* const lhs, Expression* const rhs)
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
		// *************************************************************** //
		class TernaryExpression : public Expression
		{
		private:
			std::unique_ptr<Expression> m_ConditionExpr;
			std::unique_ptr<Expression> m_TrueBranchExpr;
			std::unique_ptr<Expression> m_FalseBranchExpr;

		public:
			CRAPI TernaryExpression(Expression* const conditionExpr, Expression* const trueBranchExpr, Expression* const falseBranchExpr)
				: m_ConditionExpr(conditionExpr), m_TrueBranchExpr(trueBranchExpr), m_FalseBranchExpr(falseBranchExpr)
			{ }
		};	// class TernaryExpression

		// *************************************************************** //
		// **                     Statements parsing.                   ** //
		// *************************************************************** //

		/**
		 * Base class for all statements.
		 */
		// *************************************************************** //
		class Statement
		{
		public:
			CRINL Statement() = default;
			CRINL Statement(Statement const&) = delete;
			CRINL Statement& operator= (Statement const&) = delete;
			CRINL virtual ~Statement() = default;

		};	// class Statement

		// *************************************************************** //
		class NullStatement : public Statement
		{
		};	// class NullStatement

		// *************************************************************** //
		class CompoundStatement : public Statement
		{
		};	// class CompoundStatement

		// --------------------------------------------------------------- //
		// --               Selection statement parsing.                -- //
		// --------------------------------------------------------------- //

		// *************************************************************** //
		class SelectionStatement : public Statement
		{
		};	// class SelectionStatement

		// *************************************************************** //
		class IfSelectionStatement : public SelectionStatement
		{
		private:
			std::unique_ptr<Expression> m_ConditionExpr;
			std::unique_ptr<Statement> m_TrueBranchStatement;
			std::unique_ptr<Statement> m_FalseBranchStatement;

		public:
			CRAPI IfSelectionStatement(Expression* const conditionExpr, Statement* const trueBranchStatement, Statement* const falseBranchStatement)
				: m_ConditionExpr(conditionExpr), m_TrueBranchStatement(trueBranchStatement), m_FalseBranchStatement(falseBranchStatement)
			{ }
		};	// class IfSelectionStatement

		// --------------------------------------------------------------- //
		// --               Iteration statement parsing.                -- //
		// --------------------------------------------------------------- //

		// *************************************************************** //
		class IterationStatement : public Statement
		{
		};	// class IterationStatement

		// *************************************************************** //
		class DoIterationStatement : public IterationStatement
		{
		private:
			std::unique_ptr<Statement> m_LoopStatement;
			std::unique_ptr<Expression> m_ConditionExpr;

		public:
			CRAPI virtual void Initialize(Statement* const loopStatement, Expression* const conditionExpr)
			{
				m_LoopStatement.reset(loopStatement);
				m_ConditionExpr.reset(conditionExpr);
			}
		};	// class DoIterationStatement

		// *************************************************************** //
		class ForIterationStatement : public IterationStatement
		{
			std::unique_ptr<Statement> m_InitStatement;
			std::unique_ptr<Expression> m_ConditionExpr;
			std::unique_ptr<Expression> m_IterationExpr;
			std::unique_ptr<Statement> m_LoopStatement;

		public:
			CRAPI void Initialize(Statement* const initStatement, Expression* const conditionExpr, Expression* const iterationExpr, Statement* const loopStatement)
			{
				m_InitStatement.reset(initStatement);
				m_ConditionExpr.reset(conditionExpr);
				m_IterationExpr.reset(iterationExpr);
				m_LoopStatement.reset(loopStatement);
			}
		};	// class ForIterationStatement

		// *************************************************************** //
		class WhileIterationStatement : public IterationStatement
		{
			std::unique_ptr<Expression> m_ConditionExpr;
			std::unique_ptr<Statement> m_LoopStatement;

		public:
			CRAPI void Initialize(Expression* const conditionExpr, Statement* const loopStatement)
			{
				m_ConditionExpr.reset(conditionExpr);
				m_LoopStatement.reset(loopStatement);
			}
		};	// class WhileIterationStatement

		// --------------------------------------------------------------- //
		// --                  Jump statement parsing.                  -- //
		// --------------------------------------------------------------- //

		// *************************************************************** //
		class JumpStatement : public Statement
		{
		};	// class JumpStatement

		// *************************************************************** //
		class BreakJumpStatement : public JumpStatement
		{
		};	// class BreakJumpStatement

		// *************************************************************** //
		class ReturnJumpStatement : public JumpStatement
		{
		};	// class ReturnJumpStatement

		// *************************************************************** //
		class DiscardJumpStatement : public JumpStatement
		{
		};	// class DiscardJumpStatement

		// *************************************************************** //
		class ContinueJumpStatement : public JumpStatement
		{
		};	// class ContinueJumpStatement

		// --------------------------------------------------------------- //
		// --        Declaration & Expression statement parsing.        -- //
		// --------------------------------------------------------------- //

		// *************************************************************** //
		class ExpressionStatement : public Statement
		{
		};	// class ExpressionStatement

		// *************************************************************** //
		class DeclarationStatement : public Statement
		{
		};	// class DeclarationStatement

	}	// namespace Ast

	CrDefineExceptionBase(ParserException, WorkflowException);

	class Profile;

	class Parser
	{
	private:
		Profile* m_Profile;
		Scanner* m_Scanner;
		
		Lexeme m_Lexeme;

		Ast::Function* m_Function;
		std::stack<Ast::IterationStatement*> m_IterationStatements;
		std::deque<std::map<std::string, Ast::Identifier*>> m_ScopedIdentifiers;	// have to use deque to iterate through stack.

	private:
		CRINT void ReadNextLexeme();
		CRINL void ExpectLexemeAndReadNext(Lexeme::Type const type);

		CRINL Ast::Identifier* FindIdentifier(std::string const& name) const;

		CRAPI Ast::Expression* Parse_Expression();
		CRAPI Ast::Expression* Parse_Expression_Assignments();
		CRAPI Ast::Expression* Parse_Expression_Ternary();
		CRAPI Ast::Expression* Parse_Expression_Or();
		CRAPI Ast::Expression* Parse_Expression_And();
		CRAPI Ast::Expression* Parse_Expression_BitwiseOr();
		CRAPI Ast::Expression* Parse_Expression_BitwiseXor();
		CRAPI Ast::Expression* Parse_Expression_BitwiseAnd();
		CRAPI Ast::Expression* Parse_Expression_Equals_OR_NotEquals();
		CRAPI Ast::Expression* Parse_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals();
		CRAPI Ast::Expression* Parse_Expression_BitwiseLeftShift_OR_BitwiseRightShift();
		CRAPI Ast::Expression* Parse_Expression_Add_OR_Subtract();
		CRAPI Ast::Expression* Parse_Expression_Multiply_OR_Divide_OR_Modulo();
		CRAPI Ast::Expression* Parse_Expression_PrefixUnary();
		CRAPI Ast::Expression* Parse_Expression_OPERAND();
		
		CRAPI Ast::Statement* Parse_Statement();
		CRAPI Ast::Statement* Parse_Statement_Null();
		CRAPI Ast::Statement* Parse_Statement_Compound();
		CRAPI Ast::Statement* Parse_Statement_Selection_If();
		CRAPI Ast::Statement* Parse_Statement_Iteration_Do();
		CRAPI Ast::Statement* Parse_Statement_Iteration_For();
		CRAPI Ast::Statement* Parse_Statement_Iteration_While();
		CRAPI Ast::Statement* Parse_Statement_Jump_Break();
		CRAPI Ast::Statement* Parse_Statement_Jump_Return();
		CRAPI Ast::Statement* Parse_Statement_Jump_Discard();
		CRAPI Ast::Statement* Parse_Statement_Jump_Continue();
		CRAPI Ast::Statement* Parse_Statement_Declaration_OR_Expression();
		CRAPI Ast::Statement* Parse_Statement_Declaration();
		CRAPI Ast::Statement* Parse_Statement_Declaration_Typedef();
		CRAPI Ast::Statement* Parse_Statement_Declaration_Struct();
		CRAPI Ast::Statement* Parse_Statement_Expression();

	public:
		CRAPI void ParseProgram();

		Parser(Scanner* scanner) : m_Scanner(scanner) {}
		CRINL Parser(Parser const&) = delete;
		CRINL Parser& operator= (Parser const&) = delete;

	};	// class Parser

}	// namespace Cr
