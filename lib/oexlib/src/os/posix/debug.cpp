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
//   HOWEVER CAUSED AND ON ANY #include "opc/breloc/basic.c"THEORY OF LIABILITY, WHETHER IN
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/

#include "../../../oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

// http://www.codeguru.com/Cpp/W-P/dll/tips/article.php/c3635/
// http://www.dotnet247.com/247reference/msgs/13/65259.aspx

oexCPVOID CDebug::GetInstanceHandle()
{
	return (oexCPVOID)getpid();
}

void CDebug::Trace( oexCSTR x_pStr )
{
	puts( oexStrToMbPtr( x_pStr ) );
}

void CDebug::FmtTrace( oexCSTR x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	vFmtTrace( x_pFmt, ap );
	oexVaEnd( ap );
}

void CDebug::vFmtTrace( oexCSTR x_pFmt, oexVaList &x_pArgs )
{
//  This is just for when the string class is malfunctioning ;-)
//    oexTCHAR tcMsg[ oexSTRSIZE ] = oexT( "" );
//    os::CSys::vStrFmt( tcMsg, oexSTRSIZE, x_pFmt, x_pArgs );
//    Trace( tcMsg );

	Trace( CStr().vFmt( x_pFmt, x_pArgs ).Ptr() );
}

void CDebug::Break()
{
	// Flush the log file contents
	CLog::GlobalLog().Flush();

#ifndef OEX_ARM
//	asm ( "int $3" );
#endif
}

void CDebug::Break( oexINT x_nType, oexCSTR x_pFile, oexUINT x_uLine, oexCSTR8 x_pFunction, oexCSTR x_pStr, oexINT x_nRes, oexINT x_nErr )
{
	CStr sPath = oexGetModulePath();
	Break( x_nType, x_pFile, x_uLine, x_pFunction, sPath.Ptr(), x_pStr, x_nRes, x_nErr );
}

void CDebug::Break( oexINT x_nType, oexCSTR x_pFile, oexUINT x_uLine, oexCSTR8 x_pFunction, oexCSTR x_pModule, oexCSTR x_pStr, oexINT x_nRes, oexINT x_nErr )
{
	CStr str;

	str << oexNL;
	str << oexT( "==========================" ) << oexNL;
	str << oexT( "  CDebug::Break() Called  " ) << oexNL;
	str << oexT( "==========================" ) << oexNL;
	str << oexT( "Module     : " ) << ( oexCHECK_PTR( x_pModule ) ? x_pModule : oexT( "" ) ) << oexNL;
	str << oexT( "File       : " ) << ( oexCHECK_PTR( x_pFile ) ? x_pFile : oexT( "" ) ) << oexNL;
	str << oexT( "Line       : " ) << x_uLine << oexNL;
	str << oexT( "Function   : " ) << ( oexCHECK_PTR( x_pFunction ) ? oexMbToStrPtr( x_pFunction ) : oexT( "???" ) ) << oexT( "()" ) << oexNL;
	str << oexT( "Expression : " ) << CStr( x_pStr ).Replace( oexT( "%" ), oexT( "%%" ) ).Ptr() << oexNL;
	if ( x_nRes )
		str << oexT( "Result     : " ) << x_nRes << oexT( " sys:\"" ) << os::CTrace::GetErrorMsg( x_nRes ).Replace( oexT( "%" ), oexT( "%%" ) ).Ptr() << oexT( "\"" ) << oexNL;
	if ( x_nErr )
		str << oexT( "Error Code : " ) << x_nErr << oexT( " sys:\"" ) << os::CTrace::GetErrorMsg( x_nErr ).Replace( oexT( "%" ), oexT( "%%" ) ).Ptr() << oexT( "\"" ) << oexNL;

	puts( oexStrToMbPtr( str.Ptr() ) );
	oexERROR( 0, str.Ptr() );

#ifdef oexBACKTRACE_IN_LOG
	oexPrintf( oexStrToMbPtr( os::CTrace::GetBacktrace( 0 ).Ptr() ) );
#endif

	Break();
}

#define CDEBUG_SIG_NAME( s ) case s: return oexT( #s );

oexCSTR CDebug::GetExceptionCodeName( oexUINT x_uCode )
{
	switch( x_uCode )
	{
		CDEBUG_SIG_NAME( SIGABRT )
		CDEBUG_SIG_NAME( SIGALRM )
		CDEBUG_SIG_NAME( SIGFPE )
		CDEBUG_SIG_NAME( SIGHUP )
		CDEBUG_SIG_NAME( SIGILL )
		CDEBUG_SIG_NAME( SIGINT )
		CDEBUG_SIG_NAME( SIGKILL )
		CDEBUG_SIG_NAME( SIGPIPE )
		CDEBUG_SIG_NAME( SIGQUIT )
		CDEBUG_SIG_NAME( SIGSEGV )
		CDEBUG_SIG_NAME( SIGTERM )
		CDEBUG_SIG_NAME( SIGUSR1 )
		CDEBUG_SIG_NAME( SIGUSR2 )
		CDEBUG_SIG_NAME( SIGCHLD )
		CDEBUG_SIG_NAME( SIGCONT )
		CDEBUG_SIG_NAME( SIGSTOP )
		CDEBUG_SIG_NAME( SIGTSTP )
		CDEBUG_SIG_NAME( SIGTTIN )
		CDEBUG_SIG_NAME( SIGTTOU )
		CDEBUG_SIG_NAME( SIGBUS )
		CDEBUG_SIG_NAME( SIGPOLL )
		CDEBUG_SIG_NAME( SIGPROF )
		CDEBUG_SIG_NAME( SIGSYS )
		CDEBUG_SIG_NAME( SIGTRAP )
		CDEBUG_SIG_NAME( SIGURG )
		CDEBUG_SIG_NAME( SIGVTALRM )
		CDEBUG_SIG_NAME( SIGXCPU )
		CDEBUG_SIG_NAME( SIGXFSZ )
		default: break;

	} // end switch

	return oexT( "SIG_UNKNOWN" );
}

oexCSTR CDebug::GetExceptionCodeDesc( oexUINT x_uCode )
{
	switch( x_uCode )
	{
		case SIGABRT: return oexT( "Process abort signal" );
		case SIGALRM: return oexT( "Alarm clock" );
		case SIGFPE: return oexT( "Erroneous arithmetic operation.  Divide by zero?" );
		case SIGHUP: return oexT( "Hangup. The controlling terminal has beek disconnected" );
		case SIGILL: return oexT( "Illegal instruction" );
		case SIGINT: return oexT( "Terminal interrupt signal. CTRL-C may have been pressed" );
		case SIGKILL: return oexT( "Kill (cannot be caught or ignored)" );
		case SIGPIPE: return oexT( "Write on a pipe with no one to read it" );
		case SIGQUIT: return oexT( "Terminal quit request" );
		case SIGSEGV: return oexT( "Invalid memory reference.  Program tried to access memory for which it does not have permissions." );
		case SIGTERM: return oexT( "Termination request" );
		case SIGUSR1: return oexT( "User-defined signal 1" );
		case SIGUSR2: return oexT( "User-defined signal 2" );
		case SIGCHLD: return oexT( "Child process terminated or stopped" );
		case SIGCONT: return oexT( "Continue executing, if stopped" );
		case SIGSTOP: return oexT( "Stop executing (cannot be caught or ignored)" );
		case SIGTSTP: return oexT( "Terminal stop signal" );
		case SIGTTIN: return oexT( "Background process attempting read" );
		case SIGTTOU: return oexT( "Background process attempting write" );
		case SIGBUS: return oexT( "Access to an undefined portion of a memory object" );
		case SIGPOLL: return oexT( "Pollable event" );
		case SIGPROF: return oexT( "Profiling timer expired" );
		case SIGSYS: return oexT( "Bad system call" );
		case SIGTRAP: return oexT( "Trace/breakpoint trap" );
		case SIGURG: return oexT( "High bandwidth data is available at a socket" );
		case SIGVTALRM: return oexT( "Virtual timer expired" );
		case SIGXCPU: return oexT( "CPU time limit exceeded" );
		case SIGXFSZ: return oexT( "File size limit exceeded" );
		default: break;

	} // end switch

	return oexT( "Unknown exception." );

}

static CStr CreateStackReport( oexUINT uCurrentThreadId, CStackTrace *pSt, oexCSTR pName, oexCPVOID pAddress, oexBOOL bStacks )
{
	if ( !pSt )
		return CStr();

	CStr sSt;
	oexUINT i = 0;
	CSysTime st; st.GetSystemTime();

	// Add module header
	sSt << 		oexNL << oexNL
		<<         oexT( "===================================================" oexNL8 )
		<<         oexT( "= Module  : " ) << ( pName ? pName : oexT( "N/A" ) ) << oexNL
                << oexFmt( oexT( "= Address : 0x%08x" oexNL8 ), oexPtrToInt( pAddress ) )
		<<         oexT( "===================================================" oexNL8 );


	// Add each thread stack
	if ( bStacks )
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
					<< oexFmt(        oexT( "Thread     : %d (0x%x)" oexNL8 ), p->GetThreadId(), p->GetThreadId() )
					<< st.FormatTime( oexT( "Created    : %Y/%c/%d  %g:%m:%s GMT" oexNL8 ) )
					<<                oexT( "Name       : " ) << p->GetName() << oexNL
					<<                oexT( "Tag        : " ) << p->GetTag() << oexNL
					<< oexFmt(        oexT( "CheckPoint : %d (0x%x)" oexNL8 ), p->GetCheckpoint(), p->GetCheckpoint() )
					<< oexT( "---------------------------------------------------" oexNL8 );

				// Add the stack
				for ( oexUINT sp = 0; sp < p->GetStackPtr(); sp++ )
					if ( p->GetStack()[ sp ] )
						sSt << oexMbToStr( p->GetStack()[ sp ] ) << oexNL;

			} // end try
			catch( ... )
			{
                                sSt << oexT( "!!! ASSERT at on address : " ) << oexPtrToInt( p ) << oexNL;

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
	CStr sSt = oexNL;
	sSt <<                oexT( "Module            : " ) << oexGetModuleFileName() << oexNL;
	sSt <<                oexT( "Version           : " ) << oexVersion() << oexNL;
	sSt <<		  oexFmt( oexT( "Current Thread    : %d (0x%x)" oexNL8 ), uCurrentThreadId, uCurrentThreadId );
	sSt << st.FormatTime( oexT( "Current Time      : %Y/%c/%d  %g:%m:%s GMT" oexNL8 ) );
	sSt <<        oexFmt( oexT( "Stack Trace Slots : Using %d of %d" oexNL8 ), pSt->getUsedSlots(), pSt->getTotalSlots() );
	sSt <<                oexNL << ( pEInfo ? pEInfo : oexT( "(No other information available)" ) ) << oexNL;

	// Create a stack report for the current stack
	sSt << CreateStackReport( uCurrentThreadId, pSt, oexGetFileName( oexGetModuleFileName() ).Ptr(), GetInstanceHandle(), oexTRUE );
/*
	oexUINT i = 0;
	while ( CStackTrace::SModuleInfo *pSi = pSt->NextModule( &i ) )
		if ( pSi->pSt )
		{
			try
			{
				// Create stack report for this module
				sSt << CreateStackReport( uCurrentThreadId, pSi->pSt, pSi->szName, pSi->pAddress, oexFALSE );

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

oexTCHAR g_szUrl[ oexSTRSIZE ] = { 0 };
oexTCHAR g_szSub[ oexSTRSIZE ] = { 0 };

void sig_callback( int signal_number )
{
	oexEcho( oexMks( oexT( "sig_callback( " ), signal_number, oexT( " )" ) ).Ptr() );

	// Don't recurse crash reporting
	static oexBOOL bCrashInProgress = oexFALSE;
	if ( !bCrashInProgress )
	{
		// Crash reporting
		bCrashInProgress = oexTRUE;

		// Create the report
		CStr s;
		s
			  << oexFmt( oexT( "Signal Number : %d ( 0x%02x )" oexNL8 ), signal_number, signal_number )
			  << oexNL
			  << CDebug::GetExceptionCodeName( signal_number )
			  << oexT( " - " )
			  << CDebug::GetExceptionCodeDesc( signal_number )
			  << oexNL;

		CDebug::CreateCrashReport( g_szUrl, g_szSub, s.Ptr() );

		// Crash Complete
		bCrashInProgress = oexFALSE;

		exit( -1 );

	} // end if

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

	// Set signal handler
	struct sigaction sa; oexZero( sa );
	sa.sa_handler = bEnable ? sig_callback : 0;
	sigemptyset( &sa.sa_mask );
	sa.sa_flags = SA_RESTART;
	sigaction( SIGSEGV, &sa, 0 );
	sigaction( SIGILL, &sa, 0 );
	sigaction( SIGFPE, &sa, 0 );

}


