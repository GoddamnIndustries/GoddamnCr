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

#define CrTypeMaskCreate(m, n) ((m - 1 & 0xFF) << 24) | ((m - 1 & 0xFF) << 16)

/**
 * Substitutes all combination of scalar, vector and matrix types.
 */
#define CrTypeMxN(Type) Type, \
	Type##1, Type##1x1, Type##1x2, Type##1x3, Type##1x4, \
	Type##2, Type##2x1, Type##2x2, Type##2x3, Type##2x4, \
	Type##3, Type##3x1, Type##3x2, Type##3x3, Type##3x4, \
	Type##4, Type##4x1, Type##4x2, Type##4x3, Type##4x4
/**
 * Substitutes all combination of scalar, vector and matrix types for 'switch' statement.
 */
#define CrCaseTypeMxN(Type) Type: \
	case Type##1: case Type##1x1: case Type##1x2: case Type##1x3: case Type##1x4: \
	case Type##2: case Type##2x1: case Type##2x2: case Type##2x3: case Type##2x4: \
	case Type##3: case Type##3x1: case Type##3x2: case Type##3x3: case Type##3x4: \
	case Type##4: case Type##4x1: case Type##4x2: case Type##4x3: case Type##4x4

/**
 * Substitutes all combination of sampler and texture types.
 */
#define CrTypeND(Type) \
	Type##1D, Type##2D, Type##3D, Type##CUBE, \
/**
 * Substitutes all combination of sampler and texture types for 'switch' statement.
 */
#define CrCaseTypeND(Type) \
	Type##1D: case Type##2D: case Type##3D: case Type##CUBE \

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
			Null = 0, NewLine,
			IdIdentifier,
			CtInt, CtUInt, CtDword = CtUInt, CtFloat, CtDouble,

			// Keywords.
			/*	<-- HLSL keywords that should be added.
			column_major
			compile
			const
			discard
			emit
			extern
			fixed
			get
			half
			in
			inline
			inout
			out
			packed
			pass*
			pixelfragment*
			pixelshader*
			row_major
			sampler
			sampler_state
			shared
			sizeof
			static
			static_cast
			struct
			technique*
			template
			texture*
			uniform
			vertexfragment*
			vertexshader*
			volatile
			*/
		/**/	KwProgram,
			
			KwIf, KwElse, KwSwitch, KwCase, KwDefault,
			KwWhile, KwDo, KwFor,
			KwBreak, KwContinue, KwReturn, KwDiscard,
			KwTypedef, KwStruct, 
			KwSampler1D, KwSampler2D, KwSampler3D, KwSamplerCUBE, KwTexture1D, KwTexture2D, KwTexture3D, KwTextureCUBE, KwTextureRECT,
			KwVoid, CrTypeMxN(KwBool), CrTypeMxN(KwInt), CrTypeMxN(KwUInt), CrTypeMxN(KwDword), CrTypeMxN(KwFloat), CrTypeMxN(KwDouble),
			KwTrue, KwFalse,

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
			OpBraceOpen, OpBraceClose, OpBracketOpen, OpBracketClose, OpParenOpen, OpParenClose,
			OpSemicolon, OpColon, OpComma, OpDot, OpTernary,
			OpPreprocessor, OpPreprocessorConcat,

		}; // enum class Type

	public:
		static std::map<std::string, Type> const s_KeywordsTable;

	private:
		Type        m_Type = Type::Null;
		uint32_t     m_ValueInt = 0;
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
			assert(type <= Type::NewLine || type >= Type::KwProgram);
		}

		/**
		 * Initializes a constant lexeme.
		 * @param value The value of lexem.
		 */
		/// @{
		explicit Lexeme(Type const type, uint32_t const value)
			: m_Type(type), m_ValueInt(value)
		{}
		explicit Lexeme(Type const type, double const value)
			: m_Type(type), m_ValueReal(value)
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
		double GetValueReal() const
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
