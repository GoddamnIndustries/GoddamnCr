// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#pragma once
#include "Scanner.h"
#include <vector>

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
			std::vector<Lexeme> m_Contents;
		};	// struct PreprocessorMacro

	private:
		IO::PInputStream m_InputStream;
		std::map<std::string, PreprocessorMacro> m_Macros;
		std::string m_Line;

	private:
		bool ReadNextLine();

	public:

		/**
		 * Initializes a new scanner from the specified stream.
		 */
		explicit Preprocessor(IO::PInputStream const& inputStream)
			: m_InputStream(inputStream)
		{
			assert(inputStream != nullptr);
			ReadNextLine();
		}

		/**
		 * Reads next lexem from the specified stream.
		 */
		Lexeme GetNextLexeme();

	};	// class Preprocessor

}	// namespace Cr