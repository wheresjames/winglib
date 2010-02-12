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

//	printf( oexT( "%s : %s" ), x_pTitle, x_pStr );

	oexINT nRet = system( oexStrToMbPtr( oexMks( oexT( "xmessage -center \"" ), x_pTitle, oexT( " : " ), x_pStr, oexT( "\"" ) ).Ptr() ) );
	if ( -1 == nRet )
	{	oexERROR( errno, oexT( "Failed to dislay dialog box" ) );
		oexNOTICE( 0, oexMks( x_pTitle, oexT( " : " ), x_pStr ) );
		return -1;
	} // end if

	// +++ how's this work???
	if ( WIFSIGNALED( nRet ) && ( WTERMSIG( nRet ) == SIGINT || WTERMSIG( nRet ) == SIGQUIT ) )
		return 0;

	return -2;
}

oexINT CSys::Quit( oexINT x_nReturnCode )
{	return -1;
}

oexINT CSys::Flush_stdout()
{	return fflush( stdout );
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

	// Create format string
	if ( 0 > vsnprintf( x_pDst, x_uMax, x_pFmt, x_pArgs ) )
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

oexFLOAT CSys::StrToFloat( oexCSTR8 x_pStr )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return strtof( x_pStr, NULL );
}

oexDOUBLE CSys::StrToDouble( oexCSTR8 x_pStr )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return strtod( x_pStr, NULL );
}

// **** Unicode
#if !defined( OEX_NOWCHAR )

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

	// Create format string
	if ( 0 > vswprintf( x_pDst, x_uMax, x_pFmt, x_pArgs ) )
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

oexFLOAT CSys::StrToFloat( oexCSTRW x_pStr )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return wcstof( x_pStr, NULL );
}

oexDOUBLE CSys::StrToDouble( oexCSTRW x_pStr )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return wcstod( x_pStr, NULL );
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

/// vprintf
int CSys::vPrintf( oexCSTRW x_pFmt, oexVaList pArgs )
{	return ::vwprintf( x_pFmt, pArgs );
}

/// printf function
int CSys::Printf( oexCSTRW x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	int ret = vwprintf( x_pFmt, ap );
	oexVaEnd( ap );
	return ret;
}

int CSys::Echo( oexCSTRW x_pFmt )
{	return ::puts( oexStrWToMbPtr( x_pFmt ) );
}

#endif // OEX_NOWCHAR

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

static oexINT64 g_int = 0;
static oexGUID g_random = IID_ZEROS;
struct SRandomSeeds
{	oexPVOID		pFunction;
	oexPVOID		pHeap;
	oexINT64		nCount;
};

// +++ Probably be nice to have a bit stronger randomizer here.
oexGUID * CSys::CreateGuid( oexGUID *pGuid )
{
	if ( !oexCHECK_PTR( pGuid ) )
		return oexNULL;

	oexGUID guid = IID_ZEROS;
	oss::CMd5::Transform( &guid, &g_random, sizeof( g_random ) );

	// Use time to randomize guid
	CSysTime st( CSysTime::eGmtTime );
	oss::CMd5::Transform( &guid, &st, sizeof( st ) );

	// Use other random sources
	SRandomSeeds rs =
	{	(oexPVOID)&CreateGuid,
		CMem::GetRawAllocator().fMalloc( 4 ),
		g_int++
	};
	oss::CMd5::Transform( &guid, &rs, sizeof( rs ) );
	CMem::GetRawAllocator().fFree( rs.pHeap );

	// Add existing random data
	guid::CopyGuid( pGuid, &guid );
	guid::XorGuid( pGuid, &g_random );
	guid::XorGuid( &g_random, &guid );

    return pGuid;
}

void CSys::Sleep( oexUINT uMilliseconds, oexUINT uSeconds )
{
/*
	if ( uMilliseconds > 1000 )
	{	uSeconds += uMilliseconds / 1000;
		uMilliseconds %= 1000;
	} // end if

	if ( uSeconds )
		::sleep( uSeconds );

	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = uMilliseconds * 1000000;
	nanosleep( &ts, 0 );
*/

	if ( uSeconds )
		::sleep( uSeconds );

	if ( !uSeconds || uMilliseconds )
		::usleep( uMilliseconds * 1000 );

}

oexBOOL CSys::MicroSleep( oexUINT uMicroseconds, oexUINT uSeconds )
{
/*
	if ( uMicroseconds > 1000000 )
	{	uSeconds += uMicroseconds / 1000000;
		uMicroseconds %= 1000000;
	} // end if

	if ( uSeconds )
		::sleep( uSeconds );

	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = uMicroseconds * 1000;
	return nanosleep( &ts, 0 );
*/

	if ( uSeconds )
		::sleep( uSeconds );

	if ( !uSeconds || uMicroseconds )
		::usleep( uMicroseconds );

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

	return 0;
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

oexLONG CSys::InterlockedIncrement( oexLONG *x_puVal )
{
	// +++ Need this functionality
	//return ::InterlockedIncrement( x_puVal );
	return ++(*x_puVal);
}

oexLONG CSys::InterlockedDecrement( oexLONG *x_puVal )
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
    t.nTzBias = 0;
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

oexBOOL CSys::SetLocalTime( STime &t )
{
	struct tm tinfo;
	CSys_STimeToSystemTime( t, &tinfo );

	time_t new_time = timelocal( &tinfo );
	return 0 == stime( &new_time );

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

    return oexTRUE;
}

oexBOOL CSys::SetSystemTime( STime &t )
{
	struct tm tinfo;
	CSys_STimeToSystemTime( t, &tinfo );

	time_t new_time = timegm( &tinfo );
	return 0 == stime( &new_time );

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
//	return ( (oexINT64)mktime( &tinfo ) + FTOFF_1970 ) * 10000000LL
	return ( (oexINT64)timegm( &tinfo ) + FTOFF_1970 ) * 10000000LL
	       + ( 10000LL * (oexINT64)x_st.uMillisecond )
	       + ( 10LL * (oexINT64)x_st.uMicrosecond )
	       + ( (oexINT64)x_st.uNanosecond / 100LL )
	       ;
}

/// Converts a file time to an STime structure
void CSys::FileTimeToSystemTime( STime &x_st, oexINT64 x_ft )
{
	x_st.uNanosecond = ( x_ft * 100LL ) % 1000LL;
	x_st.uMicrosecond = ( x_ft / 10000LL ) % 1000LL;
	x_st.uMillisecond = ( x_ft / 10000000LL ) % 1000LL;

	time_t tTime = ( x_ft / 10000000LL ) - FTOFF_1970;
	struct tm tinfo;
	gmtime_r( &tTime, &tinfo );

	CSys_SystemTimeToSTime( &tinfo, x_st );
}


oexCSTR CSys::SetLocale( oexINT nCategory, oexCSTR pLocal )
{
//  +++ Resolve temporary string problem
//	return setlocale( nCategory, oexStrToMbPtr( pLocal ) );
	return oexT( "" );
}

oexUINT CSys::GetUnixTime()
{
	return time( NULL );
}

/// printf function
int CSys::Printf( oexCSTR8 x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	int ret = ::vprintf( x_pFmt, ap );
	oexVaEnd( ap );
	return ret;
}

/// vprintf
int CSys::vPrintf( oexCSTR8 x_pFmt, oexVaList pArgs )
{	return ::vprintf( x_pFmt, pArgs );
}

int CSys::Echo( oexCSTR8 x_pFmt )
{	CUtil::AddOutput( x_pFmt, 0 );
	return ::puts( x_pFmt );
}

oexUINT CSys::GetCurThreadId()
{	return (oexUINT)::pthread_self();
}

oexBOOL CSys::PumpThreadMessages()
{
	return oex::oexFALSE;
}


oexINT CSys::Fork( oexCSTR x_pWorkingDirectory, oexCSTR x_pLogFile )
{
	pid_t pid, sid;

	// Fork from the parent
	pid = fork();
	if ( 0 > pid )
	{	oexPrintf( "fork() failed : %d\n", errno );
		oexERROR( errno, oexT( "fork() failed" ) );
		return pid;
	} // end if

	// Exit parent process
	if ( 0 < pid )
	{	//CLog::GlobalLog().OpenLogFile( oexNULL, oexNULL, oexNULL );
		//oexPrintf( "Exit parent : %d\n", (int)pid );
		oexNOTICE( 0, CStr().Fmt( oexT( "fork() = %d (0x%x)" ), (int)pid, (int)pid ) );
		return pid;
	} // end if

	// Change file mode mask
	umask( 0 );

	// Child process needs a SID
	sid = setsid();
	if ( 0 > sid )
	{	oexPrintf( "setsid() failed : %d\n", (int)sid );
		oexERROR( errno, oexT( "setsid() failed" ) );
		return -1;
	} // end if

	// Switch to custom log file
	if ( oexCHECK_PTR( x_pLogFile ) && *x_pLogFile )
		CLog::GlobalLog().OpenLogFile( oexNULL, x_pLogFile, oexT( ".fork.debug.log" ) );
	else
		CLog::GlobalLog().OpenLogFile( oexNULL, oexNULL, oexT( ".fork.debug.log" ) );

	// Log child sid
	oexNOTICE( 0, CStr().Fmt( oexT( "Child fork() : setsid() = %d" ), (int)sid ) );

	// Set the current working directory
	CStr sWorkingDirectory;
	if ( !oexCHECK_PTR( x_pWorkingDirectory ) || !*x_pWorkingDirectory )
		x_pWorkingDirectory = ( sWorkingDirectory = oexGetModulePath() ).Ptr();

	if ( oexCHECK_PTR( x_pWorkingDirectory ) && *x_pWorkingDirectory )
		if ( 0 > chdir( oexStrToMbPtr( x_pWorkingDirectory ) ) )
		{	oexPrintf( "Failed to set working directory : %d : %s\n", errno, x_pWorkingDirectory );
			oexERROR( errno, oexMks( oexT( "chdir( '" ), x_pWorkingDirectory, oexT( "' ) failed" ) ) );
			return -1;
		} // end if

	// No more terminals
	if ( 0 > close( STDIN_FILENO ) )
		oexWARNING( errno, oexT( "Unable to close STDIN_FILENO" ) );
	if ( 0 > close( STDOUT_FILENO ) )
		oexWARNING( errno, oexT( "Unable to close STDIN_FILENO" ) );
	if ( 0 > close( STDERR_FILENO ) )
		oexWARNING( errno, oexT( "Unable to close STDIN_FILENO" ) );

	// Return from child process
	return 0;
}

oexBOOL CSys::Shell( oexCSTR x_pFile, oexCSTR x_pParams, oexCSTR x_pDirectory )
{
	if ( !oexCHECK_PTR( x_pFile ) )
		return oexFALSE;

	CStr sCmd;

	// Changing direcories?
	if ( oexCHECK_PTR( x_pDirectory ) && *x_pDirectory )
		sCmd = oexMks( oexT( "cd \"" ), x_pDirectory, oexT( "\"; " ) );

	// File name
	sCmd += x_pFile;

	// Params?
	if ( oexCHECK_PTR( x_pParams ) && *x_pParams )
		sCmd += oexMks( oexT( " " ), x_pParams );

//	oexEcho( oexMks( oexT( "system() : " ), sCmd ).Ptr() );

	return system( oexStrToMb( sCmd ).Ptr() ) ? oexTRUE : oexFALSE;
}

struct SProcStatInfo
{
	// Proc stat file
	CFile 			fProcStat;

	// Values
	CPropertyBag	pb;
};

static oexBOOL UpdateProcStat( SProcStatInfo *pPsi )
{
	// Verify processor pointer
	if ( !oexCHECK_PTR( pPsi ) )
		return oexFALSE;

	// Lose old values
	pPsi->pb.Destroy();

	// Open processor status file if needed
	if ( !pPsi->fProcStat.IsOpen() )
	{	if ( !pPsi->fProcStat.OpenExisting( oexT( "/proc/stat" ), os::CBaseFile::eAccessRead ).IsOpen() )
			return oexFALSE;
	} // end if

	// Start at the start
	pPsi->fProcStat.SetPtrPosBegin( 0 );

	// Read in info
	CStr sInfo = pPsi->fProcStat.Read( 64 * 1024 );
	if ( !sInfo.Length() )
		return oexFALSE;

	// Parse the data
	CStrList lst = CParser::Split( sInfo, oexT( "\r\n" ) );
	for ( CStrList::iterator it; lst.Next( it ); )
	{
		CStrList lstParams = CParser::Split( it.Obj(), oexT( " \t" ) );

		// Iterate through the values
		CStrList::iterator itK = lstParams.First();
		CStrList::iterator itV = itK;

		// Read in values
		oexLONG i = 0;
		if ( itK.Obj() != oexT( "intr" ) )
		{	CPropertyBag &r = pPsi->pb[ itK.Obj() ];
			while ( itV.Next() )
				r[ i++ ] = itV.Obj();
		} // end if
	}

	return oexTRUE;
}

static oexBOOL UpdateProcStatItem( SProcStatInfo *pPsi, oexCSTR pItem )
{
	// Verify processor pointer
	if ( !oexCHECK_PTR( pPsi ) || !oexCHECK_PTR( pItem ) )
		return oexFALSE;

	// Lose old values
	pPsi->pb.Destroy();

	// Open processor status file if needed
	if ( !pPsi->fProcStat.IsOpen() )
	{	if ( !pPsi->fProcStat.OpenExisting( oexT( "/proc/stat" ), os::CBaseFile::eAccessRead ).IsOpen() )
			return oexFALSE;
	} // end if

	// Start at the start
	pPsi->fProcStat.SetPtrPosBegin( 0 );

	// Read in info
	CStr sInfo = pPsi->fProcStat.Read( 64 * 1024 );
	if ( !sInfo.Length() )
		return oexFALSE;

	oexINT nItem = zstr::Length( pItem );
	while ( sInfo.Length() )
	{
		// Is this the one we're looking for?
		if ( sInfo.CmpLen( pItem, nItem ) )
		{
			// Skip tag name
			sInfo.LTrim( nItem );
			sInfo.Skip( oexT( "\t " ) );

			// Create a key for data
			CPropertyBag &r = pPsi->pb[ pItem ];

			// Read in values
			oexINT nSep, i = 0, nDone = 0;
			while ( !nDone && 0 < ( nSep = sInfo.FindChars( oexT( "\r\n\t " ) ) ) )
			{
				// Add item
				r[ i++ ] = CStr( sInfo.Ptr(), nSep );

				// Are we done?
				if ( oexT( ' ' ) != sInfo[ nSep ] && oexT( '\t' ) != sInfo[ nSep ] )
					nDone = 1;

				// Skip
				sInfo.LTrim( nSep );
				sInfo.Skip( oexT( "\t " ) );

			} // end while

			return oexTRUE;

		} // end if

		else
			sInfo.SkipPast( oexT( "\r\n" ) );

	} // end while

	return oexFALSE;
}

static oexBOOL ReleaseProcStat( SProcStatInfo *pPsi )
{
	// Verify processor pointer
	if ( !oexCHECK_PTR( pPsi ) )
		return oexFALSE;

	// Close the file
	if ( pPsi->fProcStat.IsOpen() )
		pPsi->fProcStat.Close();

	// Release propertys
	pPsi->pb.Destroy();

	return oexTRUE;
}

struct SCpuLoadInfo
{
	// Non zero if cpu values are valid
	oexBOOL			bValid;

	// Last cpu values
	oexINT64		cpu[ 16 ];

};

static SProcStatInfo 	g_psi;
static SCpuLoadInfo		g_cli = { oexFALSE };
static oexLock			g_cpu_lock;
oexDOUBLE CSys::GetCpuLoad()
{
	oexAutoLock ll( g_cpu_lock );
	if ( !ll.IsLocked() )
		return 0;

	// Get proc status
	if ( !UpdateProcStatItem( &g_psi, oexT( "cpu" ) ) )
		return 0;

	// Ensure cpu key
	if ( !g_psi.pb.IsKey( oexT( "cpu" ) ) )
		return 0;

	oexDOUBLE dCpu = 0;
	CPropertyBag &cpu = g_psi.pb[ oexT( "cpu" ) ];

	// Must have values
	if ( !cpu.Size() )
		return 0;

	if ( !g_cli.bValid )
	{
		g_cli.bValid = oexTRUE;
		for( oexUINT i = 0; i < oexSizeOfArray( g_cli.cpu ); i++ )
			g_cli.cpu[ i ] = cpu[ i ].ToInt64();

	} // end if

	else
	{
		oexINT64 total = 0;
		oexINT64 vals[ oexSizeOfArray( g_cli.cpu ) ];
		for( oexUINT i = 0; i < oexSizeOfArray( g_cli.cpu ) && i < cpu.Size(); i++ )
		{
			oexINT64 v = cpu[ i ].ToInt64();
			vals[ i ] = v - g_cli.cpu[ i ];
			g_cli.cpu[ i ] = v;

			total += vals[ i ];

		} // end for

		if ( total )
			dCpu = (oexDOUBLE)( total - vals[ 3 ] ) * 100. / (oexDOUBLE)total;

	} // end else

	return dCpu;
}


oexDOUBLE CSys::GetCpuLoad( oexCSTR x_pProcessName )
{
	// +++ Get process cpu load from linux?
	return 0;
}

oexBOOL CSys::Init()
{
    return oexTRUE;
}

oexBOOL CSys::Uninit()
{
	oexAutoLock ll( g_cpu_lock );
	if ( !ll.IsLocked() )
		return 0;

	// Release proc stat info
	ReleaseProcStat( &g_psi );

	return oexTRUE;
}

