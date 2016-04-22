// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#pragma once
#include <cassert>
#include <memory>

namespace Cr
{
	// Tiny exception hierarchy.
	struct Exception : public std::exception
	{
		explicit Exception(char const* const message)
			: std::exception(message) {}
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
				try
				{
					testFunctor();
				}
				catch(...)
				{
					assert(0 && "Unhandled exception was caught in tests.");
				}
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
