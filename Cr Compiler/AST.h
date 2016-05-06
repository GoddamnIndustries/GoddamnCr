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
	class Parser;
	namespace Ast
	{
		enum class BaseType
		{
			Struct = -255,
			Sampler1D,
			Sampler2D,
			Sampler3D,
			SamplerCube,
			// ...
			Null = 0,
			Void,
			Bool,
			Int,
			UInt,
			Dword = UInt,
			Half,
			Float,
			Double,
		};	// enum class ExpressionBaseType

		union Value
		{
		public:
			double m_Scalar;
			double m_Vector[4];
			double m_Matrix[4][4] = {};

		public:
			CRINL Value(double const scalar = 0.0f)
				: m_Scalar(scalar) {}
			CRINL Value& operator= (double const scalar)
			{
				m_Scalar = scalar;
				return *this;
			}

			template<typename Tp>
			CRINL Tp To() const
			{
				return static_cast<Tp>(m_Scalar);
			}

		public:
			CRINL double& operator()(size_t const i, size_t const j)
			{
				return m_Matrix[i][j];
			}
			CRINL double operator()(size_t const i, size_t const j) const
			{
				return m_Matrix[i][j];
			}

			// Unary ops.
			CRINL Value operator+() const
			{
				return *this;
			}
			CRINL Value operator-() const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = -m_Matrix[i][j];
				return result;
			}
			CRINL Value operator!() const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = !m_Matrix[i][j];
				return result;
			}
			CRINL Value operator~() const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = ~static_cast<uint64_t>(m_Matrix[i][j]);
				return result;
			}

			// Binary logic ops.
			CRINL Value operator|(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = static_cast<uint64_t>(m_Matrix[i][j]) | static_cast<uint64_t>(rhs(i, j));
				return result;
			}
			CRINL Value operator&(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = static_cast<uint64_t>(m_Matrix[i][j]) & static_cast<uint64_t>(rhs(i, j));
				return result;
			}
			CRINL Value operator^(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = static_cast<uint64_t>(m_Matrix[i][j]) ^ static_cast<uint64_t>(rhs(i, j));
				return result;
			}
			CRINL Value operator<<(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = static_cast<uint64_t>(m_Matrix[i][j]) << static_cast<uint64_t>(rhs(i, j));
				return result;
			}
			CRINL Value operator>>(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = static_cast<uint64_t>(m_Matrix[i][j]) >> static_cast<uint64_t>(rhs(i, j));
				return result;
			}

			// Binary bitwise ops.
			CRINL Value operator||(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = m_Matrix[i][j] || rhs(i, j);
				return result;
			}
			CRINL Value operator&&(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = m_Matrix[i][j] && rhs(i, j);
				return result;
			}
			CRINL Value operator==(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = m_Matrix[i][j] == rhs(i, j);
				return result;
			}
			CRINL Value operator!=(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = m_Matrix[i][j] != rhs(i, j);
				return result;
			}
			CRINL Value operator<(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = m_Matrix[i][j] < rhs(i, j);
				return result;
			}
			CRINL Value operator>(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = m_Matrix[i][j] > rhs(i, j);
				return result;
			}
			CRINL Value operator<=(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = m_Matrix[i][j] <= rhs(i, j);
				return result;
			}
			CRINL Value operator>=(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = m_Matrix[i][j] >= rhs(i, j);
				return result;
			}
			
			// Binary arithmetic ops.
			CRINL Value operator+(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = m_Matrix[i][j] + rhs(i, j);
				return result;
			}
			CRINL Value operator-(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = m_Matrix[i][j] + rhs(i, j);
				return result;
			}
			CRINL Value operator*(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = m_Matrix[i][j] * rhs(i, j);
				return result;
			}
			CRINL Value operator/(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = m_Matrix[i][j] / rhs(i, j);
				return result;
			}
			CRINL Value operator%(Value const& rhs) const
			{
				Value result;
				for (auto i = 0; i < 4; ++i)
					for (auto j = 0; j < 4; ++j)
						result(i, j) = static_cast<uint64_t>(m_Matrix[i][j]) % static_cast<uint64_t>(rhs(i, j));
				return result;
			}

		};	// union Value

		struct Type
		{
		private:
			BaseType m_BaseType;
			uint8_t  m_Rows;
			uint8_t  m_Columns;

		public:
			CRINL explicit Type(BaseType baseType = BaseType::Null, uint8_t rows = 1, uint8_t columns = 1)
				: m_BaseType(baseType), m_Rows(rows), m_Columns(columns)
			{}
			CRINL explicit Type(BaseType baseType, Type const& type)
				: m_BaseType(baseType), m_Rows(type.m_Rows), m_Columns(type.m_Columns)
			{}

			CRINL bool IsScalar() const
			{
				return m_Rows == 1 && m_Columns == 1 && m_BaseType > BaseType::Void;
			}
			CRINL bool IsScalar(BaseType const lowerType) const
			{
				return IsScalar() && m_BaseType >= lowerType;
			}
			CRINL bool IsScalar(BaseType const lowerType, BaseType const higherType) const
			{
				return IsScalar() && m_BaseType >= lowerType && m_BaseType <= higherType;
			}

			CRINL bool operator< (Type const& type) const
			{
				return m_BaseType < type.m_BaseType;
			}
			CRINL bool operator> (Type const& type) const
			{
				return m_BaseType < type.m_BaseType;
			}
			CRINL bool operator<= (BaseType const baseType) const
			{
				return m_BaseType <= baseType;
			}
			CRINL bool operator>= (BaseType const baseType) const
			{
				return m_BaseType <= baseType;
			}

			CRINL bool operator== (BaseType const baseType) const
			{
				CrAssert(0);
				return IsScalar() && m_BaseType == baseType;
			}
			CRINL bool operator!= (BaseType const baseType) const
			{
				CrAssert(0);
				return IsScalar() && m_BaseType != baseType;
			}

			CRINL bool IsIncompatibleWith(Type const& other) const
			{
				return m_Rows != other.m_Rows || m_Columns != other.m_Columns;
			}

		};

		// *************************************************************** //
		// **                     Expressions parsing.                  ** //
		// *************************************************************** //

		/**
		 * Base class for all expressions.
		 */
		class Expression
		{
			friend class Parser;

		private:
			Type m_Type;
			bool m_IsLValue = false;
			bool m_IsConstexpr = false;

		public:
			CRINL Expression() = default;
			CRINL Expression(Expression const&) = delete;
			CRINL Expression& operator= (Expression const&) = delete;
			CRINL virtual ~Expression() = default;

		public:
			CR_API virtual Value Evaluate() const
			{
				CrAssert(IsConstexpr());
				return Value();
			}

			CRINL Type GetType() const
			{
				return m_Type;
			}
			CRINL bool IsLValue() const
			{
				return m_IsLValue;
			}
			CRINL bool IsConstexpr() const
			{
				return m_IsConstexpr;
			}
		};	// class Expression

		/**
		 * Compile-time constant expression.
		 */
		class ConstantExpression : public Expression
		{
			friend class Parser;

		protected:
			Value m_Value;

		public:
			CRINL explicit ConstantExpression() : m_Value({}) {}
			CR_API Value Evaluate() const override
			{
				return m_Value;
			}
		};	// class ConstantExpression

		struct Identifier : std::enable_shared_from_this<Identifier>
		{
		protected:
			Type m_Type;
		public:
			CRINL virtual ~Identifier() = default;
		};	// struct Identifier

		struct Variable : public Identifier
		{
			friend class Parser;

		protected:
			std::string m_Name;
			std::unique_ptr<Expression> m_InitExpr;

		};	// struct Variable

		struct Function : public Identifier
		{
			CRINL Type GetReturnType() { return Type(); }
		};	// struct Function

		/**
		 * Identifier expression.
		 */
		class IdentifierExpression : public Expression
		{
			friend class Parser;

		protected:
			std::shared_ptr<Identifier> m_Ident;

		public:
			CRINL explicit IdentifierExpression(Identifier* const ident) : m_Ident(ident) {}
		};	// class ConstantExpression

		/**
		 * Comma-separated expressions.
		 * No limitations on left and right sub-expressions.
		 */
		class CommaExpression : public Expression
		{
			friend class Parser;

		protected:
			std::unique_ptr<Expression> m_Lhs;
			std::unique_ptr<Expression> m_Rhs;
		};	// class CommaExpression

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
		class UnaryExpression : public Expression
		{
			friend class Parser;
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
		class NotExpression : public UnaryExpression
		{
			friend class Parser;
		public:
			CR_API explicit NotExpression(Expression* const expr)
				: UnaryExpression(Lexeme::Type::OpNot, expr)
			{ }
			CR_API Value Evaluate() const override
			{
				CrAssert(IsConstexpr());
				return !m_Expr->Evaluate();
			}
		};	// class NotExpression

		/**
		 * Bitwise unary expression class.
		 * Sub-expression should have convertible to integral type.
		 */
		class BitwiseNotExpression : public UnaryExpression
		{
			friend class Parser;
		public:
			CR_API explicit BitwiseNotExpression(Expression* const expr)
				: UnaryExpression(Lexeme::Type::OpBitwiseNot, expr)
			{ }
			CR_API Value Evaluate() const override
			{
				CrAssert(IsConstexpr());
				return ~m_Expr->Evaluate();
			}
		};	// class BitwiseNotExpression

		/**
		 * Per-component arithmetic unary expression class.
		 */
		class NegateExpression : public UnaryExpression
		{
			friend class Parser;
		public:
			CR_API explicit NegateExpression(Expression* const expr)
				: UnaryExpression(Lexeme::Type::OpSubtract, expr)
			{ }
			CR_API Value Evaluate() const override
			{
				CrAssert(IsConstexpr());
				return -m_Expr->Evaluate();
			}
		};	// class NegateExpression

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
			friend class Parser;
		protected:
			Lexeme::Type m_Op = Lexeme::Type::Null;
			std::unique_ptr<Expression> m_Lhs;
			std::unique_ptr<Expression> m_Rhs;

		public:
			CR_API BinaryExpression(Lexeme::Type op, Expression* const lhs, Expression* const rhs) 
				: m_Op(op), m_Lhs(lhs), m_Rhs(rhs) {}
		};	// class BinaryExpression

		/**
		 * Per-component logic binary expression class.
		 * Left and right sub-expressions should have convertible to boolean types of same dimensions.
		 */
		class LogicBinaryExpression : public BinaryExpression
		{
			friend class Parser;

		public:
			CR_API LogicBinaryExpression(Lexeme::Type const op, Expression* const lhs, Expression* const rhs)
				: BinaryExpression(op, lhs, rhs)
			{ }
			CR_API Value Evaluate() const override
			{
				CrAssert(IsConstexpr());
				auto const lhs = m_Lhs->Evaluate();
				auto const rhs = m_Rhs->Evaluate();
				switch (m_Op)
				{
					case Lexeme::Type::OpOr:            return lhs || rhs;
					case Lexeme::Type::OpAnd:           return lhs && rhs;
					case Lexeme::Type::OpEquals:        return lhs == rhs;
					case Lexeme::Type::OpNotEquals:     return lhs != rhs;
					case Lexeme::Type::OpLess:          return lhs < rhs;
					case Lexeme::Type::OpGreater:       return lhs > rhs;
					case Lexeme::Type::OpLessEquals:    return lhs <= rhs;
					case Lexeme::Type::OpGreaterEquals: return lhs >= rhs;
					default:
						CrAssert(0);
						return Value();
				}
			}
		};	// class LogicBinaryExpression

		/**
		 * Per-component bitwise binary expression class.
		 * Left and right sub-expressions should have convertible to integral types of same dimensions.
		 */
		class BitwiseBinaryExpression : public BinaryExpression
		{
			friend class Parser;

		public:
			CR_API BitwiseBinaryExpression(Lexeme::Type const op, Expression* const lhs, Expression* const rhs)
				: BinaryExpression(op, lhs, rhs)
			{ }
			CR_API Value Evaluate() const override
			{
				CrAssert(IsConstexpr());
				auto const lhs = m_Lhs->Evaluate();
				auto const rhs = m_Rhs->Evaluate();
				switch (m_Op)
				{
					case Lexeme::Type::OpBitwiseOr:         return lhs | rhs;
					case Lexeme::Type::OpBitwiseAnd:        return lhs & rhs;
					case Lexeme::Type::OpBitwiseXor:        return lhs ^ rhs;
					case Lexeme::Type::OpBitwiseLeftShift:  return lhs << rhs;
					case Lexeme::Type::OpBitwiseRightShift: return lhs >> rhs;
					default:
						CrAssert(0);
						return Value();
				}
			}
		};	// class BitwiseBinaryExpression

		/**
		 * Per-component arithmetic binary expression class.
		 * Left and right sub-expressions should have same or convertible types of same dimensions.
		 */
		class ArithmeticBinaryExpression : public BinaryExpression
		{
			friend class Parser;

		public:
			CR_API ArithmeticBinaryExpression(Lexeme::Type const op, Expression* const lhs, Expression* const rhs)
				: BinaryExpression(op, lhs, rhs)
			{ }
			CR_API Value Evaluate() const override
			{
				CrAssert(IsConstexpr());
				auto const lhs = m_Lhs->Evaluate();
				auto const rhs = m_Rhs->Evaluate();
				switch (m_Op)
				{
					case Lexeme::Type::OpAdd:      return lhs + rhs;
					case Lexeme::Type::OpSubtract: return lhs - rhs;
					case Lexeme::Type::OpMultiply: return lhs * rhs;
					case Lexeme::Type::OpDivide:   return lhs / rhs;
					case Lexeme::Type::OpModulo:   return lhs % rhs;
					default:
						CrAssert(0);
						return Value();
				}
			}
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
			friend class Parser;

		private:
			std::unique_ptr<Expression> m_CondExpr;
			std::unique_ptr<Expression> m_ThenExpr;
			std::unique_ptr<Expression> m_ElseExpr;

		public:
			CR_API TernaryExpression(Expression* const condExpr, Expression* const thenExpr, Expression* const elseExpr)
				: m_CondExpr(condExpr), m_ThenExpr(thenExpr), m_ElseExpr(elseExpr)
			{ }

			CR_API Value Evaluate() const override
			{
				CrAssert(IsConstexpr());
				auto const cond = m_CondExpr->Evaluate();
				return cond.m_Scalar != 0.0 ? m_ThenExpr->Evaluate() : m_ElseExpr->Evaluate();
			}
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

		/**
		 * Compound statement class.
		 */
		class CompoundStatement : public Statement
		{
			friend class Parser;
		private:
			std::vector<std::unique_ptr<Statement>> m_Stmts;

		public:
			CR_API CompoundStatement() 
			{ }
			CR_API explicit CompoundStatement(std::vector<std::unique_ptr<Statement>>&& stmts)
				: m_Stmts(std::forward<std::vector<std::unique_ptr<Statement>>>(stmts))
			{ }
		};	// class CompoundStatement

		// --------------------------------------------------------------- //
		// --               Selection statement parsing.                -- //
		// --------------------------------------------------------------- //

		/**
		 * Condition-based selection statement class.
		 */
		class SelectionStatement : public Statement
		{
		};	// class SelectionStatement

		/**
		 * Regular 'if'-'else' selection statement class.
		 * Unlike in C, we do not allow variable declarations inside the condition.
		 */
		class IfSelectionStatement : public SelectionStatement
		{
			friend class Parser;
		private:
			std::unique_ptr<Expression> m_CondExpr;
			std::unique_ptr<Statement> m_ThenStmt;
			std::unique_ptr<Statement> m_ElseStmt;

		public:
			CR_API virtual void Initialize(Expression* const condExpr, Statement* const thenStmt, Statement* const elseStmt);
		};	// class IfSelectionStatement

		class SwitchCase
		{	
			friend class Parser;

		private:
			std::vector<std::unique_ptr<Statement>> m_Stmts;
		};	// class SwitchCase

		class SwitchDefaultCase : public SwitchCase
		{
			friend class Parser;
		};	// class SwitchCase

		/**
		 * Simplified 'switch' selection statement class.
		 * Unlike in C, we do not allow variable declarations inside the switch.
		 * Our 'switch' statements require 'break' statements on the end of each non-empty case.
		 */
		class SwitchSelectionStatement : public SelectionStatement
		{
			friend class Parser;

		private:
			std::unique_ptr<Expression> m_SwitchExpr;
			std::shared_ptr<SwitchCase> m_DfltCase;
			std::map<int64_t, std::shared_ptr<SwitchCase>> m_Cases;

		public:
			CR_API virtual void Initialize(Expression* const switchExpr, ...);
		};	// class IfSelectionStatement

		// --------------------------------------------------------------- //
		// --               Iteration statement parsing.                -- //
		// --------------------------------------------------------------- //

		/**
		 * Iteration statement class.
		 */
		class IterationStatement : public Statement
		{
		};	// class IterationStatement

		/**
		 * Regular 'while' iteration statement class.
		 * Unlike in C, we do not allow variable declarations inside the condition.
		 */
		class WhileIterationStatement : public IterationStatement
		{
			friend class Parser;

		private:
			std::unique_ptr<Expression> m_CondExpr;
			std::unique_ptr<Statement> m_LoopStmt;

		public:
			CR_API virtual void Initialize(Expression* const condExpr, Statement* const loopStmt) {}
		};	// class WhileIterationStatement

		/**
		 * Regular 'do'-'while' iteration statement class.
		 * Unlike in C, we do not allow variable declarations inside the condition.
		 */
		class DoWhileIterationStatement : public IterationStatement
		{
			friend class Parser;

		private:
			std::unique_ptr<Statement> m_LoopStmt;
			std::unique_ptr<Expression> m_CondExpr;

		public:
			CR_API virtual void Initialize(Statement* const loopStmt, Expression* const condExpr) {}
		};	// class DoIterationStatement

		/**
		 * Regular 'for' iteration statement class.
		 * Unlike in C, we do not allow variable declarations inside the condition.
		 */
		class ForIterationStatement : public IterationStatement
		{
			friend class Parser;

		private:
			std::unique_ptr<Statement> m_InitStmt;
			std::unique_ptr<Expression> m_CondExpr;
			std::unique_ptr<Expression> m_StepExpr;
			std::unique_ptr<Statement> m_LoopStmt;

		public:
			CR_API virtual void Initialize(Statement* const initStmt, Expression* const condExpr, Expression* const iterExpr, Statement* const loopStmt) {}
		};	// class ForIterationStatement

		// --------------------------------------------------------------- //
		// --                  Jump statement parsing.                  -- //
		// --------------------------------------------------------------- //

		/**
		 * Jump statement class.
		 */
		class JumpStatement : public Statement
		{
		};	// class JumpStatement

		/**
		 * Regular 'break' jump statement class.
		 */
		class BreakJumpStatement : public JumpStatement
		{
			friend class Parser;

		private:
			Statement* m_BreakTo = nullptr;

		public:
			CR_API virtual void Initialize(Statement* const breakTo) {}
		};	// class BreakJumpStatement

		/**
		 * Regular 'continue' jump statement class.
		 */
		class ContinueJumpStatement : public JumpStatement
		{
			friend class Parser;

		private:
			Statement* m_ContinueWith = nullptr;

		public:
			CR_API virtual void Initialize(Statement* const continueWith) {}
		};	// class ContinueJumpStatement

		/**
		 * Regular 'return' jump statement class.
		 */
		class ReturnJumpStatement : public JumpStatement
		{
			friend class Parser;

		private:
			Function* m_ReturnTo = nullptr;
			std::unique_ptr<Expression> m_Expr;

		public:
			CR_API virtual void Initialize(Function* const returnTo, Expression* const expr) {}
		};	// class ReturnJumpStatement

		/**
		 * Regular 'discard' jump statement class.
		 * ( It terminates execution of the fragment shader )
		 */
		class DiscardJumpStatement : public JumpStatement
		{
			friend class Parser;

		public:
			CR_API virtual void Initialize() {}
		};	// class DiscardJumpStatement

		// --------------------------------------------------------------- //
		// --        Declaration & Expression statement parsing.        -- //
		// --------------------------------------------------------------- //

		/**
		 * Discard-value expression statement class.
		 */
		class ExpressionStatement : public Statement
		{
			friend class Parser;

		private:
			std::unique_ptr<Expression> m_Expr;

		public:
			CR_API virtual void Initialize(Expression* const expr) {}
		};	// class ExpressionStatement

		// *************************************************************** //
		class DeclarationStatement : public Statement
		{
			friend class Parser;

		private:
			std::vector<std::shared_ptr<Variable>> m_Vars;

		};	// class DeclarationStatement

	}	// namespace Ast

}   // namespace Cr
