// join.cpp

#include "stdafx.h"

using namespace oex;

int process(int argc, char* argv[])
{
	if ( 2 > argc )
	{	oexEcho( oexT( "Use : join <output file> [file1] [file2] ..." ) );
		return -1;
	} // end if

	// Parse the command line params
	oex::CPropertyBag pb = oex::CParser::ParseCommandLine( argc, argv );
//	oexEcho( oexMks( oexT( "Command Line = " ), pb.PrintR() ).Ptr() );

	oex::CStr sOut = pb[ oexT( "0" ) ].ToString();
	if ( !sOut.Length() )
	{	oexEcho( oexT( "output file not specified" ) );
		return -2;
	} // end if
	
	pb.Unset( oexT( "0" ) );

	CFile f;
	if ( !f.CreateAlways( sOut.Ptr() ).IsOpen() )
	{	oexEcho( oexMks( oexT( "Could not open file for writing : " ), sOut ).Ptr() );
		return -2;
	} // end if

	// Join the rest of the files together
	for ( oex::CPropertyBag::iterator it; pb.List().Next( it ); )
	{
		CFile i;
		if ( !i.OpenExisting( it->ToString().Ptr() ).IsOpen() )
			oexEcho( oexMks( oexT( "Could not open file for reading : " ), it->ToString() ).Ptr() );
		else
			f.Write( i.Read() );

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

