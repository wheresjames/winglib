
#pragma once

#include "str.h"

// int process_binary( oex::CFile &f, oex::CStr8 &sData, oex::CStr &sVar, oex::CStr &sPre, oex::CStr &sSuf )
template< typename T, typename T_STR >
	int process_binary( const T_STR sIn, const T_STR sOut, const T_STR sPre, const T_STR sLen, const T_STR sEnd )
	{
		std::basic_string< char > sInData;

#if defined( _WIN32 )
		typedef FILE* t_hfile;
#else
		typedef int t_hfile;
#endif

		t_hfile hIn = fopen( sIn.c_str(), "rb" );
		if ( !hIn )
			return -1;

		t_hfile hOut = fopen( sOut.c_str(), "wb" );
		if ( !hOut )
		{	fclose( hIn );
			return -1;
		} // end if

		// Write out the prefix
		if ( sPre.length() )
			fwrite( sPre.data(), sizeof( T ), sPre.length(), hOut );

		// Char markers
		T *t = (T*)"0x.., ";
		long tl = zstr::Length( t );
		T *r = (T*)"0x..,\n\t";
		long rl = zstr::Length( r );

		// Buffers
		long bl = 0, lRead = 0, lTotal = 0;
		T in[ sizeof( T ) * 64 * 1024 ], out[ sizeof( in ) ], *s;
		
		// Read in data in chunks
		while ( 0 < ( lRead = fread( in, sizeof( T ), sizeof( in ), hIn ) ) )
		{
			// Track the total bytes
			lTotal += lRead;

			// Convert each byte and write it out
			for ( long i = 0; i < lRead; i++ )
			{
				// Get a pointer to our spot in the buffer
				s = &out[ bl ];

				// Copy hex prefix
				if ( !( ( i + 1 ) & 0xf ) )
					memcpy( s, r, rl ), bl += rl;
				else
					memcpy( s, t, tl ), bl += tl;

				// Convert byte to ascii
				str::ntoa( &s[ 2 * sizeof( T ) ], (T)in[ i ] );

				// Write data out if the buffer is getting full
				if ( ( sizeof( out ) - 128 ) < bl )
					fwrite( out, sizeof( T ), bl, hOut ), bl = 0;

			} // end for

			// Write whatever is left
			if ( bl )
				fwrite( out, sizeof( T ), bl, hOut ), bl = 0;

		} // end while

		if ( sLen.length() )
		{	T_STR w = sLen + tcnv::ToString< T, T_STR >( lTotal );
			fwrite( w.data(), sizeof( T ), w.length(), hOut );
		} // end if

		// Write out the prefix
		if ( sEnd.length() )
			fwrite( sEnd.data(), sizeof( T ), sEnd.length(), hOut );

		// Close the file handles
		fclose( hIn );
		fclose( hOut );

		// Return total bytes
		return lTotal;
	}
