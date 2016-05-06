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
#include "Profile.h"
#include "Utils.h"

#include <vector>
#include <algorithm>

namespace Cr
{
	// *************************************************************** //
	// **                Parser class implementation.               ** //
	// *************************************************************** //

	/**
	 * Reads next lexeme from the specified scanner.
	 */
	CRINL void Parser::ReadNextLexeme()
	{
		m_Lexeme = m_Scanner->GetNextLexemeRaw();
	}

	/**
	 * Reads next lexeme from the specified scanner and checks whether it
	 * is of expected type.
	 */
	//! @todo Do we need this method actually?
	CRINL void Parser::ReadNextAndExpectLexeme(Lexeme::Type const type)
	{
		ReadNextLexeme();
		if (m_Lexeme != type)
		{
			throw ParserException("something was expected.");
		}
	}

	/**
	 * Checks whether current lexeme is of expected type and
	 * reads next lexeme.
	 */
	CRINL void Parser::ExpectLexeme(Lexeme::Type const type)
	{
		if (m_Lexeme != type)
		{
			throw ParserException("something was expected.");
		}
	}
	CRINL void Parser::ReadNextLexeme(Lexeme::Type const type)
	{
		ExpectLexeme(type);
		ReadNextLexeme();
	}

	// *************************************************************** //
	CRINL static void VerifyLValue(Ast::Expression* const lhs)
	{
		if (!lhs->IsLValue())
		{
			throw ParserException("Expected l-value.");
		}
	}
	CRINL static void VerifyConstexpr(Ast::Expression* const lhs)
	{
		if (!lhs->IsConstexpr())
		{
			throw ParserException("Expected compile-time constant expression.");
		}
	}
	CRINL static void VerifyTypesMatch(Ast::Type const& lhsExprType, Ast::Type const& rhsExprType)
	{
		if (lhsExprType.IsIncompatibleWith(rhsExprType))
		{
			throw ParserException("Incompatible types in the sub-expressions of the binary expression.");
		}
	}
	CRINL static void VerifyTypeIntegral(Ast::Type const& exprType)
	{
		if (!exprType.IsScalar(Ast::BaseType::Bool, Ast::BaseType::UInt))
		{
			throw ParserException("Bitwise binary expression requires integral types in the sub-expressions");
		}
	}
	CRINL static void VerifyTypesIntegral(Ast::Type const& lhsExprType, Ast::Type const& rhsExprType)
	{
		if (!lhsExprType.IsScalar(Ast::BaseType::Bool, Ast::BaseType::UInt) &&
			!rhsExprType.IsScalar(Ast::BaseType::Bool, Ast::BaseType::UInt))
		{
			throw ParserException("Bitwise binary expression requires integral types in the sub-expressions");
		}
	}

	// *************************************************************** //
	// **                     Statements parsing.                   ** //
	// *************************************************************** //

	// { STATEMENT ::= ;
	//             | COMPOUND-STMT
    //             | IF-ELSE-STMT
	//             | SWITCH-STMT
	//             | WHILE-STMT
	//             | DO-WHILE-STMT
	//             | FOR-STMT
	//             | BREAK-STMT
	//             | CONTINUE-STMT
	//             | RETURN-STMT
	//             | DISCARD-STMT
	//             | DECLARATION-STMT
	//             | EXPRESSION-STMT }
	// *************************************************************** //
	CR_INTERNAL Ast::Statement* Parser::Parse_Statement()
	{
		switch (m_Lexeme.GetType())
		{
			// Null statements.
			// *************************************************************** //
			case Lexeme::Type::OpSemicolon:
				ReadNextLexeme();
				return nullptr;

			// Compound statements.
			// *************************************************************** //
			case Lexeme::Type::OpBraceOpen:
				ReadNextLexeme();
				return Parse_Statement_Compound();

			// Selection statements.
			// *************************************************************** //
			case Lexeme::Type::KwIf:
				ReadNextLexeme();
				return Parse_Statement_Selection_If();
			case Lexeme::Type::KwSwitch:
				ReadNextLexeme();
				return Parse_Statement_Selection_Switch();

			// Iteration statements.
			// *************************************************************** //
			case Lexeme::Type::KwWhile:
				ReadNextLexeme();
				return Parse_Statement_Iteration_While();
			case Lexeme::Type::KwDo:
				ReadNextLexeme();
				return Parse_Statement_Iteration_Do();
			case Lexeme::Type::KwFor:
				ReadNextLexeme();
				return Parse_Statement_Iteration_For();

			// Jump statements.
			// *************************************************************** //
			case Lexeme::Type::KwBreak:
				ReadNextLexeme();
				return Parse_Statement_Jump_Break();
			case Lexeme::Type::KwContinue:
				ReadNextLexeme();
				return Parse_Statement_Jump_Continue();
			case Lexeme::Type::KwReturn:
				ReadNextLexeme();
				return Parse_Statement_Jump_Return();
			case Lexeme::Type::KwDiscard:
				ReadNextLexeme();
				return Parse_Statement_Jump_Discard();

			// Declaration or expression statements.
			// *************************************************************** //
			default:
				return Parse_Statement_Declaration_OR_Expression();
		}
	}

	// { COMPOUND-STMT ::= { [statement] [statement] ... } }
	// *************************************************************** //
	CR_INTERNAL Ast::Statement* Parser::Parse_Statement_Compound()
	{
		if (m_Lexeme != Lexeme::Type::OpBraceClose)
		{
			// Non-empty statement.
			// Step 1. Parse statement.
			auto const compoundStmt = m_Profile->CreateCompoundStatement();
			auto compoundStatementJumpedAway = false;
			while (m_Lexeme != Lexeme::Type::OpBraceClose)
			{
				auto const innerStmt = Parse_Statement();
				if (compoundStatementJumpedAway)
				{
					/// @todo Uncomment warning.
				//	throw ParserException("Unreachable code detected.");
					delete innerStmt;
					continue;
				}

				if (innerStmt != nullptr)
				{
					compoundStmt->m_Stmts.emplace_back(innerStmt);
					if (dynamic_cast<Ast::JumpStatement*>(innerStmt) != nullptr)
					{
						// This is jump statement. That means that any other following statements  
						// of the scope are unreachable.
						compoundStatementJumpedAway = true;
					}
				}
			}
			ReadNextLexeme();

			// Step 2. Try to unroll.
			if (compoundStmt->m_Stmts.empty())
			{
				// Empty compound statement.
				delete compoundStmt;
				return nullptr;
			}
			if (compoundStmt->m_Stmts.size() == 1)
			{
				// Compound statement with single expression.
				auto const evaluatedStmt = compoundStmt->m_Stmts[0].release();
				delete compoundStmt;
				return evaluatedStmt;
			}
			return compoundStmt;
		}
		return nullptr;
	}

	// --------------------------------------------------------------- //
	// --               Selection statement parsing.                -- //
	// --------------------------------------------------------------- //
	
	// { IF-ELSE-STMT ::= if (<expression>) <statement> [else <statement>] }
	// *************************************************************** //
	CR_INTERNAL Ast::Statement* Parser::Parse_Statement_Selection_If()
	{
		// Step 1. Parse statement.
		auto const ifStmt = m_Profile->CreateIfSelectionStatement();

		ReadNextLexeme(Lexeme::Type::OpParenOpen);
		ifStmt->m_CondExpr.reset(Parse_Expression());
		if (!ifStmt->m_CondExpr->GetType().IsScalar(Ast::BaseType::Bool))
		{
			throw ParserException("Condition expression of the 'if' statement must be convertible to scalar boolean "
								  "value.");
		}
		ReadNextLexeme(Lexeme::Type::OpParenClose);
		
		ifStmt->m_ThenStmt.reset(Parse_Statement_Scoped());
		if (m_Lexeme == Lexeme::Type::KwElse)
		{
			// Else statement was specified.
			ReadNextLexeme();
			ifStmt->m_ElseStmt.reset(Parse_Statement_Scoped());
		}
		
		// Step 2. Try to unroll.
		if (ifStmt->m_CondExpr->IsConstexpr())
		{
			// 'if' statement could be evaluated at compile-time.
			auto const condExprValue = ifStmt->m_CondExpr->Evaluate().To<bool>();
			auto const evaluatedStmt = condExprValue ? ifStmt->m_ThenStmt.release() : ifStmt->m_ElseStmt.release();
			if (evaluatedStmt == nullptr)
			{
				/// @todo Uncomment warning.
			//	throw ParserException("'if' statement has a constant expression condition, evaluation resulted "
			//						  "null statement; skipping this 'if' statement.");
			}
			delete ifStmt;
			return evaluatedStmt;
		}
		return ifStmt;
	}

	// { SWITCH-STMT ::= switch (<expression>) { [<SWITCH-CASE>] ... [<SWITCH-COND>] } 
	//   SWITCH-CASE ::= default: [[<statement>] ... [<statement>] break;]
	//                 | case <constexpr>: [[<statement>] ... [<statement>] break;] }
	// *************************************************************** //
	CR_INTERNAL Ast::Statement* Parser::Parse_Statement_Selection_Switch()
	{
		// Step 1. Parse statement.
		auto const switchStmt = m_Profile->CreateSwitchSelectionStatement();
		auto const oldJumpOnBreak = m_JumpOnBreak;
		m_JumpOnBreak = switchStmt;

		ReadNextLexeme(Lexeme::Type::OpParenOpen);
		switchStmt->m_SwitchExpr.reset(Parse_Expression());
		if (!switchStmt->m_SwitchExpr->GetType().IsScalar(Ast::BaseType::Bool, Ast::BaseType::UInt))
		{
			throw ParserException("Selection expression of the 'switch' statement must be convertible to scalar "
								  "integral value.");
		}
		ReadNextLexeme(Lexeme::Type::OpParenClose);

		if (m_Lexeme == Lexeme::Type::OpSemicolon)
		{
			/// @todo Uncomment warning.
			// This is rather strange, but correct syntax pattern: switch(<expression>);
		//	throw ParserException("Suspicious empty 'switch' loop statement.");
			goto Outer;
		}

		ReadNextLexeme(Lexeme::Type::OpBraceOpen);
		while (true)
		{
			switch (m_Lexeme.GetType())
			{
				// *************************************************************** //
				case Lexeme::Type::OpBraceClose:
					ReadNextLexeme();
					goto Outer;

				//! @todo Awful. Rewrite.
				// *************************************************************** //
				case Lexeme::Type::KwCase:
					{
						auto const switchCase = m_Profile->CreateSwitchCase();
						ReadNextLexeme();
						auto const caseExpr = Parse_Expression();
						VerifyTypeIntegral(caseExpr->GetType());
						VerifyConstexpr(caseExpr);
						auto const caseExprValue = caseExpr->Evaluate().To<int64_t>();
						if (switchStmt->m_Cases.count(caseExprValue) != 0)
						{
							throw ParserException("Duplicate 'case' in switch statement.");
						}
						switchStmt->m_Cases[caseExprValue].reset(switchCase);
						ReadNextLexeme(Lexeme::Type::OpColon);
						
						while (true)
						{
							auto const switchCaseStmt = Parse_Statement();
							if (switchCaseStmt != nullptr)
							{
								switchCase->m_Stmts.emplace_back(switchCaseStmt);
								if (dynamic_cast<Ast::JumpStatement*>(switchCaseStmt) != nullptr)
								{
									// This is a jump statement that leaves outside the loop.
									break;
								}
							}
						}
					}
					break;

				//! @todo Awful. Rewrite.
				// *************************************************************** //
				case Lexeme::Type::KwDefault:
					{
						auto const switchCase = m_Profile->CreateSwitchDefaultCase();
						if (switchStmt->m_DfltCase == nullptr)
						{
							throw ParserException("Duplicate 'default' branch of the 'switch' statement.");
						}
						switchStmt->m_DfltCase.reset(switchCase);
						ReadNextLexeme(Lexeme::Type::OpColon);

						while (true)
						{
							auto const switchCaseStmt = Parse_Statement();
							if (switchCaseStmt != nullptr)
							{
								switchCase->m_Stmts.emplace_back(switchCaseStmt);
								if (dynamic_cast<Ast::JumpStatement*>(switchCaseStmt) != nullptr)
								{
									// This is a jump statement that leaves outside the loop.
									break;
								}
							}
						}
					}
					break;

				// *************************************************************** //
				default:
					throw ParserException("Unexpected token inside the body of condition statement.");
			}
		}
	Outer:
		if (switchStmt->m_DfltCase == nullptr && switchStmt->m_Cases.empty())
		{
			/// @todo Uncomment warning.
		//	throw ParserException("'switch' statement has no case label, not 'default'.");
		}

		// Step 2. Try to unroll.
		if (switchStmt->m_SwitchExpr->IsConstexpr())
		{
			// 'switch' statement could be evaluated at compile-time.
			auto const switchExprValue = switchStmt->m_SwitchExpr->Evaluate().To<int64_t>();
			auto const evaluatedCase = switchStmt->m_Cases.count(switchExprValue != 0) 
				? switchStmt->m_Cases[switchExprValue] : switchStmt->m_DfltCase;
			if (evaluatedCase == nullptr || evaluatedCase->m_Stmts.empty())
			{
				/// @todo Uncomment warning.
			//	throw ParserException("'switch' statement has a constant expression condition, evaluation resulted "
			//						  "null statement; skipping this 'switch' statement.");
				delete switchStmt;
				return nullptr;
			}
			delete switchStmt;
			if (evaluatedCase->m_Stmts.size() == 1)
			{
				auto const evaluatedStmt = evaluatedCase->m_Stmts[0].release();
				if (dynamic_cast<Ast::BreakJumpStatement*>(evaluatedStmt) != nullptr)
				{
					// Evaluated case consists only of 'break' statement that leaves the switch.
					// The whole unrolled case if empty.
					delete evaluatedStmt;
					return nullptr;
				}
				// Evaluated case consists only of some jump statement.
				return evaluatedStmt;
			}
			else
			{
				// Copying the whole content to the break operator.
				auto const evaluatedStmt = m_Profile->CreateCompoundStatement(std::move(evaluatedCase->m_Stmts));
				if (dynamic_cast<Ast::BreakJumpStatement*>(evaluatedStmt->m_Stmts.back().get()) != nullptr)
				{
					// Evaluated case ends with 'break' statement that leaves the switch.
					// Removing it.
					evaluatedStmt->m_Stmts.pop_back();
				}
				return evaluatedStmt;
			}
		}

		m_JumpOnBreak = oldJumpOnBreak;
		return switchStmt;
	}

	// --------------------------------------------------------------- //
	// --               Iteration statement parsing.                -- //
	// --------------------------------------------------------------- //

	// { WHILE-STMT ::= while (<expression>) <statement> }
	// *************************************************************** //
	CR_INTERNAL Ast::Statement* Parser::Parse_Statement_Iteration_While()
	{
		// Step 1. Parse statement.
		auto const whileStmt = m_Profile->CreateWhileIterationStatement();
		auto const oldJumpOnBreak = m_JumpOnBreak, oldJumpOnContinue = m_JumpOnContinue;
		m_JumpOnBreak = m_JumpOnContinue = whileStmt;

		ReadNextLexeme(Lexeme::Type::OpParenOpen);
		whileStmt->m_CondExpr.reset(Parse_Expression());
		if (!whileStmt->m_CondExpr->GetType().IsScalar(Ast::BaseType::Bool))
		{
			throw ParserException("Condition expression of the 'while' statement must be convertible to scalar boolean "
								  "value.");
		}
		ReadNextLexeme(Lexeme::Type::OpParenClose);

		whileStmt->m_LoopStmt.reset(Parse_Statement_Scoped());

		// Step 2. Try to unroll.
		if (whileStmt->m_CondExpr->IsConstexpr())
		{
			// 'while' statement could be evaluated at compile-time.
			auto const condExprValue = whileStmt->m_CondExpr->Evaluate().To<bool>();
			if (!condExprValue)
			{
				/// @todo Uncomment warning.
			//	throw ParserException("'while' statement has a constant expression condition, evaluation resulted "
			//						  "null statement, skipping the 'while' statement.");
				delete whileStmt;
				return nullptr;
			}

			// Now we know that this loop has true condition.
			// We need to determine, whether this loop is finite.
			/// @todo Check whether this loop is finite.
		}

		m_JumpOnBreak = oldJumpOnBreak, m_JumpOnContinue = oldJumpOnContinue;
		return whileStmt;
	}

	// { DO-WHILE-STMT ::= do <statement> while (<expression>); }
	// *************************************************************** //
	CR_INTERNAL Ast::Statement* Parser::Parse_Statement_Iteration_Do()
	{
		// Step 1. Parse statement.
		auto const doWhileStmt = m_Profile->CreateDoIterationStatement();
		auto const oldJumpOnBreak = m_JumpOnBreak, oldJumpOnContinue = m_JumpOnContinue;
		m_JumpOnBreak = m_JumpOnContinue = oldJumpOnBreak;

		doWhileStmt->m_LoopStmt.reset(Parse_Statement_Scoped());
		if (doWhileStmt->m_LoopStmt == nullptr)
		{
			/// @todo Uncomment warning.
			// This is rather strange, but correct syntax pattern: do; while(<expression>);
		//	throw ParserException("Suspicious empty 'do-while' loop statement.");
		}

		ReadNextLexeme(Lexeme::Type::KwWhile);
		ReadNextLexeme(Lexeme::Type::OpParenOpen);
		doWhileStmt->m_CondExpr.reset(Parse_Expression());
		if (!doWhileStmt->m_CondExpr->GetType().IsScalar(Ast::BaseType::Bool))
		{
			throw ParserException("Condition expression of the 'do-while' statement must be convertible to scalar "
								  "boolean value.");
		}
		ReadNextLexeme(Lexeme::Type::OpParenClose);
		ReadNextLexeme(Lexeme::Type::OpSemicolon);

		// Step 2. Try to unroll.
		if (doWhileStmt->m_CondExpr->IsConstexpr())
		{
			// 'do-while' statement could be evaluated at compile-time.
			auto const condExprValue = doWhileStmt->m_CondExpr->Evaluate().To<bool>();
			if (!condExprValue)
			{
				/// @todo Uncomment warning.
				//	throw ParserException("'do-while' statement has a constant expression condition, evaluation resulted "
				//						  "null statement, unrolling the 'do-while' statement.");
				auto const evaluatedStmt = doWhileStmt->m_LoopStmt.release();
				delete doWhileStmt;
				return evaluatedStmt;
			}

			// Now we know that this loop has true condition.
			// We need to determine, whether this loop is finite.
			/// @todo Check whether this loop is finite.
		}

		m_JumpOnBreak = oldJumpOnBreak, m_JumpOnContinue = oldJumpOnContinue;
		return doWhileStmt;
	}

	// { FOR-STMT ::= for (; [<expression>]; [<expression>]) <statement>
	//              | for (<statement> [<expression>]; [<expression>]) <statement> }
	// *************************************************************** //
	CR_INTERNAL Ast::Statement* Parser::Parse_Statement_Iteration_For()
	{
		// Step 1. Parse statement.
		auto const forStmt = m_Profile->CreateForIterationStatement();
		auto const oldJumpOnBreak = m_JumpOnBreak, oldJumpOnContinue = m_JumpOnContinue;
		m_JumpOnBreak = m_JumpOnContinue = oldJumpOnBreak;

		ReadNextLexeme(Lexeme::Type::OpParenOpen);
		if (m_Lexeme != Lexeme::Type::OpSemicolon)
		{
			// Initialization section was specified.
			forStmt->m_InitStmt.reset(Parse_Statement());
			if (dynamic_cast<Ast::DeclarationStatement*>(forStmt->m_InitStmt.get()) == nullptr &&
				dynamic_cast<Ast::ExpressionStatement*>(forStmt->m_InitStmt.get()) == nullptr)
			{
				throw ParserException("Declaration or expression statement is expected in the non-empty initialization "
									  "section of the 'for' statement.");
			}
		}
		else
		{
			ReadNextLexeme();
		}

		if (m_Lexeme != Lexeme::Type::OpSemicolon)
		{
			// Condition section was specified.
			forStmt->m_CondExpr.reset(Parse_Expression());
			if (!forStmt->m_CondExpr->GetType().IsScalar(Ast::BaseType::Bool))
			{
				throw ParserException("Condition expression of the condition section of 'for' statement must be "
									  "convertible to scalar boolean value.");
			}
			ReadNextLexeme(Lexeme::Type::OpSemicolon);
		}
		else
		{
			ReadNextLexeme();
		}

		if (m_Lexeme != Lexeme::Type::OpParenClose)
		{
			// Step section was specified.
			forStmt->m_StepExpr.reset(Parse_Expression());
			ReadNextLexeme(Lexeme::Type::OpParenClose);
		}
		else
		{
			ReadNextLexeme();
		}

		forStmt->m_LoopStmt.reset(Parse_Statement_Scoped());
		
		// Step 2. Try to unroll.
		if (forStmt->m_CondExpr != nullptr && forStmt->m_CondExpr->IsConstexpr())
		{
			// 'for' statement contains condition expression section
			// that could be evaluated at compile-time.
			auto const condExprValue = forStmt->m_CondExpr->Evaluate().To<bool>();
			if (!condExprValue)
			{
				/// @todo Uncomment warning.
				//	throw ParserException("'for' statement has a constant expression condition, evaluation resulted "
				//						  "null statement, skipping the loop section of the 'for' statement.");
				auto const evaluatedStmt = forStmt->m_LoopStmt.release();
				delete forStmt;
				if (evaluatedStmt != nullptr)
				{
					// 'for' statement should be skipped: we must find the way to unroll the initialization statement.
					/// @todo Unroll the initialization statement.
					return evaluatedStmt;
				}
				return nullptr;
			}

			// Now we know that this loop has true condition.
			// We need to determine, whether this loop is finite.
			/// @todo Check whether this loop is finite.
		}
		m_JumpOnBreak = oldJumpOnBreak, m_JumpOnContinue = oldJumpOnContinue;
		return forStmt;
	}
	
	// --------------------------------------------------------------- //
	// --                  Jump statement parsing.                  -- //
	// --------------------------------------------------------------- //

	// { BREAK-STMT ::= break; }
	// *************************************************************** //
	CR_INTERNAL Ast::Statement* Parser::Parse_Statement_Jump_Break()
	{
		auto const breakStmt = m_Profile->CreateBreakJumpStatement();
		if (m_JumpOnBreak == nullptr)
		{
			throw ParserException("Using 'break' statement is only allowed inside of the iteration statements or "
					              "a 'switch'.");
		}
		breakStmt->m_BreakTo = m_JumpOnBreak;
		ReadNextLexeme(Lexeme::Type::OpSemicolon);
		return breakStmt;
	}

	// { CONTINUE-STMT ::= continue; }
	// *************************************************************** //
	CR_INTERNAL Ast::Statement* Parser::Parse_Statement_Jump_Continue()
	{
		auto const continueStmt = m_Profile->CreateContinueJumpStatement();
		if (m_JumpOnContinue == nullptr)
		{
			throw ParserException("Using 'continue' statement is allowed only inside of the iteration statements.");
		}
		continueStmt->m_ContinueWith = m_JumpOnContinue;
		ReadNextLexeme(Lexeme::Type::OpSemicolon);
		return continueStmt;
	}

	// { RETURN-STMT ::= return [<expression>]; }
	// *************************************************************** //
	CR_INTERNAL Ast::Statement* Parser::Parse_Statement_Jump_Return()
	{
		auto const returnStmt = m_Profile->CreateReturnJumpStatement();
		returnStmt->m_ReturnTo = m_Function;

		auto const funcRetType = returnStmt->m_ReturnTo->GetReturnType();
		if (m_Lexeme != Lexeme::Type::OpSemicolon)
		{
			if (funcRetType == Ast::BaseType::Void)
			{
				throw ParserException("Function of 'void' return type cannot return a value.");
			}
			returnStmt->m_Expr.reset(Parse_Expression());
			if (returnStmt->m_Expr->GetType().IsIncompatibleWith(funcRetType))
			{
				throw ParserException("Type of the 'return' statement is inconvertible to the function return type.");
			}
			ReadNextLexeme(Lexeme::Type::OpSemicolon);
		}
		else
		{
			if (funcRetType != Ast::BaseType::Void)
			{
				throw ParserException("Function of non-'void' must return a value.");
			}
			ReadNextLexeme();
		}

		return returnStmt;
	}

	// { DISCARD-STMT ::= discard; }
	// *************************************************************** //
	CR_INTERNAL Ast::Statement* Parser::Parse_Statement_Jump_Discard()
	{
		auto const discardStmt = m_Profile->CreateDiscardJumpStatement();
		ReadNextLexeme(Lexeme::Type::OpSemicolon);
		return discardStmt;
	}
	
	// --------------------------------------------------------------- //
	// --        Declaration & Expression statement parsing.        -- //
	// --------------------------------------------------------------- //

	/**
	 * Parses DECLARATION or EXPRESSION statements.
	 * { DECLARATION-STMT ::= boolMxN | .. | doubleMxN <identifier> [ ( [<DECLARATION>, ...] ) ] }
	 */
	// *************************************************************** //
	CR_INTERNAL Ast::Statement* Parser::Parse_Statement_Declaration_OR_Expression()
	{
		switch (m_Lexeme.GetType())
		{
			//! @todo Add DECLARATION or EXPRESSION statements parsing.
			case CrCaseType(Lexeme::Type::KwBool):
			case CrCaseType(Lexeme::Type::KwInt):
			case CrCaseType(Lexeme::Type::KwUInt):
			case CrCaseType(Lexeme::Type::KwDword):
			case CrCaseType(Lexeme::Type::KwHalf):
			case CrCaseType(Lexeme::Type::KwFloat):
			case CrCaseType(Lexeme::Type::KwDouble):
				return Parse_Statement_Declaration_Variable_OR_Function();

			// Or the simplest case: expression statement.
			default:
				return Parse_Statement_Expression();
		}
	}
	
	/**
	 * Parses variable or function DECLARATION statements.
	 * { DECLARATION-STMT ::= TypeMxN <identifier> ; }
	 */
	// *************************************************************** //
	CR_INTERNAL Ast::Statement* Parser::Parse_Statement_Declaration_Variable_OR_Function()
	{
		auto const declStmt = new Ast::DeclarationStatement();
		
		auto const varDecl = new Ast::Variable();
		declStmt->m_Vars.emplace_back(varDecl);

		switch (m_Lexeme.GetType())
		{
			//! @todo Add correct variables parsing.
			case CrCaseType(Lexeme::Type::KwBool):
				varDecl->m_Type = Ast::Type(Ast::BaseType::Bool);
				break;
			case CrCaseType(Lexeme::Type::KwInt):
				varDecl->m_Type = Ast::Type(Ast::BaseType::Int);
				break;
			case CrCaseType(Lexeme::Type::KwUInt):
				varDecl->m_Type = Ast::Type(Ast::BaseType::UInt);
			case CrCaseType(Lexeme::Type::KwDword):
				varDecl->m_Type = Ast::Type(Ast::BaseType::Dword);
				break;
			case CrCaseType(Lexeme::Type::KwHalf):
				varDecl->m_Type = Ast::Type(Ast::BaseType::Half);
				break;
		//	case CrCaseType(Lexeme::Type::KwFloat):
		//		varDecl->m_Type = Ast::Type(Ast::BaseType::Float);
		//		break;
			case Lexeme::Type::KwFloat:
				varDecl->m_Type = Ast::Type(Ast::BaseType::Float);
				break;
			case Lexeme::Type::KwFloat4:
				varDecl->m_Type = Ast::Type(Ast::BaseType::Float, 4, 1);
				break;
			case CrCaseType(Lexeme::Type::KwDouble):
				varDecl->m_Type = Ast::Type(Ast::BaseType::Double);
				break;

			default:
				CrAssert(0);
		}
		ReadNextLexeme();

		ExpectLexeme(Lexeme::Type::IdIdentifier);
		varDecl->m_Name = m_Lexeme.GetValueID();
		ReadNextLexeme();
		ReadNextLexeme(Lexeme::Type::OpSemicolon);

		DeclareVariable(varDecl);
		return declStmt;
	}

	// *************************************************************** //
	CR_INTERNAL Ast::Statement* Parser::Parse_Statement_Declaration_Typedef()
	{
		CrAssert(0);
		return nullptr;
	}
	// *************************************************************** //
	CR_INTERNAL Ast::Statement* Parser::Parse_Statement_Declaration_Struct()
	{
		CrAssert(0);
		return nullptr;
	}

	// *************************************************************** //
	// **                    Expressions parsing.                   ** //
	// *************************************************************** //

	// { EXPRESSION-STMT ::= <expression>; }
	// *************************************************************** //
	CR_INTERNAL Ast::Statement* Parser::Parse_Statement_Expression()
	{
		auto const exprStmt = m_Profile->CreateExpressionStatement();
		exprStmt->m_Expr.reset(Parse_Expression());
		ReadNextLexeme(Lexeme::Type::OpSemicolon);
		return exprStmt;
	}

	// { expression ::= COMMA-SEPARATED-EXPR }
	// *************************************************************** //
	CR_INTERNAL Ast::Expression* Parser::Parse_Expression()
	{
		return Parse_Expression_Comma();
	}

	// { COMMA-SEPARATED-EXPR ::= ASSIGNMENT-EXPR [, ASSIGNMENT-EXPR] }
	// *************************************************************** //
	CR_INTERNAL Ast::Expression* Parser::Parse_Expression_Comma()
	{
		auto expr = Parse_Expression_Assignments();
		while (m_Lexeme == Lexeme::Type::OpComma)
		{
			auto const commaExpr = m_Profile->CreateCommaExpression();

			ReadNextLexeme();
			commaExpr->m_Lhs.reset(expr);
			commaExpr->m_Rhs.reset(Parse_Expression_Assignments());
			commaExpr->m_Type = commaExpr->m_Rhs->GetType();
			expr = commaExpr;
		}
		return expr;
	}

	// { ASSIGNMENT-EXPR ::= TERNARY-EXPR [@= TERNARY-EXPR] }
	// *************************************************************** //
	CR_INTERNAL Ast::Expression* Parser::Parse_Expression_Assignments()
	{
		auto expr = Parse_Expression_Ternary();
		while (true)
		{
			switch (m_Lexeme.GetType())
			{
				// *************************************************************** //
				case Lexeme::Type::OpAssignment:
					{
						ReadNextLexeme();

						auto const assignExpr = m_Profile->CreateAssignmentBinaryExpression(expr, Parse_Expression_Ternary());
						VerifyLValue(assignExpr->m_Lhs.get());

						auto const lhsExprType = assignExpr->m_Lhs->GetType();
						auto const rhsExprType = assignExpr->m_Rhs->GetType();
						VerifyTypesMatch(lhsExprType, rhsExprType);

						assignExpr->m_Type = lhsExprType;
						assignExpr->m_IsLValue = true;
						expr = assignExpr;
					}
					break;

				// *************************************************************** //
				case Lexeme::Type::OpBitwiseAndAssign: case Lexeme::Type::OpBitwiseOrAssign: case Lexeme::Type::OpBitwiseXorAssign:
				case Lexeme::Type::OpBitwiseLeftShiftAssign: case Lexeme::Type::OpBitwiseRightShiftAssign:
					{
						auto const op = m_Lexeme.GetType();
						ReadNextLexeme();

						auto const bitwiseAssignExpr = m_Profile->CreateBitwiseAssignmentBinaryExpression(op, expr, Parse_Expression_Ternary());
						VerifyLValue(bitwiseAssignExpr->m_Lhs.get());

						auto const lhsExprType = bitwiseAssignExpr->m_Lhs->GetType();
						auto const rhsExprType = bitwiseAssignExpr->m_Rhs->GetType();
						VerifyTypesMatch(lhsExprType, rhsExprType);
						VerifyTypesIntegral(lhsExprType, rhsExprType);

						bitwiseAssignExpr->m_Type = lhsExprType;
						bitwiseAssignExpr->m_IsLValue = true;
						expr = bitwiseAssignExpr;
					}
					break;

				// *************************************************************** //
				case Lexeme::Type::OpAddAssign: case Lexeme::Type::OpSubtractAssign:
				case Lexeme::Type::OpMultiplyAssign: case Lexeme::Type::OpDivideAssign: case Lexeme::Type::OpModuloAssign:
					{
						auto const op = m_Lexeme.GetType();
						ReadNextLexeme();

						auto const arithmAssignExpr = m_Profile->CreateArithmeticAssignmentBinaryExpression(op, expr, Parse_Expression_Ternary());
						VerifyLValue(arithmAssignExpr->m_Lhs.get());

						auto const lhsExprType = arithmAssignExpr->m_Lhs->GetType();
						auto const rhsExprType = arithmAssignExpr->m_Rhs->GetType();
						VerifyTypesMatch(lhsExprType, rhsExprType);
						if (op == Lexeme::Type::OpModuloAssign)
						{
							// '%' operator requires integral operands.
							VerifyTypesIntegral(lhsExprType, rhsExprType);
						}

						arithmAssignExpr->m_Type = lhsExprType;
						arithmAssignExpr->m_IsLValue = true;
						expr = arithmAssignExpr;
					}
					break;

				// *************************************************************** //
				default:
					goto Outer;
			}
		}
	Outer:
		return expr;
	}

	// --------------------------------------------------------------- //
	// --                Ternary expression parsing.                -- //
	// --------------------------------------------------------------- //

	// { TERNARY-EXPR ::= <BINARY-EXPR> [ ? <expression> : <expression> ] }
	// *************************************************************** //
	CR_INTERNAL Ast::Expression* Parser::Parse_Expression_Ternary()
	{
		auto expr = Parse_Expression_Or();
		while (m_Lexeme == Lexeme::Type::OpTernary)
		{
			auto const ternaryExpr = m_Profile->CreateTernaryExpression(nullptr, nullptr, nullptr);
			
			ternaryExpr->m_CondExpr.reset(expr);
			if (!ternaryExpr->m_CondExpr->GetType().IsScalar(Ast::BaseType::Bool))
			{
				throw ParserException("Expression in the condition section of the ternary operator must be convertible "
									  "to scalar boolean value.");
			}

			ReadNextLexeme(Lexeme::Type::OpTernary);
			ternaryExpr->m_ThenExpr.reset(Parse_Expression());
			ReadNextLexeme(Lexeme::Type::OpColon);
			ternaryExpr->m_ElseExpr.reset(Parse_Expression());
			
			auto const thenExprType = ternaryExpr->m_ThenExpr->GetType();
			auto const elseExprType = ternaryExpr->m_ElseExpr->GetType();
			VerifyTypesMatch(thenExprType, elseExprType);
			
			// Ternary operator may be evaluated at compile time.
			// Also, ternary operator cannot be l-value due to it is not l-value in HLSL.
			// Possibly, we can substitute it with 'if-else' operator in this cases.
			ternaryExpr->m_IsConstexpr = ternaryExpr->m_ThenExpr->IsConstexpr() && ternaryExpr->m_ElseExpr->IsConstexpr();
			ternaryExpr->m_Type = std::max(thenExprType, elseExprType);

			expr = ternaryExpr;
		}
		return expr;
	}

	// --------------------------------------------------------------- //
	// --                 Binary expression parsing.                -- //
	// --------------------------------------------------------------- //

	// Helpers for semantic analysis of binary expressions.
	// *************************************************************** //
	CR_HELPER Ast::Expression* Parser::ParseHelper_Expression_LogicBinary(Lexeme::Type const op
		, Ast::Expression* const lhs, Ast::Expression* const rhs) const
	{
		auto const logicBinExpr = m_Profile->CreateLogicBinaryExpression(op, lhs, rhs);

		auto const lhsExprType = logicBinExpr->m_Lhs->GetType();
		auto const rhsExprType = logicBinExpr->m_Rhs->GetType();
		VerifyTypesMatch(lhsExprType, rhsExprType);

		logicBinExpr->m_IsConstexpr = logicBinExpr->m_Lhs->IsConstexpr() && logicBinExpr->m_Rhs->IsConstexpr();
		logicBinExpr->m_Type = Ast::Type(Ast::BaseType::Bool, lhsExprType);

		return logicBinExpr;
	}
	// *************************************************************** //
	CR_HELPER Ast::Expression* Parser::ParseHelper_Expression_BitwiseBinary(Lexeme::Type const op
		, Ast::Expression* const lhs, Ast::Expression* const rhs) const
	{
		auto const bitwiseBinExpr = m_Profile->CreateBitwiseBinaryExpression(op, lhs, rhs);

		auto const lhsExprType = bitwiseBinExpr->m_Lhs->GetType();
		auto const rhsExprType = bitwiseBinExpr->m_Rhs->GetType();
		VerifyTypesMatch(lhsExprType, rhsExprType);
		VerifyTypesIntegral(lhsExprType, rhsExprType);

		bitwiseBinExpr->m_IsConstexpr = bitwiseBinExpr->m_Lhs->IsConstexpr() && bitwiseBinExpr->m_Rhs->IsConstexpr();
		bitwiseBinExpr->m_Type = std::max(lhsExprType, rhsExprType);

		return bitwiseBinExpr;
	}
	// *************************************************************** //
	CR_HELPER Ast::Expression* Parser::ParseHelper_Expression_ArithmeticBinary(Lexeme::Type const op
		, Ast::Expression* const lhs, Ast::Expression* const rhs) const
	{
		auto const arithmBinExpr = m_Profile->CreateArithmeticBinaryExpression(op, lhs, rhs);

		auto const lhsExprType = arithmBinExpr->m_Lhs->GetType();
		auto const rhsExprType = arithmBinExpr->m_Rhs->GetType();
		VerifyTypesMatch(lhsExprType, rhsExprType);
		if (op == Lexeme::Type::OpModulo)
		{
			// '%' operator requires integral operands.
			VerifyTypesIntegral(lhsExprType, rhsExprType);
		}

		arithmBinExpr->m_IsConstexpr = arithmBinExpr->m_Lhs->IsConstexpr() && arithmBinExpr->m_Rhs->IsConstexpr();
		arithmBinExpr->m_Type = std::max(lhsExprType, rhsExprType);

		return arithmBinExpr;
	}

	// { BINARY-EXPR ::= UNARY-EXPR [@ UNARY-EXPR] }
	// *************************************************************** //
	CR_INTERNAL Ast::Expression* Parser::Parse_Expression_Or()
	{
		auto expr = Parse_Expression_And();
		while (m_Lexeme == Lexeme::Type::OpOr)
		{
			ReadNextLexeme();
			expr = ParseHelper_Expression_LogicBinary(Lexeme::Type::OpOr, expr, Parse_Expression_And());
		}
		return expr;
	}
	// *************************************************************** //
	CR_INTERNAL Ast::Expression* Parser::Parse_Expression_And()
	{
		auto expr = Parse_Expression_BitwiseOr();
		while (m_Lexeme == Lexeme::Type::OpAnd)
		{
			ReadNextLexeme();
			expr = ParseHelper_Expression_LogicBinary(Lexeme::Type::OpAnd, expr, Parse_Expression_BitwiseOr());
		}
		return expr;
	}
	// *************************************************************** //
	CR_INTERNAL Ast::Expression* Parser::Parse_Expression_BitwiseOr()
	{
		auto expr = Parse_Expression_BitwiseXor();
		while (m_Lexeme == Lexeme::Type::OpBitwiseOr)
		{
			ReadNextLexeme();
			expr = ParseHelper_Expression_BitwiseBinary(Lexeme::Type::OpBitwiseOr, expr, Parse_Expression_BitwiseXor());
		}
		return expr;
	}
	// *************************************************************** //
	CR_INTERNAL Ast::Expression* Parser::Parse_Expression_BitwiseXor()
	{
		auto expr = Parse_Expression_BitwiseAnd();
		while (m_Lexeme == Lexeme::Type::OpBitwiseXor)
		{
			ReadNextLexeme();
			expr = ParseHelper_Expression_BitwiseBinary(Lexeme::Type::OpBitwiseXor, expr, Parse_Expression_BitwiseAnd());
		}
		return expr;
	}
	// *************************************************************** //
	CR_INTERNAL Ast::Expression* Parser::Parse_Expression_BitwiseAnd()
	{
		auto expr = Parse_Expression_Equals_OR_NotEquals();
		while (m_Lexeme == Lexeme::Type::OpBitwiseAnd)
		{
			ReadNextLexeme();
			expr = ParseHelper_Expression_BitwiseBinary(Lexeme::Type::OpBitwiseAnd, expr, Parse_Expression_Equals_OR_NotEquals());
		}
		return expr;
	}
	// *************************************************************** //
	CR_INTERNAL Ast::Expression* Parser::Parse_Expression_Equals_OR_NotEquals()
	{
		auto expr = Parse_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals();
		while (m_Lexeme == Lexeme::Type::OpEquals || m_Lexeme == Lexeme::Type::OpNotEquals)
		{
			auto const op = m_Lexeme.GetType();
			ReadNextLexeme();
			expr = ParseHelper_Expression_LogicBinary(op, expr, Parse_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals());
		}
		return expr;
	}
	// *************************************************************** //
	CR_INTERNAL Ast::Expression* Parser::Parse_Expression_Less_OR_LessEquals_OR_Greater_OR_GreaterEquals()
	{
		auto expr = Parse_Expression_BitwiseLeftShift_OR_BitwiseRightShift();
		while (m_Lexeme == Lexeme::Type::OpLess || m_Lexeme == Lexeme::Type::OpLessEquals
			|| m_Lexeme == Lexeme::Type::OpGreater || m_Lexeme == Lexeme::Type::OpGreaterEquals)
		{
			auto const op = m_Lexeme.GetType();
			ReadNextLexeme();
			expr = ParseHelper_Expression_LogicBinary(op, expr, Parse_Expression_BitwiseLeftShift_OR_BitwiseRightShift());
		}
		return expr;
	}
	// *************************************************************** //
	CR_INTERNAL Ast::Expression* Parser::Parse_Expression_BitwiseLeftShift_OR_BitwiseRightShift()
	{
		auto expr = Parse_Expression_Add_OR_Subtract();
		while (m_Lexeme == Lexeme::Type::OpBitwiseLeftShift || m_Lexeme == Lexeme::Type::OpBitwiseRightShift)
		{
			auto const op = m_Lexeme.GetType();
			ReadNextLexeme();
			expr = ParseHelper_Expression_BitwiseBinary(op, expr, Parse_Expression_Add_OR_Subtract());
		}
		return expr;
	}
	// *************************************************************** //
	CR_INTERNAL Ast::Expression* Parser::Parse_Expression_Add_OR_Subtract()
	{
		auto expr = Parse_Expression_Multiply_OR_Divide_OR_Modulo();
		while (m_Lexeme == Lexeme::Type::OpAdd || m_Lexeme == Lexeme::Type::OpSubtract)
		{
			auto const op = m_Lexeme.GetType();
			ReadNextLexeme();
			expr = ParseHelper_Expression_ArithmeticBinary(op, expr, Parse_Expression_Multiply_OR_Divide_OR_Modulo());
		}
		return expr;
	}
	// *************************************************************** //
	CR_INTERNAL Ast::Expression* Parser::Parse_Expression_Multiply_OR_Divide_OR_Modulo()
	{
		auto expr = Parse_Expression_Unary();
		while (m_Lexeme == Lexeme::Type::OpMultiply || m_Lexeme == Lexeme::Type::OpDivide || m_Lexeme == Lexeme::Type::OpModulo)
		{
			auto const op = m_Lexeme.GetType();
			ReadNextLexeme();
			expr = ParseHelper_Expression_ArithmeticBinary(op, expr, Parse_Expression_Unary());
		}
		return expr;
	}

	// --------------------------------------------------------------- //
	// --                  Unary expression parsing.                -- //
	// --------------------------------------------------------------- //

	// { UNARY-EXPR ::=  [@ UNARY-EXPR] }
	/// @todo Split everything into several functions.
	// *************************************************************** //
	CR_INTERNAL Ast::Expression* Parser::Parse_Expression_PrefixUnary()
	{
		//! @todo Implement 'sizeof' operation.
		//! @todo Implement 'typeMxN()' constructor.
		switch (m_Lexeme.GetType())
		{
			// Arithmetic unary operations.
			// *************************************************************** //
			case Lexeme::Type::OpAdd:
				{
					// Unary '+' operator does nothing. Just skipping it.
					ReadNextLexeme();
					auto const expr = Parse_Expression_PrefixUnary();
					return expr;
				}
			case Lexeme::Type::OpSubtract:
				{
					ReadNextLexeme();
					auto const negExpr = m_Profile->CreateNegateExpression(Parse_Expression_PrefixUnary());
					negExpr->m_Type = negExpr->m_Expr->GetType();
					negExpr->m_IsConstexpr = negExpr->m_Expr->IsConstexpr();
					return negExpr;
				}

			// Logic and bitwise unary operations.
			// *************************************************************** //
			case Lexeme::Type::OpNot:
				{
					ReadNextLexeme();
					auto const notExpr = m_Profile->CreateNotExpression(Parse_Expression_PrefixUnary());
					notExpr->m_Type = Ast::Type(Ast::BaseType::Bool, notExpr->m_Expr->GetType());
					notExpr->m_IsConstexpr = notExpr->m_Expr->IsConstexpr();
					return notExpr;
				}
			case Lexeme::Type::OpBitwiseNot:
				{
					ReadNextLexeme();
					auto const bitwiseNotExpr = m_Profile->CreateBitwiseNotExpression(Parse_Expression_PrefixUnary());
					bitwiseNotExpr->m_Type = bitwiseNotExpr->m_Expr->GetType();
					bitwiseNotExpr->m_IsConstexpr = bitwiseNotExpr->m_Expr->IsConstexpr();
					VerifyTypeIntegral(bitwiseNotExpr->m_Expr->GetType());
					return bitwiseNotExpr;
				}

			// Increment and decrement.
			// *************************************************************** //
			case Lexeme::Type::OpInc:
			case Lexeme::Type::OpDec:
				/// @todo Implement (dec)incrementing.
				CrAssert(0);
				ReadNextLexeme();
				return nullptr;

			// Parentheses or cast operations.
			// *************************************************************** //
			case Lexeme::Type::OpParenOpen:
			{
				ReadNextLexeme();
				//switch (m_Lexeme.GetType())
				//{
				//	//! @todo Add cast operation support.
				//	case CrCaseType(Lexeme::Type::KwBool):
				//	case CrCaseType(Lexeme::Type::KwInt):
				//	case CrCaseType(Lexeme::Type::KwUInt):
				//	case CrCaseType(Lexeme::Type::KwDword):
				//	case CrCaseType(Lexeme::Type::KwHalf):
				//	case CrCaseType(Lexeme::Type::KwFloat):
				//	case CrCaseType(Lexeme::Type::KwDouble):
				//		CrAssert(0);
				//		break;
				//}

				auto const parenExpr = Parse_Expression();
				ReadNextLexeme(Lexeme::Type::OpParenClose);
				return parenExpr;
			}

			case Lexeme::Type::IdIdentifier:
				{
					auto const ident = FindIdentifier(m_Lexeme.GetValueID());
					if (ident == nullptr)
					{
						throw ParserException("Undeclared identifier.");
					}
					auto const var = dynamic_cast<Ast::Variable*>(ident);
					if (var == nullptr)
					{
						throw ParserException("Variable expected.");
					}
					auto const identExpr = new Ast::IdentifierExpression(var);
					identExpr->m_Type = var->m_Type;
					identExpr->m_IsLValue = true;
					ReadNextLexeme();
					return identExpr;
				}
				break;

			// Compile-time constants.
			// *************************************************************** //
			case Lexeme::Type::KwTrue:
				ReadNextLexeme();
				return m_Profile->CreateConstExpression(true);
			case Lexeme::Type::KwFalse:
				ReadNextLexeme();
				return m_Profile->CreateConstExpression(false);
			case Lexeme::Type::CtInt:
				{
					auto const constExpr = m_Profile->CreateConstExpression();
					constExpr->m_Type = Ast::Type(Ast::BaseType::Int);
					constExpr->m_Value = m_Lexeme.GetValueInt();
					constExpr->m_IsConstexpr = true;
					ReadNextLexeme();
					return constExpr;
				}
				//! @todo Add other constant types.
			/*case Lexeme::Type::CtUInt:
				ReadNextLexeme();
				return m_Profile->CreateConstExpression(m_Lexeme.GetValueUInt());
			case Lexeme::Type::CtHalf:
				ReadNextLexeme();
				return m_Profile->CreateConstExpression(m_Lexeme.GetValueHalf());
			case Lexeme::Type::CtFloat:
				ReadNextLexeme();
				return m_Profile->CreateConstExpression(m_Lexeme.GetValueFloat());*/
			case Lexeme::Type::CtDouble:
				{
					auto const constExpr = m_Profile->CreateConstExpression();
					constExpr->m_Type = Ast::Type(Ast::BaseType::Double);
					constExpr->m_Value = m_Lexeme.GetValueDouble();
					constExpr->m_IsConstexpr = true;
					ReadNextLexeme();
					return constExpr;
				}

			default:
				throw ParserException("Unexpected lexeme in the stream.");
		}
	}
	/// @todo Split everything into several functions.
	// *************************************************************** //
	CR_INTERNAL Ast::Expression* Parser::Parse_Expression_Unary()
	{
		auto expr = Parse_Expression_PrefixUnary();
		while (true)
		{
			switch (m_Lexeme.GetType())
			{
				// Struct member or swizzling operator
				// *************************************************************** //
				case Lexeme::Type::OpDot:
					{
						ReadNextLexeme();

						auto const exprType = expr->GetType();
						if (exprType == Ast::BaseType::Struct)
						{
							// Struct member.
							CrAssert(0);
						}

						// Swizzling operator.
						//! @todo Implement swizzling.
						if (m_Lexeme == Lexeme::Type::IdIdentifier)
						{
							CrAssert(0);
						}
						else
						{
							throw ParserException("swizzle mask expected.");
						}
					}

				default:
					goto Outer;
			}
		}
	Outer:
		return expr;
	}

	// *************************************************************** //
	// *************************************************************** //

#if 0
	CR_API Ast::Statement* Parser::Parse_Statement_Definition()
	{
		CrLog(0, __FUNCSIG__);
		if (m_Lexeme.GetType() != Lexeme::Type::IdIdentifier)
		{
			throw ParserException("Identifier expected.");
		}

		auto const definitionName = m_Lexeme.GetValueID();
		CrLog(1, "declared: %s", definitionName.c_str());

		ReadNextLexeme();
		switch (m_Lexeme.GetType())
		{
			// Ending this definition.
			// *************************************************************** //
			case Lexeme::Type::OpSemicolon:
				return;

			// Reading another definition.
			// *************************************************************** //
			case Lexeme::Type::OpComma:
				ReadNextLexeme();
				Parse_Statement_Definition();
				return;

			// Reading initial value of this definition.
			// *************************************************************** //
			case Lexeme::Type::OpAssignment:
				ReadNextLexeme();
				{
					auto expression = Parse_Expression();
					expression->EvaluateType();
					return;
				}

			default:
				throw ParserException("';', ',' or '= [expression]' expected.");
		}
	}
#endif

	/**
	* Parses 'simplified' version of complete program.
	*/
	//! @todo Remove this trash.
	// *************************************************************** //
	CR_API void Parser::ParseProgram()
	{
		m_ScopedIdents.emplace_back();
		m_Profile = new Profile();
		CrLog(0, __FUNCSIG__);

		ReadNextLexeme(Lexeme::Type::KwProgram);
		ReadNextLexeme(Lexeme::Type::OpBraceOpen);

		while (true)
		{
			if (m_Lexeme == Lexeme::Type::Null)
			{
				throw ParserException("'}' expected.");
			}
			if (m_Lexeme == Lexeme::Type::OpBraceClose)
			{
				break;
			}

			auto s = Parse_Statement();
			s = nullptr;
		}
	}

	CrUnitTest(ParserEmptyStream)
	{
		Parser parser(new Scanner(std::make_shared<IO::StringInputStream>(R"(
program 
{
{ float4 a; if(a) {}}
}
)")));
		parser.ParseProgram();
		int i = 0;
	};

}	// namespace Cr
