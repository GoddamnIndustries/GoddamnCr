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

#include "../Lexer/Lexer.hpp"

namespace Cr
{
	class SyntaxFacts
	{
	public:
		static bool IsPunctuationOrKeyword(...) { return true; }
	};	// class SyntaxFacts

	class SyntaxFactory
	{
	public:
		// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //

		static SyntaxLexeme Token(...) { return nullptr; }
		static SyntaxLexeme Identifier(...) { return nullptr; }
		static SyntaxLexeme Literal(...) { return nullptr; }

		// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //

		static SyntaxTrivia Whitespace() { return nullptr; }

		static SyntaxTrivia Comment(...) { return nullptr; }

		static SyntaxTrivia CarriageReturnLineFeed() { return nullptr; }
		static SyntaxTrivia CarriageReturn() { return nullptr; }
		static SyntaxTrivia LineFeed() { return nullptr; }
	};	// class SyntaxFactory

}	// namespace Cr
