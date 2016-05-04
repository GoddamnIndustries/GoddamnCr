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
#include <string>
#include <map>
#include <cassert>

/**
 * Substitutes all combination of scalar, vector and matrix types.
 */
#define CrType(Type) Type, \
	Type##1, Type##1x1, Type##1x2, Type##1x3, Type##1x4, \
	Type##2, Type##2x1, Type##2x2, Type##2x3, Type##2x4, \
	Type##3, Type##3x1, Type##3x2, Type##3x3, Type##3x4, \
	Type##4, Type##4x1, Type##4x2, Type##4x3, Type##4x4

/**
 * Substitutes all combination of scalar, vector and matrix types for 'switch' statement.
 */
#define CrCaseType(Type) Type: \
	case Type##1: case Type##1x1: case Type##1x2: case Type##1x3: case Type##1x4: \
	case Type##2: case Type##2x1: case Type##2x2: case Type##2x3: case Type##2x4: \
	case Type##3: case Type##3x1: case Type##3x2: case Type##3x3: case Type##3x4: \
	case Type##4: case Type##4x1: case Type##4x2: case Type##4x3: case Type##4x4

namespace Cr
{
	struct Identifier { std::string m_Value; Identifier(std::string const& s): m_Value(s) {} };
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
			CtInt, CtUInt, CtDword = CtUInt, CtHalf, CtFloat, CtDouble,

			// Keywords.
			/*
			bool
			break
			char
			column_major
			compile
			const
			continue
			discard
			do
			double
			dword*
			else
			emit
			extern
			false
			fixed
			float*
			for
			get
			half
			if
			in
			inline
			inout
			int
			long
			matrix*
			out
			packed
			pass*
			pixelfragment*
			pixelshader*
			return
			row_major
			sampler
			sampler_state
			sampler1D
			sampler2D
			sampler3D
			samplerCUBE
			shared
			short
			sizeof
			static
			static_cast
			struct
			technique*
			template
			texture*
			texture1D
			texture2D
			texture3D
			textureCUBE
			textureRECT
			true
			typedef
			uniform
			unsigned
			vector*
			vertexfragment*
			vertexshader*
			void
			volatile
			while
			*/
			KwProgram,
			KwDiscard,
			KwIf, KwElse, KwSwitch,
			KwDo, KwFor, KwWhile, KwBreak, KwContinue, KwReturn,
			KwRead, KwWrite,
			KwTypedef, KwStruct,
			KwVoid, CrType(KwBool), CrType(KwInt), CrType(KwUInt), CrType(KwDword), CrType(KwHalf), CrType(KwFloat), CrType(KwDouble),
			KwTrue, KwFalse,
			KwReal,

			// Preprocessor keywords.
			KwPpDefine, KwPpUndef, KwPpDefined,
			KwPpIf = static_cast<int>(KwIf), KwPpIfdef = static_cast<int>(KwPpDefined) + 1, KwPpIfndef, 
			KwPpElse = static_cast<int>(KwElse), KwPpElif = static_cast<int>(KwPpIfndef) + 1, KwPpEndif,
			KwPpPragma, KwPpLine, KwPpError,

			// Operators
			OpAssignment, OpAdd, OpSubtract, OpMultiply, OpDivide, OpModulo, OpInc, OpDec,
			OpEquals, OpNotEquals, OpGreater, OpLess, OpGreaterEquals, OpLessEquals,
			OpNot, OpAnd, OpOr,
			OpBitwiseNot, OpBitwiseAnd, OpBitwiseOr, OpBitwiseXor, OpBitwiseLeftShift, OpBitwiseRightShift,
			OpAddAssign, OpSubtractAssign, OpMultiplyAssign, OpDivideAssign, OpModuloAssign, OpBitwiseAndAssign, OpBitwiseOrAssign, OpBitwiseXorAssign, OpBitwiseLeftShiftAssign, OpBitwiseRightShiftAssign,
			OpScopeBegin, OpScopeEnd, OpSubindexBegin, OpSubindexEnd, OpParenthesesBegin, OpParenthesesEnd,
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
		explicit Lexeme(Type const type = Type::Null)
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
			: m_Type(Type::CtInt), m_ValueInt(value)
		{}
		explicit Lexeme(double const value)
			: m_Type(Type::CtDouble), m_ValueReal(value)
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

		bool operator== (Type const type) const
		{
			return m_Type == type;
		}
		bool operator!= (Type const type) const
		{
			return m_Type != type;
		}

		/**
		 * Returns value of this lexeme.
		 */
		/// @{
		int32_t GetValueInt() const
		{
			assert(m_Type == Type::CtInt);
			return m_ValueInt;
		}
		double GetValueDouble() const
		{
			assert(m_Type == Type::CtDouble);
			return m_ValueReal;
		}
		std::string const& GetValueID() const
		{
			assert(m_Type == Type::IdIdentifier);
			return m_ValueID->m_Value;
		}
		/// @}

	};	// class Lexeme final

}	// namespace Cr
