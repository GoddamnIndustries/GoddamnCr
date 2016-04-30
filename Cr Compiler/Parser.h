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

#include <stack>

namespace Cr
{
	CrDefineExceptionBase(ParserException, WorkflowException);

	class Expression;

	class Parser
	{

	public:
		Scanner* m_Scanner;
		Lexeme m_Lexeme, m_PrevLexeme;
		std::stack<Lexeme::Type> m_OperationTypes;
		std::stack<Lexeme::Type> m_OperandTypes;
		std::stack<std::string> m_Poliz;

	public:
		CRINT void ReadNextLexeme();
		CRAPI void ParseProgram() throw(ParserException);
		CRAPI void Parse_Statement_OR_Declaration() throw(ParserException);

		template<typename TExpression>
		CRINL TExpression* CreateExpression(Expression* lhs, Expression* rhs)
		{
		}

		CRAPI void Parse_Statement() throw(ParserException);
		CRAPI void Parse_Statement_Null() throw(ParserException);
		CRAPI void Parse_Statement_Definition() throw(ParserException);
		CRAPI Expression* Parse_Statement_Expression() throw(ParserException);
		CRAPI Expression* Parse_Statement_Expression_Assignments() throw(ParserException);
		CRAPI Expression* Parse_Statement_Expression_Ternary() throw(ParserException);
		CRAPI Expression* Parse_Statement_Expression_Or() throw(ParserException);
		CRAPI Expression* Parse_Statement_Expression_And() throw(ParserException);
		CRAPI Expression* Parse_Statement_Expression_BitwiseOr() throw(ParserException);
		CRAPI Expression* Parse_Statement_Expression_BitwiseXor() throw(ParserException);
		CRAPI Expression* Parse_Statement_Expression_BitwiseAnd() throw(ParserException);
		CRAPI Expression* Parse_Statement_Expression_Equals_OR_NotEquals() throw(ParserException);
		CRAPI Expression* Parse_Statement_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals() throw(ParserException);
		CRAPI Expression* Parse_Statement_Expression_BitwiseLeftShift_OR_BitwiseRightShift() throw(ParserException);
		CRAPI Expression* Parse_Statement_Expression_Add_OR_Subtract() throw(ParserException);
		CRAPI Expression* Parse_Statement_Expression_Multiply_OR_Divide_OR_Modulo() throw(ParserException);
		CRAPI Expression* Parse_Statement_Expression_UNARY() throw(ParserException);
		CRAPI Expression* Parse_Statement_Expression_OPERAND() throw(ParserException);
		
		CRAPI void Parse_Expression() throw(ParserException) {}
		
		CRAPI void Parse_Declaration() throw(ParserException) {}

		void ValidateIdentifier() {}

	public:
		Parser(Scanner* scanner) : m_Scanner(scanner) {}
		CRINL Parser(Parser const&) = delete;
		CRINL Parser& operator= (Parser const&) = delete;

	};	// class Parser

}	// namespace Cr
