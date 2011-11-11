// cii.cpp

#if defined( _WIN32 )
#	include <windows.h>
#endif

#include "cmd_line.h"
#include "find_files.h"

int main( int argc, char* argv[] )
{
	// Parse the command line
    TCmdLine< char > cl( argc, argv );

	// Dump the command line
//	for ( TCmdLine< char >::iterator it = cl.begin(); cl.end() != it; it++ )
//		printf( ": %s = %s\n", it->first.c_str(), it->second->c_str() );

	ff::SFindData fd;
	ff::HFIND hFind = ff::FindFirst( cl.pb()[ "f" ].c_str(), "*", &fd );
	if ( ff::c_invalid_hfind == hFind )
	{	printf( "No files found" );
		return 0;
	} // end if


	return 0;
}

/*
template < typename T >
	CStr prepare_inline( const T &sName, const T &sScript, int bFile )
	{_STT();

		// Code header / footer
		static oex::oexTCHAR szHeader1[] = oexT( "CBin " );
		static oex::oexTCHAR szHeader2[] = oexT( "()" ) oexNL oexT( "{" ) oexNL;
		static oex::oexTCHAR szFooter[] = oexNL oexT( "return out; " ) oexNL oexT( "}" oexNL oexT( "};" ) );
		static oex::oexTCHAR szOpenStr[] = oexNL oexT( "out << \"" );
		static oex::oexTCHAR szCloseStr[] = oexT( "\";" ) oexNL;
		static oex::CStr::t_size nOverhead = (oex::CStr::t_size)
												 sizeof( szHeader1 ) + sizeof( szHeader2 )
												 + sName.Length() + sizeof( szFooter )
												 + sizeof( szOpenStr ) + sizeof( szCloseStr );

		// Global name space
		T sGlobal;
		sGlobal << oexT( "namespace " ) << sName << oexT( " {" ) << oexNL << oexT( "CBin out;" ) << oexNL;

		// Get the source data
		oex::CStr sSrc;
		if ( bFile )
			sSrc = oexFileGetContents( sScript.Ptr() );
		else
			sSrc = sScript;

		// Did we get anything
		oex::oexCSTR pSrc = sSrc.Ptr();
		oex::CStr::t_size szSrc = sSrc.Length(), nPos = 0, nStart = 0;
		if ( !szSrc )
			return oexT( "" );

		// Allocate space for output
		oex::CStr sDst;
		oex::CStr::t_size nDst = 0;
		oex::oexSTR pDst = sDst.OexAllocate( ( szSrc * 3 ) + nOverhead );
		if ( !pDst )
			return oexT( "" );

		// Copy header into buffer
		oexMemCpy( &pDst[ nDst ], szHeader1, sizeof( szHeader1 ) - sizeof( oex::oexTCHAR ) );
		nDst += sizeof( szHeader1 ) - sizeof( oex::oexTCHAR );

		// Copy function name
		oexMemCpy( &pDst[ nDst ], sName.Ptr(), sName.Length() );
		nDst += sName.Length();

		// Copy header into buffer
		oexMemCpy( &pDst[ nDst ], szHeader2, sizeof( szHeader2 ) - sizeof( oex::oexTCHAR ) );
		nDst += sizeof( szHeader2 ) - sizeof( oex::oexTCHAR );

		// Open close positions
		oex::CStr::t_size nOpen = 0, nClose = 0;

		// Block type
		oex::oexINT nType = 0;

		// Open tag for locals
		oex::oexTCHAR tcOpen[] = oexT( "<?c" );
		oex::CStr::t_size szOpen = (oex::CStr::t_size)oex::zstr::Length( tcOpen );

		// Open tag for globals
		oex::oexTCHAR tcOpenG[] = oexT( "<?global" );
		oex::CStr::t_size szOpenG = (oex::CStr::t_size)oex::zstr::Length( tcOpenG );

		// Close tag
		oex::oexTCHAR tcClose[] = oexT( "?>" );
		oex::CStr::t_size szClose = (oex::CStr::t_size)oex::zstr::Length( tcClose );

		// First let's strip the global code and put it at the top
		while ( ( nPos + szOpenG + szClose ) < szSrc )
		{
			// Ensure we have space
			if ( ( sDst.Size() - nDst ) < ( ( szSrc - nPos ) * 3 ) + nOverhead )
				pDst = sDst.OexAllocate( sDst.Size() * 3 );

			// Initialize
			nStart = nPos;
			nOpen = nClose = szSrc;

			// Attempt to find an open bracket
			while ( nOpen == szSrc && ( nPos + szOpen + szClose ) < szSrc )
				if ( !oexMemCmp( &pSrc[ nPos ], tcOpenG, szOpenG ) )
					nOpen = nPos;
				else
					nPos++;

			// Find a closing bracket
			while ( nClose == szSrc && ( nPos + szClose ) < szSrc )
				if ( !oexMemCmp( &pSrc[ nPos ], tcClose, szClose ) )
					nClose = nPos;
				else
					nPos++;

			// Did we find embedded code?
			if ( nOpen < szSrc && nClose < szSrc )
			{
				// Skip the open bracket
				nOpen += szOpenG;

				// Save any code?
				if ( nOpen < nClose )
					sGlobal.Append( &pSrc[ nOpen ], nClose - nOpen );

				// Point to data after code
				nPos += szClose;

			} // end if

		} // end while

		// Now let's process the rest of the script
		nPos = 0; nStart = 0;
		while ( ( nPos + szOpenG + szClose ) < szSrc )
		{
			// Ensure we have space
			if ( ( sDst.Size() - nDst ) < ( ( szSrc - nPos ) * 2 ) + nOverhead )
				pDst = sDst.OexAllocate( sDst.Size() * 2 );

			// Initialize
			nStart = nPos;
			nOpen = nClose = szSrc;

			// Attempt to find an open bracket
			while ( nOpen == szSrc && ( nPos + szOpen + szClose ) < szSrc )
				if ( !oexMemCmp( &pSrc[ nPos ], tcOpen, szOpen ) )
					nOpen = nPos, nType = 0;
				else if ( !oexMemCmp( &pSrc[ nPos ], tcOpenG, szOpenG ) )
					nOpen = nPos, nType = 1;
				else
					nPos++;

			// Find a closing bracket
			while ( nClose == szSrc && ( nPos + szClose ) < szSrc )
				if ( !oexMemCmp( &pSrc[ nPos ], tcClose, szClose ) )
					nClose = nPos;
				else
					nPos++;

			// Did we find embedded code?
			if ( nOpen < szSrc && nClose < szSrc )
			{
				// Text data to be copied?
				if ( nStart < nOpen )
				{	oexMemCpy( &pDst[ nDst ], szOpenStr, sizeof( szOpenStr ) - sizeof( oex::oexTCHAR ) );
					nDst += sizeof( szOpenStr ) - sizeof( oex::oexTCHAR );

					T s = oexCppEncode( T( &pSrc[ nStart ], nOpen - nStart ) );
					oexMemCpy( &pDst[ nDst ], s.Ptr(), s.Length() );
					nDst += s.Length(); nStart += nOpen - nStart;

					oexMemCpy( &pDst[ nDst ], szCloseStr, sizeof( szCloseStr ) - sizeof( oex::oexTCHAR ) );
					nDst += sizeof( szCloseStr ) - sizeof( oex::oexTCHAR );

				} // end if

				// Any code to copy?
				nOpen += szOpen;
				if ( !nType && nOpen < nClose )
				{
					// Add new line
					oexMemCpy( &pDst[ nDst ], oexNL, sizeof( oexNL ) - sizeof( oex::oexTCHAR ) );
					nDst += sizeof( oexNL ) - sizeof( oex::oexTCHAR );

					// Copy the code
					oexMemCpy( &pDst[ nDst ], &pSrc[ nOpen ], nClose - nOpen );
					nDst += nClose - nOpen;

					// Close statement, just in case
					pDst[ nDst++ ] = oexT( ';' );

				} // endif

				// Point to data after code
				nPos += szClose;

			} // end if

		} // end while

		// Copy whatever remains
		if ( nStart < szSrc )
		{	oexMemCpy( &pDst[ nDst ], szOpenStr, sizeof( szOpenStr ) - sizeof( oex::oexTCHAR ) );
			nDst += sizeof( szOpenStr ) - sizeof( oex::oexTCHAR );
			T s = oexCppEncode( T( &pSrc[ nStart ], nOpen - nStart ) );
			oexMemCpy( &pDst[ nDst ], s.Ptr(), s.Length() );
			nDst += s.Length(); nStart += nOpen - nStart;
			oexMemCpy( &pDst[ nDst ], szCloseStr, sizeof( szCloseStr ) - sizeof( oex::oexTCHAR ) );
			nDst += sizeof( szCloseStr ) - sizeof( oex::oexTCHAR );
		} // end if

		// Copy footer into buffer
		oexMemCpy( &pDst[ nDst ], szFooter, sizeof( szFooter ) - sizeof( oex::oexTCHAR ) );
		nDst += sizeof( szFooter ) - sizeof( oex::oexTCHAR );

		// Set the number of bytes in the string
		sDst.SetLength( nDst );

		// Build the full thing and return
		return sGlobal << sDst;
	}



int process(int argc, char* argv[])
{_STT();

	if ( 2 > argc )
	{	oexEcho( oexT( "Use : join <output file> [file1] [file2] ..." ) );
		return -1;
	} // end if

	// Parse the command line params
	oex::CPropertyBag pb = oex::CParser::ParseCommandLine( argc, (const char**)argv );
//	oexEcho( oexMks( oexT( "Command Line = " ), pb.PrintR() ).Ptr() );

	oex::CStr sOut = pb[ oexT( "0" ) ].ToString();
	if ( !sOut.Length() )
	{	oexEcho( oexT( "output directory not specified" ) );
		return -2;
	} // end if
	
	pb.Unset( oexT( "0" ) );

	if ( !oexExists( sOut.Ptr() ) && !oexCreatePath( sOut.Ptr() ) )
	{	oexEcho( oexMks( oexT( "Failed to create path : " ), sOut ).Ptr() );
		return -3;
	} // end if

	// Process each input directory
	for ( oex::CPropertyBag::iterator it; pb.List().Next( it ); )
	{
		TRecursiveFindFiles< 16 > rff;
		if ( rff.FindFirst( it->ToString().Ptr(), "*" ) )
			do
			{
				if ( !rff.IsDirectory() )
				{
					if ( rff.GetFileName().GetFileExtension() == oexT( "htm" ) )
					{	CStr s = prepare_inline( sOut.GetFileName() << oexT( "_" ) << rff.GetFileName().RemoveFileExtension(), rff.GetFullPath(), 1 );
						oexFilePutContents( oexBuildPath( sOut, rff.GetFileName().RemoveFileExtension() << oexT( ".cpp" ) ).Ptr(), s );
					} // end if

				} // end if

			} while ( rff.FindNext() );

	} // end for

	return 0;
}


int main(int argc, char* argv[])
{
    // Initialize the oex library
	oexINIT();

	int ret = process( argc, argv );

	// Unitialize the oex library
    oexUNINIT();

	return ret;
}

*/