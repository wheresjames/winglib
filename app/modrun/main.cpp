
#include "stdafx.h"
#include "stdio.h"

// sqengine_d.dll ../../winglib/etc/scripts/test_cell.nut

int main(int argc, char* argv[])
{
    // Initialize the oex library
	oexINIT();

	// Initialize sockets
    if ( !oex::os::CIpSocket::InitSockets() )
    	oexERROR( 0, oexT( "Unable to initialize sockets" ) );

	// Start a log file
	oexNOTICE( 0, oexT( "Application startup" ) );

	oex::CStr sModule, sCmd;

	if ( argc > 1 && oexCHECK_PTR( argv[ 1 ] ) )
		sModule = oexMbToStrPtr( argv[ 1 ] );

	// Calculate a module name if not specified
	if ( !sModule.Length() )
	{
		// Look for a .cfg file
		oex::CStr sSettings = oexGetModuleFileName() << oexT( ".cfg" );
		if ( oex::CFile::Exists( sSettings.Ptr() ) )
		{
			// Decode settings file
			oex::CPropertyBag pb = oex::CParser::DecodeIni( oexMbToStr( oex::CFile().OpenExisting( sSettings.Ptr() ).Read() ) );

			// Module specified?
			if ( pb.IsKey( oexT( "module" ) ) )
				sModule = pb[ oexT( "module" ) ].ToString().DecorateName( oex::oexTRUE, oex::oexTRUE );

			// Command line?
			if ( pb.IsKey( oexT( "cmd" ) ) )
				sCmd = pb[ oexT( "cmd" ) ].ToString();

		} // end if

		// Use the name of the default module
		else if ( !sModule.Length() )
			sModule = oexMks( oexT( "srvmod" ) ).DecorateName( oex::oexTRUE, oex::oexTRUE );

	} // end if

	oexPrintf( oexT( "Starting...\n" ) );

	if ( !sCmd.Length() && argc > 2 && oexCHECK_PTR( argv[ 2 ] ) )
		sCmd = oexMbToStrPtr( argv[ 2 ] );

	int nRet = oex::os::CService::RunModule( oexGetModulePath().BuildPath( sModule.Ptr() ), sCmd, oexNULL, oexNULL );

	if ( 0 > nRet )
	{	oexERROR( nRet, oexT( "Failed to start service module..." ) );
		oexPrintf( oexT( "Failed to start service module %s\n" ), oexStrToMb( sModule ).Ptr() );
	} // end if

	oexNOTICE( 0, oexT( "Shutting down..." ) );

	// Uninitialize sockets
    oex::os::CIpSocket::UninitSockets();

	// Uninitialize the oex library
    oexUNINIT();

	return 0;
}

