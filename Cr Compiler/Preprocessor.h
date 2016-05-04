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
#include <vector>
#include <set>
#include <list>

namespace Cr
{
	CrDefineExceptionBase(PreprocessorException, WorkflowException);

	/**
	 * Represents a simple preprocessor for Cr language.
	 */
	class Preprocessor
	{
		struct PreprocessorMacro
		{
			bool m_HasAnyParams = false;
			bool m_HasVaParams = false;
			std::set<std::string> m_Params;
			std::vector<Lexeme> m_Contents;
		};	// struct PreprocessorMacro

	private:
		IO::PInputStream m_InputStream;
		std::map<std::string, PreprocessorMacro> m_Macros;
		std::string m_Line;
		std::vector<Lexeme> m_CachedLexems;

	private:
		CRINT bool ReadNextLine();
		void InternallyReadLexemes();

		CRINT void ParseDirective_DEFINE(Scanner& scanner) throw(PreprocessorException);
		CRINT void ParseDirective_UNDEF(Scanner& scanner) throw(PreprocessorException);
		CRINT void SubstituteForLine(std::list<Lexeme>& line);

	public:

		/**
		 * Initializes a new scanner from the specified stream.
		 */
		CRINL explicit Preprocessor(IO::PInputStream const& inputStream)
			: m_InputStream(inputStream)
		{
			assert(inputStream != nullptr);
			ReadNextLine();
		}

		/**
		 * Reads next lexem from the specified stream.
		 */
		CR_API Lexeme GetNextLexeme();

	};	// class Preprocessor

}	// namespace Cr
