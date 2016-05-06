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

//! @todo Implement preprocessor.
#include "Preprocessor.h"
#include "Scanner.h"
#include <list>

namespace Cr
{
	// *************************************************************** //
	// **            Preprocessor class implementation.             ** //
	// *************************************************************** //

	/**
	 * Reads next line from the specified stream.
	 * @returns False on the end of stream.
	 */
	CRINL bool Preprocessor::ReadNextLine()
	{
		//! @todo Add parsing backslashes at line breaks.
		m_Line.resize(0);
		auto c = m_InputStream->ReadNextChar();
		for (; c != '\n' && c != EOF; c = m_InputStream->ReadNextChar())
		{
			m_Line.push_back(static_cast<char>(c));
		}
		return c != EOF;
	}

	/**
	 * Reads next lexeme from the specified stream.
	 */
	void Preprocessor::InternallyReadLexemes()
	{
		do
		{
			Scanner scanner(std::make_shared<IO::StringInputStream>(m_Line.c_str()));

			auto const firstLexeme = scanner.GetNextLexemeRaw();
			if (firstLexeme.GetType() == Lexeme::Type::OpPreprocessor)
			{
				// This line contains preprocessor directive.
				auto const preprocessorDirectiveLexeme = scanner.GetNextLexemeRaw();
				switch (preprocessorDirectiveLexeme.GetType())
				{
					case Lexeme::Type::KwPpDefine: ParseDirective_DEFINE(scanner); break;
					case Lexeme::Type::KwPpUndef: ParseDirective_UNDEF(scanner); break;

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
						//! @todo Parse error message in preprocessor.
						throw PreprocessorException("[[error message in preprocessor.]]");

					default: 
						throw PreprocessorException("Unrecognized preprocessor directive.");
				}
			}
			else
			{
				// Reading the whole line to substitute macros in it.
				std::list<Lexeme> lineContents = { firstLexeme };
				for (auto lineContentLexeme = scanner.GetNextLexemeRaw()
						; lineContentLexeme.GetType() != Lexeme::Type::Null
						; lineContentLexeme = scanner.GetNextLexemeRaw())
				{
					lineContents.push_back(lineContentLexeme);
				}

				SubstituteForLine(lineContents);
			}

			ReadNextLine();
		} while (true);
	}

	/**
	 * Parses '#define' directive.
	 */
	CR_INTERNAL void Preprocessor::ParseDirective_DEFINE(Scanner& scanner) throw(PreprocessorException)
	{
		auto const macroIdentifierLexeme = scanner.GetNextLexemeRaw();
		if (macroIdentifierLexeme.GetType() != Lexeme::Type::IdIdentifier)
		{
			throw PreprocessorException("Identifier expected in the '#define' directive.");
		}

		auto& macro = m_Macros[macroIdentifierLexeme.GetValueID()];

		// *************************************************************** //
		// Determining whether this directive has any parameters. 
		auto const macroFirstContentLexemeOrParams = scanner.GetNextLexemeRaw();
		if (macroFirstContentLexemeOrParams.GetType() == Lexeme::Type::OpParenOpen)
		{
			macro.m_HasAnyParams = true;

			// This macro comes with parameters.
			auto const macroFirstParamOrEndParams = scanner.GetNextLexemeRaw();
			if (macroFirstParamOrEndParams.GetType() != Lexeme::Type::OpParenClose)
			{
				// Macro has not empty parameter list.
				for (auto macroParam = macroFirstParamOrEndParams;;)
				{
					if (macroFirstParamOrEndParams.GetType() != Lexeme::Type::IdIdentifier)
					{
						throw PreprocessorException("Identifier expected in macro parameter name.");
					}

					macro.m_Params.insert(macroParam.GetValueID());
					
					auto macroParamCommaOrEnd = scanner.GetNextLexemeRaw();
					if (macroParamCommaOrEnd.GetType() != Lexeme::Type::OpParenClose && macroParamCommaOrEnd.GetType() != Lexeme::Type::OpComma)
					{
						throw PreprocessorException("Comma or closing parentheses expected.");
					}

					if (macroParamCommaOrEnd.GetType() == Lexeme::Type::OpParenClose)
					{
						break;
					}
					macroParam = scanner.GetNextLexemeRaw();
				}
			}
		}

		// *************************************************************** //
		// Reading whole contents of a macro.
		for (auto macroContentLexeme = scanner.GetNextLexemeRaw()
			; macroContentLexeme.GetType() != Lexeme::Type::Null
			; macroContentLexeme = scanner.GetNextLexemeRaw())
		{
			macro.m_Contents.push_back(macroContentLexeme);
		}
	}

	/**
	 * Parses '#undef' directive.
	 */
	CR_INTERNAL void Preprocessor::ParseDirective_UNDEF(Scanner& scanner) throw(PreprocessorException)
	{
		auto const macroIdLexeme = scanner.GetNextLexemeRaw();
		if (macroIdLexeme.GetType() != Lexeme::Type::IdIdentifier)
		{
			throw PreprocessorException("Identifier expected in the '#undef' directive.");
		}
		m_Macros.erase(macroIdLexeme.GetValueID());
	}

	// *************************************************************** //
	// *************************************************************** //
	// *************************************************************** //

	/**
	 * Substitues all entries of predefined macros in the list of lexemes.
	 * @param lineContents List of the lexemes.
	 */
	CR_INTERNAL void Preprocessor::SubstituteForLine(std::list<Lexeme>& lineContents)
	{
		// Substituting all existing macros.
		for (auto allMacroSubstituted = false; !allMacroSubstituted;)
		{
			auto anyMacroSubstituted = false;
			for (auto lineContentsIter = lineContents.begin(); lineContentsIter != lineContents.end(); ++lineContentsIter)
			{
				auto const& lineContentsLexeme = *lineContentsIter;
				if (lineContentsLexeme.GetType() == Lexeme::Type::IdIdentifier && m_Macros.count(lineContentsLexeme.GetValueID()) != 0)
				{
					anyMacroSubstituted = true;

					// Found a macro to substitute.
					auto const& macro = m_Macros[lineContentsLexeme.GetValueID()];
					if (macro.m_HasAnyParams)
					{
						assert(0);
					}

					for (auto const& macroContentsLexeme : macro.m_Contents)
					{
						lineContents.insert(lineContentsIter, macroContentsLexeme);
					}
					lineContents.erase(lineContentsIter);
					break;
				}
			}

			// *************************************************************** //
			// Substitution of any macros during the pass means that this could possibly lead
			// to appearence of new macros in the line.
			if (!anyMacroSubstituted)
			{
				allMacroSubstituted = true;
			}
		}
	}

	Lexeme Preprocessor::GetNextLexeme()
	{
		if (m_CachedLexems.empty())
		{
			InternallyReadLexemes();
		}
		return m_CachedLexems[0];
	}

	// *************************************************************** //
	// **              Preprocessor class unit tests.               ** //
	// *************************************************************** //

	/*CrUnitTest(PreprocessorEmptyStream)
	{
		auto inputStream = std::make_shared<IO::StringInputStream>(
			"#define AAA 3\n "
			"#define BBB(a, b) a b \n "
			"BBB((a,c),) += AAA \n"
			"#undef AAA \n"
			"AAA ?"
			);
		Preprocessor preprocessor(inputStream);

		auto lexeme = preprocessor.GetNextLexeme();
	};*/

}	// namespace Cr
