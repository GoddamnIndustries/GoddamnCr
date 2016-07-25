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

#include <memory>
#include <istream>

#define CR_API /*__declspec(dllexport)*/

namespace Cr
{
	/// <summary>
	/// A base node for lexeme lists.
	/// </summary>
	class CR_API ASyntaxNode : public std::enable_shared_from_this<ASyntaxNode>
	{
		friend class CASyntaxLexer;
		std::shared_ptr<ASyntaxNode> _prevNode, _nextNode;

	public:
		template<typename TTo>
		std::shared_ptr<TTo> To()
		{
			return dynamic_cast<TTo*>(this)->shared_from_this();
		}
		template<typename TTo>
		std::shared_ptr<TTo const> To() const
		{
			return dynamic_cast<TTo*>(this)->shared_from_this();
		}

	};	// class ASyntaxNode
	
	typedef std::shared_ptr<ASyntaxNode> SyntaxNode;

	// *************************************************************** //
	// **                      Syntax trivia.                       ** //
	// *************************************************************** //

	class CR_API ASyntaxTrivia : public ASyntaxNode
	{
	public:
		enum class Type
		{
			Undefined,
			Whitespace,
			Newline,
			SingleLineComment,
			MultilineComment,
		};	// enum class Type

	private:
		Type mType;

	public:
		bool Is(Type const type) const
		{
			return mType == type;
		}


	};	// class ASyntaxTrivia

	typedef std::shared_ptr<ASyntaxTrivia> SyntaxTrivia;

	class CR_API ASyntaxWhitespaceTrivia final : public ASyntaxTrivia
	{
	};	// class ASyntaxWhitespaceTrivia

	class CR_API ASyntaxNewlineTrivia final : public ASyntaxTrivia
	{
	};	// class ASyntaxNewlineTrivia

	typedef std::shared_ptr<ASyntaxNewlineTrivia> PSyntaxNewlineTrivia;

	class CR_API ASyntaxCommentTrivia : public ASyntaxTrivia
	{
	};	// class ASyntaxCommentTrivia

	class CR_API ASyntaxSingleLineCommentTrivia final : public ASyntaxCommentTrivia
	{
	};	// class ASyntaxSingleLineCommentTrivia

	class CR_API ASyntaxMultiLineCommentTrivia final : public ASyntaxCommentTrivia
	{
	};	// class ASyntaxMultiLineCommentTrivia

	// *************************************************************** //
	// **                      Syntax lexeme.                       ** //
	// *************************************************************** //

	enum class CR_API SyntaxKind
	{
		None,
		Identifier,

		__ConstantBegin__,
		ConstantInt,
		ConstantUInt,
		ConstantLong,
		ConstantULong,
		ConstantDouble,
		ConstantFloat,
		ConstantHalf,
		ConstantChar,
		ConstantString,
		__ConstantEnd__,

		__KeywordBegin__,
#define KEYWORD(keyword, ...) Kw_##keyword,
#define PPKEYWORD(keyword, ...) KwPp_##keyword,
#include "LexerKeywords.hxx"
		__KeywordEnd__,

		__PunctuationBegin__,
		OpHash, OpHashHash,
		OpDot, OpDotDotDot,
		OpColon, OpColonColon,
		OpComma, OpSemicolon,
		OpParenOpen, OpParenClose,
		OpBraceOpen, OpBraceClose,
		OpBracketOpen, OpBracketClose,
		OpQuestion,
		OpEquals, OpEqualsEquals,
		OpPlus, OpPlusPlus, OpPlusEquals,
		OpMinus, OpMinusMinus, OpMinusEquals, OpMinusGreater,
		OpAsterisk, OpAsteriskEquals,
		OpSlash, OpSlashEquals,
		OpPercent, OpPercentEquals,
		OpExclamation, OpExclamationEquals,
		OpGreater, OpGreaterGreater, OpGreaterGreaterEquals, OpGreaterEquals,
		OpLess, OpLessLess, OpLessLessEquals, OpLessEquals,
		OpTilde,
		OpAmpersand, OpAmpersandAmpersand, OpAmpersandEquals,
		OpBar, OpBarBar, OpBarEquals,
		OpCaret, OpCaretEquals,
		__PunctuationEnd__,

	};	// enum class Type

	class CR_API ASyntaxLexeme : public ASyntaxNode
	{
	public:
		

	};	// class ASyntaxLexeme

	typedef std::shared_ptr<ASyntaxLexeme> SyntaxLexeme;

	// *************************************************************** //
	// **                      Syntax lexer.                        ** //
	// *************************************************************** //

	class CR_API ASyntaxLexer : public std::enable_shared_from_this<ASyntaxLexer>
	{
	public:
		virtual ~ASyntaxLexer() = default;
		virtual SyntaxLexeme Lex() = 0;

	};	// class ASyntaxLexer

}	// namespace Cr
