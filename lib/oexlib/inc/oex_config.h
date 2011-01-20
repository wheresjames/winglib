/*------------------------------------------------------------------
// oex_config.h
//
// Copyright (c) 1997
// Robert Umbehant
// winglib@wheresjames.com
// http://www.wheresjames.com
//
// Redistribution and use in source and binary forms, with or
// without modification, are permitted for commercial and
// non-commercial purposes, provided that the following
// conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * The names of the developers or contributors may not be used to
//   endorse or promote products derived from this software without
//   specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/

#pragma once

// 3rd party stuff
#define OEX_ENABLE_SQLITE

// Compile messages
#define OEX_VERBOSE_COMPILE

// The namespace to use
#define OEX_NAMESPACE		oex

// Do we want class testing enabled?
#define OEX_ENABLE_TESTS
//#undef OEX_ENABLE_TESTS

// Enables buffer over-run/under-run checking in release mode
//#define OEX_ENABLE_RELEASE_MODE_MEM_CHECK
#undef OEX_ENABLE_RELEASE_MODE_MEM_CHECK

// Attempt to detect 64 bit processor
#if defined( _WIN64 ) || defined( _M_X64 ) || defined( __amd64__ ) || defined( _LP64 )
#	define OEX_CPU_64
#endif

#if defined( OEX_CPU_64 )
#	define OEX_NOCASTPTR
#endif

#if defined( __GNUC__ )
#	define OEX_GCC
#endif

#if defined( _MSC_VER )
#	define OEX_MSC
#endif

// Are we using windows?
#if defined( _WIN64_WCE )
#	define OEX_WINCE
#	define OEX_WIN64
#	define OEX_WINDOWS
#elif defined( _WIN32_WCE )
#	define OEX_WINCE
#	define OEX_WIN32
#	define OEX_WINDOWS
#elif defined( WIN64 ) || defined( _WIN64 )
#	define OEX_WIN64
#	define OEX_WINDOWS
#elif defined( WIN32 ) || defined( _WIN32 )
#	define OEX_WIN32
#	define OEX_WINDOWS
#else
#	define OEX_LINUX
#endif


#if defined( CPU_ARM )
#	define OEX_CPU_ARM
#endif

#define OEX_LITTLE_ENDIAN
//#define OEX_BIG_ENDIAN

// Debug build?
#if defined ( _DEBUG )
#	define oexDEBUG
#endif

// Unicode build?
#if defined( UNICODE ) || defined( OEX_WINCE )
#   define oexUNICODE
#endif

// Align memory on 16 byte boundry
// OEX_ALIGNEDMEM must be set to a power of 2
#if defined( OEX_WINDOWS )
//#	define OEX_ALIGNEDMEM 16
#endif

// By default, the thread locks time out and usually the function
// will give up and treat it as an error, sixty seconds should be
// plenty, but you may have some reason, like a reeeeaaaallllyyy
// slow system and you need longer waits.  I really don't recommend
// you set this lower.  If you're timing out that indicates a serious
// error (deadlocks, race conditions and such) and you should fix it.
#define oexDEFAULT_WAIT_TIMEOUT         60000
//#define oexDEFAULT_WAIT_TIMEOUT       3000

#if defined( OEX_WINDOWS )

// Just informative under Windows.
// This is windows default resolution
#	define oexWAIT_RESOLUTION			15000

#else

// Wait resolution in uSeconds
// iii Make it the same as Windows
#	define oexWAIT_RESOLUTION			15000

#endif

// Enable CRT leak detection
#define OEX_CRT_LEAK_DETECTION

// Enable zip file support
#define OEX_ENABLE_ZIP

// Enable image support, requires CxImage
#if !defined( OEX_NOXIMAGE )
#	define OEX_ENABLE_XIMAGE
#endif

#if !defined( OEX_NOVIDEO )
#	define OEX_ENABLE_VIDEO
#endif

#if defined( OEX_WIN32 )
#	define oexCDECL	_cdecl
#else
#	define oexCDECL
#endif

// Not big endian
#define OEX_BIGENDIAN		0

// Inline template specialization
#define oexITS 				inline

//#define oexUSE_EXCEPTIONS
#undef oexUSE_EXCEPTIONS

// Define to include backtraces for log entries
#if defined( oexDEBUG )
#	define oexBACKTRACE_IN_LOG
#endif

// Define to have log entries printed to the command line
#if defined( oexDEBUG )
#	define oexPRINT_LOGS 2
#endif

//#define OEXLIB_EXTRA_POINTER_CHECK

// Define to generate stack traces
#define OEXLIB_STACK_TRACING
//#define OEXLIB_STACK_HISTORY
//#define OEXLIB_STACK_KEEP_INACTIVE_TRACES

#if defined ( OEX_WINDOWS )

// !!! Native TLS support is broken in the Visual Studio Compiler when using DLL's
//     So you probably want to use the TLS API with VS

//#	define OEXLIB_USING_TLS
#	define OEXLIB_USING_TLSAPI

#	define oexTLS _declspec( thread )

#else

#	define OEXLIB_USING_TLS

#	define oexTLS __thread

#endif

// Define to include full file paths in log entries
//#define oexFULL_FILENAME_IN_LOG

// Define to have full path in markers
//#define oexDEBUG_FULLFILENAMES

// Maximum number of data dump lines to show in CAlloc::GetBlockReport()
#define OEX_MAX_LINES_IN_MEMDUMP	8

// debug macros
#define oexFILE				__FILE__
#define oexLINE				__LINE__

#if defined( OEX_LINUX )
#	define oexFUNCTION			__FUNCTION__
#	define oexPRETTYFUNCTION	__PRETTY_FUNCTION__
#	define oexSTTFUNCTION		__PRETTY_FUNCTION__
#	define oexSTTSTR			oexCSTR8
#else
#	define oexFUNCTION			__FUNCTION__
#	define oexPRETTYFUNCTION	__FUNCTION__ oexT( "()" )
#	define oexSTTFUNCTION		__FUNCTION__
#	define oexSTTSTR			oexCSTR
#endif

#define oexDATE					__DATE__
#define oexDATE_FMT				"%b* %D %Y"

#define oexTIME					__TIME__
#define oexTIME_FMT				"%g:%m:%s"


#define oexGetBuildTime()		OEX_NAMESPACE::CSysTime( oexTEXT( oexDATE_FMT )	oexT( " " )	oexTEXT( oexTIME_FMT ), \
														 oexTEXT( oexDATE )		oexT( " " ) oexTEXT( oexTIME ) )

#if defined( OEX_PROJECT_NAME )
#define oexAppNamePtr()			oexTEXT( OEX_PROJECT_NAME )
#else
#define oexAppNamePtr()			oexT( "" )
#endif

#if defined( OEX_PROJECT_DESC )
#define oexAppDescPtr()			oexTEXT( OEX_PROJECT_DESC )
#else
#define oexAppDescPtr()			oexAppNamePtr()
#endif

#if defined( OEX_PROJECT_LNAME )
#define oexAppLongNamePtr()		oexTEXT( OEX_PROJECT_LNAME )
#else
#define oexAppLongNamePtr()		oexAppDescPtr()
#endif

#define oexAppName()			OEX_NAMESPACE::CStr( oexAppNamePtr() )
#define oexAppDesc()			OEX_NAMESPACE::CStr( oexAppDescPtr() )
#define oexAppLongName()		OEX_NAMESPACE::CStr( oexAppLongNamePtr() )

#define oexBuild()				oexGetBuildTime().FormatTime( oexT( "%y.%C.%D.%G%m" ) )
#define oexBuildPtr()			oexGetBuildTime().FormatTime( oexT( "%y.%C.%D.%G%m" ) ).Ptr()

#if defined( OEX_PROJECT_VERSION )
#	define oexVersion()			OEX_NAMESPACE::CStr( oexTEXT( OEX_PROJECT_VERSION ) )
#	define oexVersionPtr()		oexTEXT( OEX_PROJECT_VERSION )
#else
#	define oexVersion()			oexBuild()
#	define oexVersionPtr()		oexBuildPtr()
#endif

#if defined( OEX_WINDOWS )
#	undef OEX_CASE_SENSITIVE_FILES
#else
#	define OEX_CASE_SENSITIVE_FILES
#endif

// Native processor integer size in bytes
#define oexSIZEOFINT 4

#if defined( OEX_WIN64 )
	typedef size_t					oexSIZE_T;
#elif defined( OEX_CPU_64 ) || defined( OEX_IPHONE )
	typedef long unsigned int		oexSIZE_T;
#else
	typedef unsigned int			oexSIZE_T;
#endif

#if defined( _MSC_VER )
	typedef __int64					oexFILESIZE_T;
#else
	typedef long long				oexFILESIZE_T;
#endif

#define oexMAXSIZE					( (oexSIZE_T)-1 )
#define oexMAXFILESIZE				( (oexFILESIZE_T)-1 )

#if defined( OEX_WINDOWS )
#	define oexTmpPath				"C:/Temp"
#else		
#	define oexTmpPath				"/tmp"	
#endif



