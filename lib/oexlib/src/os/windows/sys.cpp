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

#include "../../../oexlib.h"


#include "std_os.h"

#include <stdlib.h>
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

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

oexSTATIC_ASSERT( oexSTRSIZE >= MAX_PATH );

oexSTATIC_ASSERT( sizeof( GUID ) == sizeof( oexGUID ) );

oexSTATIC_ASSERT( sizeof( CSys::t_WAITABLE ) == sizeof( HANDLE ) );

const oexUINT		CSys::c_StrErr_OK = S_OK;

// Disable microsoft security warnings
// Wouldn't have to do this but StringCchVPrintfW() has issues.
#pragma warning( disable : 4995 4996 )

#ifdef __INTEL_COMPILER
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

oexINT CSys::ShowMessageBox( oexCSTR x_pTitle, oexCSTR x_pStr )
{	if ( !oexCHECK_PTR( x_pTitle ) )
		x_pTitle = oexT( "" );
 	if ( !oexCHECK_PTR( x_pStr ) )
		x_pStr = oexT( "" );
	return ::MessageBox( NULL, x_pStr, x_pTitle, MB_OK );
}

oexINT CSys::Quit( oexINT x_nReturnCode )
{	return PostThreadMessage( 0, WM_QUIT, x_nReturnCode, 0L );
}

void CSys::Exit( oexINT x_nRet )
{
	exit( x_nRet );
}

oexINT CSys::Flush_stdout()
{	return fflush( stdout );
}


// **** Multi-byte

oexCSTR8 CSys::StrFmt( oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	oexCSTR8 pStr = vStrFmt( x_pDst, x_uMax, x_pFmt, ap );
	oexVaEnd( ap );
	return pStr;
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTR8 CSys::vStrFmt( oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, oexVaList x_pArgs )
{
	return vStrFmt( oexNULL, x_pDst, x_uMax, x_pFmt, x_pArgs );
}

oexCSTR8 CSys::StrFmt( oexRESULT *x_pRes, oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	oexCSTR8 pStr = vStrFmt( x_pRes, x_pDst, x_uMax, x_pFmt, ap );
	oexVaEnd( ap );
	return pStr;
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTR8 CSys::vStrFmt( oexRESULT *x_pRes, oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, oexVaList x_pArgs )
{
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
	if ( 0 > nRet || x_uMax < nRet )
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
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
    return STRTOLL( x_pStr, NULL, x_uRadix );
}

oexUINT64 CSys::StrToUInt64( oexCSTR8 x_pStr, oexUINT x_uRadix )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
    return STRTOLL( x_pStr, NULL, x_uRadix );
}

oexLONG CSys::StrToLong( oexCSTR8 x_pStr, oexUINT x_uRadix )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return strtol( x_pStr, NULL, x_uRadix );
}

oexULONG CSys::StrToULong( oexCSTR8 x_pStr, oexUINT x_uRadix )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return strtoul( x_pStr, NULL, x_uRadix );
}

oexFLOAT CSys::StrToFloat( oexCSTR8 x_pStr )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return (oexFLOAT)strtod( x_pStr, NULL );
}

oexDOUBLE CSys::StrToDouble( oexCSTR8 x_pStr )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return strtod( x_pStr, NULL );
}

// **** Unicode

oexCSTRW CSys::StrFmt( oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	oexCSTRW pStr = vStrFmt( x_pDst, x_uMax, x_pFmt, ap );
	oexVaEnd( ap );
	return pStr;
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTRW CSys::vStrFmt( oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, oexVaList x_pArgs )
{
	return vStrFmt( oexNULL, x_pDst, x_uMax, x_pFmt, x_pArgs );
}

oexCSTRW CSys::StrFmt( oexRESULT *x_pRes, oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	oexCSTRW pStr = vStrFmt( x_pRes, x_pDst, x_uMax, x_pFmt, ap );
	oexVaEnd( ap );
	return pStr;
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTRW CSys::vStrFmt( oexRESULT *x_pRes, oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, oexVaList x_pArgs )
{
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
	if ( 0 > nRet || x_uMax < nRet )
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
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return WCSTOLL( x_pStr, NULL, x_uRadix );
}

oexUINT64 CSys::StrToUInt64( oexCSTRW x_pStr, oexUINT x_uRadix )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return WCSTOLL( x_pStr, NULL, x_uRadix );
}

oexLONG CSys::StrToLong( oexCSTRW x_pStr, oexUINT x_uRadix )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return wcstol( x_pStr, NULL, x_uRadix );
}

oexULONG CSys::StrToULong( oexCSTRW x_pStr, oexUINT x_uRadix )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return wcstoul( x_pStr, NULL, x_uRadix );
}

oexFLOAT CSys::StrToFloat( oexCSTRW x_pStr )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return (oexFLOAT)wcstod( x_pStr, NULL );
}

oexDOUBLE CSys::StrToDouble( oexCSTRW x_pStr )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return wcstod( x_pStr, NULL );
}

/// vprintf
int CSys::vPrintf( oexCSTRW x_pFmt, oexVaList pArgs )
{
	return ::vwprintf( x_pFmt, (va_list)pArgs );
}

/// printf function
int CSys::Printf( oexCSTRW x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	int ret = ::vwprintf( x_pFmt, (va_list)ap );
	oexVaEnd( ap );
	return ret;
}

int CSys::Echo( oexCSTRW x_pFmt )
{	return ::puts( oexStrWToMbPtr( x_pFmt ) );
}

oexPVOID CSys::MemCpy( oexPVOID x_pDst, oexCPVOID x_pSrc, oexUINT x_uSize )
{
	return ::memcpy( x_pDst, x_pSrc, x_uSize );
}

oexINT CSys::MemCmp( oexCPVOID x_pDst, oexCPVOID x_pSrc, oexUINT x_uSize )
{
	return ::memcmp( x_pDst, x_pSrc, x_uSize );
}

oexPVOID CSys::MemSet( oexPVOID x_pDst, oexINT x_nCh, oexUINT x_uSize )
{
	return ::memset( x_pDst, x_nCh, x_uSize );
}

oexGUID * CSys::CreateGuid( oexGUID *pGuid )
{
    CoCreateGuid( (GUID*)pGuid );
    return pGuid;
}


void CSys::Sleep( oexUINT uMilliseconds, oexUINT uSeconds )
{   ::Sleep( uMilliseconds + ( uSeconds * 1000 ) );
}

static timeval  g_tv_microsleep_socket;
static fd_set   g_fd_set_microsleep_socket;
static oexINT   g_microsleep_socket_init = -1;
static SOCKET   g_microsleep_socket = INVALID_SOCKET;

static oexBOOL CSys_ReleaseMicroSleep()
{
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
{
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
{
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
{
#if defined( OEX_WINCE )
	CoInitializeEx( NULL, COINIT_MULTITHREADED );
#else
	CoInitialize( NULL );
#endif

    return oexTRUE;
}

oexBOOL CSys::Uninit()
{
	CoUninitialize();

    return CSys_ReleaseMicroSleep();
}

void CSys::CloseHandle( t_WAITABLE x_pHandle )
{	if ( x_pHandle ) CloseHandle( (HANDLE)x_pHandle );
}

CSys::t_WAITABLE CSys::DuplicateHandle( t_WAITABLE x_pHandle )
{
    HANDLE hDup = NULL;
    if ( !::DuplicateHandle( GetCurrentProcess(), (HANDLE)x_pHandle,
                             GetCurrentProcess(), &hDup,
                             0, FALSE, DUPLICATE_SAME_ACCESS ) )
        return NULL;
    return (t_WAITABLE)hDup;
}

oexINT CSys::WaitForSingleObject( CSys::t_WAITABLE x_pHandle, oexUINT x_uTimeout )
{	DWORD dwRet = ::WaitForSingleObject( (HANDLE)x_pHandle, (DWORD)x_uTimeout );
	if ( WAIT_OBJECT_0 == dwRet )
        return waitSuccess;
	else if ( WAIT_TIMEOUT == dwRet )
        return waitTimeout;
	return waitFailed;
}

oexINT CSys::WaitForMultipleObjects( oexUINT x_uObjects, CSys::t_WAITABLE *x_pHandle, oexBOOL x_bWaitAll, oexUINT x_uTimeout )
{
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

oexLONG CSys::InterlockedIncrement( oexLONG *x_puVal )
{	return ::InterlockedIncrement( x_puVal );
}

oexLONG CSys::InterlockedDecrement( oexLONG *x_puVal )
{	return ::InterlockedDecrement( x_puVal );
}

void CSys_SystemTimeToSTime( SYSTEMTIME &st, CSys::STime &t )
{
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
{
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
{
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

oexINT CSys::GetLocalTzBias()
{
    TIME_ZONE_INFORMATION tz;
    CSys::Zero( &tz, sizeof( tz ) );

    ::GetTimeZoneInformation( &tz );
    return tz.Bias;
}


oexBOOL CSys::GetSystemTime( STime &t )
{
    CSys::Zero( &t, sizeof( t ) );

    SYSTEMTIME st;
    CSys::Zero( &st, sizeof( st ) );

    ::GetSystemTime( &st );
    CSys_SystemTimeToSTime( st, t );

    return oexTRUE;
}

/// Converts an STime structure to file time
oexINT64 CSys::SystemTimeToFileTime( STime &x_st )
{
    SYSTEMTIME st;
    CSys_STimeToSystemTime( x_st, st );

    FILETIME ft;
    ::SystemTimeToFileTime( &st, &ft );

    return (oexINT64)ft.dwLowDateTime | ( (oexINT64)ft.dwHighDateTime << 32 );
}

/// Converts a file time to an STime structure
void CSys::FileTimeToSystemTime( STime &x_st, oexINT64 x_ft )
{
    FILETIME ft;
    ft.dwLowDateTime = (oexINT32)( x_ft & 0xffffffff );
    ft.dwHighDateTime = (oexINT32)( ( x_ft >> 32 ) & 0xffffffff );

    SYSTEMTIME st;
    ::FileTimeToSystemTime( &ft, &st );

    CSys_SystemTimeToSTime( st, x_st );
}

oexUINT CSys::GetUnixTime()
{
	return time( NULL );
}


oexUINT CSys::WcsToMbs( oexSTR8 pDst, oexUINT uMax, oexCSTRW pSrc, oexUINT uLen )
{
	if ( !pSrc || !uLen )
		return 0;

	// +++ Ensure source is NULL terminated
//	pSrc[ uLen - 1 ] = 0;

	return wcstombs( pDst, pSrc, uMax );
}

oexUINT CSys::MbsToWcs( oexSTRW pDst, oexUINT uMax, oexCSTR8 pSrc, oexUINT uLen )
{
	if ( !pSrc || !uLen )
		return 0;

	// +++ Ensure source is NULL terminated
//	pSrc[ uLen - 1 ] = 0;

	return mbstowcs( pDst, pSrc, uMax );
}

oexCSTR CSys::SetLocale( oexINT nCategory, oexCSTR pLocal )
{
//  +++ Resolve temporary string problem
//	return setlocale( nCategory, oexStrToMbPtr( pLocal ) );
	return oexT( "" );
}

/// printf function
int CSys::Printf( oexCSTR8 x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	int ret = vprintf( x_pFmt, (va_list)ap );
	oexVaEnd( ap );
	return ret;
}

/// vprintf
int CSys::vPrintf( oexCSTR8 x_pFmt, oexVaList pArgs )
{	return ::vprintf( x_pFmt, (va_list)pArgs );
}

int CSys::Echo( oexCSTR8 x_pFmt )
{	return ::puts( x_pFmt );
}

oexUINT CSys::GetCurThreadId()
{	return GetCurrentThreadId();
}

oexBOOL CSys::PumpThreadMessages()
{
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
{
	// Hmmmmm
	oexERROR( 0, oexT( "Cant fork() on Windows" ) );

	return -1;
}

oexBOOL CSys::Shell( oexCSTR x_pFile, oexCSTR x_pParams, oexCSTR x_pDirectory )
{
	if ( !oexCHECK_PTR( x_pFile ) )
		return oexFALSE;

	return ( 32 < (unsigned int)ShellExecute( NULL, oexT( "open" ), 
											  x_pFile, x_pParams, 
											  x_pDirectory, SW_SHOWNORMAL ) )
		   ? oexTRUE : oexFALSE;
}

