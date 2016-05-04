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

#include "Lexeme.h"
#include <iostream>

namespace Cr
{
	std::map<std::string, Lexeme::Type> const Lexeme::s_KeywordsTable = {
		
		// Keywords
		{ "program", Type::KwProgram },
		{ "if",      Type::KwIf      }, { "else",   Type::KwElse   },
		{ "for",     Type::KwFor     }, { "while",  Type::KwWhile  }, { "break", Type::KwBreak }, { "continue", Type::KwContinue },
		{ "read",    Type::KwRead    }, { "write",  Type::KwWrite  }, 
		{ "int",     Type::KwInt     }, { "real", Type::KwReal },

		// Preprocessor keywords.
		{ "define", Type::KwPpDefine }, { "undef",  Type::KwPpUndef  }, { "defined", Type::KwPpDefined },
		{ "ifdef",  Type::KwPpIfdef  }, { "ifndef", Type::KwPpIfndef }, { "elif",    Type::KwPpElif    }, { "endif",    Type::KwPpEndif },
		{ "pragma", Type::KwPpPragma }, { "line",   Type::KwPpLine   }, { "error",   Type::KwPpError   },
	};

}	// namespace Cr
