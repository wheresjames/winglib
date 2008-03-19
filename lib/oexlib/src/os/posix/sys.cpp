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

static SResInfo* Create( oexCSTR x_pName )
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

static void Release( SResInfo *x_pRi )	
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

void CSys::Exit( oexINT x_nRet )
{
	exit( x_nRet );
}


// **** Multi-byte

oexCSTR8 CSys::StrFmt( oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, ... )
{
	return vStrFmt( x_pDst, x_uMax, x_pFmt, ( ( (oexPVOID*)&x_pFmt ) + 1 ) );
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTR8 CSys::vStrFmt( oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, oexCPVOID x_pArgs )
{
	return vStrFmt( oexNULL, x_pDst, x_uMax, x_pFmt, x_pArgs );
}

oexCSTR8 CSys::StrFmt( oexRESULT *x_pRes, oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, ... )
{
	return vStrFmt( x_pRes, x_pDst, x_uMax, x_pFmt, ( ( (oexPVOID*)&x_pFmt ) + 1 ) );
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTR8 CSys::vStrFmt( oexRESULT *x_pRes, oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, oexCPVOID x_pArgs )
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
//    return _strtoi64( x_pStr, NULL, x_uRadix ); 
	return 0;
}

oexUINT64 CSys::StrToUInt64( oexCSTR8 x_pStr, oexUINT x_uRadix ) 
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
//    return _strtoui64( x_pStr, NULL, x_uRadix ); 
	return 0;
}

oexLONG CSys::StrToLong( oexCSTR8 x_pStr, oexUINT x_uRadix ) 
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
//	return strtol( x_pStr, NULL, x_uRadix ); 
	return 0;
}

oexULONG CSys::StrToULong( oexCSTR8 x_pStr, oexUINT x_uRadix ) 
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
//	return strtoul( x_pStr, NULL, x_uRadix ); 
	return 0;
}

oexDOUBLE CSys::StrToDouble( oexCSTR8 x_pStr ) 
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
//	return strtod( x_pStr, NULL ); 
	return 0;
}

// **** Unicode

oexCSTRW CSys::StrFmt( oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, ... )
{
	return vStrFmt( x_pDst, x_uMax, x_pFmt, ( ( (oexPVOID*)&x_pFmt ) + 1 ) );
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTRW CSys::vStrFmt( oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, oexCPVOID x_pArgs )
{
	return vStrFmt( oexNULL, x_pDst, x_uMax, x_pFmt, x_pArgs );
}

oexCSTRW CSys::StrFmt( oexRESULT *x_pRes, oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, ... )
{
	return vStrFmt( x_pRes, x_pDst, x_uMax, x_pFmt, ( ( (oexPVOID*)&x_pFmt ) + 1 ) );
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTRW CSys::vStrFmt( oexRESULT *x_pRes, oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, oexCPVOID x_pArgs )
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
//	if ( 0 > vsnwprintf( x_pDst, x_uMax, x_pFmt, (va_list)x_pArgs ) )
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
//	return _wcstoi64( x_pStr, NULL, x_uRadix ); 
	return 0;
}

oexUINT64 CSys::StrToUInt64( oexCSTRW x_pStr, oexUINT x_uRadix ) 
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
//	return _wcstoui64( x_pStr, NULL, x_uRadix ); 
	return 0;
}

oexLONG CSys::StrToLong( oexCSTRW x_pStr, oexUINT x_uRadix ) 
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
//	return wcstol( x_pStr, NULL, x_uRadix ); 
	return 0;
}

oexULONG CSys::StrToULong( oexCSTRW x_pStr, oexUINT x_uRadix ) 
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
//	return wcstoul( x_pStr, NULL, x_uRadix ); 
	return 0;
}

oexDOUBLE CSys::StrToDouble( oexCSTRW x_pStr ) 
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
//	return wcstod( x_pStr, NULL ); 
	return 0;
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
//    CoCreateGuid( (GUID*)pGuid );
//	uuid_generate( (uuid_t*)pGuid );
    return pGuid;
}


void CSys::Sleep( oexUINT uMilliseconds, oexUINT uSeconds )
{   ::sleep( uMilliseconds + ( uSeconds * 1000 ) );
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
	//return ::InterlockedIncrement( x_puVal );
	return (*x_puVal)++;
}

oexULONG CSys::InterlockedDecrement( oexLONG *x_puVal )
{
//	return ::InterlockedDecrement( x_puVal );
	return (*x_puVal)--;
}
/*
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
*/
oexBOOL CSys::GetLocalTime( STime &t )
{
/*    CSys::Zero( &t, sizeof( t ) );

    SYSTEMTIME st;
    CSys::Zero( &st, sizeof( st ) );

    ::GetLocalTime( &st ); 
    CSys_SystemTimeToSTime( st, t );

    TIME_ZONE_INFORMATION tz;
    CSys::Zero( &tz, sizeof( tz ) );

    ::GetTimeZoneInformation( &tz ); 
    t.nTzBias = tz.Bias;
*/
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
/*
    CSys::Zero( &t, sizeof( t ) );

    SYSTEMTIME st;
    CSys::Zero( &st, sizeof( st ) );

    ::GetSystemTime( &st ); 
    CSys_SystemTimeToSTime( st, t );
*/
    return oexTRUE;
}

/// Converts an STime structure to file time
oexINT64 CSys::SystemTimeToFileTime( STime &x_st )
{
/*
    SYSTEMTIME st;
    CSys_STimeToSystemTime( x_st, st );

    FILETIME ft;
    ::SystemTimeToFileTime( &st, &ft );

    return (oexINT64)ft.dwLowDateTime | ( (oexINT64)ft.dwHighDateTime << 32 );
*/
	return 0;
}

/// Converts a file time to an STime structure
void CSys::FileTimeToSystemTime( STime &x_st, oexINT64 x_ft )
{
/*
    FILETIME ft;
    ft.dwLowDateTime = (oexINT32)( x_ft & 0xffffffff );
    ft.dwHighDateTime = (oexINT32)( ( x_ft >> 32 ) & 0xffffffff );

    SYSTEMTIME st;
    ::FileTimeToSystemTime( &ft, &st );

    CSys_SystemTimeToSTime( st, x_st );
*/
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

oexCSTR SetLocale( oexINT nCategory, oexCSTR pLocal )
{
//  +++ Resolve temporary string problem
//	return setlocale( nCategory, oexStrToMbPtr( pLocal ) );
	return oexT( "" );
}


