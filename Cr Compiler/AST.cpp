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

#include "Parser.h"

namespace Cr
{
	// --------------------------------------------------------------- //
	// --               Selection statement parsing.                -- //
	// --------------------------------------------------------------- //

	// *************************************************************** //
	CR_API void Ast::IfSelectionStatement::Initialize(Expression* const condExpr, Statement* const thenStmt
		, Statement* const elseStmt)
	{
		auto const condExprType = condExpr->GetType();
		if (!condExprType.IsScalar(BaseType::Bool))
		{
			throw ParserException("Condition expression of the 'if' statement must be convertible to scalar boolean "
				"value.");
		}
		m_CondExpr.reset(condExpr);
		m_ThenStmt.reset(thenStmt);
		m_ElseStmt.reset(elseStmt);
	}

	// *************************************************************** //
	CR_API void Ast::SwitchSelectionStatement::Initialize(Expression* const switchExpr, ...)
	{
		if (!switchExpr->GetType().IsScalar(BaseType::Bool, BaseType::UInt))
		{
			throw ParserException("Expression of the 'switch' statement must be convertible to scalar integral value.");
		}
		m_SelectionExpr.reset(switchExpr);
	}
}
