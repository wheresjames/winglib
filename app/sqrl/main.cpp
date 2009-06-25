
#include "stdafx.h"
#include "stdio.h"

// Include statically linked modules
#include "../sq_modules/static_build.hpp"

/// Pointer to script thread
sqbind::CScriptThread	*g_psqScriptThread = oexNULL;

/// Pointer to module manager
sqbind::CModuleManager	*g_psqModuleManager = oexNULL;

int main(int argc, char* argv[])
{
    // Initialize the oex library
	oexINIT();

	// Initialize sockets
    if ( !oex::os::CIpSocket::InitSockets() )
    	oexERROR( 0, oexT( "Unable to initialize sockets" ) );

	// Start a log file
	oexNOTICE( 0, oexT( "Application startup" ) );

	oex::CStr sCmd;

	if ( argc > 1 && oexCHECK_PTR( argv[ 1 ] ) )
		sCmd = oexMbToStrPtr( argv[ 1 ] );

	// Calculate a module name if not specified
	if ( !sCmd.Length() )
	{
		// Look for a .cfg file
		oex::CStr sSettings = oexGetModuleFileName() << oexT( ".cfg" );
		if ( oex::CFile::Exists( sSettings.Ptr() ) )
		{
			// Decode settings file
			oex::CPropertyBag pb = oex::CParser::DecodeIni( oexMbToStr( oex::CFile().OpenExisting( sSettings.Ptr() ).Read() ) );

			// Command line?
			if ( pb.IsKey( oexT( "cmd" ) ) )
				sCmd = pb[ oexT( "cmd" ) ].ToString();

		} // end if

	} // end if

	// Do we have a script
	if ( !sCmd.Length() )
		return oexERROR( -1, oexT( "Script not specified" ) );

	// Ensure the file exists
	if ( !oexExists( sCmd.Ptr() ) )
	{
		if ( oexExists( oexGetModulePath( oexT( "scripts" ) ).BuildPath( sCmd ).Ptr() ) )
			sCmd = oexGetModulePath( oexT( "scripts" ) ).BuildPath( sCmd );

		else
			return oexERROR( -2, oexMks( oexT( "Script not found : " ), sCmd ) );

	} // end if

	// Create objects
	g_psqScriptThread = OexAllocConstruct< sqbind::CScriptThread >();
	if ( !oexCHECK_PTR( g_psqScriptThread ) )
		return oexERROR( -3, oexT( "Out of memory!" ) );

	g_psqModuleManager = OexAllocConstruct< sqbind::CModuleManager >();
	if ( !oexCHECK_PTR( g_psqModuleManager ) )
		return oexERROR( -4, oexT( "Out of memory!" ) );

	// Log the script name
	oexNOTICE( 0, oexMks( oexT( "Running script : " ), sCmd ) );

	g_psqScriptThread->SetModuleManager( g_psqModuleManager );

	g_psqScriptThread->SetScript( sCmd.Ptr(), oex::oexTRUE );

	g_psqScriptThread->SetExportFunction( SQBIND_Export_Symbols, oexNULL );

	if ( g_psqScriptThread->Start() )
		return oexERROR( -5, oexT( "Failed to start script thread" ) );


	// Attempt to execute idle function
	while ( g_psqScriptThread->IsRunning() )
		oexSleep( 100 );

	oexNOTICE( 0, oexT( "Script thread has terminated" ) );

	if ( oexCHECK_PTR( g_psqScriptThread ) )
	{	g_psqScriptThread->Destroy();
		OexAllocDelete( g_psqScriptThread );
	} // end if

	if ( oexCHECK_PTR( g_psqModuleManager ) )
	{	g_psqModuleManager->Destroy();
		OexAllocDelete( g_psqModuleManager );
	} // end if

	oexNOTICE( 0, oexT( "Shutting down..." ) );

	// Uninitialize sockets
    oex::os::CIpSocket::UninitSockets();

	// Uninitialize the oex library
    oexUNINIT();

	return 0;
}

