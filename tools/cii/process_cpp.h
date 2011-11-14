
#pragma once

#include "str.h"

/// Returns non-zero if the character is a valid html character
template< typename T >
	static bool IsCppChar( T x_ch )
{
	switch( x_ch )
	{	case tcTC( T, '"' ) :
		case tcTC( T, '\\' ) :
			return false;
	} // end switch

	return ( 0 > x_ch || tcTC( T, ' ' ) <= x_ch ) ? true : false;
}

template< typename T, typename T_STR >
	static T_STR CppEncodeChar( T x_ch )
	{
		switch( x_ch )
		{
			case tcTC( T, '"' ) :
				return tcTT( T, "\\\"" );

			case tcTC( T, '\'' ) :
				return tcTT( T, "\\\\" );

			case tcTC( T, '\t' ) :
				return tcTT( T, "\\t" );

			case tcTC( T, '\r' ) :
				return tcTT( T, "\\r" );

			case tcTC( T, '\n' ) :
				return tcTT( T, "\\n" );

		} // end switch

		// Convert to two byte character
		T s[ 16 ] = { '"', ' ', '"', '\\', 'x', 0, 0, '"', ' ', '"', 0 };
		str::ntoa< char >( &s[ 5 ], (char)x_ch );
		
		return T_STR( s, 11 );
	}

template< typename T, typename T_STR >
	static T_STR CppEncode( const T *x_pStr, typename T_STR::size_type x_lSize = 0 )
	{
		if ( !x_pStr || !*x_pStr || 0 >= x_lSize )
			return T_STR();
		
		T_STR ret;
		T_STR::size_type nStart = 0, nPos = 0;

		while ( nPos < x_lSize )
		{
			// Must we encode this one?
			if ( !IsCppChar( x_pStr[ nPos ] ) )
			{
				// Copy data that's ok
				if ( nStart < nPos )
					ret.append( &x_pStr[ nStart ], nPos - nStart );

				// Encode this character
				ret.append( CppEncodeChar< T, T_STR >( x_pStr[ nPos ] ) );

				// Next
				nStart = ++nPos;

			} // end if

			else
				nPos++;

		} // end while

		// Copy remaining data
		if ( nStart < nPos )
			ret.append( &x_pStr[ nStart ], nPos - nStart );

		return ret;
	}

template< typename T, typename T_STR >
	static T_STR CppEncode( const T_STR &x_str )
	{
		return CppEncode< T, T_STR >( x_str.data(), x_str.length() );
	}


/// This function turns html with embedded c++ code inside out
/**

  @param[in] sName		- Namespace / function name
  @param[in] sScript	- File or script data
  @param[in] bFile		- Non-zero if sScript is a file name
  
  @code
  @endcode

*/
template< typename T, typename T_STR >
	int process_cpp( const T_STR sIn, const T_STR sOut, const T_STR sVar )
	{
		// Attempt to read in the file data, it must fit in memory
		T_STR sSrc = disk::ReadFile< T, T_STR >( sIn ), sDst;
		const T *pSrc = sSrc.data();
		long szSrc = sSrc.length();
		if ( 0 >= szSrc )
			return -1;

		// Start out with some space
		sDst.reserve( sSrc.length() * 2 );

		// Start off the file
		sDst = T_STR() + tcTT( T, "\n\nstatic int _do_run_run_run( TPropertyBag< char > &in, basic_string< char > &out )\n{\n" );

		// Global data
		T_STR sGlobal;

		// Tags
		const T *tsOpen = tcTT( T, "<?c" ), *tsGlobal = tcTT( T, "<?global" ), *tsClose = tcTT( T, "?>" );
		long szOpen = zstr::Length( tsOpen ), szGlobal = zstr::Length( tsGlobal ), szClose = zstr::Length( tsClose );

		// Process the data
		long nPos = 0, nOpen = 0, nClose = 0, nSkip = 0, nStart = 0, nType = 0;
		while ( ( nPos + szOpen + szClose ) < szSrc )
		{
			// Initialize
			nStart = nPos;
			nOpen = nClose = szSrc;

			// Attempt to find an open bracket
			while ( nOpen == szSrc && ( nPos + szOpen + szClose ) < szSrc )
				if ( pSrc[ nPos ] == tsGlobal[ 0 ] && !memcmp( &pSrc[ nPos ], tsGlobal, szGlobal ) )
					nOpen = nPos, nType = 0, nSkip = szGlobal;
				else if ( pSrc[ nPos ] == tsOpen[ 0 ] && !memcmp( &pSrc[ nPos ], tsOpen, szOpen ) )
					nOpen = nPos, nType = 1, nSkip = szOpen;
				else
					nPos++;

			// Find a closing bracket
			while ( nClose == szSrc && ( nPos + szClose ) < szSrc )
				if ( pSrc[ nPos ] == tsClose[ 0 ] && !memcmp( &pSrc[ nPos ], tsClose, szClose ) )
					nClose = nPos;
				else
					nPos++;

			// Did we find embedded code?
			if ( nOpen < szSrc && nClose < szSrc )
			{
				// Encode any text
				if ( nStart < nOpen )
				{	sDst += tcTT( T, "\n\tout << \"" );
					sDst += CppEncode< T, T_STR >( &pSrc[ nStart ], nOpen - nStart );
					sDst += tcTT( T, "\";\n" );
				} // end if

				// Just copy code straight over
				nOpen += nSkip;
				if ( nOpen < nClose )
				switch( nType )
				{
					// Global data
					case 0 :
						sGlobal.append( &pSrc[ nOpen ], nClose - nOpen );
						break;

					// c code
					case 1 :
						sDst.append( &pSrc[ nOpen ], nClose - nOpen ), sDst += tcTT( T, ";\n" );
						break;

				} // end switch

				// Point to data after code
				nPos += szClose;
				nStart = nPos;

			} // end if

		} // end while

		// Copy whatever is left
		if ( nStart < szSrc )
		{	sDst += tcTT( T, "\n\tout << \"" );
			sDst += CppEncode< T, T_STR >( &pSrc[ nStart ], szSrc - nStart );
			sDst += tcTT( T, "\";\n" );
		} // end if

		// Footer
		sDst += tcTT( T, "\n\treturn 0;\n}\n" );

		// Add extern pointer to function
		sDst += T_STR() + tcTT( T, "\n\nextern void * f_" ) + sVar + tcTT( T, " = &_do_run_run_run;\n" );

		// Write out the file
		return disk::WriteFile< T >( sOut, sGlobal, sDst );
	}
