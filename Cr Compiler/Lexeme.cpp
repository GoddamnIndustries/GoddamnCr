// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#include "Lexeme.h"
#include <iostream>

namespace Cr
{
	std::map<std::string, Lexeme::Type> const Lexeme::s_KeywordsTable = {
		
		// Keywords
		{ "program", Type::KwProgram },
		{ "goto",    Type::KwGoto    },
		{ "if",      Type::KwIf      }, { "else",   Type::KwElse   },
		{ "for",     Type::KwFor     }, { "while",  Type::KwWhile  }, { "break", Type::KwBreak }, { "continue", Type::KwContinue },
		{ "read",    Type::KwRead    }, { "write",  Type::KwWrite  }, 
		{ "int",     Type::KwInt     }, { "string", Type::KwString }, { "real", Type::KwReal },

		// Preprocessor keywords.
		{ "define", Type::KwPpDefine }, { "undef",  Type::KwPpUndef  }, { "defined", Type::KwPpDefined },
		{ "ifdef",  Type::KwPpIfdef  }, { "ifndef", Type::KwPpIfndef }, { "elif",    Type::KwPpElif    }, { "endif",    Type::KwPpEndif },
		{ "pragma", Type::KwPpPragma }, { "line",   Type::KwPpLine   }, { "error",   Type::KwPpError   },

		//! @todo Remove this trash.
		{ "paktor",  Type::KwProgram }, 
		{ "poidemte",Type::KwGoto    },
		{ "esli",    Type::KwIf      }, { "inache", Type::KwElse   },
		{ "kek",     Type::KwFor     }, { "while",  Type::KwWhile  }, { "makarov", Type::KwBreak }, { "prodolzhai", Type::KwContinue },
		{ "vvod",    Type::KwRead    }, { "vivod",  Type::KwWrite  }, 
		{ "celznach",Type::KwInt     }, { "massiv_simvolov",Type::KwString }, { "chislo", Type::KwReal },

		{ "type_of_my_mom",Type::KwInt }, { "massiv_simvolov",Type::KwString }, { "my_type_too", Type::KwReal },
	};

}	// namespace Cr
