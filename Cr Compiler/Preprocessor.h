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
#include "Scanner.h"

#include <deque>

namespace Cr
{
	CrDefineExceptionBase(PreprocessorException, WorkflowException);

	/**
	 * Represents a simple preprocessor for Cr language.
	 */
	class Preprocessor
	{
	public:

		/**
		 * Initializes a new scanner from the specified stream.
		 */
		CRINL explicit Preprocessor(IO::PInputStream const& inputStream)
			: m_InputStream(inputStream), m_DoWriteLexemes(true)
		{
			CrAssert(inputStream != nullptr);
			ReadNextLexeme();
		}

		/**
		 * Reads next lexem from the specified stream.
		 */
		CR_API Lexeme GetNextLexeme();

	private:
		struct Macro
		{
			std::deque<Lexeme> m_Lexemes;
		};	// struct Macro

		IO::PInputStream             m_InputStream;
		Lexeme                       m_Lexeme;
		bool                         m_DoWriteLexemes;
		std::deque<Lexeme>           m_LinePipe;
		std::deque<Lexeme>           m_LexemesPipe;
		std::map<std::string, Macro> m_Macros;

		CR_INTERNAL void ReadNextLexeme();
		CR_INTERNAL void ReadNextLexeme(Lexeme::Type const type);
		CR_INTERNAL void ExpectLexeme(Lexeme::Type const type) const;

		CR_INTERNAL void Parse_Block(bool const isGlobalScope = false);
		CR_INTERNAL void Parse_OrdinaryLine();
		CR_INTERNAL void Parse_Directive_Define();
		CR_INTERNAL void Parse_Directive_Undef();
		CR_INTERNAL void Parse_Directive_If();
		CR_INTERNAL void Parse_Directive_Ifdef();
		CR_INTERNAL void Parse_Directive_Ifndef();
		CR_INTERNAL void Parse_Directive_ElifElseEndif_Section(bool const cond, bool const allowElifOrElse = true);

		CR_INTERNAL int64_t EvaluateExpression();
		CR_INTERNAL int64_t EvaluateExpression_Or();
		CR_INTERNAL int64_t EvaluateExpression_And();
		CR_INTERNAL int64_t EvaluateExpression_BitwiseOr();
		CR_INTERNAL int64_t EvaluateExpression_BitwiseXor();
		CR_INTERNAL int64_t EvaluateExpression_BitwiseAnd();
		CR_INTERNAL int64_t EvaluateExpression_Equals_OR_NotEquals();
		CR_INTERNAL int64_t EvaluateExpression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals();
		CR_INTERNAL int64_t EvaluateExpression_BitwiseLeftShift_OR_BitwiseRightShift();
		CR_INTERNAL int64_t EvaluateExpression_Add_OR_Subtract();
		CR_INTERNAL int64_t EvaluateExpression_Multiply_OR_Divide_OR_Modulo();
		CR_INTERNAL int64_t EvaluateExpression_Unary();
		CR_INTERNAL int64_t EvaluateExpression_Primary();

	};	// class Preprocessor

}	// namespace Cr
