/*------------------------------------------------------------------
// debug.cpp
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

#include "../../../oexlib.h"
#include "std_os.h"

#ifndef OEX_NOCRTDEBUG
#	include <crtdbg.h>
#endif

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

// http://www.codeguru.com/Cpp/W-P/dll/tips/article.php/c3635/
// http://www.dotnet247.com/247reference/msgs/13/65259.aspx

oexCPVOID CDebug::GetInstanceHandle()
{// _STT();
#if defined( __ImageBase )

	return (oexCPVOID)&__ImageBase;

#else

	MEMORY_BASIC_INFORMATION mbi;
	static int dummy;
	VirtualQuery( &dummy, &mbi, sizeof( mbi ) );

	return (oexCPVOID)mbi.AllocationBase;

#endif
}

void CDebug::Trace( oexCSTR x_pStr )
{// _STT();
/*
#if defined( oexUNICODE )
#   define TRACE0( s )  _CrtDbgReportW( _CRT_WARN, 0, 0, NULL, x_pStr )
#else
#   define TRACE0( s )  _CrtDbgReport( _CRT_WARN, 0, 0, NULL, x_pStr )
#endif
*/
    OutputDebugString( x_pStr );

}

void CDebug::FmtTrace( oexCSTR x_pFmt, ... )
{// _STT();
	oexVaList ap; oexVaStart( ap, x_pFmt );
	vFmtTrace( x_pFmt, ap );
	oexVaEnd( ap );
}

void CDebug::vFmtTrace( oexCSTR x_pFmt, oexVaList &x_pArgs )
{// _STT();
//    oexTCHAR tcMsg[ oexSTRSIZE ] = oexT( "" );
//    os::CSys::vStrFmt( tcMsg, oexSTRSIZE, x_pFmt, x_pArgs );
//    Trace( tcMsg );

    Trace( CStr().vFmt( x_pFmt, x_pArgs ).Ptr() );
}

void CDebug::Break()
{// _STT();
#if defined( oexDEBUG ) && !defined( OEX_NOCRTDEBUG )

	_CrtDbgBreak();

#else

	DebugBreak();

#endif
}

void CDebug::Break( oexINT x_nType, oexCSTR x_pFile, oexUINT x_uLine, oexCSTR8 x_pFunction, oexCSTR x_pStr, oexINT x_nRes, oexINT x_nErr )
{// _STT();
	oexTCHAR tcModule[ oexSTRSIZE ] = oexT( "" );
	GetModuleFileName( (HMODULE)GetInstanceHandle(), tcModule, sizeof( tcModule ) );
	Break( x_nType, x_pFile, x_uLine, x_pFunction, tcModule, x_pStr );
}

void CDebug::Break( oexINT x_nType, oexCSTR x_pFile, oexUINT x_uLine, oexCSTR8 x_pFunction, oexCSTR x_pModule, oexCSTR x_pStr, oexINT x_nRes, oexINT x_nErr )
{// _STT();

	CStr str;

	str << oexT( "Module : " ) << x_pModule << oexNL;
	str << oexT( "File : " ) << x_pFile << oexNL;
	str << oexT( "Line : " ) << x_uLine << oexNL << oexNL;
	str << oexT( "Function : " ) << oexMbToStr( x_pFunction ) << oexNL << oexNL;
	str << oexT( "Expression : " ) << CStr( x_pStr ).Replace( oexT( "%" ), oexT( "%%" ) );
	if ( x_nRes )
		str << oexT( "Result     : " ) << x_nRes << oexT( " sys:\"" ) << os::CTrace::GetErrorMsg( x_nRes ).Replace( oexT( "%" ), oexT( "%%" ) ).Ptr() << oexT( "\"" ) << oexNL;
	if ( x_nErr )
		str << oexT( "Error Code : " ) << x_nErr << oexT( " sys:\"" ) << os::CTrace::GetErrorMsg( x_nErr ).Replace( oexT( "%" ), oexT( "%%" ) ).Ptr() << oexT( "\"" ) << oexNL;

	oex::os::CDebug::CreateCrashReport( oexNULL, oexT( "logs" ), str.Ptr() );

#if defined( oexDEBUG ) && !defined( OEX_NOCRTDEBUG )

#if defined( oexUNICODE )

	if ( 1 == _CrtDbgReport( _CRT_ASSERT, oexStrToMb( x_pFile ).Ptr(), x_uLine,
                             oexStrToMb( x_pModule ).Replace( '%', '.' ).Ptr(),
                             oexStrToMb( str ).Ptr() ) )

#else

    if ( 1 == _CrtDbgReport( _CRT_ASSERT, x_pFile, x_uLine, x_pModule,
                             ( oexStrToMb( str ).Replace( '%', '.' ).Ptr() ), x_pStr ) )

#endif

#else

	// Simulate the _CrtDbgReport box
	int nRet = MessageBox( NULL, str.Ptr(), oexT( "Program Self Verification Error" ), MB_ICONSTOP | MB_ABORTRETRYIGNORE );

	// Does the user want to quit?
	if ( IDABORT == nRet )
		os::CSys::Exit( 0 );

	// Does the user want to debug?
	else if ( IDRETRY == nRet )

#endif
		Break();
}

oexCSTR CDebug::GetExceptionCodeName( oexUINT x_uCode )
{
	switch( x_uCode )
	{
		case EXCEPTION_ACCESS_VIOLATION:
			return oexT( "EXCEPTION_ACCESS_VIOLATION" );
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			return oexT( "EXCEPTION_ARRAY_BOUNDS_EXCEEDED" );
		case EXCEPTION_BREAKPOINT:
			return oexT( "EXCEPTION_BREAKPOINT" );
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			return oexT( "EXCEPTION_DATATYPE_MISALIGNMENT" );
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			return oexT( "EXCEPTION_FLT_DENORMAL_OPERAND" );
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			return oexT( "EXCEPTION_FLT_DIVIDE_BY_ZERO" );
		case EXCEPTION_FLT_INEXACT_RESULT:
			return oexT( "EXCEPTION_FLT_INEXACT_RESULT" );
		case EXCEPTION_FLT_INVALID_OPERATION:
			return oexT( "EXCEPTION_FLT_INVALID_OPERATION" );
		case EXCEPTION_FLT_OVERFLOW:
			return oexT( "EXCEPTION_FLT_OVERFLOW" );
		case EXCEPTION_FLT_STACK_CHECK:
			return oexT( "EXCEPTION_FLT_STACK_CHECK" );
		case EXCEPTION_FLT_UNDERFLOW:
			return oexT( "EXCEPTION_FLT_UNDERFLOW" );
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			return oexT( "EXCEPTION_ILLEGAL_INSTRUCTION" );
		case EXCEPTION_IN_PAGE_ERROR:
			return oexT( "EXCEPTION_IN_PAGE_ERROR" );
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			return oexT( "EXCEPTION_INT_DIVIDE_BY_ZERO" );
		case EXCEPTION_INT_OVERFLOW:
			return oexT( "EXCEPTION_INT_OVERFLOW" );
		case EXCEPTION_INVALID_DISPOSITION:
			return oexT( "EXCEPTION_INVALID_DISPOSITION" );
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			return oexT( "EXCEPTION_NONCONTINUABLE_EXCEPTION" );
		case EXCEPTION_PRIV_INSTRUCTION:
			return oexT( "EXCEPTION_PRIV_INSTRUCTION" );
		case EXCEPTION_SINGLE_STEP:
			return oexT( "EXCEPTION_SINGLE_STEP" );
		case EXCEPTION_STACK_OVERFLOW:
			return oexT( "EXCEPTION_STACK_OVERFLOW" );
		default:
			break;

	} // end switch

	return oexT( "EXCEPTION_UNKNOWN" );
}

oexCSTR CDebug::GetExceptionCodeDesc( oexUINT x_uCode )
{
	switch( x_uCode )
	{
		case EXCEPTION_ACCESS_VIOLATION:
			return oexT( "The thread tried to read from or write to a virtual address for which it does not have the appropriate access." );
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			return oexT( "The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking." );
		case EXCEPTION_BREAKPOINT:
			return oexT( "A breakpoint was encountered." );
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			return oexT( "The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on." );
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			return oexT( "One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value." );
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			return oexT( "The thread tried to divide a floating-point value by a floating-point divisor of zero." );
		case EXCEPTION_FLT_INEXACT_RESULT:
			return oexT( "The result of a floating-point operation cannot be represented exactly as a decimal fraction." );
		case EXCEPTION_FLT_INVALID_OPERATION:
			return oexT( "This exception represents any floating-point exception not included in this list." );
		case EXCEPTION_FLT_OVERFLOW:
			return oexT( "The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type." );
		case EXCEPTION_FLT_STACK_CHECK:
			return oexT( "The stack overflowed or underflowed as the result of a floating-point operation." );
		case EXCEPTION_FLT_UNDERFLOW:
			return oexT( "The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type." );
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			return oexT( "The thread tried to execute an invalid instruction." );
		case EXCEPTION_IN_PAGE_ERROR:
			return oexT( "The thread tried to access a page that was not present, and the system was unable to load the page.\nFor example, this exception might occur if a network connection is lost while running a program over the network." );
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			return oexT( "The thread tried to divide an integer value by an integer divisor of zero." );
		case EXCEPTION_INT_OVERFLOW:
			return oexT( "The result of an integer operation caused a carry out of the most significant bit of the result." );
		case EXCEPTION_INVALID_DISPOSITION:
			return oexT( "An exception handler returned an invalid disposition to the exception dispatcher.\nProgrammers using a high-level language such as C should never encounter this exception." );
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			return oexT( "The thread tried to continue execution after a noncontinuable exception occurred." );
		case EXCEPTION_PRIV_INSTRUCTION:
			return oexT( "The thread tried to execute an instruction whose operation is not allowed in the current machine mode." );
		case EXCEPTION_SINGLE_STEP:
			return oexT( "A trace trap or other single-instruction mechanism signaled that one instruction has been executed." );
		case EXCEPTION_STACK_OVERFLOW:
			return oexT( "The thread used up its stack." );
		default:
			break;

	} // end switch

	return oexT( "Unknown exception." );

}

static CStr CreateStackReport( oexUINT uCurrentThreadId, CStackTrace *pSt, oexCSTR pName, oexCPVOID pAddress )
{
	if ( !pSt )
		return CStr();

	CStr sSt;
	oexUINT i = 0;
	CSysTime st; st.GetSystemTime();

	// Add module header
	sSt << oexT( oexNL8 oexNL8 )
		<<         oexT( "===================================================" oexNL8 )
		<<         oexT( "= Module  : " ) << ( pName ? pName : oexT( "N/A" ) ) << oexNL
		<< oexFmt( oexT( "= Address : 0x%08x" oexNL8 ), (oexUINT)pAddress )
		<<         oexT( "===================================================" oexNL8 );


	// Add each thread stack
	while ( CStackTrace::CStack *p = pSt->Next( &i ) )
	{
		try
		{
			// Set the unix time stamp
			st.SetUnixTime( p->GetCreatedTime() );

			// Show the thread id
			sSt << oexNL
				<< oexT( "---------------------------------------------------" oexNL8 )
				<< ( p->GetThreadId() == uCurrentThreadId
				   ? oexT( "***************************************************" oexNL8 )
				   : oexT( "" ) )
				<< oexFmt( oexT( "Thread  : %d (0x%x)" oexNL8 ), p->GetThreadId(), p->GetThreadId() )
				<< st.FormatTime( oexT( "Created : %Y/%c/%d  %g:%m:%s GMT" oexNL8 ) )
				<< oexT( "---------------------------------------------------" oexNL8 );

			// Add the stack
			for ( oexUINT sp = 0; sp < p->GetStackPtr(); sp++ )
				if ( p->GetStack()[ sp ] )
					sSt << p->GetStack()[ sp ] << oexT( "()" ) << oexNL;

		} // end try
		catch( ... )
		{
			sSt << oexT( "!!! ASSERT at on address : " ) << (oexUINT)p << oexNL;

		} // end catch

	} // end while

	return sSt;
}

void CDebug::CreateCrashReport( oexCSTR pUrl, oexCSTR pSub, oexCSTR pEInfo )
{
	// Does user want crash reports?
	if ( ( !pUrl || !*pUrl ) && ( !pSub || !*pSub ) )
		return;

	// Ensure stack trace object
	CStackTrace* pSt = &oexSt();
	if ( !pSt )
		return;

	oexUINT uCurrentThreadId = oexGetCurThreadId();
	CSysTime st; st.GetSystemTime();

	// Stack trace header
	CStr sSt =    oexFmt( oexT( "Current Thread    : %d (0x%x)" oexNL8 ), uCurrentThreadId, uCurrentThreadId );
	sSt << st.FormatTime( oexT( "Current Time      : %Y/%c/%d  %g:%m:%s GMT" oexNL8 ) );
	sSt <<        oexFmt( oexT( "Stack Trace Slots : Using %d of %d" oexNL8 ), pSt->getUsedSlots(), pSt->getTotalSlots() );
	sSt <<                oexNL << ( pEInfo ? pEInfo : oexT( "(No other information available)" ) ) << oexNL;

	// Create a stack report for the current stack
	sSt << CreateStackReport( uCurrentThreadId, pSt, oexGetFileName( oexGetModuleFileName() ).Ptr(), GetInstanceHandle() );
/*
	oexUINT i = 0;
	while ( CStackTrace::SModuleInfo *pSi = pSt->NextModule( &i ) )
		if ( pSi->pSt )
		{
			try
			{
				// Create stack report for this module
				sSt << CreateStackReport( uCurrentThreadId, pSi->pSt, pSi->szName, pSi->pAddress );

			} // end try
			catch( ... )
			{
				sSt << oexT( oexNL8 "!!! Assert in module !!!" oexNL8 );

			} // end catch

		} // end while
*/
	sSt << oexNL;

	// Save to file?
	if ( pSub && *pSub )
	{
		// Where to save the error log
		CSysTime wt; wt.GetLocalTime();
		CStr sPath = oexGetModulePath( pSub );
		sPath.BuildPath( oexGetFileName( oexGetModuleFileName() ) );
		sPath.BuildPath( wt.FormatTime( oexT( "%Y_%c_%d__%g_%m_%s" ) ) );
		oexCreatePath( sPath.Ptr() );

		// Write to file
		CFile().CreateNew( oexBuildPath( sPath, oexT( "stack_trace.txt" ) ).Ptr() ).Write( sSt );

	} // end if

}


oexCHAR g_szUrl[ oexSTRSIZE ] = { 0 };
oexCHAR g_szSub[ oexSTRSIZE ] = { 0 };

LONG WINAPI OexExceptionHandler( struct _EXCEPTION_POINTERS *pEp )
{_STT();

	oexEcho( oexFmt( oexT( "exception( 0x%08x )" oexNL8 ), pEp->ExceptionRecord->ExceptionCode ).Ptr() );

	// Don't recurse crash reporting
	static BOOL bCrashInProgress = FALSE;
	if ( !bCrashInProgress )
	{
		// Crash reporting
		bCrashInProgress = TRUE;

		// Create the report
		CStr s;
		if ( pEp && pEp->ExceptionRecord )
			s << oexFmt( oexT( "Exception Code    : 0x%08x" oexNL8 ), pEp->ExceptionRecord->ExceptionCode )
			  << oexFmt( oexT( "Exception Flags   : 0x%08x" oexNL8 ),  pEp->ExceptionRecord->ExceptionFlags )
			  << oexFmt( oexT( "Exception Address : 0x%08x" oexNL8 ), pEp->ExceptionRecord->ExceptionAddress )
			  << oexNL
			  << CDebug::GetExceptionCodeName( pEp->ExceptionRecord->ExceptionCode )
			  << oexT( " - " )
			  << CDebug::GetExceptionCodeDesc( pEp->ExceptionRecord->ExceptionCode )
			  << oexNL;

		CDebug::CreateCrashReport( g_szUrl, g_szSub, s.Ptr() );

		// Crash Complete
		bCrashInProgress = FALSE;

	} // end if

	return 0;
}

void CDebug::EnableCrashReporting( oexCSTR pUrl, oexCSTR pSub )
{_STT();

	// Save user url
	if ( oexCHECK_PTR( pUrl ) )
		zstr::Copy( g_szUrl, oexSizeOfArray( g_szUrl ), pUrl );
	else
		*g_szUrl = 0;

	if ( oexCHECK_PTR( pSub ) )
		zstr::Copy( g_szSub, oexSizeOfArray( g_szSub ), pSub );
	else
		*g_szSub = 0;

	oexBOOL bEnable = ( *g_szUrl || *g_szSub );

	// Set exception handler
	::SetUnhandledExceptionFilter( bEnable ? &OexExceptionHandler : oexNULL );
}

