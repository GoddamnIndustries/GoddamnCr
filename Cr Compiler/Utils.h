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
#include <cassert>
#include <memory>
#include <vector>

#define CR_API
#define CRINT
#if _MSC_VER
#	define CRINL __forceinline
#else
#	define __FUNCSIG__ __FUNCTION__
#	define CRINL
#endif
#define CrAssert assert
#define CrLog(l, a, ...) if(l > 0) { fprintf(stderr, a, ##__VA_ARGS__); }
#pragma warning(disable : 4127)
#pragma warning(disable : 4290)

namespace Cr
{
	template<typename Tp>
	struct AutoReset
	{
		Tp& m_What, m_Default;
	public:
		AutoReset(AutoReset const&) = delete;
		AutoReset& operator= (AutoReset const&) = delete;

		AutoReset(Tp& what, Tp const& to)
			: m_What(what), m_Default(what)
		{
			m_What = to;
		}
		~AutoReset()
		{
			m_What = m_Default;
		}
	};	// struct AutoReset
	template<typename Tp>
	struct AutoResize
	{
		std::vector<Tp>& m_What;
		size_t m_DefaultSize;
	public:
		AutoResize(AutoResize const&) = delete;
		AutoResize& operator= (AutoResize const&) = delete;

		explicit AutoResize(std::vector<Tp>& what)
			: m_What(what), m_DefaultSize(what.size())
		{
		}
		~AutoResize()
		{
			m_What.resize(m_DefaultSize);
		}
	};	// struct AutoReset

	// Tiny exception hierarchy.
	struct Exception : public std::exception
	{
		explicit Exception(char const* const /*message*/)
			: std::exception(/*message*/) {}
	};	// class Exception

#define CrDefineExceptionBase(ClassName, ClassBaseName) \
	struct ClassName : public ClassBaseName \
	{ \
		explicit ClassName(char const* const message) \
			: ClassBaseName(message) {} \
	}
#define CrDefineException(ClassName) CrDefineExceptionBase(ClassName, Exception)

	CrDefineException(WorkflowException);

	// Tiny unit-testing framework.
	namespace Testing
	{
		typedef void(*TestFunctor)();
		class Test final
		{
		public:
			Test(TestFunctor const testFunctor)
			{
				/*try
				{
					testFunctor();
				}
				catch(...)
				{
					assert(0 && "Unhandled exception was caught in tests.");
				}*/
				testFunctor();
			}
		};	// class Test
#define CrUnitTest(TestName) static const ::Cr::Testing::Test __ ## TestName = (::Cr::Testing::TestFunctor)[]()
	}	// namespace Testing

	// Tiny IO framework.
	namespace IO
	{
		class Exception : public Cr::Exception
		{
		};	// class Exception

		class EndOfStreamException : Exception
		{
		};	// class EndOfStreamException

		class InputStream
		{
		public:
			InputStream() {}
			InputStream(InputStream const&) = delete;
			virtual ~InputStream() {}

		public:
			virtual int ReadNextChar() = 0;
		};	// class InputStream

		typedef std::shared_ptr<InputStream> PInputStream;

		class StringInputStream final : public InputStream
		{
		private:
			char const* m_String;

		public:
			explicit StringInputStream(char const* string) : m_String(string) { assert(string != nullptr); }
			virtual int ReadNextChar() override final
			{
				auto const result = *m_String;
				if (result == '\0')
					return EOF;
				++m_String;
				return result;
			}
		};	// class StringInputStream

	}	// namespace IO

}	// namespace Cr
