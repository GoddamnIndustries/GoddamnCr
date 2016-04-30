// *************************************************************** //
// Goddamn "C for Rendering" project.                              //
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.     //
//                                                                 //
// This software or any its part is distributed under terms of     //
// Goddamn Industries End User License Agreement. By downloading   //
// or using this software or any its part you agree with           //
// terms of Goddamn Industries End User License Agreement.         //
// *************************************************************** //

#include <iostream>
#include "Parser.h"
#include "Utils.h"

namespace Cr
{

	CRINL void Parser::ReadNextLexeme()
	{
		m_Lexeme = m_Scanner->GetNextLexeme();
	}

	CRAPI void Parser::ParseProgram() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);

		ReadNextLexeme();
		if (m_Lexeme.GetType() != Lexeme::Type::KwProgram)
		{
			throw ParserException("'program' expected.");
		}

		ReadNextLexeme();
		if (m_Lexeme.GetType() != Lexeme::Type::OpScopeBegin)
		{
			throw ParserException("'{' expected.");
		}

		while (true)
		{
			ReadNextLexeme();
			if (m_Lexeme.GetType() == Lexeme::Type::Null)
			{
				throw ParserException("'}' expected.");
			}
			if (m_Lexeme.GetType() == Lexeme::Type::OpScopeEnd)
			{
				break;
			}

			Parse_Statement();
		}
	}

	CRAPI void Parser::Parse_Statement() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		switch (m_Lexeme.GetType())
		{
			// *************************************************************** //
			case Lexeme::Type::OpSemicolon:
				ReadNextLexeme();
				Parse_Statement_Null();
				break;

			// *************************************************************** //
			case Lexeme::Type::KwInt:
			case Lexeme::Type::KwString:
			case Lexeme::Type::KwReal:
				ReadNextLexeme();
				Parse_Statement_Definition();
				break;

			default:
				CrAssert(0);
				break;
		}
	}

	CRAPI void Parser::Parse_Statement_Null() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
	}

	CRAPI void Parser::Parse_Statement_Definition() throw(ParserException)
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
					auto expression = Parse_Statement_Expression();
					expression = nullptr;
					return;
				}

		default:
			throw ParserException("';', ',' or '= [expression]' expected.");
		}
	}

	// *************************************************************** //
	// very important class for the world existence.
	// do not remove
	// the world will collapse
#define Stepan_daun_autist true

	class Stepan
	{
		bool const daun;
		bool const autist;
		int DrPepperCounter;
		int LondonCounter;

	public:
		Stepan(): daun(true), autist(true), DrPepperCounter(5), LondonCounter(5) {}
		void DaunoAutismo() const
		{
			while(Stepan_daun_autist)
				std::cout << "Chto takoe proizvedenie funkcij?\n";
		}
	};
	Stepan* Styopa = new Stepan();

	class DrunkenStepan: public Stepan
	{
		int bottlesOfWineDrunk;
	public:
		DrunkenStepan(): Stepan()
		{
			bottlesOfWineDrunk = 1;
		}
		void OneMoreWineDrunk()
		{
			++ bottlesOfWineDrunk;
		}
		void GoDrinkMore() const
		{
			std::cout << "POCONI GO EWE VIP'EM\n";
		}
	};

	// *************************************************************** //

	// *************************************************************** //
	// **                    Expressions parsing.                   ** //
	// *************************************************************** //

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

	/**
	 * Base class for all expressions.
	 */
	class Expression
	{
	public:
		CRINL Expression() = default;
		CRINL Expression(Expression const&) = delete;
		CRINL Expression& operator= (Expression const&) = delete;

		/**
		 * Returns true if this expression can be assigned.
		 */
		CRAPI virtual bool IsLValue() const
		{
			return false;
		}

		virtual ExpressionType GetType() const { return ExpressionType(ExpressionBaseType::Void); }
	};	// class Expression

	/**
	 * Base class for all unary expressions.
	 */
	class UnaryExpression : public Expression
	{
	protected:
		std::unique_ptr<Expression> const m_Expr;

	public:
		UnaryExpression(Expression* const expr)
			: m_Expr(expr)
		{ }
	};	// class UnaryExpression

	/**
	 * Base class for all binary expressions.
	 */
	class BinaryExpression : public Expression
	{
	protected:
		std::unique_ptr<Expression> const m_Lhs;
		std::unique_ptr<Expression> const m_Rhs;

	public:
		BinaryExpression(Expression* const lhs, Expression* const rhs)
			: m_Lhs(lhs), m_Rhs(rhs)
		{
		}
	};	// class BinaryExpression

	class CommaExpression : public Expression
	{
	public:
		CommaExpression(...) {}
	};

	/**
	 * Parses all kinds of expressions.
	 * { Internally parses: <expression> [, <expression>] }
	 */
	CRAPI Expression* Parser::Parse_Statement_Expression() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		auto expression = Parse_Statement_Expression_Assignments();
		while (m_Lexeme.GetType() == Lexeme::Type::OpComma)
		{
			CrLog(1, ",");
			ReadNextLexeme();
			expression = new CommaExpression(expression, Parse_Statement_Expression_Assignments());
		}
		return expression;
	}

	class AssignmentExpression : public BinaryExpression
	{
	public:
		AssignmentExpression(Expression* const lhs, Expression* const rhs)
			: BinaryExpression(lhs, rhs)
		{
		}
	};

	/**
	 * Parses all kinds of assignment expressions.
	 * { Internally parses: <expression> [ @= <expression> ] }
	 */
	CRAPI Expression* Parser::Parse_Statement_Expression_Assignments() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		auto expression = Parse_Statement_Expression_Ternary();
		while (true)
		{
			switch (m_Lexeme.GetType())
			{
				case Lexeme::Type::OpAssignment:
					CrLog(2, "=");
					expression = new AssignmentExpression(expression, Parse_Statement_Expression_Ternary());
					break;

				case Lexeme::Type::OpAddAssign: case Lexeme::Type::OpSubtractAssign:
				case Lexeme::Type::OpMultiplyAssign: case Lexeme::Type::OpDivideAssign: case Lexeme::Type::OpModuloAssign:
					CrLog(2, "+=_-=_*=_/=_%%=");
					CrAssert(0);
					break;

				case Lexeme::Type::OpBitwiseAndAssign: case Lexeme::Type::OpBitwiseOrAssign: case Lexeme::Type::OpBitwiseXorAssign:
				case Lexeme::Type::OpBitwiseLeftShiftAssign: case Lexeme::Type::OpBitwiseRightShiftAssign:
					CrLog(2, "&=_|=_^=_<<=_>>=");
					CrAssert(0);
					break;

				default:
					return expression;
			}
		}
	}

	// --------------------------------------------------------------- //
	// --                Ternary expression parsing.                -- //
	// --------------------------------------------------------------- //

	/**
	 * Ternary expression class.
	 * Condition expression should be castable to boolean expression.
	 * Branch expression should be of equal types.
	 */
	class TernaryExpression final : public Expression
	{
	private:
		std::unique_ptr<Expression> m_ConditionExpression;
		std::unique_ptr<Expression> m_TrueBranchExpression;
		std::unique_ptr<Expression> m_FalseBranchExpression;

	public:
		TernaryExpression(Expression* const conditionExpression
			, Expression* const trueBranchExpression, Expression* const falseBranchExpression)
			: m_ConditionExpression(conditionExpression)
			, m_TrueBranchExpression(trueBranchExpression), m_FalseBranchExpression(falseBranchExpression)
		{
		}

	public:
		CRAPI bool IsLValue() const override final
		{
			return true;
		}

	};	// class TernaryExpression

	/**
	 * Parses all kinds of ternary expressions.
	 * { Internally parses: <expression> [ ? <expression> : <expression> ] }
	 */
	CRAPI Expression* Parser::Parse_Statement_Expression_Ternary() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		auto expression = Parse_Statement_Expression_Or();
		while (m_Lexeme.GetType() == Lexeme::Type::OpTernary)
		{
			CrLog(3, "?:");
			auto const trueBranchExpression = Parse_Statement_Expression();
			if (m_Lexeme.GetType() != Lexeme::Type::OpColon)
			{
				throw ParserException("':' expected in ternary operator.");
			}

			ReadNextLexeme();
			auto const falseBranchExpression = Parse_Statement_Expression();
			expression = new TernaryExpression(expression, trueBranchExpression, falseBranchExpression);
		}
		return expression;
	}

	// --------------------------------------------------------------- //
	// --                 Binary expression parsing.                -- //
	// --------------------------------------------------------------- //

	/**
	 * Logic binary expression class.
	 * Left and right expressions should be castable to boolean types.
	 */
	class LogicBinaryExpression final : public BinaryExpression
	{
	public:
		LogicBinaryExpression(Lexeme::Type const type, Expression* const lhs, Expression* const rhs)
			: BinaryExpression(lhs, rhs)
		{
		}
	};	// class LogicBinaryExpression

	/**
	 * Bitwise binary expression class.
	 * Left and right expressions should be castable to integral types of same dimensions.
	 */
	class BitwiseBinaryExpression final : public BinaryExpression
	{
	public:
		BitwiseBinaryExpression(Lexeme::Type const type, Expression* const lhs, Expression* const rhs)
			: BinaryExpression(lhs, rhs)
		{
		}
	};	// class BitwiseBinaryExpression

	/**
	 * Per-component arithmetic binary expression class.
	 * Left and right expressions should be same or castable types.
	 */
	class ArithmeticBinaryExpression final : public BinaryExpression
	{
	public:
		ArithmeticBinaryExpression(Lexeme::Type const type, Expression* const lhs, Expression* const rhs)
			: BinaryExpression(lhs, rhs)
		{
		}
	};	// class ArithmeticBinaryExpression

	/**
	 * Parses all kinds of binary expressions.
	 * { Internally parses: <expression> [ @ <expression>] }
	 */
	CRAPI Expression* Parser::Parse_Statement_Expression_Or() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		auto expression = Parse_Statement_Expression_And();
		while (m_Lexeme.GetType() == Lexeme::Type::OpOr)
		{
			CrLog(4, "||");
			ReadNextLexeme();
			expression = CreateExpression<LogicBinaryExpression>(expression, Parse_Statement_Expression_And());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Expression* Parser::Parse_Statement_Expression_And() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		auto expression = Parse_Statement_Expression_BitwiseOr();
		while (m_Lexeme.GetType() == Lexeme::Type::OpAnd)
		{
			CrLog(5, "&&");
			ReadNextLexeme();
			expression = CreateExpression<LogicBinaryExpression>(expression, Parse_Statement_Expression_BitwiseOr());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Expression* Parser::Parse_Statement_Expression_BitwiseOr() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		auto expression = Parse_Statement_Expression_BitwiseXor();
		while (m_Lexeme.GetType() == Lexeme::Type::OpBitwiseOr)
		{
			CrLog(6, "|");
			ReadNextLexeme();
			expression = CreateExpression<BitwiseBinaryExpression>(expression, Parse_Statement_Expression_BitwiseXor());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Expression* Parser::Parse_Statement_Expression_BitwiseXor() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		auto expression = Parse_Statement_Expression_BitwiseAnd();
		while (m_Lexeme.GetType() == Lexeme::Type::OpBitwiseXor)
		{
			CrLog(7, "^");
			ReadNextLexeme();
			expression = CreateExpression<BitwiseBinaryExpression>(expression, Parse_Statement_Expression_BitwiseAnd());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Expression* Parser::Parse_Statement_Expression_BitwiseAnd() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		auto expression = Parse_Statement_Expression_Equals_OR_NotEquals();
		while (m_Lexeme.GetType() == Lexeme::Type::OpBitwiseAnd)
		{
			CrLog(8, "&");
			ReadNextLexeme();
			expression = CreateExpression<BitwiseBinaryExpression>(expression, Parse_Statement_Expression_Equals_OR_NotEquals());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Expression* Parser::Parse_Statement_Expression_Equals_OR_NotEquals() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		auto expression = Parse_Statement_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals();
		while (m_Lexeme.GetType() == Lexeme::Type::OpEquals || m_Lexeme.GetType() == Lexeme::Type::OpNotEquals)
		{
			CrLog(9, "&&_||");
			ReadNextLexeme();
			expression = CreateExpression<LogicBinaryExpression>(expression, Parse_Statement_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Expression* Parser::Parse_Statement_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		auto expression = Parse_Statement_Expression_BitwiseLeftShift_OR_BitwiseRightShift();
		while (m_Lexeme.GetType() == Lexeme::Type::OpLess || m_Lexeme.GetType() == Lexeme::Type::OpBitwiseRightShift
			|| m_Lexeme.GetType() == Lexeme::Type::OpGreater || m_Lexeme.GetType() == Lexeme::Type::OpGreaterEquals)
		{
			CrLog(10, "<_<=_>_>=");
			ReadNextLexeme();
			expression = CreateExpression<LogicBinaryExpression>(expression, Parse_Statement_Expression_BitwiseLeftShift_OR_BitwiseRightShift());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Expression* Parser::Parse_Statement_Expression_BitwiseLeftShift_OR_BitwiseRightShift() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		auto expression = Parse_Statement_Expression_Add_OR_Subtract();
		while (m_Lexeme.GetType() == Lexeme::Type::OpBitwiseLeftShift || m_Lexeme.GetType() == Lexeme::Type::OpBitwiseRightShift)
		{
			CrLog(11, "<<_>>");
			ReadNextLexeme();
			expression = CreateExpression<BitwiseBinaryExpression>(expression, Parse_Statement_Expression_Add_OR_Subtract());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Expression* Parser::Parse_Statement_Expression_Add_OR_Subtract() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		auto expression = Parse_Statement_Expression_Multiply_OR_Divide_OR_Modulo();
		while (m_Lexeme.GetType() == Lexeme::Type::OpAdd 
			|| m_Lexeme.GetType() == Lexeme::Type::OpSubtract)
		{
			CrLog(12, "+_-");
			ReadNextLexeme();
			expression = CreateExpression<ArithmeticBinaryExpression>(expression, Parse_Statement_Expression_Multiply_OR_Divide_OR_Modulo());
		}
		return expression;
	}
	// *************************************************************** //
	CRAPI Expression* Parser::Parse_Statement_Expression_Multiply_OR_Divide_OR_Modulo() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		auto expression = Parse_Statement_Expression_UNARY();
		while (m_Lexeme.GetType() == Lexeme::Type::OpMultiply || m_Lexeme.GetType() == Lexeme::Type::OpDivide
			|| m_Lexeme.GetType() == Lexeme::Type::OpModulo)
		{
			CrLog(13, "*_/_%%");
			ReadNextLexeme();
			expression = CreateExpression<ArithmeticBinaryExpression>(expression, Parse_Statement_Expression_UNARY());
		}
		return expression;
	}

	// --------------------------------------------------------------- //
	// --                  Unary expression parsing.                -- //
	// --------------------------------------------------------------- //

	/**
	 * Logic unary expression class.
	 * Expressions should be castable to boolean type.
	 */
	class LogicUnaryExpression final : public UnaryExpression
	{
	public:
		LogicUnaryExpression(Lexeme::Type const type, Expression* const expr)
			: UnaryExpression(expr)
		{
		}
	};	// class LogicUnaryExpression

	/**
	 * Bitwise unary expression class.
	 * Expression should be castable to integral type.
	 */
	class BitwiseUnaryExpression final : public UnaryExpression
	{
	public:
		BitwiseUnaryExpression(Lexeme::Type const type, Expression* const expr)
			: UnaryExpression(expr)
		{
		}
	};	// class BitwiseUnaryExpression

	/**
	 * Per-component arithmetic unary expression class.
	 */
	class ArithmeticUnaryExpression final : public UnaryExpression
	{
	public:
		ArithmeticUnaryExpression(Lexeme::Type const type, Expression* const expr)
			: UnaryExpression(expr)
		{
		}
	};	// class ArithmeticUnaryExpression

	/**
	 * Parses all kinds of unary expressions.
	 * { Internally parses: [@]<expression> }
	 */
	CRAPI Expression* Parser::Parse_Statement_Expression_UNARY() throw(ParserException)
	{
		//! @todo Fix up everything here.
		//! @todo Add cast expressions.
		CrLog(0, __FUNCSIG__);
		return Parse_Statement_Expression_OPERAND();
		/*while (m_Lexeme.GetType() == Lexeme::Type::OpAdd || m_Lexeme.GetType() == Lexeme::Type::OpSubtract
			|| m_Lexeme.GetType() == Lexeme::Type::OpNot || m_Lexeme.GetType() == Lexeme::Type::OpBitwiseNot
			|| m_Lexeme.GetType() == Lexeme::Type::OpInc || m_Lexeme.GetType() == Lexeme::Type::OpDec)
		{
			CrLog(14, "+_-_!_~_++_--");
			ReadNextLexeme();
			Parse_Statement_Expression_OPERAND();
			break;
		}*/
	}

	class OperandExpression : public Expression
	{
	public:
		OperandExpression(...) {}
	};

	CRAPI Expression* Parser::Parse_Statement_Expression_OPERAND() throw(ParserException)
	{
		CrLog(0, __FUNCSIG__);
		switch (m_Lexeme.GetType())
		{
			case Lexeme::Type::IdIdentifier:
				CrAssert(0);
				ValidateIdentifier();
				CrLog(1, "id"); // prog.put_lex (Lex (LEX_ID, c_val));
				ReadNextLexeme();
				break;

			case Lexeme::Type::CtInteger:
			case Lexeme::Type::CtReal:
			case Lexeme::Type::CtString:
				CrLog(1, "ct"); // prog.put_lex (Lex (LEX_ID, c_val));
				ReadNextLexeme();
				return new OperandExpression();

			case Lexeme::Type::OpParenthesesBegin:
				{
					ReadNextLexeme();
					auto const parenthesesExpression = Parse_Statement_Expression();
					if (m_Lexeme.GetType() != Lexeme::Type::OpParenthesesEnd)
					{
						throw ParserException("')' expected");
					}
					ReadNextLexeme();
					return parenthesesExpression;
				}

			default:
				throw ParserException("Unexpected lexeme in the stream.");
		}
	}

	// *************************************************************** //
	// *************************************************************** //

	CrUnitTest(ScannerEmptyStream)
	{
		Parser parser(new Scanner(std::make_shared<IO::StringInputStream>("program { int a = (3 - \"aa\") * 4 * 3; real b, c; }")));
		parser.ParseProgram();
		int i = 0;
	};

}	// namespace Cr
