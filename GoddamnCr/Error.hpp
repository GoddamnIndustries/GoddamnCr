#pragma once

namespace Cr
{
	enum class Error
	{
		ErrorInvalidSymbol,
		ErrorInvalidReal,
		ErrorInvalidNumber,
		ErrorInvalidHexNumber,
		ErrorInvalidOctalNumber,
		WarningLowercaseLSuffix,
		ErrorFloatOverflow,
		ErrorIntOverflow,
		ErrorTooManyCharsInConst,
		ErrorEmptyCharConst,
		ErrorNewlineInConst,
		ErrorIllegalEscape,
		ErrorOpenEndedComment,
	};	// enum class Error

	static void AddError(Error const error)
	{
		throw error;
	}

}	// namespace Cr
