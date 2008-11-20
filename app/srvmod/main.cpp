
#include "stdafx.h"
#include "stdio.h"

extern oex::oexBOOL SRV_Start()
{

	return oex::oexTRUE;
}

extern oex::oexBOOL SRV_GetModuleInfo( oex::os::service::SSrvInfo *pDi )
{

	return oex::oexTRUE;
}

/*
int main(int argc, char* argv[])
{
    // Initialize the oex library
	oexINIT();

	// Start a log file
	oexNOTICE( 0, "Application startup" );

	oex::os::CSys::printf( "Starting...\n" );

	int nRet = oex::os::CService::Run( oexGetModulePath( oexT( "test_module.so" ) ) );

	if ( 0 > nRet )
		return -1;

	else if ( 0 < nRet )
		oex::os::CSys::printf( "Return from parent...\n" );

	else
		oex::os::CSys::printf( "Return from child...\n" );

	// Initialize the oex library
    oexUNINIT();

	return 0;
}

*/
