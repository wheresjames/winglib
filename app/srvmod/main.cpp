
#include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "errno.h"

extern "C" oex::oexBOOL SRV_Start( oex::oexCSTR x_pPath )
{
    // Initialize the oex library
	oexINIT();

	if ( oexCHECK_PTR( x_pPath ) && *x_pPath )
		oex::CLog::GlobalLog().OpenLogFile( oexNULL, oex::CStr( x_pPath ).GetFileName().Ptr(), oexT( ".module.debug.log" ) );
	else
		oex::CLog::GlobalLog().OpenLogFile( oexNULL, oexNULL, oexT( ".module.debug.log" ) );

	// Start a log file
	oexNOTICE( 0, "Module startup" );

	// Uninitialize the oex library
	oexUNINIT();

	return 0;
}

extern "C" oex::oexBOOL SRV_GetModuleInfo( oex::os::service::SSrvInfo *pDi )
{
	if ( !oexCHECK_PTR( pDi ) )
	{	oexERROR( EINVAL, "Invalid function argument" );
		return 0;
	} // end if

	// Clear structure
	oexZeroMemory( pDi, sizeof( oex::os::service::SSrvInfo ) );

	// Module name
	strncpy( pDi->szName, "Test Module", sizeof( pDi->szName ) - 1 );

	// Module description
	strncpy( pDi->szDesc, "This is just a test module", sizeof( pDi->szDesc ) - 1 );

	// pDi->guidType = ;

	// pDi->guidId = ;

	// Create random guid
	oexStringToGuid( &pDi->guidInstance );

	// Set version
	pDi->lVer = oexVERSION( 1, 0 );

	return 1;
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
