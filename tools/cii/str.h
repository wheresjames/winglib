// str.h

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

namespace str
{
	// Size type
	typedef long t_size;
	
	typedef signed long long int tc_int64;
	typedef unsigned long long int tc_uint64;

	/// String format
	long vPrint( const char *x_pFmt, tcVaList x_pArgs );

	/// String format
	long Print( const char *x_pFmt, ... );

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
	long vPrint( const wchar_t *x_pFmt, tcVaList x_pArgs );

	/// String format
	long Print( const wchar_t *x_pFmt, ... );

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

	template< typename T, typename T_STR >
		T_STR ToString( int n ) 
		{   T szNum[ 256 ] = { 0 }; 
			return T_STR( szNum, StrFmt( szNum, sizeof( szNum ), tcTT( T, "%d" ), n ) );
		}

	template< typename T, typename T_STR >
		T_STR ToString( unsigned int n ) 
		{   T szNum[ 256 ] = { 0 }; 
			return T_STR( szNum, StrFmt( szNum, sizeof( szNum ), tcTT( T, "%u" ), n ) );
		}

	template< typename T, typename T_STR >
		T_STR ToString( long n ) 
		{   T szNum[ 256 ] = { 0 }; 
			return T_STR( szNum, StrFmt( szNum, sizeof( szNum ), tcTT( T, "%li" ), n ) );
		}

	template< typename T, typename T_STR >
		T_STR ToString( unsigned long n ) 
		{   T szNum[ 256 ] = { 0 }; 
			return T_STR( szNum, StrFmt( szNum, sizeof( szNum ), tcTT( T, "%lu" ), n ) );
		}

	template< typename T, typename T_STR >
		T_STR ToString( tc_int64 n ) 
		{   T szNum[ 256 ] = { 0 }; 
			return T_STR( szNum, StrFmt( szNum, sizeof( szNum ), tcTT( T, "%lld" ), n ) );
		}

	template< typename T, typename T_STR >
		T_STR ToString( tc_uint64 n ) 
		{   T szNum[ 256 ] = { 0 }; 
			return T_STR( szNum, StrFmt( szNum, sizeof( szNum ), tcTT( T, "%llu" ), n ) );
		}

	template< typename T, typename T_STR >
		T_STR ToString( float n ) 
		{   T szNum[ 256 ] = { 0 }; 
			return T_STR( szNum, StrFmt( szNum, sizeof( szNum ), tcTT( T, "%f" ), (double)n ) );
		}

	template< typename T, typename T_STR >
		T_STR ToString( double n ) 
		{   T szNum[ 256 ] = { 0 }; 
			return T_STR( szNum, StrFmt( szNum, sizeof( szNum ), tcTT( T, "%f" ), n ) );
		}

}; // namespace str

/**
	@warning Functions in the zstr namespace depend on strings being null terminated.
*/
namespace zstr
{
    /// Returns the length of the null terminated string in s
    /**
        \param [in] s       -   Pointer to string buffer
	*/
	template < typename T >
		static str::t_size Length( const T *x_pStr )
	{
		if ( !x_pStr )
			return 0;
	
		str::t_size l = 0;
		while ( *x_pStr )
			l++, x_pStr++;

		return l;
	}

    /// Copies string, returns the number of bytes copied
    /**
        \param [out] dst    -   Buffer that receives the string copy
        \param [in] sz_dst  -   Size of the buffer in dst
        \param [in] src     -   Pointer to the string to copy
    */
    template < typename T, typename T_SZ >
        T_SZ Copy( T *dst, T_SZ sz_dst, const T *src )
        {
			if ( !dst || !src )
				return 0;

            if ( 0 >= sz_dst )
                return 0;

			T_SZ ln_src = zstr::Length( src );
            if ( ( ln_src + 1 ) > sz_dst )
                ln_src = sz_dst - 1;

            memcpy( dst, src, ln_src * sizeof( T ) );

            dst[ ln_src ] = 0;

            return ln_src;
        }

}; // namespace zstr

namespace str
{

	/// 'Fast' number to ascii conversion
    /**
        \param [out] b	-   Destination buffer
        \param [in] ch	-   Character to serialize
    */
	template< typename T, typename N >
		void ntoa( T *b, N ch )
		{
			T c;
			const long sz = sizeof( N ) * 2;

			// For each nibble
			for ( long i = 0; i < sz; i++ )
			{
				// Grab a nibble
				c = (T)( ch & 0x0f ); 
				ch >>= 4;

				if ( 9 >= c )
					b[ sz - i - 1 ] = tcTC( T, '0' ) + c;
				else
					b[ sz - i - 1 ] = tcTC( T, 'a' ) + ( c - 10 );

			} // end for

		}

	/// Retuns the offset of ch
    /**
        \param [in] s   -   String in which to search
        \param [in] ln  -   Length of string in s
        \param [in] ch  -   Character to search for
    */
	template< typename T >
		t_size FindCharacter( const T *s, t_size ln, T ch )
		{
			if ( !s )
				return -1;

			t_size i = 0;
			while ( 0 < ln )
			{
				if ( *s == ch )
					return i;

				i++, s++; ln--;

			} // end while

			return -1;
		}

    /// Finds the first character in s2 that is in string s1
    /**
        \param [in] s1  -   String of characters in which to search
        \param [in] ln1 -   Length of the string in s1
        \param [in] s2  -   Character list
        \param [in] ln2 -   Number of characters in s2
    */
	template< typename T >
		t_size FindCharacters( const T *s1, t_size ln1, const T *s2, t_size ln2 )
	    {
			if ( !s1 || !s2 )
				return -1;

		    t_size i = 0;
		    while ( 0 < ln1 )
		    {
			    const T *start = s2;
                t_size ln_start = ln2;
			    while ( ln_start-- )
			    {	if ( *s1 == *start )
					    return i;
				    start++;
			    } // end while

			    i++; s1++; ln1--;

		    } // end while

		    return -1;
	    }

    /// Parses through the string looking for terminating characters
    /**
        \param [in] s       -   String of characters in which to search
        \param [in] ln      -   Length of the string in s
        \param [in] term    -   Terminator character list
        \param [in] ln_term -   Length of string in term
        \param [in] esc     -   Escape characters
        \param [in] ln_esc  -   Length of string in esc
    */
	template< typename T >
		t_size FindTerm( const T *s, t_size ln, const T *term, t_size ln_term, const T *esc = 0, t_size ln_esc = 0 )
	    {
			if ( !s || !term )
				return -1;

		    t_size i = 0, o = 0;
		    while ( 0 < ln )
		    {
			    // Finda terminator
			    i = FindCharacters( s, ln, term, ln_term );
			    if ( 0 > i )
				    return -1;

			    // Is it escaped?
			    if ( !i || !esc || 0 > FindCharacter( esc, ln_esc, s[ i - 1 ] ) )
				    return o + i;

			    // Next
			    s += i + 1;
				o += i + 1;

                ln -= i;

		    } // end while

		    return -1;
	    }

    /// Unquotes a string inplace
    /**
        \param [in] s       -   Quoted string
        \param [in] ln      -   Length of the string in s
        \param [in] open    -   List of opening quotes
        \param [in] ln_open -   Length of string in term
        \param [in] close   -   List of closing quotes
        \param [in] ln_close-   Length of string in term
        \param [in] esc     -   Escape characters
        \param [in] ln_esc  -   Length of string in esc

        The opening and closing quote characters must correspond within the string.

        open    = "<{[(";
        close   = ">}])";
		
		@return The size of the new string

    */
	template< typename T >
		t_size UnquoteInplace( T* s, t_size ln, const T* open, t_size ln_open, const T *close, t_size ln_close, const T *esc = 0, t_size ln_esc = 0 )
	    {
			if ( !s )
				return 0;

            // What quote character is being used
            t_size q = FindCharacter( open, ln_open, *s );

			// Is it quoted?
		    if ( 0 > q )
				return ln;

			// Destination
			T* d = s;
			t_size o = 0;
			
			// Skip the quote character
			s++; ln--;

			while ( 0 < ln )
			{
				// Is this an escape character?
				t_size e = FindCharacter( esc, ln_esc, *s );
				if ( 0 <= e )
				{	s++; ln--;
					if ( 0 < ln ) 
						d[ o++ ] = *s, s++, ln--;
				} // end if

				// Is it the end of the quoted string?
				else if ( *s == close[ q ] ) 
					ln = 0;

				// Just copy the character
				else
					d[ o++ ] = *s, s++, ln--;

			} // end while

			// Null terminate the string
			d[ o ] = 0;

			return o;
	    }

    /// Returns the offset of the first character in s1 not in s2
    /**
        \param [in] s1  -   String of characters in which to search
        \param [in] ln1 -   Length of the string in s1
        \param [in] s2  -   Character list
        \param [in] ln2 -   Number of characters in s2
    */
	template < typename T >
		static t_size SkipCharacters( T *s1, t_size ln1, const T *s2, t_size ln2 )
	{
		if ( !s1 || !ln1 || !s2 || !ln2 )
			return -1;

		t_size i = 0;
		while ( ln1 )
		{
			const T *start = s2;
			t_size ln_start = ln2;
			while ( start && ln_start-- )
				if ( *s1 == *start )
					start = 0;
				else
					start++;

			if ( start )
				return i;

			i++; s1++; ln1--;

		} // end while

		return i;
	}

    /// Finds the end of a quoted sub string
    /**
        \param [in] s       -   String of characters in which to search
        \param [in] ln      -   Length of the string in s
        \param [in] open    -   List of opening quotes
        \param [in] ln_open -   Length of string in term
        \param [in] close   -   List of closing quotes
        \param [in] ln_close-   Length of string in term
        \param [in] esc     -   Escape characters
        \param [in] ln_esc  -   Length of string in esc

        The opening and closing quote characters must correspond within the string.

        open    = "<{[(";
        close   = ">}])";

    */
	template< typename T >
		t_size ParseWithQuoted( const T* s, t_size ln, 
								const T *term, t_size ln_term, 
							    const T* open, t_size ln_open, 
								const T *close, t_size ln_close, 
								const T *esc = 0, t_size ln_esc = 0 )
	{
		if ( !s || !term || !esc )
			return -1;

		t_size q = -1;
		t_size i = 0;
		while ( i < ln )
		{
			// Is this the terminator?
			if ( 0 <= FindCharacter( term, ln_term, *s ) )
				return i;

			// Is it an escape character?
			if ( esc && 0 <= FindCharacter( esc, ln_esc, *s ) )
			{
				i++; s++;

				if ( i >= ln )
					return -1;

			} // end if

			else
			{
				// Is it a 'start quote' character?
				q = FindCharacter( open, ln_open, *s );
				if ( 0 <= q )
				{
					// Skip open quote
					s++, i++;
					if ( i >= ln )
						return -1;

					// Find end quote
					t_size e = FindTerm( s, ln - i, &close[ q ], 1, esc, ln_esc );

					// Adjust if found
					if ( 0 <= e )
						i += e, s += e;

					// iii I'm choosing to ignore an unterminated quote,
					// 	   could also have just grabed the rest of the string

				} // end if

			} // end if

			// Next character
			i++; s++;

		} // end while

		return ln;
	}

	/// Splits a string into an array by cutting on any character in m_pSep, while respecting quotes
	/**
		@param[in,out] 	x_pStr		- String to be split
		@param[in]		x_nSize		- Size of the buffer at x_pStr
		@param[in]		x_pSep		- NULL terminated list of separators
		@param[in]		x_pOpen		- NULL terminated list of open quotes
		@param[in]		x_pClose 	- NULL terminated list of close quotes
		@param[in]		x_pEsc		- NULL terminated list of escape characters
		@param[in]		x_bInPlace	- If not-zero, the buffer is unquoted in place.
									  This destroys the content of x_pStr.

        The opening and closing quote characters must correspond within the string.

        open    = "<{[(";
        close   = ">}])";

		@warning If x_bInPlace is non-zero, this function destroys the contents of x_pStr while parsing.

		+++ Remove the dependency on null terminated strings
	*/
	template < typename T, typename T_STR, typename T_LST >
		static T_LST SplitQuoted( T *x_pStr, t_size x_nSize,
								  const T *x_pSep, const T *x_pOpen,
								  const T *x_pClose, const T *x_pEsc,
								  bool x_bInPlace )
	{
		T_LST lst;

		// Sanity check
		if ( !x_pStr || 0 >= x_nSize )
			return lst;

		// Get sep string length
		t_size nSep = x_pSep ? zstr::Length( x_pSep ) : 0;
		if ( !nSep )
			return lst;

		t_size nOpen = x_pOpen ? zstr::Length( x_pOpen ) : 0;
		t_size nClose = x_pClose ? zstr::Length( x_pSep ) : 0;
		t_size nEsc = x_pEsc ? zstr::Length( x_pEsc ) : 0;

		// While we have bytes
		while ( x_nSize )
		{
			// Skip any separators
			t_size nSkip = SkipCharacters( x_pStr, x_nSize, x_pSep, nSep );
			if ( 0 <= nSkip )
				x_pStr += nSkip, x_nSize -= nSkip;

			// Are we done?
			if ( !x_nSize )
				return lst;

			// Find a closing separator
			t_size nPos = ParseWithQuoted( x_pStr, x_nSize, x_pSep, nSep,
										   x_pOpen, nOpen, x_pClose, nClose, x_pEsc, nEsc );

			// Did we find a separator?
			if ( 0 > nPos )
			{
				// Add what's left
				lst.push_back( T_STR( x_pStr, x_nSize ) );

				// This is the end
				return lst;

			} // end if

			else if ( nPos )
			{
				// Unquote the string and add it
				lst.push_back( T_STR( x_pStr, UnquoteInplace< T >( x_pStr, nPos, x_pOpen, nOpen, x_pClose, nClose, x_pEsc, nEsc ) ) );

				// Skip
				x_nSize -= nPos; x_pStr += nPos;

			} // end if

		} // end while

		return lst;

	}

	template< class T >
		t_size MatchPattern( const T *s, t_size ln_s, const T* pat, t_size ln_pat, bool ignore_case )
	{
		if ( !s || !pat )
			return -1;

		// Match empty pattern
		if ( !ln_pat )
			return 0;

		t_size i = 0, p = 0;

		// Skip multiple '*'
		while ( p < ( ln_pat + 1 ) && pat[ p ] == tcTC( T, '*' ) && pat[ p + 1 ] == tcTC( T, '*' ) )
			p++;

		// Check for the 'any' pattern
		if ( pat[ p ] == tcTC( T, '*' ) && p + 1 >= ln_pat )
			return 0;

		// While we're not at the end
		while ( i < ln_s )
		{
			// Are we on a wildcard?
			if ( pat[ p ] == tcTC( T, '*' ) )
			{
				// Are we matching everything?
				if ( p + 1 >= ln_pat )
					return 0;

				// Check for pattern advance
				if ( s[ i ] == pat[ p + 1 ] ||
						( ignore_case &&
							(
								(
									s[ i ] >= tcTC( T, 'a' ) && s[ i ] <= tcTC( T, 'z' ) &&
									( s[ i ] - ( tcTC( T, 'a' ) - tcTC( T, 'A' ) ) ) == pat[ p + 1 ]
								) ||

								(
									s[ i ] >= tcTC( T, 'A' ) && s[ i ] <= tcTC( T, 'Z' ) &&
									( s[ i ] + ( tcTC( T, 'a' ) - tcTC( T, 'A' ) ) ) == pat[ p + 1 ]
								)
							)
						)
					) p += 2;

			} // end if

			// Just accept this character
			else if ( pat[ p ] == tcTC( T, '?' ) )
				p++;

			// Otherwise advance if equal
			else if ( s[ i ] == pat[ p ] )
				p++;

			// Case insensitive
			else if ( ignore_case &&
						(
							(
								s[ i ] >= tcTC( T, 'a' ) && s[ i ] <= tcTC( T, 'z' ) &&
								( s[ i ] - ( tcTC( T, 'a' ) - tcTC( T, 'A' ) ) ) == pat[ p ]
							) ||
							(
								s[ i ] >= tcTC( T, 'A' ) && s[ i ] <= tcTC( T, 'Z' ) &&
								( s[ i ] + ( tcTC( T, 'a' ) - tcTC( T, 'A' ) ) ) == pat[ p ]
							)
						)
					) p++;

			else
			{
				// Attempt to back up in the pattern
				while ( p && pat[ p ] != tcTC( T, '*' ) )
					p--;

				// Did we find the 'any' pattern?
				if ( pat[ p ] != tcTC( T, '*' ) )
					return -1;

			} // end else

			// Next char
			i++;

			// Are we at the end of the pattern?
			if ( p >= ln_pat  )
			{
				// Quit if at the end of the string too
				if ( i >= ln_s )
					return 0;

				// Attempt to back up in the pattern
				while ( p && pat[ p ] != tcTC( T, '*' ) )
					p--;

				// Did we find the 'any' pattern?
				if ( pat[ p ] != tcTC( T, '*' ) )
					return -1;

			} // end if

		} // end while

		// Skip wild cards
		while ( p < ln_pat && pat[ p ] == tcTC( T, '*' ) )
			p++;

		// Did we match?
		return ( p >= ln_pat ) ? 0 : -1;
	}

}; // namespace str