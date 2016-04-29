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

	

	class Parser
	{
		enum class ExpressionBaseType
		{
			Void,
			Int = (int)Lexeme::Type::CtInteger,
			Real = (int)Lexeme::Type::CtReal,
			String = (int)Lexeme::Type::CtString,
		};

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
	
		CRAPI void Parse_Statement() throw(ParserException);
		CRAPI void Parse_Statement_Null();
		CRAPI void Parse_Statement_Definition();
		CRAPI ExpressionType Parse_Statement_Expression();
		CRAPI ExpressionType Parse_Statement_Expression_Assignments();
		CRAPI ExpressionType Parse_Statement_Expression_Ternary();
		CRAPI ExpressionType Parse_Statement_Expression_Or();
		CRAPI ExpressionType Parse_Statement_Expression_And();
		CRAPI ExpressionType Parse_Statement_Expression_BitwiseOr();
		CRAPI ExpressionType Parse_Statement_Expression_BitwiseXor();
		CRAPI ExpressionType Parse_Statement_Expression_BitwiseAnd();
		CRAPI ExpressionType Parse_Statement_Expression_Equals_OR_NotEquals();
		CRAPI ExpressionType Parse_Statement_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals();
		CRAPI ExpressionType Parse_Statement_Expression_BitwiseLeftShift_OR_BitwiseRightShift();
		CRAPI ExpressionType Parse_Statement_Expression_Add_OR_Subtract();
		CRAPI ExpressionType Parse_Statement_Expression_Multiply_OR_Divide_OR_Modulo();
		CRAPI ExpressionType Parse_Statement_Expression_UNARY();
		CRAPI ExpressionType Parse_Statement_Expression_OPERAND();
		
		CRAPI void Parse_Expression() throw(ParserException) {}
		
		CRAPI void Parse_Declaration() throw(ParserException) {}

		void ValidateIdentifier() {}

	public:
		Parser(Scanner* scanner) : m_Scanner(scanner) {}
		CRINL Parser(Parser const&) = delete;
		CRINL Parser& operator= (Parser const&) = delete;

	};	// class Parser

}	// namespace Cr
