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

#include <string>
#include <cassert>

namespace Cr
{
	typedef int Char;
	typedef char const* CStr;
	typedef std::string String;
	typedef std::string StringBuilder;

	struct StringSlice
	{
		static StringSlice Empty;

		CStr Start, End;
	};	// struct StringSlice

}	// namespace Cr
