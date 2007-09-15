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
#include <tchar.h>

#if ( _MSC_VER >= 1300 )
#	include <strsafe.h>
#else
#	include <stdio.h>
#	include <wchar.h>
#	include <stdarg.h>
#endif

#include <ObjBase.h>

#include <WinSock2.h>
#pragma comment( lib, "WS2_32.lib" )

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

oexSTATIC_ASSERT( oexSTRSIZE >= MAX_PATH );

oexSTATIC_ASSERT( sizeof( GUID ) == sizeof( oexGUID ) );

oexSTATIC_ASSERT( sizeof( CSys::t_WAITABLE ) == sizeof( HANDLE ) );

oexSTATIC_ASSERT( !( sizeof( oexGUID ) % sizeof( oexUCHAR ) ) );

const oexUINT		CSys::c_StrErr_OK = S_OK;

#if ( _MSC_VER >= 1300 )

	const oexUINT		CSys::c_StrErr_INVALID_PARAMETER = STRSAFE_E_INVALID_PARAMETER;
	const oexUINT		CSys::c_StrErr_INSUFFICIENT_BUFFER = STRSAFE_E_INSUFFICIENT_BUFFER;

#else

	const oexUINT		CSys::c_StrErr_INVALID_PARAMETER = (oexUINT)-1;
	const oexUINT		CSys::c_StrErr_INSUFFICIENT_BUFFER = (oexUINT)-2;

#endif

const oexUINT	    CSys::c_Infinite = INFINITE;


void CSys::Exit( oexINT x_nRet )
{
	exit( x_nRet );
}


oexCSTR CSys::StrFmt( oexSTR x_pDst, oexUINT x_uMax, oexCSTR x_pFmt, ... )
{
	return vStrFmt( x_pDst, x_uMax, x_pFmt, ( ( (LPVOID*)&x_pFmt ) + 1 ) );
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTR CSys::vStrFmt( oexSTR x_pDst, oexUINT x_uMax, oexCSTR x_pFmt, oexCPVOID x_pArgs )
{
	// Verify data pointers
	if ( !oexVERIFY_PTR( x_pDst ) || !oexVERIFY_PTR( x_pFmt ) )
		return x_pDst;

	vStrFmt( oexNULL, x_pDst, x_uMax, x_pFmt, x_pArgs );

//	// Create format string
//	oexASSERT( S_OK == StringCchVPrintf( pDst, uMax, pFmt, (va_list)pArgs ) );

	return x_pDst;
}

oexCSTR CSys::StrFmt( oexRESULT *x_pRes, oexSTR x_pDst, oexUINT x_uMax, oexCSTR x_pFmt, ... )
{
	return vStrFmt( x_pRes, x_pDst, x_uMax, x_pFmt, ( ( (LPVOID*)&x_pFmt ) + 1 ) );
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTR CSys::vStrFmt( oexRESULT *x_pRes, oexSTR x_pDst, oexUINT x_uMax, oexCSTR x_pFmt, oexCPVOID x_pArgs )
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

#if ( _MSC_VER >= 1300 )

	// Create format string
	res = StringCchVPrintf( x_pDst, x_uMax, x_pFmt, (va_list)x_pArgs );

#else

	// Create format string
	if ( 0 > _vsntprintf( x_pDst, x_uMax, x_pFmt, (va_list)x_pArgs ) )
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

oexLONG CSys::StrToLong( oexCSTR x_pStr, oexUINT x_uRadix ) 
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return _tcstol( x_pStr, NULL, x_uRadix ); 
}

oexULONG CSys::StrToULong( oexCSTR x_pStr, oexUINT x_uRadix ) 
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return _tcstoul( x_pStr, NULL, x_uRadix ); 
}

oexDOUBLE CSys::StrToDouble( oexCSTR x_pStr ) 
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return _tcstod( x_pStr, NULL ); 
}

oexINT64 CSys::StrToNum( oexCSTR x_pStr, oexUINT x_uSize, oexUINT x_uRadix, oexINT *x_pnEnd )
{
    oexASSERT( x_uRadix < 36 );
    if ( 0 >= x_uRadix || 36 < x_uRadix )
        x_uRadix = 10;

    // Zero size means NULL terminated
    if( !x_uSize ) 
        x_uSize = zstr::Length( x_pStr );

    oexINT i = 0;
    oexINT64 llNum = 0;
    oexBOOL bErr = oexFALSE;
    oexBOOL bNeg = oexFALSE;

    // Anything to do?
    if ( !x_uSize ) 
    {    if ( x_pnEnd )
            *x_pnEnd = 0;
        return llNum;
    } // end if

    // Check sign
    if ( x_pStr[ i ] == oexT( '+' ) )
        i++, bNeg = oexFALSE;
    else if ( x_pStr[ i ] == oexT( '-' ) )
        i++, bNeg = oexTRUE;

    // Make special exception for the 0x in front of hex numbers
    if ( 16 == x_uRadix && i + 2 <= (oexINT)x_uSize )
        if ( oexT( '0' ) == x_pStr[ i ] && 
             ( oexT( 'x' ) == x_pStr[ i + 1 ] || oexT( 'X' ) == x_pStr[ i + 1 ] ) )
            i += 2;

    // Decode the number
    while ( !bErr && (oexUINT)i < x_uSize )
    {
        oexINT ch = (oexINT)x_pStr[ i ];

        if ( oexT( '0' ) <= ch && oexT( '9' ) >= ch )
            ch -= oexT( '0' );

        else if ( oexT( 'a' ) <= ch && oexT( 'z' ) >= ch )
            ch -= oexT( 'a' ) - 10;

        else if ( oexT( 'A' ) <= ch && oexT( 'A' ) >= ch )
            ch -= oexT( 'A' ) - 10;

        else bErr = oexTRUE;
    
        // Next character
        if ( !bErr ) 
        {
            // Verify it's within the radix
            if ( ch >= (oexINT)x_uRadix )
                bErr = oexTRUE;

            else 
            {
                // Accumulate number
                llNum *= x_uRadix;
                llNum += ch;

            } // end else

        } // end if

        // Next char
        if ( !bErr )
            i++;

    } // end while

    // Is the value negative?
    if ( bNeg ) 
        llNum = -llNum;

    // Does the caller care about errors?
    if ( x_pnEnd )
        *x_pnEnd = i;

    return llNum;
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
    {   WSACleanup();
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
//    if ( !CSys_InitMicroSleep() )
//        return oexFALSE;

    return oexTRUE;
}

oexBOOL CSys::Uninit()
{
    return CSys_ReleaseMicroSleep();
}


oexINT CSys::WaitForSingleObject( CSys::t_WAITABLE x_pHandle, oexUINT x_uTimeout )
{	DWORD dwRet = ::WaitForSingleObject( (HANDLE)x_pHandle, (DWORD)x_uTimeout );
	if ( WAIT_OBJECT_0 == dwRet ) return waitSuccess;
	else if ( WAIT_TIMEOUT == dwRet ) return waitTimeout;
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
        return (etWait)( dwRet - WAIT_OBJECT_0 );

	else if ( WAIT_TIMEOUT == dwRet ) 
        return waitTimeout;

	return waitFailed;
}

oexULONG CSys::InterlockedIncrement( oexLONG *x_puVal )
{	return ::InterlockedIncrement( x_puVal );
}

oexULONG CSys::InterlockedDecrement( oexLONG *x_puVal )
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
