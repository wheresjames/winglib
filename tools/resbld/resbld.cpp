// resbld.cpp

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

int create_res( oex::CStr sIn, oex::CStr sOut, oex::CStr sVar, oex::CStr sPre, oex::CStr sSuf, oex::CStr sInc )
{_STT();
//	oexCHAR8 *_p = ;
//	oexINT _l = ;
//	oexEcho( oexMbToStr( oex::zip::CUncompress::Uncompress( oex::CStr8( _p, _l ) ) ).Ptr() );

	if ( !oexExists( sIn.Ptr() ) )
	{	oexEcho( oexMks( oexT( "No such file : " ), sIn ).Ptr() );
		return -20;
	} // end if

	// Do we need to create a name?
	if ( !sOut.Length() )
		sOut << sIn.RemoveFileExtension() << oexT( ".cpp" );

	oexEcho( ( oex::CStr() << sIn.GetFileName() << oexT( " -> " ) << sOut.GetFileName() ).Ptr() );

	// Read in and compress the file
    oex::CStr8 sData = oex::zip::CCompress::Compress( CFile().OpenExisting( sIn.Ptr() ).Read() );

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

		// Add to include
		i.Write( CStr8() << oexNL8 "extern unsigned long " << oexStrToMb( sVar ) << "_len;"
				 	     << oexNL8 "extern char " << oexStrToMb( sVar ) << "[];" oexNL8 );

	} // end if

	oex::CFile f;
	if ( !f.CreateAlways( sOut.Ptr() ).IsOpen() )
	{	oexEcho( oexMks( oexT( "Unable to create output file : " ), sOut ).Ptr() );
		return -22;
	} // end if

	oexCONST oexUCHAR *p = (oexCONST oexUCHAR*)sData.Ptr();
	oexULONG u = sData.Length();

	// Write out the data length
	f.Write( ( CStr8() << oexNL8 "unsigned long " << oexStrToMb( sVar ) << "_len = " << u << ";" oexNL8 ) );

	// Write out the data
	f.Write( sPre );

	CStr8 buf;
	oexUCHAR *t = (oexUCHAR*)"0x.., ";
	oexUCHAR *r = (oexUCHAR*)"0x..," oexNL8 "\t ";
	oexUCHAR *b = (oexUCHAR*)buf.OexAllocate( 1024 ), *s;
	oexINT bl = 0;
	for ( oexULONG i = 0; i < u; i++ )
	{
		s = &b[ bl ];

		if ( !( ( i + 1 ) & 0xf ) )
			oexMemCpy( s, r, 8 ), bl += 8;
		else
			oexMemCpy( s, t, 6 ), bl += 6;

		chtoa( &s[ 2 ], (oexUCHAR)p[ i ] );

		if ( 1000 < bl )
			f.Write( b, bl ), bl = 0;

	} // end for

	if ( bl )
		f.Write( b, bl );

	f.Write( sSuf );

	return 0;
}

int from_dir( oex::CStr dir, oex::CStr &sOutDir, oex::CPropertyBag &pb,
			  oex::CStr sPath, oex::CFile *pInc, oex::CFile *pCmp,
			  oex::CFile *pRes, oex::CFile *pDep, oex::CFile *pSym )
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
				from_dir( ff.GetFullPath(), sOutDir, pb, sRel, pInc, pCmp, pRes, pDep, pSym );

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
				if ( 0 == create_res( ff.GetFullPath(), sOut, sVar,
									  oexT( "" ), oexT( "" ), oexT( "" ) ) )
				{
					if ( pSym )
						pSym->Write( CStr8() << oexNL8 "extern unsigned long " << oexStrToMb( sVar ) << "_len;"
				 							 << oexNL8 "extern char " << oexStrToMb( sVar ) << "[];" oexNL8 );

					if ( pCmp )
						pCmp->Write( CStr8() << oexNL8
												"\t{" oexNL8
												"\t\t\"" << oexStrToMb( sRel ) << "\"," oexNL8
												"\t\t" << oexStrToMb( sVar ) << "," oexNL8
												"\t\t" << oexStrToMb( sVar ) << "_len" oexNL8
												"\t}," oexNL8 );

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
			   oex::CFile *pRes, oex::CFile *pDep, oex::CFile *pSym )
{_STT();
	int ret = -1;
	oex::CStrList lst = oex::CParser::Split( dir.Ptr(), oexT( " \r\n\t" ) );
	for ( oex::CStrList::iterator it; lst.Next( it ); )
		ret = from_dir( *it, sOutDir, pb, *it, pInc, pCmp, pRes, pDep, pSym );
	return ret;
}


int process(int argc, char* argv[])
{_STT();
	if ( 2 > argc )
	{	oexEcho( oexT( "Use : resbld <file>" ) );
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
				"#if defined( OEX_NO_RESOURCES )" oexNL8
				"#\terror 'oexres.h' MUST be included BEFORE 'oexlib.h'" oexNL8
				"#endif" oexNL8
				oexNL8
				"struct SOexResourceInfo" oexNL8
				"{" oexNL8
				"\tconst char *   name;" oexNL8
				"\tconst char *   data;" oexNL8
				"\tunsigned long  size;" oexNL8
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
		int ret = from_dirs( pb[ oexT( "d" ) ].ToString(), sOutDir, pb, oexT( "" ), &fInc, &fCmp, &fRes, &fDep, &fSym );

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
					   pb[ oexT( "i" ) ].ToString() );
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

