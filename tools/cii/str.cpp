// str.cpp

#include <stdio.h>
#include <stdlib.h>

#if defined( _WIN32_WCE ) || defined( __MINGW32__ )
#	include <wchar.h>
#	define VSNPRINTF		vsnprintf
#	define STRTOLL			strtoll
#	define VSNWPRINTF		vsnwprintf
#	define WCSTOLL			wcstoll
#	define CAST_VL			tcVaList
#else
#	include <tchar.h>
#	define VSNPRINTF		_vsnprintf
#	define STRTOLL			_strtoi64
#	define VSNWPRINTF		_vsnwprintf
#	define WCSTOLL			_wcstoi64
#	define CAST_VL			va_list
#endif

#include "str.h"

namespace str
{

long vPrint( const char *x_pFmt, tcVaList x_pArgs )
{
	return vprintf( x_pFmt, (CAST_VL)x_pArgs );
}

long Print( const char *x_pFmt, ... )
{
	// Sanity check
	if ( !x_pFmt)
		return -1;

	// Call vargs version
	tcVaList ap; tcVaStart( ap, x_pFmt );
	long lRet = vPrint( x_pFmt, ap );
	tcVaEnd( ap );

	return lRet;
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
long vStrFmt( char *x_pDst, unsigned long x_uMax, const char *x_pFmt, tcVaList x_pArgs )
{
	// Verify data pointers
	if ( !x_pDst || !x_pFmt || !x_uMax )
		return -1;

	// Create format string
	long nRet = (long)VSNPRINTF( x_pDst, x_uMax, x_pFmt, (CAST_VL)x_pArgs );
	if ( 0 > nRet || x_uMax < (unsigned long)nRet )
	{
		// Null terminate buffer
		x_pDst[ x_uMax - 1 ] = 0;

		// Let the user know what went wrong
		return -1;

	} // end if

	// Null terminate
	x_pDst[ nRet ] = 0;

	return nRet;
}

long StrFmt( char *x_pDst, unsigned long x_uMax, const char *x_pFmt, ... )
{
	// Sanity check
	if ( !x_pDst || !x_pFmt || !x_uMax )
		return -1;

	// Call vargs version
	tcVaList ap; tcVaStart( ap, x_pFmt );
	long lRet = vStrFmt( x_pDst, x_uMax, x_pFmt, ap );
	tcVaEnd( ap );

	return lRet;
}

tc_int64 StrToInt64( const char *x_pStr, long x_lRadix )
{
	if ( !x_pStr )
		return 0;
    return STRTOLL( x_pStr, NULL, x_lRadix );
}

tc_uint64 StrToUInt64( const char *x_pStr, long x_lRadix )
{
	if ( !x_pStr )
		return 0;
    return STRTOLL( x_pStr, NULL, x_lRadix );
}

long StrToLong( const char *x_pStr, long x_lRadix )
{
	if ( !x_pStr )
		return 0;
	return strtol( x_pStr, NULL, x_lRadix );
}

unsigned long StrToULong( const char *x_pStr, long x_lRadix )
{
	if ( !x_pStr )
		return 0;
	return strtoul( x_pStr, NULL, x_lRadix );
}

float StrToFloat( const char *x_pStr )
{
	if ( !x_pStr )
		return 0;
	return (float)strtod( x_pStr, NULL );
}

double StrToDouble( const char *x_pStr )
{
	if ( !x_pStr )
		return 0;
	return strtod( x_pStr, NULL );
}

#ifndef CII_NO_WCHAR

long vPrint( const wchar_t *x_pFmt, tcVaList x_pArgs )
{
	return vwprintf( x_pFmt, (CAST_VL)x_pArgs );
}

long Print( const wchar_t *x_pFmt, ... )
{
	// Sanity check
	if ( !x_pFmt)
		return -1;

	// Call vargs version
	tcVaList ap; tcVaStart( ap, x_pFmt );
	long lRet = vPrint( x_pFmt, ap );
	tcVaEnd( ap );

	return lRet;
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
long vStrFmt( wchar_t *x_pDst, unsigned long x_uMax, const wchar_t *x_pFmt, tcVaList x_pArgs )
{
	// Verify data pointers
	if ( !x_pDst || !x_pFmt || !x_uMax )
		return -1;

	// Create format string
	long nRet = (long)VSNWPRINTF( x_pDst, x_uMax, x_pFmt, (CAST_VL)x_pArgs );
	if ( 0 > nRet || x_uMax < (unsigned long)nRet )
	{
		// Null terminate buffer
		x_pDst[ x_uMax - 1 ] = 0;

		// Let the user know what went wrong
		return -1;

	} // end if

	// Null terminate
	x_pDst[ nRet ] = 0;

	return nRet;
}

long StrFmt( wchar_t *x_pDst, unsigned long x_uMax, const wchar_t *x_pFmt, ... )
{
	// Sanity check
	if ( !x_pDst || !x_pFmt || !x_uMax )
		return -1;

	// Call vargs version
	tcVaList ap; tcVaStart( ap, x_pFmt );
	long lRet = vStrFmt( x_pDst, x_uMax, x_pFmt, ap );
	tcVaEnd( ap );

	return lRet;
}

/// Converts to int64
tc_int64 StrToInt64( const wchar_t *x_pStr, long x_lRadix )
{
	if ( !x_pStr )
		return 0;
	return WCSTOLL( x_pStr, NULL, x_lRadix );
}

/// Converts to uint64
tc_uint64 StrToUInt64( const wchar_t *x_pStr, long x_lRadix )
{
	if ( !x_pStr )
		return 0;
	return WCSTOLL( x_pStr, NULL, x_lRadix );
}

/// Converts to long
long StrToLong( const wchar_t *x_pStr, long x_lRadix )
{
	if ( !x_pStr )
		return 0;
	return wcstol( x_pStr, NULL, x_lRadix );
}

/// Converts to unsigned long
unsigned long StrToULong( const wchar_t *x_pStr, long x_lRadix )
{
	if ( !x_pStr )
		return 0;
	return wcstoul( x_pStr, NULL, x_lRadix );
}

/// Converts to double
float StrToFloat( const wchar_t *x_pStr )
{
	if ( !x_pStr )
		return 0;
	return (float)wcstod( x_pStr, NULL );
}

/// Converts to double
double StrToDouble( const wchar_t *x_pStr )
{
	if ( !x_pStr )
		return 0;
	return wcstod( x_pStr, NULL );
}

#endif

}; // namespace str