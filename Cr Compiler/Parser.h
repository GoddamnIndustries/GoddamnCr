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

#include "Preprocessor.h"
#include "AST.h"

#include <list>

namespace Cr
{
	class Profile;

	/** 
	 * Represents a simple syntax and semantic analyzer and evaluator for Cr language.
	 */
	class Parser final
	{
	public:
		CR_API Parser(Parser const&) = delete;
		CR_API Parser& operator= (Parser const&) = delete;

		/**
		 * Initializes a new parser from the specified preprocessor.
		 */
		CR_API explicit Parser(Preprocessor* scanner)
			: m_Preprocesser(scanner) { ReadNextLexeme(); }
		
		CR_API void ParseProgram();

	private:
		Profile*        m_Profile;
		Preprocessor*   m_Preprocesser;
		Lexeme          m_Lexeme;
		Ast::Function*  m_Function;
		Ast::Statement* m_JumpOnBreak;
		Ast::Statement* m_JumpOnContinue;
		std::list<std::map<std::string, std__shared_ptr<Ast::Identifier>>> m_ScopedIdents;

		CRINL Ast::Identifier* FindIdentifier(std::string const& name)
		{
			return m_ScopedIdents.back()[name];
		}
		CRINL Ast::Identifier* FindIdentifier()
		{
			return m_ScopedIdents.back()[m_Lexeme.GetValueID()];
		}

		CRINL void DeclareVariable(Ast::Variable* const var)
		{
			m_ScopedIdents.back()[var->m_Name] = var;
		}

		CR_INTERNAL void ReadNextLexeme();
		CRINL void ReadNextLexeme(Lexeme::Type const type);
		CRINL void ExpectLexeme(Lexeme::Type const type);
		CRINL void ReadNextAndExpectLexeme(Lexeme::Type const type);

		// Recursive descent parsing methods.
		CR_INTERNAL Ast::Statement* Parse_Statement();
		CR_INTERNAL Ast::Statement* Parse_Statement_Compound();
		CR_INTERNAL Ast::Statement* Parse_Statement_Selection_If();
		CR_INTERNAL Ast::Statement* Parse_Statement_Selection_Switch();
		CR_INTERNAL Ast::Statement* Parse_Statement_Iteration_Do();
		CR_INTERNAL Ast::Statement* Parse_Statement_Iteration_For();
		CR_INTERNAL Ast::Statement* Parse_Statement_Iteration_While();
		CR_INTERNAL Ast::Statement* Parse_Statement_Jump_Break();
		CR_INTERNAL Ast::Statement* Parse_Statement_Jump_Return();
		CR_INTERNAL Ast::Statement* Parse_Statement_Jump_Discard();
		CR_INTERNAL Ast::Statement* Parse_Statement_Jump_Continue();
		
		CR_HELPER Ast::Type ParseHelper_Type();
		CR_INTERNAL Ast::Statement* Parse_Statement_Declaration_OR_Expression(bool const allowFuncDecl = false);
		CR_INTERNAL Ast::Statement* Parse_Statement_Declaration_Variable_OR_Function(Ast::Type const& type, bool const allowFuncDecl = false);
		CR_INTERNAL Ast::Statement* Parse_Statement_Declaration_Typedef();
		CR_INTERNAL Ast::Statement* Parse_Statement_Declaration_Struct();

		CR_INTERNAL Ast::Statement* Parse_Statement_Expression();
		CR_INTERNAL Ast::Statement* Parse_Statement_Scoped()
		{
			m_ScopedIdents.emplace_back();
			auto const stmt = Parse_Statement();
			m_ScopedIdents.pop_back();
			return stmt;
		}

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
		CR_INTERNAL Ast::Expression* Parse_Expression_PrefixUnary_Plus();
		CR_INTERNAL Ast::Expression* Parse_Expression_PrefixUnary_Negate();
		CR_INTERNAL Ast::Expression* Parse_Expression_PrefixUnary_Not();
		CR_INTERNAL Ast::Expression* Parse_Expression_PrefixUnary_BitwiseNot();
		CR_INTERNAL Ast::Expression* Parse_Expression_PrefixUnary_Cast_OR_Paren();
		CR_INTERNAL Ast::Expression* Parse_Expression_PrefixUnary_Paren();
		CR_INTERNAL Ast::Expression* Parse_Expression_PrefixUnary_Factor();
		CR_INTERNAL Ast::Expression* Parse_Expression_PrefixUnary_Operand();

	};	// class Parser

}	// namespace Cr
