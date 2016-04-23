// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

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
	bool Preprocessor::ReadNextLine()
	{
		m_Line.resize(0);
		auto c = m_InputStream->ReadNextChar();
		for (; c != '\n' && c != EOF; c = m_InputStream->ReadNextChar())
			m_Line.push_back(static_cast<char>(c));
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
								throw PreprocessorException("Identifier expected in the '#define' directive.");
							}

							// Found a macro declaration.
							auto& macro = m_Macros[macroIdentifierLexeme.GetValueID()->m_Value];
							auto const macroFirstContentLexemeOrParams = scanner.GetNextLexeme();
							if (macroFirstContentLexemeOrParams.GetType() == Lexeme::Type::OpParenthesesBegin)
							{
								// This macro comes with parameters.
								macro.m_HasAnyParams = true;

								auto const macroFirstParamOrEndParams = scanner.GetNextLexeme();
								if (macroFirstParamOrEndParams.GetType() != Lexeme::Type::OpParenthesesEnd)
								{
									for (auto macroParam = macroFirstParamOrEndParams;; )
									{
										if (macroFirstParamOrEndParams.GetType() != Lexeme::Type::IdIdentifier)
										{
											throw PreprocessorException("Identifier expected in macro parameter name.");
										}

										macro.m_Params.insert(macroParam.GetValueID()->m_Value);
										auto macroParamCommaOrEnd = scanner.GetNextLexeme();
										if (macroParamCommaOrEnd.GetType() == Lexeme::Type::OpParenthesesEnd)
										{
											break;
										}
										else if (macroParamCommaOrEnd.GetType() == Lexeme::Type::OpComma)
										{
											macroParam = scanner.GetNextLexeme();
											continue;
										}
										else
										{
											throw PreprocessorException("Comma or closing parentheses expected.");
										}
									}
								}
							}

							for (auto macroContentLexeme = scanner.GetNextLexeme()
								; macroContentLexeme.GetType() != Lexeme::Type::Null
								; macroContentLexeme = scanner.GetNextLexeme())
							{
								macro.m_Contents.push_back(macroContentLexeme);
							}
						}
						break;

					// *************************************************************** //
					case Lexeme::Type::KwPpUndef: 
						{
							auto const macroIdentifierLexeme = scanner.GetNextLexeme();
							if (macroIdentifierLexeme.GetType() != Lexeme::Type::IdIdentifier)
							{
								throw PreprocessorException("Identifier expected in the '#undef' directive.");
							}
							m_Macros.erase(macroIdentifierLexeme.GetValueID()->m_Value);
						}
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
				for (auto lineContentLexeme = scanner.GetNextLexeme()
						; lineContentLexeme.GetType() != Lexeme::Type::Null
						; lineContentLexeme = scanner.GetNextLexeme())
				{
					lineContents.push_back(lineContentLexeme);
				}

				// Substituting all existing macros.
				for (bool allMacroSubstituted = false; !allMacroSubstituted;)
				{
					bool anyMacroSubstituted = false;
					for (auto lineContentsIter = lineContents.begin()
							; lineContentsIter != lineContents.end(); ++lineContentsIter)
					{
						auto const& lineContentsLexeme = *lineContentsIter;
						if (lineContentsLexeme.GetType() == Lexeme::Type::IdIdentifier
								&& m_Macros.count(lineContentsLexeme.GetValueID()->m_Value) != 0)
						{
							anyMacroSubstituted = true;
							auto const& macro = m_Macros[lineContentsLexeme.GetValueID()->m_Value];
							if (macro.m_HasAnyParams)
							{

							}

							for (auto const& macroContentsLexeme : macro.m_Contents)
							{
								lineContents.insert(lineContentsIter, macroContentsLexeme);
							}
							lineContents.erase(lineContentsIter);
							break;
						}
					}
					if (!anyMacroSubstituted)
					{
						allMacroSubstituted = true;
					}
				}

				int i  =0;
			}

			ReadNextLine();
		} while (true);
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

	CrUnitTest(PreprocessorEmptyStream)
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
	};

}	// namespace Cr
