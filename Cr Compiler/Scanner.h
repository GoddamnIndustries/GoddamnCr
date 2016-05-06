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
		CRINL void ReadNextChar();

	public:
		CRINL Scanner(Scanner const&) = delete;
		CRINL Scanner& operator= (Scanner const&) = delete;

		/**
		 * Initializes a new scanner from the specified stream.
		 */
		CRINL explicit Scanner(IO::PInputStream const& inputStream, IdentifierTable* const identifierTable = nullptr)
			: m_InputStream(inputStream), m_IdentifierTable(identifierTable)
		{
			assert(inputStream != nullptr);
			ReadNextChar();
		}

	public:

		/**
		 * Reads next lexem from the specified stream.
		 * @returns Scanned lexeme or null lexeme on end of stream.
		 */
		CR_API Lexeme GetNextLexemeRaw() throw(ScannerException);
		CR_API Lexeme GetNextLexeme() throw(ScannerException)
		{
			return GetNextLexemeRaw();
		}

	};	// class Scanner

}	// namespace Cr
