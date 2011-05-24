/*------------------------------------------------------------------
// sys.cpp
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

#include "oexlib.h"

#include "std_os.h"

#include <stdlib.h>
#include <conio.h>
#if defined( OEX_WINCE ) || defined( __MINGW32__ )
//#	include <wtypes.h>
#	define VSNPRINTF	vsnprintf
#	define STRTOLL		strtoll
#	define VSNWPRINTF	vswprintf
#	define WCSTOLL		wcstoll
#else
#	include <tchar.h>
#	define VSNPRINTF	_vsnprintf
#	define STRTOLL		_strtoi64
#	define VSNWPRINTF	_vsnwprintf
#	define WCSTOLL		_wcstoi64
#endif
#include <time.h>

#if ( _MSC_VER >= 1300 )
#	include <strsafe.h>
#else
#	include <stdio.h>
#	include <wchar.h>
#	include <stdarg.h>
#endif

#include <objbase.h>

// Linked in
#define _wautostr oex::CStrW
#define _wautostr_tomb( s ) (oexStrWToMbPtr( s ))
#define _wautostr_towc( s ) (oexStrToStrWPtr( s ))
#include "../../../oss/cpu/cpu_usage.hpp"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

oexSTATIC_ASSERT( oexSTRSIZE >= MAX_PATH );

oexSTATIC_ASSERT( sizeof( GUID ) == sizeof( oexGUID ) );

oexSTATIC_ASSERT( sizeof( CSys::t_WAITABLE ) == sizeof( HANDLE ) );

const oexUINT		CSys::c_StrErr_OK = S_OK;

static oexLock		*g_lockCpu = oexNULL;
static CCpuUsage	*g_cpu = oexNULL;

// Disable microsoft security warnings
// Wouldn't have to do this but StringCchVPrintfW() has issues.
#if defined( _MSC_VER )
#	pragma warning( disable : 4995 4996 )
#elif defined( __INTEL_COMPILER )
#	pragma warning( disable : 1786 )
#endif

//#if ( _MSC_VER >= 1300 )
// !!! Don't use StringCchVPrintfW()
//     it contains a very crazy buffer overrun
#if 0
#	define AWESOMENEWFULLOFBUGSWINDOWSSTRINGFUNCTION
#else
#	undef AWESOMENEWFULLOFBUGSWINDOWSSTRINGFUNCTION
#endif

#if defined( AWESOMENEWFULLOFBUGSWINDOWSSTRINGFUNCTION )

	const oexUINT		CSys::c_StrErr_INVALID_PARAMETER = STRSAFE_E_INVALID_PARAMETER;
	const oexUINT		CSys::c_StrErr_INSUFFICIENT_BUFFER = STRSAFE_E_INSUFFICIENT_BUFFER;

#else

	const oexUINT		CSys::c_StrErr_INVALID_PARAMETER = (oexUINT)-1;
	const oexUINT		CSys::c_StrErr_INSUFFICIENT_BUFFER = (oexUINT)-2;

#endif

const oexUINT	    CSys::c_Infinite = INFINITE;

const oexUINT       CSys::c_uMaximumWaitObjects = MAXIMUM_WAIT_OBJECTS;

oexSTATIC_ASSERT( CSys::eMaximumWaitObjects == MAXIMUM_WAIT_OBJECTS );

oexCPVOID CSys::GetInstanceHandle()
{
#if defined( __ImageBase )

	return (oexCPVOID)&__ImageBase;

#else

	MEMORY_BASIC_INFORMATION mbi;
	static int dummy;
	VirtualQuery( &dummy, &mbi, sizeof( mbi ) );

	return (oexCPVOID)mbi.AllocationBase;

#endif
}

oexBOOL CSys::SetThreadPriority( oexINT x_nPriority )
{
	return ::SetThreadPriority( ::GetCurrentThread(), x_nPriority );
}

oexUINT CSys::SetThreadAffinity( oexUINT x_nAffinity )
{
	return ::SetThreadAffinityMask( ::GetCurrentThread(), x_nAffinity );
}

oexINT CSys::ShowMessageBox( oexCSTR x_pTitle, oexCSTR x_pStr )
{//_STT();
	if ( !oexCHECK_PTR( x_pTitle ) )
		x_pTitle = oexT( "" );
 	if ( !oexCHECK_PTR( x_pStr ) )
		x_pStr = oexT( "" );
	return ::MessageBox( NULL, x_pStr, x_pTitle, MB_OK );
}

oexINT CSys::Quit( oexINT x_nReturnCode )
{//_STT();
	return PostThreadMessage( 0, WM_QUIT, x_nReturnCode, 0L );
}

void CSys::Exit( oexINT x_nRet )
{//_STT();
	exit( x_nRet );
}

oexINT CSys::Flush_stdout()
{//_STT();
	return fflush( stdout );
}

int CSys::GetKey()
{//_STT();
	return _getch();
}

int CSys::IsKey()
{//_STT();
	return _kbhit();
}

oexINT CSys::SetRoot()
{
	// Get operating system info
	OSVERSIONINFO  osVersion ;
	osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO) ;
	if ( !::GetVersionEx( &osVersion ) )
		osVersion.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
	
	// If no UAC
	if ( osVersion.dwMajorVersion < 6 )
		return 1;
		
	HANDLE hToken;
	if ( !OpenProcessToken( GetCurrentProcess(), TOKEN_WRITE | TOKEN_WRITE, &hToken ) )
		return 0;

	DWORD len = 0;
	TOKEN_ELEVATION_TYPE tet = TokenElevationTypeDefault;
	if ( !GetTokenInformation( hToken, TokenElevationType, &tet, sizeof( tet ), &len ) || tet != TokenElevationTypeFull )
	{	tet = TokenElevationTypeFull;
		if ( !SetTokenInformation( hToken, TokenElevationType, &tet, sizeof( tet ) ) )
			tet = TokenElevationTypeDefault;
	} // end if
	
	CloseHandle( hToken );
	
	return ( tet == TokenElevationTypeFull ) ? 1 : 0;	
}

oexINT CSys::IsRoot()
{
	// Get operating system info
	OSVERSIONINFO  osVersion ;
	osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO) ;
	if ( !::GetVersionEx( &osVersion ) )
		osVersion.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
	
	// If no UAC
	if ( osVersion.dwMajorVersion < 6 )
		return 1;
		
	HANDLE hToken;
	if ( !OpenProcessToken( GetCurrentProcess(), TOKEN_READ, &hToken ) )
		return 0;

	DWORD len = 0;
	TOKEN_ELEVATION_TYPE tet = TokenElevationTypeDefault;
	if ( !GetTokenInformation( hToken, TokenElevationType, &tet, sizeof( tet ), &len ) )
		tet = TokenElevationTypeDefault;
	
	CloseHandle( hToken );
	
	return ( tet == TokenElevationTypeFull ) ? 1 : 0;	
}

oexINT CSys::CtrlComputer( int nCmd, int nForce, oexCSTR pMsg )
{
	if ( !pMsg || !*pMsg )
		pMsg = 0;

		// Get operating system info
//	OSVERSIONINFO  osVersion ;
//	osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO) ;
//	if ( !::GetVersionEx( &osVersion ) )
//		osVersion.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
	
	// NT doesn't just let us reboot the computer
//	if ( osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT ) 
	{
		HANDLE				hToken;
		TOKEN_PRIVILEGES	tp;

		// Get our token information
		if ( OpenProcessToken(	GetCurrentProcess(), 
								TOKEN_ADJUST_PRIVILEGES |
								TOKEN_QUERY,
								&hToken ) )
		{
			// Get 
			if ( LookupPrivilegeValue(	NULL,
										SE_SHUTDOWN_NAME,
										&tp.Privileges[ 0 ].Luid ) )
			{
				// Set up structure
				tp.PrivilegeCount = 1;
				tp.Privileges[ 0 ].Attributes = SE_PRIVILEGE_ENABLED;

				// Attempt to adjust security token
				AdjustTokenPrivileges( hToken, FALSE, &tp, 0, NULL, 0 );
				
			} // end if

		} // end if

	} // end NT
	
	UINT uFlags = 0;
	
	// What to do?
	switch( nCmd )
	{	case eReboot : uFlags |= EWX_REBOOT; break;
		case eRestart : uFlags |= EWX_REBOOT; break; // EWX_RESTARTAPPS; break;
		case ePowerOff : uFlags |= EWX_POWEROFF; break;
		case eLogOff : uFlags |= EWX_LOGOFF; break;
		case eShutdown : uFlags |= EWX_SHUTDOWN; break;
	} // end switch

	// Force
	switch( nForce )
	{	case 1 : uFlags |= EWX_FORCE; break;
		case 2 : uFlags |= EWX_FORCEIFHUNG; break;
	} // end switch
	
	// User mode command
	ExitWindowsEx( uFlags, 1 );

	// In case we're a service
	if ( nCmd != eLogOff && nCmd != eShutdown )
		InitiateSystemShutdown( NULL, (oexSTR)pMsg, ( pMsg && *pMsg ) ? 8 : 0, nForce, 
								( nCmd == eReboot || nCmd == eRestart ) ? TRUE : FALSE );
	
	return 1;
}

// **** Multi-byte

oexCSTR8 CSys::StrFmt( oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, ... )
{//_STT();
	oexVaList ap; oexVaStart( ap, x_pFmt );
	oexCSTR8 pStr = vStrFmt( x_pDst, x_uMax, x_pFmt, ap );
	oexVaEnd( ap );
	return pStr;
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTR8 CSys::vStrFmt( oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, oexVaList x_pArgs )
{//_STT();
	return vStrFmt( oexNULL, x_pDst, x_uMax, x_pFmt, x_pArgs );
}

oexCSTR8 CSys::StrFmt( oexRESULT *x_pRes, oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, ... )
{//_STT();
	oexVaList ap; oexVaStart( ap, x_pFmt );
	oexCSTR8 pStr = vStrFmt( x_pRes, x_pDst, x_uMax, x_pFmt, ap );
	oexVaEnd( ap );
	return pStr;
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTR8 CSys::vStrFmt( oexRESULT *x_pRes, oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, oexVaList x_pArgs )
{//_STT();
	if ( x_pRes )
		*x_pRes = 0;

	// Verify data pointers
	if ( !oexVERIFY_PTR( x_pDst ) || !oexVERIFY_PTR( x_pFmt ) || !x_uMax )
	{
		if ( x_pRes )
			*x_pRes = c_StrErr_INVALID_PARAMETER;

		return x_pDst;
	} // end if

	oexRESULT res = 0;

#if defined( AWESOMENEWFULLOFBUGSWINDOWSSTRINGFUNCTION )

	// Create format string
	res = StringCchVPrintfA( x_pDst, x_uMax, x_pFmt, (va_list)x_pArgs );

#else

	// Create format string
	oexINT nRet = VSNPRINTF( x_pDst, x_uMax, x_pFmt, (va_list)x_pArgs );
	if ( 0 > nRet || x_uMax < (oexUINT)nRet )
	{
		// Null terminate buffer
		x_pDst[ x_uMax - 1 ] = 0;

		// Let the user know what went wrong
		res = c_StrErr_INSUFFICIENT_BUFFER;

	} // end if

#endif

	// What to do with the result
	if ( x_pRes )
		*x_pRes = res;

	else
		oexASSERT( 0 == res );

	return x_pDst;
}

oexINT64 CSys::StrToInt64( oexCSTR8 x_pStr, oexUINT x_uRadix )
{//_STT();
	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
    return STRTOLL( x_pStr, NULL, x_uRadix );
}

oexUINT64 CSys::StrToUInt64( oexCSTR8 x_pStr, oexUINT x_uRadix )
{//_STT();
	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
    return STRTOLL( x_pStr, NULL, x_uRadix );
}

oexLONG CSys::StrToLong( oexCSTR8 x_pStr, oexUINT x_uRadix )
{//_STT();
	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return strtol( x_pStr, NULL, x_uRadix );
}

oexULONG CSys::StrToULong( oexCSTR8 x_pStr, oexUINT x_uRadix )
{//_STT();
	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return strtoul( x_pStr, NULL, x_uRadix );
}

oexFLOAT CSys::StrToFloat( oexCSTR8 x_pStr )
{//_STT();
	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return (oexFLOAT)strtod( x_pStr, NULL );
}

oexDOUBLE CSys::StrToDouble( oexCSTR8 x_pStr )
{//_STT();
	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return strtod( x_pStr, NULL );
}

// **** Unicode

oexCSTRW CSys::StrFmt( oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, ... )
{//_STT();
	oexVaList ap; oexVaStart( ap, x_pFmt );
	oexCSTRW pStr = vStrFmt( x_pDst, x_uMax, x_pFmt, ap );
	oexVaEnd( ap );
	return pStr;
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTRW CSys::vStrFmt( oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, oexVaList x_pArgs )
{//_STT();
	return vStrFmt( oexNULL, x_pDst, x_uMax, x_pFmt, x_pArgs );
}

oexCSTRW CSys::StrFmt( oexRESULT *x_pRes, oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, ... )
{//_STT();
	oexVaList ap; oexVaStart( ap, x_pFmt );
	oexCSTRW pStr = vStrFmt( x_pRes, x_pDst, x_uMax, x_pFmt, ap );
	oexVaEnd( ap );
	return pStr;
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTRW CSys::vStrFmt( oexRESULT *x_pRes, oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, oexVaList x_pArgs )
{//_STT();
	if ( x_pRes )
		*x_pRes = 0;

	// Verify data pointers
	if ( !oexVERIFY_PTR( x_pDst ) || !oexVERIFY_PTR( x_pFmt ) || !x_uMax )
	{
		if ( x_pRes )
			*x_pRes = c_StrErr_INVALID_PARAMETER;

		return x_pDst;
	} // end if

	oexRESULT res = 0;

#if defined( AWESOMENEWFULLOFBUGSWINDOWSSTRINGFUNCTION )

	// Create format string
	res = StringCchVPrintfW( x_pDst, x_uMax, x_pFmt, (va_list)x_pArgs );

#else

#if defined( OEX_WINCE ) || defined( __MINGW32__ )

	// Create format string
	oexINT nRet = VSNWPRINTF( x_pDst, x_pFmt, (va_list)x_pArgs );
	if ( 0 > nRet || x_uMax < (oexUINT)nRet )
	{
		// Null terminate buffer
		x_pDst[ x_uMax - 1 ] = 0;

		// Let the user know what went wrong
		res = c_StrErr_INSUFFICIENT_BUFFER;

	} // end if

#else

	// Create format string
	oexINT nRet = VSNWPRINTF( x_pDst, x_uMax, x_pFmt, (va_list)x_pArgs );
	if ( 0 > nRet || x_uMax < nRet )
	{
		// Null terminate buffer
		x_pDst[ x_uMax - 1 ] = 0;

		// Let the user know what went wrong
		res = c_StrErr_INSUFFICIENT_BUFFER;

	} // end if

#endif
#endif

	// What to do with the result
	if ( x_pRes )
		*x_pRes = res;

	else
		oexASSERT( 0 == res );

	return x_pDst;
}

oexINT64 CSys::StrToInt64( oexCSTRW x_pStr, oexUINT x_uRadix )
{//_STT();
	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return WCSTOLL( x_pStr, NULL, x_uRadix );
}

oexUINT64 CSys::StrToUInt64( oexCSTRW x_pStr, oexUINT x_uRadix )
{//_STT();
	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return WCSTOLL( x_pStr, NULL, x_uRadix );
}

oexLONG CSys::StrToLong( oexCSTRW x_pStr, oexUINT x_uRadix )
{//_STT();
	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return wcstol( x_pStr, NULL, x_uRadix );
}

oexULONG CSys::StrToULong( oexCSTRW x_pStr, oexUINT x_uRadix )
{//_STT();
	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return wcstoul( x_pStr, NULL, x_uRadix );
}

oexFLOAT CSys::StrToFloat( oexCSTRW x_pStr )
{//_STT();
	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return (oexFLOAT)wcstod( x_pStr, NULL );
}

oexDOUBLE CSys::StrToDouble( oexCSTRW x_pStr )
{//_STT();
	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return wcstod( x_pStr, NULL );
}

/// vprintf
int CSys::vPrintf( oexCSTRW x_pFmt, oexVaList pArgs )
{//_STT();
	return ::vwprintf( x_pFmt, (va_list)pArgs );
}

/// printf function
int CSys::Printf( oexCSTRW x_pFmt, ... )
{//_STT();
	oexVaList ap; oexVaStart( ap, x_pFmt );
	int ret = ::vwprintf( x_pFmt, (va_list)ap );
	oexVaEnd( ap );
	return ret;
}

int CSys::Echo( oexCSTRW x_pFmt )
{//_STT();
	if ( !x_pFmt )
		return 0;
	CStr8 s = oexStrWToStr( x_pFmt );
#if defined( oexUNICODE )
	CUtil::AddOutput( x_pFmt, s.Length(), oexTRUE );
#else
	CUtil::AddOutput( s.Ptr(), s.Length(), oexTRUE );
#endif
	return ::puts( s.Ptr() );
}

int CSys::Echo( oexCSTRW x_pFmt, oexLONG x_lLen )
{//_STT();
	if ( !x_pFmt || 0 >= x_lLen )
		return 0;
#if defined( oexUNICODE )
	CUtil::AddOutput( x_pFmt, x_lLen, oexTRUE );
#else
	CStr s = oexStrWToStr( CStrW( x_pFmt, x_lLen ) );
	CUtil::AddOutput( s.Ptr(), s.Length(), oexTRUE );
#endif
	return ::fwrite( x_pFmt, 1, x_lLen, stdout );
}

oexPVOID CSys::MemCpy( oexPVOID x_pDst, oexCPVOID x_pSrc, oexUINT x_uSize )
{// //_STT();
	return ::memcpy( x_pDst, x_pSrc, x_uSize );
}

oexINT CSys::MemCmp( oexCPVOID x_pDst, oexCPVOID x_pSrc, oexUINT x_uSize )
{// //_STT();
	return ::memcmp( x_pDst, x_pSrc, x_uSize );
}

oexPVOID CSys::MemSet( oexPVOID x_pDst, oexINT x_nCh, oexUINT x_uSize )
{// //_STT();
	return ::memset( x_pDst, x_nCh, x_uSize );
}

oexGUID * CSys::CreateGuid( oexGUID *pGuid )
{//_STT();
    CoCreateGuid( (GUID*)pGuid );
    return pGuid;
}


void CSys::Sleep( oexUINT uMilliseconds, oexUINT uSeconds )
{//_STT();
	::Sleep( uMilliseconds + ( uSeconds * 1000 ) );
}

static timeval  g_tv_microsleep_socket;
static fd_set   g_fd_set_microsleep_socket;
static oexINT   g_microsleep_socket_init = -1;
static SOCKET   g_microsleep_socket = INVALID_SOCKET;

static oexBOOL CSys_ReleaseMicroSleep()
{//_STT();
    if ( INVALID_SOCKET != g_microsleep_socket )
    {   closesocket( g_microsleep_socket );
        g_microsleep_socket = INVALID_SOCKET;
    } // end if

    if ( 0 <= g_microsleep_socket_init )
    {
#if !defined( OEX_NOSOCKET2 )
       WSACleanup();
#endif
        g_microsleep_socket_init = -1;
    } // end if

    return oexTRUE;
}

static oexBOOL CSys_InitMicroSleep()
{//_STT();
    if ( 0 <= g_microsleep_socket_init )
        return oexTRUE;

	WSADATA wd;
	if ( 0 != WSAStartup( 0x0101, &wd ) )
    {   oexASSERT( 0 ); return oexFALSE; }

    g_microsleep_socket_init = 0;

    g_microsleep_socket = socket( AF_INET, SOCK_RAW, 0 );
    if ( INVALID_SOCKET == g_microsleep_socket )
    {   oexASSERT( 0 );
        CSys_ReleaseMicroSleep();
        return oexFALSE;
    } // end if

    // Set the fd_set structure
    g_fd_set_microsleep_socket.fd_count = 1;
    g_fd_set_microsleep_socket.fd_array[ 0 ] = g_microsleep_socket;

    return oexTRUE;
}

// +++ Anyone know of another accurate microsecond wait on Windows?
oexBOOL CSys::MicroSleep( oexUINT uMicroseconds, oexUINT uSeconds )
{//_STT();
    // Ensure we're initialized
    if ( 0 > g_microsleep_socket_init )
        if ( !CSys_InitMicroSleep() )
        {   Sleep( uMicroseconds / 1000, uSeconds );
            return oexFALSE;
        } // end if

    // Fill in wait structure
    g_tv_microsleep_socket.tv_sec = uSeconds;
    g_tv_microsleep_socket.tv_usec = uMicroseconds;

    // Let the socket do the waiting
    return 0 == select( 0, NULL, NULL,
                        &g_fd_set_microsleep_socket,
                        &g_tv_microsleep_socket ) ? oexTRUE : oexFALSE;
}

oexBOOL CSys::Init()
{//_STT();
#if defined( OEX_WINCE )
	CoInitializeEx( NULL, COINIT_MULTITHREADED );
#else
	CoInitialize( NULL );
#endif

    return oexTRUE;
}

oexBOOL CSys::Uninit()
{//_STT();
	CoUninitialize();

	if ( g_lockCpu )
	{
		oexAutoLock ll( g_lockCpu );
		if ( ll.IsLocked() )
		{
			// Kill cpu class
			if ( g_cpu )
			{	OexAllocDelete< CCpuUsage >( g_cpu );
				g_cpu = 0;
			} // end if

			oexLock *p = g_lockCpu;
			g_lockCpu = oexNULL;

			ll.Unlock();
			OexAllocDelete< oexLock >( p );

		} // end if

	} // end if

    return CSys_ReleaseMicroSleep();
}

void CSys::CloseHandle( t_WAITABLE x_pHandle )
{//_STT();
	if ( x_pHandle ) CloseHandle( (HANDLE)x_pHandle );
}

CSys::t_WAITABLE CSys::DuplicateHandle( t_WAITABLE x_pHandle )
{//_STT();
    HANDLE hDup = NULL;
    if ( !::DuplicateHandle( GetCurrentProcess(), (HANDLE)x_pHandle,
                             GetCurrentProcess(), &hDup,
                             0, FALSE, DUPLICATE_SAME_ACCESS ) )
        return NULL;
    return (t_WAITABLE)hDup;
}

oexINT CSys::WaitForSingleObject( CSys::t_WAITABLE x_pHandle, oexUINT x_uTimeout )
{//_STT();
	DWORD dwRet = ::WaitForSingleObject( (HANDLE)x_pHandle, (DWORD)x_uTimeout );
	if ( WAIT_OBJECT_0 == dwRet )
        return waitSuccess;
	else if ( WAIT_TIMEOUT == dwRet )
        return waitTimeout;
	return waitFailed;
}

oexINT CSys::WaitForMultipleObjects( oexUINT x_uObjects, CSys::t_WAITABLE *x_pHandle, oexBOOL x_bWaitAll, oexUINT x_uTimeout )
{//_STT();
    // Currently 64, don't blame me, this is a Windows limitation...
    oexASSERT( MAXIMUM_WAIT_OBJECTS >= x_uObjects );

    // Do the wait
    DWORD dwRet = ::WaitForMultipleObjects( (DWORD)x_uObjects, (HANDLE*)x_pHandle, x_bWaitAll, x_uTimeout );

	// WAIT_OBJECT_0 should be zero
	oexSTATIC_ASSERT( WAIT_OBJECT_0 == 0 );

	// Use this line if WAIT_OBJECT_0 is not zero
//	if ( WAIT_OBJECT_0 <= dwRet && ( WAIT_OBJECT_0 + x_uObjects ) > dwRet )
//        return dwRet - WAIT_OBJECT_0;

	if ( ( WAIT_OBJECT_0 + x_uObjects ) > dwRet )
        return (etWait)dwRet;

	else if ( WAIT_TIMEOUT == dwRet )
        return waitTimeout;

	return waitFailed;
}

oexLONG CSys::increment( oexLONG *x_puVal )
{//_STT();
	return ::InterlockedIncrement( x_puVal );
}

oexLONG CSys::decrement( oexLONG *x_puVal )
{//_STT();
	return ::InterlockedDecrement( x_puVal );
}

void CSys_SystemTimeToSTime( SYSTEMTIME &st, CSys::STime &t )
{//_STT();
    t.uYear = st.wYear;
    t.uMonth = st.wMonth;
    t.uDayOfWeek = st.wDayOfWeek;
    t.uDay = st.wDay;
    t.uHour = st.wHour;
    t.uMinute = st.wMinute;
    t.uSecond = st.wSecond;
    t.uMillisecond = st.wMilliseconds;
}

void CSys_STimeToSystemTime( CSys::STime &t, SYSTEMTIME &st )
{//_STT();
    st.wYear = t.uYear;
    st.wMonth = t.uMonth;
    st.wDayOfWeek = t.uDayOfWeek;
    st.wDay = t.uDay;
    st.wHour = t.uHour;
    st.wMinute = t.uMinute;
    st.wSecond = t.uSecond;
    st.wMilliseconds = t.uMillisecond;
}

oexBOOL CSys::GetLocalTime( STime &t )
{//_STT();
    CSys::Zero( &t, sizeof( t ) );

    SYSTEMTIME st;
    CSys::Zero( &st, sizeof( st ) );

    ::GetLocalTime( &st );
    CSys_SystemTimeToSTime( st, t );

    TIME_ZONE_INFORMATION tz;
    CSys::Zero( &tz, sizeof( tz ) );

    ::GetTimeZoneInformation( &tz );
    t.nTzBias = tz.Bias;

    return oexTRUE;
}

oexBOOL CSys::SetLocalTime( STime &t )
{//_STT();
    SYSTEMTIME st;
    CSys::Zero( &st, sizeof( st ) );

	CSys_STimeToSystemTime( t, st );

	// Set local time must be called twice to ensure
	// daylight saving is updated correctly.  Why the
	// function couldn't do this on it's own is beyond me.
    ::SetLocalTime( &st );
    ::SetLocalTime( &st );

    return oexTRUE;
}

oexINT CSys::GetLocalTzBias()
{//_STT();
    TIME_ZONE_INFORMATION tz;
    CSys::Zero( &tz, sizeof( tz ) );

    ::GetTimeZoneInformation( &tz );
    return tz.Bias;
}


oexBOOL CSys::GetSystemTime( STime &t )
{//_STT();
    CSys::Zero( &t, sizeof( t ) );

    SYSTEMTIME st;
    CSys::Zero( &st, sizeof( st ) );

    ::GetSystemTime( &st );
    CSys_SystemTimeToSTime( st, t );

    return oexTRUE;
}

oexBOOL CSys::SetSystemTime( STime &t )
{//_STT();
    SYSTEMTIME st;
    CSys::Zero( &st, sizeof( st ) );

	CSys_STimeToSystemTime( t, st );

    ::SetSystemTime( &st );

    return oexTRUE;
}

/// Converts an STime structure to file time
oexINT64 CSys::SystemTimeToFileTime( STime &x_st )
{//_STT();
    SYSTEMTIME st;
    CSys_STimeToSystemTime( x_st, st );

    FILETIME ft;
    ::SystemTimeToFileTime( &st, &ft );

    return (oexINT64)ft.dwLowDateTime | ( (oexINT64)ft.dwHighDateTime << 32 );
}

/// Converts a file time to an STime structure
void CSys::FileTimeToSystemTime( STime &x_st, oexINT64 x_ft )
{//_STT();
    FILETIME ft;
    ft.dwLowDateTime = (oexINT32)( x_ft & 0xffffffff );
    ft.dwHighDateTime = (oexINT32)( ( x_ft >> 32 ) & 0xffffffff );

    SYSTEMTIME st;
    ::FileTimeToSystemTime( &ft, &st );

    CSys_SystemTimeToSTime( st, x_st );
}

oexUINT CSys::GetUnixTime()
{// //_STT();
	return time( NULL );
}


oexUINT CSys::WcsToMbs( oexSTR8 pDst, oexUINT uMax, oexCSTRW pSrc, oexUINT uLen )
{//_STT();
	if ( !pSrc || !uLen )
		return 0;

	// +++ Ensure source is NULL terminated
//	pSrc[ uLen - 1 ] = 0;

	return wcstombs( pDst, pSrc, uMax );
}

oexUINT CSys::MbsToWcs( oexSTRW pDst, oexUINT uMax, oexCSTR8 pSrc, oexUINT uLen )
{//_STT();
	if ( !pSrc || !uLen )
		return 0;

	// +++ Ensure source is NULL terminated
//	pSrc[ uLen - 1 ] = 0;

	return mbstowcs( pDst, pSrc, uMax );
}

oexCSTR CSys::SetLocale( oexINT nCategory, oexCSTR pLocal )
{//_STT();
//  +++ Resolve temporary string problem
//	return setlocale( nCategory, oexStrToMbPtr( pLocal ) );
	return oexT( "" );
}

/// printf function
int CSys::Printf( oexCSTR8 x_pFmt, ... )
{//_STT();
	oexVaList ap; oexVaStart( ap, x_pFmt );
	int ret = vprintf( x_pFmt, (va_list)ap );
	oexVaEnd( ap );
	return ret;
}

/// vprintf
int CSys::vPrintf( oexCSTR8 x_pFmt, oexVaList pArgs )
{//_STT();
	return ::vprintf( x_pFmt, (va_list)pArgs );
}

int CSys::Echo( oexCSTR8 x_pFmt )
{//_STT();
	if ( !x_pFmt )
		return 0;
#if defined( oexUNICODE )
	CStr s = oexMbToStr( x_pFmt );
	CUtil::AddOutput( s.Ptr(), s.Length(), oexTRUE );
#else
	CUtil::AddOutput( x_pFmt, 0, oexTRUE );
#endif
	return ::puts( x_pFmt );
}

int CSys::Echo( oexCSTR8 x_pFmt, oexLONG x_lLen )
{//_STT();
	if ( !x_pFmt || 0 >= x_lLen )
		return 0;
#if !defined( oexUNICODE )
	CUtil::AddOutput( x_pFmt, x_lLen, oexTRUE );
#else
	CStr s = oexStrToStrW( CStr8( x_pFmt, x_lLen ) );
	CUtil::AddOutput( s.Ptr(), s.Length(), oexTRUE );
#endif
	return ::fwrite( x_pFmt, 1, x_lLen, stdout );
}

oexLONG CSys::Read_stdin( oexSTR8 x_pBuf, oexLONG x_lMax )
{//_STT();

	if ( !x_pBuf || 0 >= x_lMax )
		return 0;

	char  c;
	DWORD dwRead = 0;
    HANDLE h = GetStdHandle( STD_INPUT_HANDLE );
	if ( INVALID_HANDLE_VALUE == h )
		return 0;
		
	DWORD dwEvents = 0;
	if ( GetNumberOfConsoleInputEvents( h, &dwEvents ) && !dwEvents )
		dwRead = 0;
		
	else if ( !PeekNamedPipe( h, x_pBuf, (DWORD)x_lMax, &dwRead, NULL, NULL ) )
		dwRead = 0;
		
	CloseHandle( h );
		
	return (oexLONG)dwRead;
}

oexUINT CSys::GetCurThreadId()
{// //_STT();
	return GetCurrentThreadId();
}

oexBOOL CSys::PumpThreadMessages()
{//_STT();
    MSG msg;

    // See if a message is waiting
    while ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
    {
        // Get if from the queue
        if( GetMessage( &msg, NULL, 0, 0 ) )
        {
            // Give the dialog box a chance at this message
            if ( !IsDialogMessage( msg.hwnd, &msg ) )
            {
                // Translate the message
                TranslateMessage ( &msg );

                // Dispatch the message
                DispatchMessage ( &msg );

            } // end if

        } // end else

        else return oexFALSE;

    } // end if

    return oexTRUE;
}

/// +++ Possibly we can somehow create another process and call this function ???
/*
extern "C" oex::oexRESULT OEX_SRVMODULE_20081230192357EST_Run( oex::oexCSTR x_pPath )
{

	return -1;
}
*/

oexINT CSys::Fork( oexCSTR x_pWorkingDirectory, oexCSTR x_pLogFile )
{//_STT();

	// Hmmmmm
	oexERROR( 0, oexT( "Cant fork() on Windows" ) );

	return -1;
}

oexBOOL CSys::Shell( oexCSTR x_pFile, oexCSTR x_pParams, oexCSTR x_pDirectory )
{//_STT();
	if ( !oexCHECK_PTR( x_pFile ) )
		return oexFALSE;

#if !defined( OEX_WINCE )

	return ( 32 < oexPtrToInt( ShellExecute( NULL, oexT( "open" ),
											 x_pFile, x_pParams,
											 x_pDirectory, SW_SHOWNORMAL ) ) )
		   ? oexTRUE : oexFALSE;
#else

	SHELLEXECUTEINFO sei;
	oexZero( sei );
	sei.cbSize = sizeof( sei );
	sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
	sei.lpVerb = oexT( "open" );
	sei.lpFile = x_pFile;
	sei.lpParameters = x_pParams;
	sei.lpDirectory = x_pDirectory;
	sei.nShow = SW_SHOWNORMAL;

	if ( !ShellExecuteEx( &sei ) )
		return oexFALSE;

	return ( 32 < (oexUINT)sei.hInstApp ) ? oexTRUE : oexFALSE;

#endif
}

oexUINT CSys::StartProcess( oexCSTR x_pFile, oexCSTR x_pParams, oexCSTR x_pDirectory )
{//_STT();

	if ( !oexCHECK_PTR( x_pFile ) )
		return 0;

#if !defined( OEX_WINCE )

	PROCESS_INFORMATION pi; oexZero( pi );
	if ( !CreateProcess( x_pFile, (LPSTR)NULL, NULL, NULL, FALSE, 0, NULL, x_pDirectory, NULL, &pi ) )
		return 0;

	CloseHandle( pi.hThread );
	CloseHandle( pi.hProcess );

	return (oexUINT)pi.dwProcessId;

#else

#endif
}

#if !defined( OEX_GCC )

oexDOUBLE CSys::GetCpuLoad()
{//_STT();
	if ( !g_lockCpu )
		g_lockCpu = OexAllocConstruct< oexLock >();
	if ( !g_lockCpu )
		return 0;

	oexAutoLock ll( g_lockCpu );
	if ( !ll.IsLocked() )
		return 0;

	if ( !g_cpu )
		g_cpu = OexAllocConstruct< CCpuUsage >();
	if ( !g_cpu )
		return 0;

	return g_cpu->GetCpuUsage();
}

oexDOUBLE CSys::GetCpuLoad( oexCSTR x_pProcessName )
{//_STT();
	if ( !g_lockCpu )
		g_lockCpu = OexAllocConstruct< oexLock >();
	if ( !g_lockCpu )
		return 0;

	oexAutoLock ll( g_lockCpu );
	if ( !ll.IsLocked() )
		return 0;

	if ( !g_cpu )
		g_cpu = OexAllocConstruct< CCpuUsage >();
	if ( !g_cpu )
		return 0;

	return g_cpu->GetCpuUsage( x_pProcessName );
}

#else

// +++ At the moment, the cpu code above is crashing in gcc,
//     and isn't working anyway

oexDOUBLE CSys::GetCpuLoad()
{//_STT();
	return 0;
}

oexDOUBLE CSys::GetCpuLoad( oexCSTR x_pProcessName )
{//_STT();
	return 0;
}

#endif
