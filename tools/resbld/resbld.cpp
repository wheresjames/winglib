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
{
//	oexCHAR8 *_p = ;
//	oexINT _l = ;
//	oexEcho( oexMbToStr( oex::zip::CUncompress::Uncompress( oex::CStr8( _p, _l ) ) ).Ptr() );

	if ( !oexExists( sIn.Ptr() ) )
	{	oexEcho( oexMks( oexT( "No such file : " ), sIn ).Ptr() );
		return -2;
	} // end if

	// Do we need to create a name?
	if ( !sOut.Length() )
		sOut << sIn.RemoveFileExtension() << oexT( ".cpp" );

	oexEcho( sOut.GetFileName().Ptr() );

	// Read in and compress the file
    oex::CStr8 sData = oex::zip::CCompress::Compress( CFile().OpenExisting( sIn.Ptr() ).Read() );

	// Create a variable name based on the md5 hash
	if ( !sVar.Length() )
		sVar << oexT( "g_oexres_" ) << oexMd5( sIn );

	if ( !sPre.Length() )
		sPre << oexT( "char " ) << sVar << oexT( "[] = \r\n{\r\n\t" );

	if ( !sSuf.Length() )
		sSuf = oexT( "\r\n\t0\r\n};\r\n" );

	if ( sInc.Length() )
	{
		oex::CFile i;
		if ( !i.OpenAlways( sInc.Ptr() ).IsOpen() )
		{	oexEcho( oexMks( oexT( "Unable to open include file for writing : " ), sInc ).Ptr() );
			return -2;
		} // end if

		i.SetPtrPosEnd( 0 );

		// Add to include
		i.Write( CStr8() << "\r\nextern unsigned long " << oexStrToMb( sVar ) << "_len;"
				 	     << "\r\nextern char " << sVar << "[];\r\n" );

	} // end if

	oex::CFile f;
	if ( !f.CreateAlways( sOut.Ptr() ).IsOpen() )
	{	oexEcho( oexMks( oexT( "Unable to create output file : " ), sOut ).Ptr() );
		return -3;
	} // end if

	oexCONST oexUCHAR *p = (oexCONST oexUCHAR*)sData.Ptr();
	oexULONG u = sData.Length();

	// Write out the data length
	f.Write( ( CStr8() << "\r\nunsigned long " << oexStrToMb( sVar ) << "_len = " << u << ";\r\n" ) );

	// Write out the data
	f.Write( sPre );

	CStr8 buf;
	oexUCHAR *t = (oexUCHAR*)"0x.., ";
	oexUCHAR *r = (oexUCHAR*)"0x..,\r\n\t";
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

int process(int argc, char* argv[])
{
	if ( 2 > argc )
	{	oexEcho( oexT( "Use : resbld <file>" ) );
		return -1;
	} // end if

	// Parse the command line params
	oex::CPropertyBag pb = oex::CParser::ParseCommandLine( argc, argv );
//	oexEcho( oexMks( oexT( "Command Line = " ), pb.PrintR() ).Ptr() );

	int ret = create_res( pb[ oexT( "0" ) ].ToString(),
						  pb[ oexT( "1" ) ].ToString(),
						  pb[ oexT( "v" ) ].ToString(),
						  pb[ oexT( "p" ) ].ToString(),
						  pb[ oexT( "s" ) ].ToString(),
						  pb[ oexT( "i" ) ].ToString() );

	return ret;
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

