#pragma once
//------------------------------------------------------------------------------
/**
	@class Debug::StackTrace

	Helper class for generating stack traces

	(C) 2015 Individual contributors, see AUTHORS file
*/
#if __WIN32__
#include "debug/win32/win32stacktrace.h"
namespace Debug
{
	class StackTrace : public Win32::Win32StackTrace

	{ };
}
#endif
//------------------------------------------------------------------------------
