// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#include "Preprocessor.h"
#include "Scanner.h"

namespace Cr
{
	// *************************************************************** //
	// **            Preprocessor class implementation.             ** //
	// *************************************************************** //

	/**
	 * Reads next line from the specified stream.
	 * @returns False on the end of stream.
	 */
	bool Preprocessor::ReadNextLine()
	{
		m_Line.resize(0);
		auto c = m_InputStream->ReadNextChar();
		for (; c != '\n' && c != EOF; c = m_InputStream->ReadNextChar())
			m_Line.push_back(c);
		return c != EOF;
	}

	/**
	 * Reads next lexem from the specified stream.
	 */
	Lexeme Preprocessor::GetNextLexeme()
	{
		do
		{
			Scanner scanner(std::make_shared<IO::StringInputStream>(m_Line.c_str()));

			auto const firstLexeme = scanner.GetNextLexeme();
			if (firstLexeme.GetType() == Lexeme::Type::OpPreprocessor)
			{
				// This line contains preprocessor directive.
				auto const preprocessorDirectiveLexeme = scanner.GetNextLexeme();
				switch (preprocessorDirectiveLexeme.GetType())
				{
					// *************************************************************** //
					case Lexeme::Type::KwPpDefine: 
						{
							auto const macroIdentifierLexeme = scanner.GetNextLexeme();
							if (macroIdentifierLexeme.GetType() != Lexeme::Type::IdIdentifier)
							{
								throw PreprocessorException("Identifier expected in macro declaration.");
							}

							/*auto macroFirstContentLexemeOrParams = scanner.GetNextLexeme();
							if (macroFirstContentLexemeOrParams.GetType() == Lexeme::Type::OpPenthesesBegin)
							{

							}*/

							std::vector<Lexeme> macroContent;
							for (auto macroContentLexeme = scanner.GetNextLexeme()
								; macroContentLexeme.GetType() != Lexeme::Type::Null
								; macroContentLexeme = scanner.GetNextLexeme())
							{
								macroContent.push_back(macroContentLexeme);
							}
						}
						break;

					// *************************************************************** //
					case Lexeme::Type::KwPpUndef: 
						assert(0 && "Not implemented.");
						break;

					// *************************************************************** //
					case Lexeme::Type::KwPpIf: 
						assert(0 && "Not implemented.");
						break;

					// *************************************************************** //
					case Lexeme::Type::KwPpIfdef: 
						assert(0 && "Not implemented.");
						break;

					// *************************************************************** //
					case Lexeme::Type::KwPpIfndef: 
						assert(0 && "Not implemented.");
						break;

					// *************************************************************** //
					case Lexeme::Type::KwPpElse: 
						assert(0 && "Not implemented.");
						break;

					// *************************************************************** //
					case Lexeme::Type::KwPpElif: 
						assert(0 && "Not implemented.");
						break;

					// *************************************************************** //
					case Lexeme::Type::KwPpEndif: 
						assert(0 && "Not implemented.");
						break;

					// *************************************************************** //
					case Lexeme::Type::KwPpPragma: 
						assert(0 && "Not implemented."); 
						break;

					// *************************************************************** //
					case Lexeme::Type::KwPpLine: 
						assert(0 && "Not implemented."); 
						break;

					// *************************************************************** //
					case Lexeme::Type::KwPpError: 
						//! @todo Parse error message in preprocessoser.
						throw PreprocessorException("[[error message in preprocessoser.]]");

					default: 
						throw PreprocessorException("Unrecognized preprocessor directive.");
				}
			}
			else
			{
			}

		} while (ReadNextLine());
	}

	// *************************************************************** //
	// **              Preprocessor class unit tests.               ** //
	// *************************************************************** //

	CrUnitTest(PreprocessorEmptyStream)
	{
		auto inputStream = std::make_shared<IO::StringInputStream>("#define if ");
		Preprocessor preprocessor(inputStream);

		auto lexeme = preprocessor.GetNextLexeme();
	};

}	// namespace Cr
