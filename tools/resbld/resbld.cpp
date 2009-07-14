// resbld.cpp

#include "stdafx.h"

using namespace oex;

int process(int argc, char* argv[])
{
	if ( 2 > argc )
	{	oexEcho( oexT( "Use : resbld <file>" ) );
		return -1;
	} // end if

	// Parse the command line params
	oex::CPropertyBag pb = oex::CParser::ParseCommandLine( argc, argv );
//	oexEcho( pb.PrintR().Ptr() );

	oex::CStr sFile = pb[ oexT( "0" ) ].ToString();

	if ( !oexExists( sFile.Ptr() ) )
	{	oexEcho( oexMks( oexT( "No such file : " ), sFile ).Ptr() );
		return -2;
	} // end if

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

