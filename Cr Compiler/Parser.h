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
#include "AST.h"

#include <vector>
#include <list>

namespace Cr
{
	CrDefineExceptionBase(ParserException, WorkflowException);

	class Profile;

	class Parser
	{
	private:
		Profile* m_Profile;
		Scanner* m_Scanner;
		
		Lexeme m_Lexeme;

	private:
		bool m_DoUnroll = true, m_DoAnalyse = true;
		Ast::Function* m_Function;
		Ast::Statement* m_JumpOnBreak;
		Ast::/*IterationStatement*/Statement* m_JumpOnContinue;
		std::list<std::map<std::string, std::shared_ptr<Ast::Identifier>>> m_ScopedIdents;

	private:
		CRINL Ast::Identifier* FindIdentifier(std::string const& name)
		{
			return m_ScopedIdents.back()[name].get();
		}
		CRINL void DeclareVariable(Ast::Variable* const var)
		{
			m_ScopedIdents.back()[var->m_Name].reset(var);
		}

		CR_INTERNAL void ReadNextLexeme();
		CRINL void ReadNextLexeme(Lexeme::Type const type);
		CRINL void ExpectLexeme(Lexeme::Type const type);
		CRINL void ReadNextAndExpectLexeme(Lexeme::Type const type);

		CR_API Ast::Statement* Parse_Statement();
		CR_API Ast::Statement* Parse_Statement_Compound();
		CR_API Ast::Statement* Parse_Statement_Selection_If();
		CR_API Ast::Statement* Parse_Statement_Selection_Switch();
		CR_API Ast::Statement* Parse_Statement_Iteration_Do();
		CR_API Ast::Statement* Parse_Statement_Iteration_For();
		CR_API Ast::Statement* Parse_Statement_Iteration_While();
		CR_API Ast::Statement* Parse_Statement_Jump_Break();
		CR_API Ast::Statement* Parse_Statement_Jump_Return();
		CR_API Ast::Statement* Parse_Statement_Jump_Discard();
		CR_API Ast::Statement* Parse_Statement_Jump_Continue();
		CR_API Ast::Statement* Parse_Statement_Declaration_OR_Expression();
		CR_API Ast::Statement* Parse_Statement_Declaration_Variable_OR_Function();
		CR_API Ast::Statement* Parse_Statement_Declaration_Typedef();
		CR_API Ast::Statement* Parse_Statement_Declaration_Struct();
		CR_API Ast::Statement* Parse_Statement_Expression();

		CR_API Ast::Statement* Parse_Statement_Scoped() { return Parse_Statement(); }
		
		CR_INTERNAL Ast::Expression* Parse_Expression();
		CR_INTERNAL Ast::Expression* Parse_Expression_Comma();
		CR_INTERNAL Ast::Expression* Parse_Expression_Assignments();
		CR_INTERNAL Ast::Expression* Parse_Expression_Ternary();
		CR_HELPER Ast::Expression* ParseHelper_Expression_LogicBinary(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) const;
		CR_HELPER Ast::Expression* ParseHelper_Expression_BitwiseBinary(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) const;
		CR_HELPER Ast::Expression* ParseHelper_Expression_ArithmeticBinary(Lexeme::Type const op, Ast::Expression* const lhs, Ast::Expression* const rhs) const;
		CR_INTERNAL Ast::Expression* Parse_Expression_Or();
		CR_INTERNAL Ast::Expression* Parse_Expression_And();
		CR_INTERNAL Ast::Expression* Parse_Expression_BitwiseOr();
		CR_INTERNAL Ast::Expression* Parse_Expression_BitwiseXor();
		CR_INTERNAL Ast::Expression* Parse_Expression_BitwiseAnd();
		CR_INTERNAL Ast::Expression* Parse_Expression_Equals_OR_NotEquals();
		CR_INTERNAL Ast::Expression* Parse_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals();
		CR_INTERNAL Ast::Expression* Parse_Expression_BitwiseLeftShift_OR_BitwiseRightShift();
		CR_INTERNAL Ast::Expression* Parse_Expression_Add_OR_Subtract();
		CR_INTERNAL Ast::Expression* Parse_Expression_Multiply_OR_Divide_OR_Modulo();
		CR_INTERNAL Ast::Expression* Parse_Expression_PrefixUnary();
		CR_INTERNAL Ast::Expression* Parse_Expression_Unary();

	public:
		CR_API void ParseProgram();

		Parser(Scanner* scanner) : m_Scanner(scanner) { ReadNextLexeme(); }
		CRINL Parser(Parser const&) = delete;
		CRINL Parser& operator= (Parser const&) = delete;

	};	// class Parser

}	// namespace Cr
