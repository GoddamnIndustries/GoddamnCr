// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#pragma once
#include "Utils.h"
#include "Lexeme.h"

namespace Cr
{
	CrDefineExceptionBase(ScannerException, WorkflowException);

	/** 
	 * Represents a simple lexical analyzer for Cr language.
	 */
	class Scanner final
	{	
	private:
		IO::PInputStream m_InputStream;
		IdentifierTable* m_IdentifierTable;
		int m_Char, m_PrevChar;

	private:
		void ReadNextChar();

	public:

		/**
		 * Initializes a new scanner from the specified stream.
		 */
		explicit Scanner(IO::PInputStream const& inputStream, IdentifierTable* const identifierTable = nullptr)
			: m_InputStream(inputStream), m_IdentifierTable(identifierTable)
		{
			assert(inputStream != nullptr);
			ReadNextChar();
		}

		/**
		 * Reads next lexem from the specified stream.
		 */
		Lexeme GetNextLexeme();

	};	// class Scanner

}	// namespace Cr
