// sqrbld.cpp

#include "stdafx.h"

using namespace oex;

void chtoa( oexUCHAR *b, oexUCHAR ch )
{
	oexUCHAR c = ch & 0x0f;

	if ( 9 >= c )
		b[ 1 ] = '0' + c;
	else
		b[ 1 ] = 'a' + ( c - 10 );

	c = ( ch >> 4 ) & 0x0f;
	if ( 9 >= c )
		b[ 0 ] = '0' + c;
	else
		b[ 0 ] = 'a' + ( c - 10 );
}

// +++ Someone please come back later and make prepare_cpp() more efficient

/// This function turns html with embedded c++ code inside out
/**

  @param[in] sName		- Namespace / function name
  @param[in] sScript	- File or script data
  @param[in] bFile		- Non-zero if sScript is a file name

  This is an example of what an input script to this function might look like
  @code
	<html>
		<head>

			<?global

				// This will be a global c++ function
				int add( a, b )
				{
					return a + b;
				}
			?>

		</head>
		<body>

			<b><big>Plain old HTML stuff</big></b>

			<?c
				out << "This will be inline c++ code";

				out << "1 + 7 = " << add( 1, 7 );

				// The globals are available no matter where declared
				out << say_something();
			?>

		   <?global

			   // The global tag can be anywhere
			   char* say_something()
			   {
				   return "Something";
			   }
		   ?>

		</body>
	</html>
  @endcode

*/
template < typename T >
	CStr prepare_cpp( const T &sName, const T &sScript, int bFile )
	{_STT();

		// Code header / footer
		static oex::oexTCHAR szHeader1[] = oexNL oexNL oexT( "static int f_" );
		static oex::oexTCHAR szHeader2[] = oexT( "( oex::CPropertyBag &in, oex::CStr &out )" ) oexNL oexT( "{" ) oexNL;
		static oex::oexTCHAR szFooter[] = oexNL oexT( "return 0; " ) oexNL oexT( "}" ) oexNL oexT( "};" );
		static oex::oexTCHAR szOpenStr[] = oexNL oexT( "out << oexT( \"" );
		static oex::oexTCHAR szCloseStr[] = oexT( "\" );" ) oexNL;
		static oex::CStr::t_size nOverhead = (oex::CStr::t_size)
												 sizeof( szHeader1 ) + sizeof( szHeader2 )
												 + sName.Length() + sizeof( szFooter )
												 + sizeof( szOpenStr ) + sizeof( szCloseStr );

		// Global name space
		T sInc, sGlobal, sEnd;
		sGlobal << oexNL << oexT( "#include \"oexres.h\"" ) << oexNL
				<< oexT( "#include \"oexlib.h\"" ) << oexNL
				<< oexT( "namespace ns_" ) << sName << oexT( " {" ) << oexNL;
		sEnd << oexNL << oexT( "void* fn_") << sName <<
						 oexNL << oexT( "\t = (void*)&ns_" ) << sName << oexT( "::f_" ) << sName << oexT( ";" ) << oexNL;

		// Get the source data
		oex::CStr sSrc;
		if ( bFile )
			sSrc = oexMbToStr( oexFileGetContents( sScript.Ptr() ) );
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
		oex::CStr::t_size nOpen = 0, nClose = 0, nSkip = 0;

		// Block type
		oex::oexINT nType = 0;

		// Open tag for locals
		oex::oexTCHAR tcOpen[] = oexT( "<?c" );
		oex::CStr::t_size szOpen = (oex::CStr::t_size)oex::zstr::Length( tcOpen );

		// Open tag for locals
		oex::oexTCHAR tcOpenI[] = oexT( "<?inc" );
		oex::CStr::t_size szOpenI = (oex::CStr::t_size)oex::zstr::Length( tcOpenI );

		// Open tag for globals
		oex::oexTCHAR tcOpenG[] = oexT( "<?global" );
		oex::CStr::t_size szOpenG = (oex::CStr::t_size)oex::zstr::Length( tcOpenG );

		// Close tag
		oex::oexTCHAR tcClose[] = oexT( "?>" );
		oex::CStr::t_size szClose = (oex::CStr::t_size)oex::zstr::Length( tcClose );

		// Inc section goes outside the namespace
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
				if ( !oexMemCmp( &pSrc[ nPos ], tcOpenI, szOpenI ) )
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
				nOpen += szOpenI;

				// Save any code?
				if ( nOpen < nClose )
					sInc.Append( &pSrc[ nOpen ], nClose - nOpen );

				// Point to data after code
				nPos += szClose;
				nStart = nPos;

			} // end if

		} // end while

		// Global section is inside the namespace
		nPos = 0; nStart = 0;
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
				nStart = nPos;

			} // end if

		} // end while

		// Now let's process the rest of the script
		nPos = 0; nStart = 0;
		while ( ( nPos + szOpen + szClose ) < szSrc )
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
					nOpen = nPos, nType = 0, nSkip = szOpen;
				else if ( !oexMemCmp( &pSrc[ nPos ], tcOpenI, szOpenI ) )
					nOpen = nPos, nType = 1, nSkip = szOpenI;
				else if ( !oexMemCmp( &pSrc[ nPos ], tcOpenG, szOpenG ) )
					nOpen = nPos, nType = 2, nSkip = szOpenG;
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
				nOpen += nSkip;
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
				nStart = nPos;

			} // end if

		} // end while

		// Copy whatever remains
		if ( nStart < szSrc )
		{	oexMemCpy( &pDst[ nDst ], szOpenStr, sizeof( szOpenStr ) - sizeof( oex::oexTCHAR ) );
			nDst += sizeof( szOpenStr ) - sizeof( oex::oexTCHAR );
			T s = oexCppEncode( T( &pSrc[ nStart ], szSrc - nStart ) );
			oexMemCpy( &pDst[ nDst ], s.Ptr(), s.Length() );
			nDst += s.Length(); nStart += szSrc - nStart;
			oexMemCpy( &pDst[ nDst ], szCloseStr, sizeof( szCloseStr ) - sizeof( oex::oexTCHAR ) );
			nDst += sizeof( szCloseStr ) - sizeof( oex::oexTCHAR );
		} // end if

		// Copy footer into buffer
		oexMemCpy( &pDst[ nDst ], szFooter, sizeof( szFooter ) - sizeof( oex::oexTCHAR ) );
		nDst += sizeof( szFooter ) - sizeof( oex::oexTCHAR );

		// Set the number of bytes in the string
		sDst.SetLength( nDst );

		// Build the full thing and return
		return sInc << sGlobal << sDst << sEnd;
	}


int to_binary( oex::CFile &f, oex::CStr8 &sData, oex::CStr &sVar, oex::CStr &sPre, oex::CStr &sSuf )
{_STT();

	oexCONST oexUCHAR *p = (oexCONST oexUCHAR*)sData.Ptr();
	oexULONG u = sData.Length();

	// Write out the data length
	f.Write( ( CStr8() << oexNL8 "unsigned long " << oexStrToMb( sVar ) << "_len = " << u << ";" oexNL8 ) );

	// Write out the data
	f.Write( sPre );

	CStr8 buf;
	oexUCHAR *t = (oexUCHAR*)"0x.., ";
	oexINT tl = oex::zstr::Length( t );
	oexUCHAR *r = (oexUCHAR*)"0x..," oexNL8 "\t ";
	oexINT rl = oex::zstr::Length( r );
	oexUCHAR *b = (oexUCHAR*)buf.OexAllocate( 1024 ), *s;
	oexINT bl = 0;
	for ( oexULONG i = 0; i < u; i++ )
	{
		s = &b[ bl ];

		if ( !( ( i + 1 ) & 0xf ) )
			oexMemCpy( s, r, rl ), bl += rl;
		else
			oexMemCpy( s, t, tl ), bl += tl;

		chtoa( &s[ 2 ], (oexUCHAR)p[ i ] );

		if ( 1000 < bl )
			f.Write( b, bl ), bl = 0;

	} // end for

	if ( bl )
		f.Write( b, bl );

	f.Write( sSuf );

	return 1;
}


int create_res( oex::CStr sIn, oex::CStr sOut, oex::CStr sVar, oex::CStr sPre, oex::CStr sSuf, oex::CStr sInc, oex::CStrList &lstSq, oex::CStrList &lstCii )
{_STT();

//	oexCHAR8 *_p = ;
//	oexINT _l = ;
//	oexEcho( oexMbToStr( oex::zip::CUncompress::Uncompress( oex::CStr8( _p, _l ) ) ).Ptr() );

	oexINT nType = 0;

	if ( !oexExists( sIn.Ptr() ) )
	{	oexEcho( oexMks( oexT( "No such file : " ), sIn ).Ptr() );
		return -20;
	} // end if

	// Do we need to create a name?
	if ( !sOut.Length() )
		sOut << sIn.RemoveFileExtension() << oexT( ".cpp" );

	oexEcho( ( oex::CStr() << sIn.GetFileName() << oexT( " -> " ) << sOut.GetFileName() ).Ptr() );

	// Read in and compress the file
	oex::CStr8 sData;
	
	// Squirrel script?
	if ( oex::CParser::MatchPatterns( sIn, lstSq, oex::oexTRUE ) )
	{
		nType = 1;

		// Check for inline extension
		oex::oexBOOL bInline = sIn.GetFileExtension() == oexT( "squ" );

		sqbind::CSqEngine se;
		if ( se.Load( sIn.Ptr(), oexNULL, oex::oexTRUE, oex::oexFALSE, oex::oexFALSE, bInline ) )
			sData = oex::zip::CCompress::Compress( se.GetCompiledScript() );

	} // end if

	// Inline c++ file?
	else if ( oex::CParser::MatchPatterns( sIn, lstCii, oex::oexTRUE ) )
	{	nType = 2;
		sData = oexStrToMb( prepare_cpp( sVar, sIn, 1 ) );
	} // end if

	// Just serialize
	else
		sData = oex::zip::CCompress::Compress( CFile().OpenExisting( sIn.Ptr() ).Read() );

	// Create a variable name based on the md5 hash
	if ( !sVar.Length() )
		sVar << oexT( "g_oexres_" ) << oexMd5( sIn );

	if ( !sPre.Length() )
		sPre << oexT( "char " ) << sVar << oexT( "[] = " ) << oexNL << oexT( "{" ) << oexNL << oexT( "\t" );

	if ( !sSuf.Length() )
		sSuf << oexNL << oexT( "\t0" ) << oexNL << oexT( "};" ) << oexNL;

	if ( sInc.Length() )
	{
		oex::CFile i;
		if ( !i.OpenAlways( sInc.Ptr() ).IsOpen() )
		{	oexEcho( oexMks( oexT( "Unable to open include file for writing : " ), sInc ).Ptr() );
			return -21;
		} // end if

		i.SetPtrPosEnd( 0 );

		if ( 2 == nType )
			i.Write( CStr8() << oexNL8 "extern void* fn_" << oexStrToMb( sVar ) << ";" oexNL8 );
		else
			i.Write( CStr8() << oexNL8 "extern unsigned long " << oexStrToMb( sVar ) << "_len;"
							 << oexNL8 "extern char " << oexStrToMb( sVar ) << "[];" oexNL8 );

	} // end if

	oex::CFile f;
	if ( !f.CreateAlways( sOut.Ptr() ).IsOpen() )
	{	oexEcho( oexMks( oexT( "Unable to create output file : " ), sOut ).Ptr() );
		return -22;
	} // end if

	if ( 2 == nType )
		f.Write( sData );
	else
		to_binary( f, sData, sVar, sPre, sSuf );

	return nType;
}

int from_dir( oex::CStr dir, oex::CStr &sOutDir, oex::CPropertyBag &pb,
			  oex::CStr sPath, oex::CFile *pInc, oex::CFile *pCmp,
			  oex::CFile *pRes, oex::CFile *pDep, oex::CFile *pSym,
			  oex::CStrList &lstSq, oex::CStrList &lstCii )
{_STT();
	// Ensure directory exists
	if ( !oexExists( dir.Ptr() ) )
		return -10;

	oex::CFindFiles ff;
	if ( ff.FindFirst( dir.Ptr(), oexT( "*" ) ) )
		do
		{
			// Build relative path
			oex::CStr sRel = oexBuildPath( sPath, ff.GetFileName() );
//			oexSHOW( sRel );

			// Recurse into directory
			if ( ff.IsDirectory() )
				from_dir( ff.GetFullPath(), sOutDir, pb, sRel, pInc, pCmp, pRes, pDep, pSym, lstSq, lstCii );

			else
			{
				// Create variable name
				oex::CStr sHash = oexMd5( sRel );
				oex::CStr sVar; sVar << oexT( "g_oexres_" ) << sHash;
				oex::CStr sOut = oexBuildPath( sOutDir, sHash << oexT( ".cpp" ) );

//				oexSHOW( ff.GetFullPath() );
//				oexSHOW( sVar );
//				oexSHOW( sOut );

				// Create file
				oexINT nType = create_res( ff.GetFullPath(), sOut, sVar,
									       oexT( "" ), oexT( "" ), oexT( "" ),
									       lstSq, lstCii );
				if ( 0 <= nType )
				{
					if ( pSym )
					{
						if ( 2 == nType )
							pSym->Write( CStr8() << oexNL8 "extern void* fn_" << oexStrToMb( sVar ) << ";" oexNL8 );
						else
							pSym->Write( CStr8() << oexNL8 "extern unsigned long " << oexStrToMb( sVar ) << "_len;"
												 << oexNL8 "extern char " << oexStrToMb( sVar ) << "[];" oexNL8 );
					} // end if

					if ( pCmp )
					{
						if ( 2 == nType )
							pCmp->Write( CStr8() << oexNL8
													"\t{" oexNL8
													"\t\t\"" << oexStrToMb( sRel ) << "\"," oexNL8
													"\t\t0," oexNL8
													"\t\t0," oexNL8
													"\t\tfn_" << oexStrToMb( sVar ) << oexNL8
													"\t}," oexNL8 );
						else
							pCmp->Write( CStr8() << oexNL8
													"\t{" oexNL8
													"\t\t\"" << oexStrToMb( sRel ) << "\"," oexNL8
													"\t\t" << oexStrToMb( sVar ) << "," oexNL8
													"\t\t" << oexStrToMb( sVar ) << "_len," oexNL8
													"\t\t0" oexNL8
													"\t}," oexNL8 );
					} // end if

					if ( pRes )
						pRes->Write( CStr8() << " \\" oexNL8 "\t" << oexStrToMb( sOut ) );

					if ( pDep )
						pDep->Write( CStr8() << oexStrToMb( sOut ) << ": " << oexStrToMb( ff.GetFullPath() ) << oexNL8 oexNL8 );

				} // end if

			} // end else

		} while ( ff.FindNext() );

	return 0;
}

int from_dirs( oex::CStr dir, oex::CStr &sOutDir, oex::CPropertyBag &pb,
			   oex::CStr sPath, oex::CFile *pInc, oex::CFile *pCmp,
			   oex::CFile *pRes, oex::CFile *pDep, oex::CFile *pSym,
			   oex::CStrList &lstSq, oex::CStrList &lstCii )
{_STT();
	int ret = -1;
	oex::CStrList lst = oex::CParser::Split( dir.Ptr(), oexT( " \r\n\t" ) );
	for ( oex::CStrList::iterator it; lst.Next( it ); )
		ret = from_dir( *it, sOutDir, pb, *it, pInc, pCmp, pRes, pDep, pSym, lstSq, lstCii );
	return ret;
}


int process(int argc, char* argv[])
{_STT();
	if ( 2 > argc )
	{	oexEcho( oexT( "Use : sqrbld <file>" ) );
		return -1;
	} // end if

	// Parse the command line params
	oex::CPropertyBag pb = oex::CParser::ParseCommandLine( argc, (const char**)argv );
//	oexEcho( oexMks( oexT( "Command Line = " ), pb.PrintR() ).Ptr() );

	// Check for version request
	if ( pb.IsKey( oexT( "version" ) ) )
	{	oexEcho( oexVersion().Ptr() );		
		return 0;					  
	} // end if

	else if ( pb.IsKey( oexT( "build" ) ) )
	{	oexEcho( oexBuild().Ptr() );		
		return 0;					  
	} // end if

	// File extensions to compile
	oex::CStrList lstSq = oex::CParser::Split( pb[ oexT( "c" ) ].ToString(), oexT( ";" ) );
	oex::CStrList lstCii = oex::CParser::Split( pb[ oexT( "i" ) ].ToString(), oexT( ";" ) );

	// Was an entire directory specified?
	if ( pb.IsKey( oexT( "d" ) ) )
	{
		// Output directory
		oex::CStr sOutDir = pb[ oexT( "o" ) ].ToString();

		// Ensure output directory exists if specified
		if ( sOutDir.Length() )
			if ( !oexExists( sOutDir.Ptr() ) )
				oexCreatePath( sOutDir.Ptr() );

		// Include file?
		oex::CFile fInc;
		if ( fInc.CreateAlways( oexBuildPath( sOutDir, oexT( "oexres.h" ) ).Ptr() ).IsOpen() )
			fInc.Write( CStr8() << oexNL8
				"#define OEX_RESOURCES 1" oexNL8
				"#define OEX_RES_VERSION \"" << oexStrToMb( oexVersion() ) << "\"" oexNL8
				"#define OEX_RES_BUILD \"" << oexStrToMb( oexBuild() ) << "\"" oexNL8
				"#if defined( OEX_NO_RESOURCES )" oexNL8
				"#\terror 'oexres.h' MUST be included BEFORE 'oexlib.h'" oexNL8
				"#endif" oexNL8
				oexNL8
				"struct SOexResourceInfo" oexNL8
				"{" oexNL8
				"\tconst char *    name;" oexNL8
				"\tconst char *    data;" oexNL8
				"\tunsigned long   size;" oexNL8
				"\tconst void *    func;" oexNL8
				"};" oexNL8
				oexNL8
				"extern const SOexResourceInfo _g_oexlib_resources[];" oexNL8
				);

		oex::CFile fCmp;
		if ( fCmp.CreateAlways( oexBuildPath( sOutDir, oexT( "oexres.cpp" ) ).Ptr() ).IsOpen() )
			fCmp.Write( CStr8() << oexNL8
				"#include \"oexres.h\"" oexNL8
				"#include \"oexsym.h\"" oexNL8
				oexNL8
				"const SOexResourceInfo _g_oexlib_resources[] = " oexNL8
				"{" oexNL8
				);

		oex::CFile fRes;
		if ( fRes.CreateAlways( oexBuildPath( sOutDir, oexT( "oexres.mk" ) ).Ptr() ).IsOpen() )
			fRes.Write( CStr8() << oexNL8
				"RES_CPP := \\" oexNL8
				"\t" << oexStrToMb( oexBuildPath( sOutDir, oexT( "oexres.cpp" ) ) )
				);

		oex::CFile fDep;
		if ( fDep.CreateAlways( oexBuildPath( sOutDir, oexT( "oexres.dpp" ) ).Ptr() ).IsOpen() )
			fDep.Write( CStr8() << oexNL8
				);

		oex::CFile fSym;
		if ( fSym.CreateAlways( oexBuildPath( sOutDir, oexT( "oexsym.h" ) ).Ptr() ).IsOpen() )
			fSym.Write( CStr8() << oexNL8
				);

		// Create resources from directory
		int ret = from_dirs( pb[ oexT( "d" ) ].ToString(), sOutDir, pb, oexT( "" ), 
							 &fInc, &fCmp, &fRes, &fDep, &fSym, lstSq, lstCii );

		if ( fRes.IsOpen() )
			fRes.Write( oexNL );

		if ( fCmp.IsOpen() )
			fCmp.Write( CStr8() <<
				oexNL8
				"\t{ 0, 0, 0 }" oexNL8
				oexNL8
				"};" oexNL8
			);

		return ret;

	} // end if

	// Compile single file
	return create_res( pb[ oexT( "0" ) ].ToString(),
					   pb[ oexT( "1" ) ].ToString(),
					   pb[ oexT( "v" ) ].ToString(),
					   pb[ oexT( "p" ) ].ToString(),
					   pb[ oexT( "s" ) ].ToString(),
					   pb[ oexT( "i" ) ].ToString(),
					   lstSq, lstCii );
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

