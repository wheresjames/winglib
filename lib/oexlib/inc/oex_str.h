/*------------------------------------------------------------------
// oex_str.h
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

#pragma once

/// !!! This namespace contains functions that depend
///     on strings being NULL terminated.
/**
    The NULL terminated string is evil, if you use these
    functions, you take your code into your own hands!
*/
namespace zstr
{
    template < typename T >
        oexUINT Length( oexCONST T *s )
        {   oexUINT l = 0;
            oexASSERT_PTR( s );
            while ( *s )
            	s++, l++;
            return l;
        }

	/// Returns end of string
    template < typename T >
        T* eos( T *s )
        {   return &s[ Length( s ) ];
        }

};


/// This namespace contains string functions that do *not* depend
/// on the string being NULL terminated.  These are the *good*
/// string functions ;)
namespace str
{
    /// Returns the length of the null terminated string in s
    /**
        \param [in] s       -   Pointer to string buffer
        \param [in] sz      -   Length of the buffer in s
    */
    template < typename T >
        oexUINT Length( oexCONST T *s, oexINT sz )
        {   oexUINT l = 0;
            oexASSERT_PTR( s );
            while ( sz-- && *s )
            	s++, l++;
            return l;
        }

    /// Copies string, returns the number of bytes copied
    /**
        \param [out] dst    -   Buffer that receives the string copy
        \param [in] sz_dst  -   Size of the buffer in dst
        \param [in] src     -   Pointer to the string to copy
        \param [in] ln_src  -   Length of the string in src
    */
    template < typename T >
        oexUINT Copy( T *dst, oexINT sz_dst, oexCONST T *src, oexINT ln_src )
        {
            if ( 0 >= sz_dst )
                return 0;

            if ( ( ln_src + 1 ) > sz_dst )
                ln_src = sz_dst - 1;

            os::CSys::MemCpy( dst, src, ln_src * sizeof( T ) );

            dst[ ln_src ] = 0;

            return ln_src;
        }

    /// Appends string, returns the new length of the destination string
    /*
        \param [out] dst    -   Buffer that receives the string copy
        \param [in] ln_dst  -   Length of the string in dst
        \param [in] sz_dst  -   Size of the buffer in dst
        \param [in] src     -   Pointer to the string to copy
        \param [in] ln_src  -   Length of the string in src
    */
    template< typename T >
        oexUINT Append( T *dst, oexINT ln_dst, oexINT sz_dst, oexCONST T* src, oexINT sz_src )
        {
            if ( ln_dst >= sz_dst )
                return ln_dst;

            dst += ln_dst;

            sz_dst -= ln_dst;

            return ln_dst + Copy( dst, sz_dst, src, sz_src );
        }

	/// Converts upper case letters to lower case
    /**
        \param [in] dst     -   String to modify
        \param [in] ln_dst  -   Length of string in dst
    */
    template< typename T >
	    T* ToLower( T *dst, oexINT ln_dst )
	{
        oexASSERT_PTR( dst );

		while ( 0 < ln_dst-- )
		{
            if ( *dst >= oexT( 'A' ) && *dst <= oexT( 'Z' ) )
				*dst -= oexT( 'A' ) - oexT( 'a' );

			dst++;

		} // end while

        return dst;
	}

	/// Converts lower case letters to upper case
    /**
        \param [in] dst     -   String to modify
        \param [in] ln_dst  -   Length of string in dst
    */
    template< typename T >
    	T* ToUpper( T *dst, oexINT ln_dst )
	{
        oexASSERT_PTR( dst );

		while ( 0 < ln_dst-- )
		{
            if ( *dst >= oexT( 'a' ) && *dst <= oexT( 'z' ) )
				*dst += oexT( 'A' ) - oexT( 'a' );

			dst++;

		} // end while

		return dst;
	}

    /// Replaces occurences a character in a string with another
    /*
        \param [in] dst     	-   Buffer that receives the string copy
        \param [in] ln_dst  	-   Length of the string in dst
        \param [in] x_tFind 	-	Character to find
        \param [in] x_tReplace	- 	Replacement character
    */
    template< typename T >
    	T* Replace( T *dst, oexINT ln_dst, oexCONST T x_tFind, oexCONST T x_tReplace )
	{
		if ( !ln_dst )
			return dst;

		while( ln_dst-- )
		{
			if ( *dst == x_tFind )
				*dst = x_tReplace;

			dst++;

		} // end while

		return dst;
	}


    /// Returns the offset of the first character in s1 not in s2
    /**
        \param [in] s1  -   String of characters in which to search
        \param [in] ln1 -   Length of the string in s1
        \param [in] s2  -   Character list
        \param [in] ln2 -   Number of characters in s2
    */
	template< class T >
		static oexINT SkipCharacters( T *s1, oexINT ln1, T *s2, oexINT ln2 )
	    {
            oexASSERT_PTR( s1 );
            oexASSERT_PTR( s2 );

			if ( !ln1 || !ln2 )
				return -1;

		    oexINT i = 0;
		    while ( ln1 )
		    {
			    T *start = s2;
                oexINT ln_start = ln2;
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

    /// Reverse skip, Returns the offset of the first character in s1 not in s2
    /// starting at the end of s1
    /**
        \param [in] s1  -   String of characters in which to search
        \param [in] ln1 -   Length of the string in s1
        \param [in] s2  -   Character list
        \param [in] ln2 -   Number of characters in s2
    */
	template< class T >
		oexINT RSkipCharacters( oexCONST T *s1, oexUINT ln1, oexCONST T *s2, oexUINT ln2 )
	    {
            oexASSERT_PTR( s1 );
            oexASSERT_PTR( s2 );

            if ( !ln1 || !ln2 )
                return -1;

		    while ( ln1-- )
		    {
			    oexCONST T *start = s2;
                oexINT ln_start = ln2;
			    while ( start && ln_start-- )
                    if ( s1[ ln1 ] == *start )
                        start = 0;
                    else
                        start++;

			    if ( start )
				    return ln1;

		    } // end while

		    return -1;
	    }

    /// Retuns the offset of ch
    /**
        \param [in] s   -   String in which to search
        \param [in] ln  -   Length of string in s
        \param [in] ch  -   Character to search for
    */
	template< class T >
		oexINT FindCharacter( oexCONST T *s, oexUINT ln, T ch )
		{
            oexASSERT_PTR( s );

			oexINT i = 0;
			while ( ln )
			{
				if ( *s == ch )
					return i;

				i++, s++; ln--;

			} // end while

			return -1;
		}

    /// Compares two strings
    /**
        \param [in] s1  -   First string
        \param [in] ln1 -   Length of the string in s1
        \param [in] s2  -   Second string
        \param [in] ln2 -   Length of the string in s2

        Returns zero if the two strings match
    */
    template< class T >
        oexINT Compare( T *s1, oexINT ln1, T *s2, oexINT ln2 )
        {
            oexASSERT_PTR( s1 );
            oexASSERT_PTR( s2 );

            if ( ln1 != ln2 )
                return ( ln1 >= ln2 ) ? 1 : -1;

            while ( ln1 )
            {
                if ( *s1 != *s2 )
                    return ( *s1 >= *s2 ) ? 1 : -1;

                s1++;
                s2++;
                ln1--;

            } // end while

            return 0;
        }

    /// Compares two strings
    /**
        \param [in] s1  -       First string
        \param [in] ln1 -       Length of the string in s1
        \param [in] s2  -       Second string
        \param [in] ln2 -       Length of the string in s2
        \param [in] ln_cmp -    Number of bytes to compare

        Returns zero if the two strings match
    */
    template< class T >
        oexINT CompareLen( T *s1, oexINT ln1, T *s2, oexINT ln2, oexINT ln_cmp )
        {
            oexASSERT_PTR( s1 );
            oexASSERT_PTR( s2 );

            // Enough bytes to compare?
            if ( ln_cmp > ln1 || ln_cmp > ln2 )
                return ( ln1 >= ln_cmp ) ? 1 : -1;

            while ( ln_cmp )
            {
                if ( *s1 != *s2 )
                    return ( *s1 >= *s2 ) ? 1 : -1;

                s1++;
                s2++;
                ln_cmp--;

            } // end while

            return 0;
        }


    /// Compares two strings ignoring case
    /**
        \param [in] s1   -       First string
        \param [in] ln1  -       Length of the string in s1
        \param [in] s2   -       Second string
        \param [in] ln2  -       Length of the string in s2
        \param [in] ln_cmp -    Number of bytes to compare

        Returns zero if the two strings match
    */
    template< class T >
        oexINT ICompare( oexCONST T *s1, oexINT ln1, oexCONST T *s2, oexINT ln2,
                         T tSLow = oexT( 'a' ), T tELow = oexT( 'z' ),
                         T tSHi = oexT( 'A' ), T tEHi = oexT( 'Z' ) )
        {
            oexASSERT_PTR( s1 );
            oexASSERT_PTR( s2 );

            if ( ln1 != ln2 )
                return ( ln1 >= ln2 ) ? 1 : -1;

            T tOff = tSHi - tSLow;
            while ( ln1 )
            {
                // Convert case
                T ch1 = *s1, ch2 = *s2;
                if ( ch1 >= tSHi && ch1 <= tEHi )
                    ch1 -= tOff;
                if ( ch2 >= tSHi && ch2 <= tEHi )
                    ch2 -= tOff;

                if ( ch1 != ch2 )
                    return ( ch1 > ch2 ) ? 1 : -1;

                s1++;
                s2++;
                ln1--;

            } // end while

            return 0;
        }

    /// Compares two strings ignoring case
    /**
        \param [in] s1   -       First string
        \param [in] ln1  -       Length of the string in s1
        \param [in] s2   -       Second string
        \param [in] ln2  -       Length of the string in s2
        \param [in] ln_cmp -    Number of bytes to compare

        Returns zero if the two strings match
    */
    template< class T >
        oexINT ICompareLen( oexCONST T *s1, oexINT ln1, oexCONST T *s2, oexINT ln2, oexINT ln_cmp,
                         T tSLow = oexT( 'a' ), T tELow = oexT( 'z' ),
                         T tSHi = oexT( 'A' ), T tEHi = oexT( 'Z' ) )
        {
            oexASSERT_PTR( s1 );
            oexASSERT_PTR( s2 );

            // Enough bytes to compare?
            if ( ln_cmp > ln1 || ln_cmp > ln2 )
                return ( ln1 >= ln_cmp ) ? 1 : -1;

            T tOff = tSHi - tSLow;
            while ( ln_cmp )
            {
                // Convert case
                T ch1 = *s1, ch2 = *s2;
                if ( ch1 >= tSHi && ch1 <= tEHi )
                    ch1 -= tOff;
                if ( ch2 >= tSHi && ch2 <= tEHi )
                    ch2 -= tOff;

                if ( ch1 != ch2 )
                    return ( ch1 > ch2 ) ? 1 : -1;

                s1++;
                s2++;
                ln_cmp--;

            } // end while

            return 0;
        }


    /// Finds the sub string s2 in s1
    /**
        \param [in] s1      -   String in which to search
        \param [in] ln_s1   -   Length of string in s1
        \param [in] s2      -   Sub string to find
        \param [in] ln_s2   -   Length of string in s2
    */
	template< class T >
		oexINT FindSubStr( T *s1, oexINT ln_s1, T *s2, oexINT ln_s2 )
	    {
            oexINT i = 0;
		    while ( ln_s1 )
		    {
			    // Match here?
			    if ( !CompareLen( s1, ln_s1, s2, ln_s2, ln_s2 ) )
				    return i;

			    i++; s1++; ln_s1--;

		    } // end while

		    return -1;
	    }

    /// Finds the sub string s2 in s1
    /**
        \param [in] s1      -   String in which to search
        \param [in] ln_s1   -   Length of string in s1
        \param [in] s2      -   Sub string to find
        \param [in] ln_s2   -   Length of string in s2
    */
	template< class T >
		oexINT IFindSubStr( T *s1, oexINT ln_s1, T *s2, oexINT ln_s2,
                            T tSLow = oexT( 'a' ), T tELow = oexT( 'z' ),
                            T tSHi = oexT( 'A' ), T tEHi = oexT( 'Z' ) )
	    {
		    oexINT i = 0;
		    while ( ln_s1 )
		    {
			    // Match here?
			    if ( !ICompareLen( s1, ln_s1, s2, ln_s2, ln_s2, tSLow, tELow, tSHi, tEHi ) )
				    return i;

			    i++; s1++; ln_s1--;

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
	template< class T >
		oexINT FindCharacters( T *s1, oexINT ln1, T *s2, oexINT ln2 )
	    {
            oexASSERT_PTR( s1 );
            oexASSERT_PTR( s2 );

		    oexINT i = 0;
		    while ( ln1 )
		    {
			    T *start = s2;
                oexINT ln_start = ln2;
			    while ( ln_start-- )
			    {	if ( *s1 == *start )
					    return i;
				    start++;
			    } // end while

			    i++; s1++; ln1--;

		    } // end while

		    return -1;
	    }

    /// Reverse find, Finds the first character in s2 that is in string s1
    /**
        \param [in] s1  -   String of characters in which to search
        \param [in] ln1 -   Length of the string in s1
        \param [in] s2  -   Character list
        \param [in] ln2 -   Number of characters in s2
    */
	template< class T >
		oexINT RFindCharacters( oexCONST T *s1, oexUINT ln1, oexCONST T *s2, oexUINT ln2 )
	    {
            oexASSERT_PTR( s1 );
            oexASSERT_PTR( s2 );

            if ( !ln1 || !ln2 )
                return -1;

		    while ( ln1-- )
		    {
			    T *start = s2;
			    while ( ln2-- )
			    {	if ( s1[ ln1 ] == *start )
					    return ln1;
				    start++;
			    } // end while

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
	template< class T >
		oexINT FindTerm( oexCONST T *s, oexINT ln, oexCONST T *term, oexINT ln_term, oexCONST T *esc = oexNULL, oexINT ln_esc = 0 )
	    {
            oexASSERT_PTR( s );
            oexASSERT_PTR( term );
            oexASSERT_PTR_NULL_OK( esc );

		    oexINT i = 0;
		    while ( ln )
		    {
			    // Finda terminator
			    i = FindCharacters( s, ln, term, ln_term );
			    if ( 0 > i )
				    return -1;

			    // Is it escaped?
			    if ( !i || !esc || 0 > FindCharacter( esc, ln_esc, s[ i - 1 ] ) )
				    return i;

			    // Next
			    s += i + 1;

                ln--;

		    } // end while

		    return -1;
	    }


    /// Parses backward through the string looking for terminating characters
    /**
        \param [in] s       -   String of characters in which to search
        \param [in] ln      -   Length of the string in s
        \param [in] term    -   Terminator character list
        \param [in] ln_term -   Length of string in term
        \param [in] esc     -   Escape characters
        \param [in] ln_esc  -   Length of string in esc
    */
	template< class T >
		oexINT RFindTerm( oexCONST T *s, oexINT ln, oexCONST T *term, oexINT ln_term )
	    {
            oexASSERT_PTR( s );

		    oexINT i = 0;
		    while ( ln-- )
		    {
                if ( 0 <= FindCharacter( term, ln_term, s[ ln ] ) )
                    return ln;

		    } // end while

		    return -1;
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
	template< class T >
		oexINT ParseQuoted( oexCONST T* s, oexINT ln, oexCONST T* open, oexINT ln_open, oexCONST T *close, oexINT ln_close, oexCONST T *esc = oexNULL, oexINT ln_esc = 0 )
	    {
            oexASSERT_PTR( s );

            // Must have at least two characters to be quoted
            if ( ln < 2 )
                return -1;

            // What quote character is being used
            oexINT q = FindCharacter( open, ln_open, *s );
		    if ( 0 > q )
			    return -1;

		    s++;
            oexINT i = FindTerm( s, ln - 1, close[ q ], 1, esc, ln_esc );
		    if ( 0 > i )
			    return -1;

		    return i + 1;

	    }

    /// Finds the first character in s within the specified range
    /**
        \param [in] s       -   String of characters in which to search
        \param [in] ln      -   Length of the string in s
        \param [in] min     -   Minimum character value
        \param [in] max     -   Maximum character value
    */
	template< class T >
		static oexINT FindInRange( oexCONST T *s, oexINT ln, T min, T max )
	{
        oexASSERT_PTR( s );

		oexINT i = 0;
		while ( ln )
		{
			if ( *s >= min && *s <= max )
				return i;

			i++; s++; ln--;

		} // end while

		return -1;
	}

    /// Finds the first character in s within the specified range
    /**
        \param [in] s       -   String of characters in which to search
        \param [in] ln      -   Length of the string in s
        \param [in] min     -   Minimum character value
        \param [in] max     -   Maximum character value
    */
	template< class T >
		static oexINT RFindInRange( oexCONST T *s, oexINT ln, T min, T max )
	{
        oexASSERT_PTR( s );

		while ( ln-- )
		{
			if ( s[ ln ] >= min && s[ ln ] <= max )
				return ln;

		} // end while

		return -1;
	}


    /// Skips the specified character range in string s
    /**
        \param [in] s       -   String of characters in which to search
        \param [in] ln      -   Length of the string in s
        \param [in] min     -   Minimum character value
        \param [in] max     -   Maximum character value
    */
	template< class T >
		static oexINT SkipInRange( oexCONST T *s, oexINT ln, T min, T max )
	{
		oexINT i = 0;
		while ( ln )
		{
			if ( *s < min || *s > max )
				return i;

			i++; s++; ln--;

		} // end while

		return -1;
	}


    /// Skips the specified character range in string s
    /**
        \param [in] s       -   String of characters in which to search
        \param [in] ln      -   Length of the string in s
        \param [in] min     -   Minimum character value
        \param [in] max     -   Maximum character value
    */
	template< class T >
		static oexINT RSkipInRange( oexCONST T *s, oexINT ln, T min, T max )
    	{
            oexASSERT_PTR( s );

		    while ( ln-- )
		    {
    			if ( s[ ln ] < min || s[ ln ] > max )

	    			return ln;

            } // end while

		    return -1;
	    }


    /// Reverses the specified string
    /**
        \param [in] s       -   String of characters in which to search
        \param [in] ln      -   Length of the string in s
    */
    template < typename T >
        oexINT Reverse( T *s, oexINT ln )
        {
            oexASSERT_PTR( s );

            if ( !*s || !ln )
                return 0;

            T * e = s + ln - 1;
            while ( s < e )
                cmn::Swap( *( s++ ), *( e-- ) );

            return ln;
        }

    /// Converts a string to a number
	template< class T >
        oexINT64 StrToNum( oexCONST T *x_pStr, oexUINT x_uSize = 0, oexUINT x_uRadix = 10, oexINT *x_pnEnd = oexNULL )
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

};

/// !!! This namespace contains functions that depend
///     on strings being NULL terminated.
/**
    The NULL terminated string is evil, if you use these
    functions, you take your code into your own hands!
*/
namespace zstr
{
    template < typename T >
        oexUINT Copy( T *dst, oexCONST T *src )
        {   oexUINT uLen = Length( src );
            return str::Copy( dst, uLen + 1, src, uLen );
        }

    template < typename T >
        oexUINT Copy( T *dst, oexUINT sz_dst, oexCONST T *src )
        {   return str::Copy( dst, sz_dst, src, Length( src ) );
        }

    template< typename T >
        oexUINT Append( T *dst, oexCONST T* src )
        {   oexUINT uLenSrc = Length( src );
        	oexUINT uLenDst = Length( dst );
            return str::Append( dst, uLenDst, uLenDst + uLenSrc + 1, src, uLenSrc );
        }

    template< typename T >
        oexUINT Append( T *dst, oexINT sz_dst, oexCONST T* src )
        {   return str::Append( dst, Length( dst ), sz_dst, src, Length( src ) );
        }

    /// Compares two strings
    /**
        \param [in] s1  -   First string
        \param [in] s2  -   Second string

        Returns zero if the two strings match
    */
    template< class T >
        oexINT Compare( T *s1, T *s2 )
        {	return str::Compare( s1, Length( s1 ), s2, Length( s2 ) );
		}

};


namespace guid
{
    // We've assumed this structure is 16 bytes
    oexSTATIC_ASSERT(  sizeof( oexGUID ) == 16 );

    template< typename T_STR >
        oexBOOL ahtoui( oexUINT *x_puNum, oexCONST T_STR *x_pBuffer, oexUINT x_uBytes, oexBOOL x_bSkipInvalid = oexFALSE )
    {
	    oexUINT num = 0;

	    // For Each ASCII Digit
	    for ( oexUINT i = 0; x_uBytes && x_pBuffer[ i ]; i++ )
	    {
		    // Convert ASCII Digit Between 0 And 9
		    if ( x_pBuffer[ i ] >= '0' && x_pBuffer[ i ] <= '9' )
			    num = ( num << 4 ) + ( x_pBuffer[ i ] - '0' );

		    // Convert ASCII Digit Between A And F
		    else if ( x_pBuffer[ i ] >= 'A' && x_pBuffer[ i ] <= 'F' )
			    num = ( num << 4 ) + ( x_pBuffer[ i ] - 'A' ) + 10;

		    // Convert ASCII Digit Between a And f
		    else if ( x_pBuffer[ i ] >= 'a' && x_pBuffer[ i ] <= 'f' )
			    num = ( num << 4 ) + ( x_pBuffer[ i ] - 'a' ) + 10;

		    // Do we just skip invalid digits?
		    else if ( !x_bSkipInvalid )
                return oexFALSE;

		    x_uBytes--;

	    } // end for

	    // Save number
	    if ( x_puNum )
            *x_puNum = num;

	    return oexTRUE;
    }

    template< typename T_CHAR, typename T_GUID >
        oexCONST T_CHAR* GuidToString( T_CHAR *x_pStr, oexUINT x_uMax, oexCONST T_GUID *x_pGuid )
    {
	    // Sanity checks
	    if ( !oexCHECK_PTR( x_pStr ) || !oexCHECK( x_uMax ) )
		    return oexNULL;

	    // Verify there is enough room for the string
	    if ( 36 > x_uMax )
	    {	*x_pStr = 0; return x_pStr; }

	    // Create new guid if one is not provided
	    T_GUID guid;
	    if ( !x_pGuid )
	    {	x_pGuid = &guid;
            os::CSys::CreateGuid( &guid );
	    } // end if

	    // Example GUID : DD05F574-2D69-4463-95DD-F76C9F7C5E6D

        return os::CSys::StrFmt( x_pStr, x_uMax,
                 oexTT( T_CHAR, "%0.8lX-%0.4lX-%0.4lX-%0.2lX%0.2lX-%0.2lX%0.2lX%0.2lX%0.2lX%0.2lX%0.2lX" ),
			     (oexUINT) x_pGuid->Data1,
			     (oexUINT) x_pGuid->Data2,
			     (oexUINT) x_pGuid->Data3,
			     (oexUINT) x_pGuid->Data4[ 0 ],
			     (oexUINT) x_pGuid->Data4[ 1 ],
			     (oexUINT) x_pGuid->Data4[ 2 ],
			     (oexUINT) x_pGuid->Data4[ 3 ],
			     (oexUINT) x_pGuid->Data4[ 4 ],
			     (oexUINT) x_pGuid->Data4[ 5 ],
			     (oexUINT) x_pGuid->Data4[ 6 ],
			     (oexUINT) x_pGuid->Data4[ 7 ] );
    }

    template< typename T_CHAR, typename T_GUID >
        T_GUID * StringToGuid( T_GUID *x_pGuid, oexCONST T_CHAR *x_pString, oexUINT x_uMax )
    {
	    if ( !oexCHECK_PTR( x_pGuid ) )
		    return oexNULL;

        // Does the caller want a new GUID?
        if ( x_pString == oexNULL )
        {   os::CSys::CreateGuid( x_pGuid );
            return x_pGuid;
        } // end if

	    // Ensure buffer is large enough to hold guid
	    if ( 36 > x_uMax )
		    return oexNULL;

	    // Verify separators
	    // Example : DD05F574-2D69-4463-95DD-F76C9F7C5E6D
	    //                   ^    ^    ^    ^
	    if (	x_pString[ 8 ] != '-' || x_pString[ 13 ] != '-' ||
			    x_pString[ 18 ] != '-' || x_pString[ 23 ] != '-' )
		    return oexNULL;

	    // Convert each component
	    oexUINT ul = 0;
	    if ( !ahtoui( &ul, x_pString, 8 ) )
            return oexNULL;
	    x_pGuid->Data1 = ul;

	    if ( !ahtoui( &ul, &x_pString[ 9 ], 4 ) )
            return oexNULL;
	    x_pGuid->Data2 = (oexUINT16)ul;

	    if ( !ahtoui( &ul, &x_pString[ 14 ], 4 ) )
            return oexNULL;
	    x_pGuid->Data3 = (oexUINT16)ul;

	    oexUINT i;
	    for ( i = 0; i < 2; i++ )
	    {	if ( !ahtoui( &ul, &x_pString[ 19 + ( i << 1 ) ], 2 ) )
                return oexNULL;
		    x_pGuid->Data4[ i ] = (oexUCHAR)ul;
	    } // end for

	    for ( i = 0; i < 6; i++ )
	    {	if ( !ahtoui( &ul, &x_pString[ 24 + ( i << 1 ) ], 2 ) )
                return oexNULL;
		    x_pGuid->Data4[ 2 + i ] = (oexUCHAR)ul;
	    } // end for

	    return x_pGuid;
    }

    template< typename T_GUID1 >
        T_GUID1 * ZeroGuid( T_GUID1 *pDst )
    {
        oexASSERT_PTR( pDst );

        *(oexINT64*)pDst = 0;
        ( (oexINT64*)pDst )[ 1 ] = 0;

        return pDst;
    }

    template< typename T_GUID1, typename T_GUID2 >
        T_GUID1 * CopyGuid( T_GUID1 *pDst, oexCONST T_GUID2 *pSrc )
    {
        oexASSERT_PTR( pDst );
        oexASSERT_PTR( pSrc );

        // This should be fast
        *(oexINT64*)pDst = *(oexINT64*)pSrc;
        ( (oexINT64*)pDst )[ 1 ] = ( (oexINT64*)pSrc )[ 1 ];

        return pDst;
    }

    template< typename T_GUID1, typename T_GUID2 >
        oexBOOL CmpGuid( oexCONST T_GUID1 *pGuid1, oexCONST T_GUID2 *pGuid2 )
    {
	oexSTATIC_ASSERT( sizeof( T_GUID1 ) == ( sizeof( oexINT64 ) * 2 ) );

        oexASSERT_PTR( pGuid1 );
        oexASSERT_PTR( pGuid2 );

        // This should be fast
        return ( *(oexINT64*)pGuid1 == *(oexINT64*)pGuid2
                 && ( (oexINT64*)pGuid1 )[ 1 ] == ( (oexINT64*)pGuid2 )[ 1 ] );
    }

    template< typename T_GUID1, typename T_GUID2, typename T_GUID3 >
        oexBOOL CmpGuid( T_GUID1 *pGuid1, oexCONST T_GUID2 *pGuid2, oexCONST T_GUID3 *pMask )
    {
	oexSTATIC_ASSERT(    sizeof( T_GUID1 ) == sizeof( T_GUID2 )
	                  && sizeof( T_GUID1 ) == sizeof( T_GUID3 ) );

        oexBOOL bMatch = oexTRUE;
        oexUCHAR *p1 = (oexUCHAR*)pGuid1;
        oexUCHAR *p2 = (oexUCHAR*)pGuid2;
        oexUCHAR *m = (oexUCHAR*)pMask;

        for ( oexUINT i = 0; bMatch && i < sizeof( T_GUID1 ) / sizeof( oexUCHAR ); i++ )
            if ( p1[ i ] ^ p2[ i ] & m[ i ] )
                bMatch = oexFALSE;

        return bMatch;
    }

    template< typename T_GUID1, typename T_GUID2 >
        T_GUID1 * XorGuid( T_GUID1 *pGuid1, oexCONST T_GUID2 *pGuid2 )
    {
        oexASSERT_PTR( pGuid1 );
        oexASSERT_PTR( pGuid2 );

        *(oexINT64*)pGuid1 ^= *(oexINT64*)pGuid2;
        ( (oexINT64*)pGuid1 )[ 1 ] ^= ( (oexINT64*)pGuid2 )[ 1 ];

        return pGuid1;
    }

    template< typename T_GUID1, typename T_GUID2 >
        T_GUID1 * AndGuid( T_GUID1 *pGuid1, oexCONST T_GUID2 *pGuid2 )
    {
        oexASSERT_PTR( pGuid1 );
        oexASSERT_PTR( pGuid2 );

        *(oexINT64*)pGuid1 &= *(oexINT64*)pGuid2;
        ( (oexINT64*)pGuid1 )[ 1 ] &= ( (oexINT64*)pGuid2 )[ 1 ];

        return pGuid1;
    }

    template< typename T_GUID1, typename T_GUID2 >
        T_GUID1 * OrGuid( T_GUID1 *pGuid1, oexCONST T_GUID2 *pGuid2 )
    {
        oexASSERT_PTR( pGuid1 );
        oexASSERT_PTR( pGuid2 );

        *(oexINT64*)pGuid1 |= *(oexINT64*)pGuid2;
        ( (oexINT64*)pGuid1 )[ 1 ] |= ( (oexINT64*)pGuid2 )[ 1 ];

        return pGuid1;
    }

    template< typename T_GUID, typename T >
        T_GUID * SetGuid( T_GUID *pGuid1, oexCONST T pVal, oexUINT uOffset, oexUINT uCount )
    {
        oexASSERT_PTR( pGuid1 );

        // Set GUID values
        T *p = &( (T*)pGuid1 )[ uOffset ];
        for ( oexUINT i = 0; ( sizeof( T ) * ( uOffset + i + 1 ) ) <= sizeof( T_GUID ); i++ )
            p[ i ] = pVal;

        return pGuid1;
    }

};
