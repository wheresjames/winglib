
#include "stdafx.h"
#include "stdio.h"

int main(int argc, char* argv[])
{
    // Initialize the oex library
	oexINIT();

	// Start a log file
	oexNOTICE( 0, "Application startup" );

	oex::os::CSys::printf( "Starting...\n" );

	int nRet = oex::os::CService::Run( oexGetModulePath( oexT( "libsrvmod.so" ) ), oexNULL, oexNULL );

	if ( 0 > nRet )
		return -1;

	else if ( 0 < nRet )
	{	oexNOTICE( 0, "Return from parent..." );
		oex::os::CSys::printf( "Return from parent...\n" );
	} // end else if

	else
	{	oexNOTICE( 0, "Return from child..." );
		oex::os::CSys::printf( "Return from child...\n" );
	} // end else

	// Initialize the oex library
    oexUNINIT();

	return 0;
}

