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

/*
   oOoOo
,==|||||
|| |||||
|| |||||
'==#####

*/

#include "../../../oexlib.h"


#include "std_os.h"
/*
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
*/
OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

//oexSTATIC_ASSERT( oexSTRSIZE >= MAX_PATH );

//oexSTATIC_ASSERT( sizeof( uuid_t ) == sizeof( oexGUID ) );

//oexSTATIC_ASSERT( sizeof( CSys::t_WAITABLE ) == sizeof( HANDLE ) );

const oexUINT		CSys::c_StrErr_OK = 0;
const oexUINT		CSys::c_StrErr_INVALID_PARAMETER = (oexUINT)-1;
const oexUINT		CSys::c_StrErr_INSUFFICIENT_BUFFER = (oexUINT)-2;

//const oexUINT	    CSys::c_Infinite = INFINITE;
//const oexUINT		CSys::c_uMaximumWaitObjects = MAXIMUM_WAIT_OBJECTS;

//oexSTATIC_ASSERT( CSys::eMaximumWaitObjects == MAXIMUM_WAIT_OBJECTS );

SResInfo* SResInfo::Create( oexCSTR x_pName )
{
	SResInfo *pRi = new SResInfo;
	if ( !oexCHECK( pRi ) )
		return oexNULL;

	// Initialize event structure
	oexZeroMemory( pRi, sizeof( SResInfo ) );

	if ( x_pName && *x_pName )
	{
		// Save name
		pRi->pName = new CStr( x_pName );
		if ( !oexCHECK( pRi->pName ) )
		{	SResInfo::Release( pRi );
			return oexNULL;
		} // end if

	} // end if

	return pRi;
}

void SResInfo::Release( SResInfo *x_pRi )
{
	if ( !x_pRi )
		return;

	oexVERIFY_PTR( x_pRi );

	// Lose any name object
	if ( x_pRi->pName )
	{	oexVERIFY_PTR( x_pRi->pName );
		delete (CStr*)x_pRi->pName;
	} // end if

	delete x_pRi;
}

oexINT CSys::ShowMessageBox( oexCSTR x_pTitle, oexCSTR x_pStr )
{
	if ( !oexCHECK_PTR( x_pTitle ) )
		x_pTitle = oexT( "" );
 	if ( !oexCHECK_PTR( x_pStr ) )
		x_pStr = oexT( "" );
		
	printf( oexT( "%s : %s" ), x_pTitle, x_pStr );
		
	return -1;
}

oexINT CSys::Quit( oexINT x_nReturnCode )
{	return -1;
}

void CSys::Exit( oexINT x_nRet )
{
	exit( x_nRet );
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
oexCSTR8 CSys::vStrFmt( oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, oexVaList &x_pArgs )
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
oexCSTR8 CSys::vStrFmt( oexRESULT *x_pRes, oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, oexVaList &x_pArgs )
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

	// Create format string
	if ( 0 > vsnprintf( x_pDst, x_uMax, x_pFmt, (va_list)x_pArgs ) )
	{
		// Null terminate buffer
		x_pDst[ x_uMax - 1 ] = 0;

		// Let the user know what went wrong
		res = c_StrErr_INSUFFICIENT_BUFFER;

	} // end if

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
    return strtoll( x_pStr, NULL, x_uRadix );
}

oexUINT64 CSys::StrToUInt64( oexCSTR8 x_pStr, oexUINT x_uRadix )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
    return strtoll( x_pStr, NULL, x_uRadix );
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
oexCSTRW CSys::vStrFmt( oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, oexVaList &x_pArgs )
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
oexCSTRW CSys::vStrFmt( oexRESULT *x_pRes, oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, oexVaList &x_pArgs )
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

	// Create format string
	if ( 0 > vswprintf( x_pDst, x_uMax, x_pFmt, (va_list)x_pArgs ) )
	{
		// Null terminate buffer
		x_pDst[ x_uMax - 1 ] = 0;

		// Let the user know what went wrong
		res = c_StrErr_INSUFFICIENT_BUFFER;

	} // end if

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
	return wcstoll( x_pStr, NULL, x_uRadix );
}

oexUINT64 CSys::StrToUInt64( oexCSTRW x_pStr, oexUINT x_uRadix )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return wcstoll( x_pStr, NULL, x_uRadix );
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

oexDOUBLE CSys::StrToDouble( oexCSTRW x_pStr )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return wcstod( x_pStr, NULL );
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
// +++ Need implementation for the ARM
#if !defined( OEX_NOUUID )

//	uuid_generate( (unsigned char*)pGuid );

#else

#endif

    return pGuid;
}


void CSys::Sleep( oexUINT uMilliseconds, oexUINT uSeconds )
{
	if ( uSeconds )
		::sleep( uSeconds );

	if ( uMilliseconds )
		::usleep( uMilliseconds * 1000 );
}

oexBOOL CSys::MicroSleep( oexUINT uMicroseconds, oexUINT uSeconds )
{
	if ( uSeconds )
		CSys::Sleep( 0, uSeconds );

	if ( uMicroseconds )
		::usleep( uMicroseconds );
}

oexBOOL CSys::Init()
{
    return oexTRUE;
}

oexBOOL CSys::Uninit()
{
	return oexTRUE;
}

void CSys::CloseHandle( t_WAITABLE x_pHandle )
{
//	if ( x_pHandle )
//		CloseHandle( (HANDLE)x_pHandle );
}

CSys::t_WAITABLE CSys::DuplicateHandle( t_WAITABLE x_pHandle )
{
/*    HANDLE hDup = oexNULL;
    if ( !::DuplicateHandle( GetCurrentProcess(), (HANDLE)x_pHandle,
                             GetCurrentProcess(), &hDup,
                             0, oexFALSE, DUPLICATE_SAME_ACCESS ) )
        return oexNULL;
    return (t_WAITABLE)hDup;
*/
}

oexINT CSys::WaitForSingleObject( CSys::t_WAITABLE x_pHandle, oexUINT x_uTimeout )
{
/*
	DWORD dwRet = ::WaitForSingleObject( (HANDLE)x_pHandle, (DWORD)x_uTimeout );
	if ( WAIT_OBJECT_0 == dwRet )
        return waitSuccess;
	else if ( WAIT_TIMEOUT == dwRet )
        return waitTimeout;
*/
	return waitFailed;
}

oexINT CSys::WaitForMultipleObjects( oexUINT x_uObjects, CSys::t_WAITABLE *x_pHandle, oexBOOL x_bWaitAll, oexUINT x_uTimeout )
{
/*    // Currently 64, don't blame me, this is a Windows limitation...
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
*/
	return waitFailed;
}

oexULONG CSys::InterlockedIncrement( oexLONG *x_puVal )
{
	// +++ Need this functionality
	//return ::InterlockedIncrement( x_puVal );
	return ++(*x_puVal);
}

oexULONG CSys::InterlockedDecrement( oexLONG *x_puVal )
{
	// +++ Need this functionality
//	return ::InterlockedDecrement( x_puVal );
	return --(*x_puVal);
}

void CSys_SystemTimeToSTime( struct tm* tinfo, CSys::STime &t )
{
    t.uYear = 1900 + tinfo->tm_year;
    t.uMonth = tinfo->tm_mon + 1;
    t.uDayOfWeek = tinfo->tm_wday;
    t.uDay = tinfo->tm_mday;
    t.uHour = tinfo->tm_hour;
    t.uMinute = tinfo->tm_min;
    t.uSecond = tinfo->tm_sec;
    t.uMillisecond = 0;
    t.uMicrosecond = 0;
    t.uNanosecond = 0;
}

void CSys_STimeToSystemTime( CSys::STime &t, struct tm* tinfo )
{
    tinfo->tm_year = t.uYear - 1900;
    tinfo->tm_mon = t.uMonth ? t.uMonth - 1 : 0;
    tinfo->tm_wday = t.uDayOfWeek;
    tinfo->tm_mday = t.uDay;
    tinfo->tm_hour = t.uHour;
    tinfo->tm_min = t.uMinute;
    tinfo->tm_sec = t.uSecond;
//   = t.uMillisecond;
//   = t.uMicrosecond;
//   = t.uNanosecond;
}

oexBOOL CSys::GetLocalTime( STime &t )
{
	oexZeroMemory( &t, sizeof( t ) );

	time_t current_time;
	time( &current_time );
	struct tm tinfo;
	localtime_r( &current_time, &tinfo );

	CSys_SystemTimeToSTime( &tinfo, t );

#ifdef OEX_NANOSECONDS
	struct timespec	ts;
	clock_gettime( CLOCK_REALTIME, &ts );
	t.uMillisecond = ts.tv_nsec / 1000 / 1000;
	t.uMicrosecond = ts.tv_nsec / 1000 % 1000;
	t.uNanosecond = ts.tv_nsec % 1000;
#else
	struct timeval tp;
	gettimeofday( &tp, oexNULL );
	t.uMillisecond = tp.tv_usec / 1000;
	t.uMicrosecond = tp.tv_usec % 1000;
#endif

	// +++ Add time zone crap
//	t.nTzBias = tz.Bias;

    return oexTRUE;
}

oexINT CSys::GetLocalTzBias()
{
/*    TIME_ZONE_INFORMATION tz;
    CSys::Zero( &tz, sizeof( tz ) );

    ::GetTimeZoneInformation( &tz );
    return tz.Bias;
*/

	return 0;
}


oexBOOL CSys::GetSystemTime( STime &t )
{
	oexZeroMemory( &t, sizeof( t ) );

	time_t current_time;
	time( &current_time );
	struct tm tinfo;
	gmtime_r( &current_time, &tinfo );

	struct timeval tp;
	gettimeofday( &tp, oexNULL );

	CSys_SystemTimeToSTime( &tinfo, t );

	t.uMillisecond = tp.tv_usec / 1000;
	t.uMicrosecond = tp.tv_usec % 1000;

    return oexTRUE;
}

// Some helpful delta's
// 1970 = 1900 + 2208988800
// 1980 = 1970 + 315536400
// 1980 = 1900 + 2524525200
// 1900 = 1601 + 9435484800
// 1970 = 1601 + 11644473600
// 1980 = 1601 + 11960010000

#define FTOFF_1900		9435484800LL
#define FTOFF_1970		11644473600LL
#define FTOFF_1980		11960010000LL

/// Converts an STime structure to file time
oexINT64 CSys::SystemTimeToFileTime( STime &x_st )
{
	struct tm tinfo;
	CSys_STimeToSystemTime( x_st, &tinfo );
	return (oexINT64)mktime( &tinfo ) + FTOFF_1970;
}

/// Converts a file time to an STime structure
void CSys::FileTimeToSystemTime( STime &x_st, oexINT64 x_ft )
{
	time_t tTime = x_ft - FTOFF_1970;
	struct tm tinfo;
	gmtime_r( &tTime, &tinfo );

	CSys_SystemTimeToSTime( &tinfo, x_st );
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
int CSys::printf( oexCSTR8 x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	int ret = CSys::vprintf( x_pFmt, ap );
	oexVaEnd( ap );
	return ret;
}

/// vprintf
int CSys::vprintf( oexCSTR8 x_pFmt, oexVaList &pArgs )
{	return ::vprintf( x_pFmt, pArgs );
}



