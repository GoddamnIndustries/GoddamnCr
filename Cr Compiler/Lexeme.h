// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#pragma once
#include <map>
#include <cassert>
#include <memory>

namespace Cr
{
	class Identifier {};
	typedef Identifier* PIdentifier;
	typedef std::map<std::string, std::unique_ptr<Identifier>> IdentifierTable;

	/**
	 * Represents a single lexem.
	 */
	class Lexeme final
	{
	public:
		enum class Type
		{
			Null = 0, 
			IdIdentifier,
			CtInteger, CtString, CtReal,

			// Keywords.
			KwProgram,
			KwGoto,
			KwIf, KwElse,
			KwFor, KwWhile, KwBreak, KwContinue,
			KwRead, KwWrite,
			KwInt, KwString, KwReal,

			// Preprocessor keywords.
			KwPpDefine, KwPpUndef, KwPpDefined,
			KwPpIf = KwIf, KwPpIfdef = KwPpDefined + 1, KwPpIfndef, KwPpElse = KwElse, KwPpElif = KwPpIfndef + 1, KwPpEndif,
			KwPpPragma, KwPpLine, KwPpError,

			// Operators
			OpAssignment, OpAdd, OpSubtract, OpMultiply, OpDivide, OpModulo, OpInc, OpDec,
			OpEquals, OpNotEquals, OpGreater, OpLess, OpGreaterEquals, OpLessEquals,
			OpNot, OpAnd, OpOr,
			OpBitwiseNot, OpBitwiseAnd, OpBitwiseOr, OpBitwiseXor, OpBitwiseLeftShift, OpBitwiseRightShift,
			OpAddAssign, OpSubtractAssign, OpMultiplyAssign, OpDivideAssign, OpModuloAssign, OpBitwiseAndAssign, OpBitwiseOrAssign, OpBitwiseXorAssign, OpBitwiseLeftShiftAssign, OpBitwiseRightShiftAssign,
			OpScopeBegin, OpScopeEnd, OpSubindexBegin, OpSubindexEnd, OpPenthesesBegin, OpPenthesesEnd,
			OpSemicolon, OpColon, OpComma, OpDot, OpTernary,
			OpPreprocessor, OpPreprocessorGlue,

		}; // enum class Type

	public:
		static std::map<std::string, Type> const s_KeywordsTable;

	private:
		Type        m_Type = Type::Null;
		int32_t     m_ValueInt = 0;
		std::string m_ValueString;
		double      m_ValueReal = 0.0;
		PIdentifier m_ValueID = nullptr;

	public:

		/**
		 * Initializes a keyword or operator lexeme.
		 * @param type The type of lexem.
		 */
		explicit Lexeme(Type const type)
			: m_Type(type)
		{
			assert(type == Type::Null || type >= Type::KwProgram);
		}

		/**
		 * Initializes a constant lexeme.
		 * @param value The value of lexem.
		 */
		/// @{
		explicit Lexeme(int32_t const value)
			: m_Type(Type::CtInteger), m_ValueInt(value)
		{}
		explicit Lexeme(std::string const& value)
			: m_Type(Type::CtString), m_ValueString(value)
		{}
		explicit Lexeme(double const value)
			: m_Type(Type::CtReal), m_ValueReal(value)
		{}
		explicit Lexeme(PIdentifier const& value)
			: m_Type(Type::IdIdentifier), m_ValueID(value)
		{}
		/// @}

		/**
		 * Returns type of this lexeme.
		 */
		Type GetType() const
		{
			return m_Type;
		}

		/**
		 * Returns value of this lexeme.
		 */
		/// @{
		int32_t GetValueInt() const
		{
			assert(m_Type == Type::CtInteger);
			return m_ValueInt;
		}
		std::string const& GetValueString() const
		{
			assert(m_Type == Type::CtString);
			return m_ValueString;
		}
		double GetValueReal() const
		{
			assert(m_Type == Type::CtReal);
			return m_ValueReal;
		}
		PIdentifier GetValueID() const
		{
			assert(m_Type == Type::IdIdentifier);
			return m_ValueID;
		}
		/// @}

		void __DebugPrint() const;

	};	// class Lexeme final

}	// namespace Cr
