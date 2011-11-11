// typecnv.h

#pragma once

#ifndef stdForeach
#	define stdForeach( t, i, o ) for ( t i = o.begin(); o.end() != i; i++ )
#endif

// Anyone know a better way?
#define tcTT( c, s )		( 1 == sizeof( c ) ? ( ( c* )( s ) ) : ( ( c* )( L##s ) ) )
#define tcTC( c, s )		( 1 == sizeof( c ) ? ( ( c )( s ) ) : ( ( c )( L##s ) ) )
#define tcTTEXT( c, s )		pbTT( c, s )

#if defined( __GNUC__ )
#	define tcVaList				__builtin_va_list
#	define tcVaStart			__builtin_va_start
#	define tcVaEnd				__builtin_va_end
#	define tcVaArg				__builtin_va_arg
#elif defined( _WIN32 )
#	define tcVaList				void*
#	define tcVaStart( v, p )	( v = ( ( (void**)&p ) + 1 ) )
#	define tcVaEnd( v )
#	define tcVaArg( v, t )		( (t)( v++ ) )
#else
#	if defined( _WIN32_WCE )
#		include <windows.h>
#	endif
#	include <stdarg.h>
#	define tcVaList				va_list
#	define tcVaStart			va_start
#	define tcVaEnd				va_end
#	define tcVaArg				va_arg
#endif

namespace tcnv
{

typedef signed long long int tc_int64;
typedef unsigned long long int tc_uint64;

template< typename T, typename T_STR >
	T_STR ToString( int n ) 
    {   T szNum[ 256 ] = { 0 }; 
		return T_STR( szNum, tcnv::StrFmt( szNum, sizeof( szNum ), tcTT( T, "%d" ), n ) );
	}

template< typename T, typename T_STR >
	T_STR ToString( unsigned int n ) 
    {   T szNum[ 256 ] = { 0 }; 
		return T_STR( szNum, tcnv::StrFmt( szNum, sizeof( szNum ), tcTT( T, "%u" ), n ) );
	}

template< typename T, typename T_STR >
	T_STR ToString( long n ) 
    {   T szNum[ 256 ] = { 0 }; 
		return T_STR( szNum, tcnv::StrFmt( szNum, sizeof( szNum ), tcTT( T, "%li" ), n ) );
	}

template< typename T, typename T_STR >
	T_STR ToString( unsigned long n ) 
    {   T szNum[ 256 ] = { 0 }; 
		return T_STR( szNum, tcnv::StrFmt( szNum, sizeof( szNum ), tcTT( T, "%lu" ), n ) );
	}

template< typename T, typename T_STR >
	T_STR ToString( tc_int64 n ) 
    {   T szNum[ 256 ] = { 0 }; 
		return T_STR( szNum, tcnv::StrFmt( szNum, sizeof( szNum ), tcTT( T, "%lld" ), n ) );
	}

template< typename T, typename T_STR >
	T_STR ToString( tc_uint64 n ) 
    {   T szNum[ 256 ] = { 0 }; 
		return T_STR( szNum, tcnv::StrFmt( szNum, sizeof( szNum ), tcTT( T, "%llu" ), n ) );
	}

template< typename T, typename T_STR >
	T_STR ToString( float n ) 
    {   T szNum[ 256 ] = { 0 }; 
		return T_STR( szNum, tcnv::StrFmt( szNum, sizeof( szNum ), tcTT( T, "%f" ), (double)n ) );
	}

template< typename T, typename T_STR >
	T_STR ToString( double n ) 
    {   T szNum[ 256 ] = { 0 }; 
		return T_STR( szNum, tcnv::StrFmt( szNum, sizeof( szNum ), tcTT( T, "%f" ), n ) );
	}

/// String format
long vStrFmt( char *x_pDst, unsigned long x_uMax, const char *x_pFmt, tcVaList x_pArgs );

/// String format
long StrFmt( char *x_pDst, unsigned long x_uMax, const char *x_pFmt, ... );

/// Converts to int64
tc_int64 StrToInt64( const char *x_pStr, long x_lRadix = 10 );

/// Converts to uint64
tc_uint64 StrToUInt64( const char *x_pStr, long x_lRadix = 10 );

/// Converts to long
long StrToLong( const char *x_pStr, long x_lRadix = 10 );

/// Converts to unsigned long
unsigned long StrToULong( const char *x_pStr, long x_lRadix = 10 );

/// Converts to double
float StrToFloat( const char *x_pStr );

/// Converts to double
double StrToDouble( const char *x_pStr );

#ifndef CII_NO_WCHAR

	/// String format
	long vStrFmt( wchar_t *x_pDst, unsigned long x_uMax, const wchar_t *x_pFmt, tcVaList x_pArgs );

	/// String format
	long StrFmt( wchar_t *x_pDst, unsigned long x_uMax, const wchar_t *x_pFmt, ... );

	/// Converts to int64
	tc_int64 StrToInt64( const wchar_t *x_pStr, long x_lRadix = 10 );

	/// Converts to uint64
	tc_uint64 StrToUInt64( const wchar_t *x_pStr, long x_lRadix = 10 );

	/// Converts to long
	long StrToLong( const wchar_t *x_pStr, long x_lRadix = 10 );

	/// Converts to unsigned long
	unsigned long StrToULong( const wchar_t *x_pStr, long x_lRadix = 10 );

	/// Converts to double
	float StrToFloat( const wchar_t *x_pStr );

	/// Converts to double
	double StrToDouble( const wchar_t *x_pStr );

#endif

};