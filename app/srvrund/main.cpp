
#include "stdafx.h"
#include "stdio.h"

int main(int argc, char* argv[])
{
    // Initialize the oex library
	oexINIT();

	// Start a log file
	oexNOTICE( 0, "Application startup" );

	oex::CStr sModule;

	if ( argc > 2 )
		sModule = argv[ 1 ];

	if ( !sModule.Length() )
#if defined( _DEBUG )
#	if defined( OEX_WIN32 )
		sModule = oexT( "srvmod_d.dll" );
#	else
		sModule = oexT( "srvmod_d.so" );
#	endif
#else
#	if defined( OEX_WIN32 )
		sModule = oexT( "srvmod.dll" );
#	else
		sModule = oexT( "srvmod.so" );
#	endif
#endif

	oex::os::CSys::printf( "Starting...\n" );

	int nRet = oex::os::CService::Run( oexGetModulePath().BuildPath( sModule.Ptr() ), oexNULL, oexNULL );

	if ( 0 <= nRet )
	{	oexERROR( 0, "Failed to start service module..." );
		oex::os::CSys::printf( "Failed to start service module...\n" );
	} // end if

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

