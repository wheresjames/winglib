
#include "stdafx.h"
#include "stdio.h"

/*
	you put your eyes in your pocket,
	and your nose on the ground,
	there ought to be a law,
	against you coming around
*/

// Include statically linked modules
#include "sqbind_static_build.hpp"

/// Pointer to script thread
sqbind::CScriptThread	*g_psqScriptThread = oexNULL;

/// Pointer to module manager
sqbind::CModuleManager	*g_psqModuleManager = oexNULL;

int run( oex::CPropertyBag &pbCmdLine )
{_STT();

	oex::CStr sCmd = pbCmdLine[ 0 ].ToString();

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

		else
		{
			sSettings = oexGetModuleFileName() << oexT( ".cfg.txt" );
			if ( oex::CFile::Exists( sSettings.Ptr() ) )
			{
				// Decode settings file
				oex::CPropertyBag pb = oex::CParser::DecodeIni( oexMbToStr( oex::CFile().OpenExisting( sSettings.Ptr() ).Read() ) );

				// Command line?
				if ( pb.IsKey( oexT( "cmd" ) ) )
					sCmd = pb[ oexT( "cmd" ) ].ToString();

			} // end if

		} // end else

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
		{	oexEcho( oexMks( oexT( "Script not found : " ), sCmd ).Ptr() );
			return oexERROR( -2, oexMks( oexT( "Script not found : " ), sCmd ) );
		} // end else

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

	g_psqScriptThread->SetScriptName( sCmd.Ptr() );

	g_psqScriptThread->SetModuleManager( g_psqModuleManager );

	g_psqScriptThread->SetScript( sCmd.Ptr(), oex::oexTRUE );

	g_psqScriptThread->SetExportFunction( SQBIND_Export_Symbols, oexNULL );

	if ( g_psqScriptThread->Start() )
		return oexERROR( -5, oexT( "Failed to start script thread" ) );

	// Attempt to execute idle function
	oex::oexINT gc = 0;
	while ( g_psqScriptThread->IsRunning() )
	{
		// Clean up binary shares
		if ( gc ) gc--; else { gc = 10; oexCleanupBin(); }

		// Wait
		oexSleep( 100 );

	} // end while

	oexNOTICE( 0, oexT( "Script thread has terminated" ) );

	if ( oexCHECK_PTR( g_psqScriptThread ) )
	{	g_psqScriptThread->Destroy();
		OexAllocDelete( g_psqScriptThread );
	} // end if

	if ( oexCHECK_PTR( g_psqModuleManager ) )
	{	g_psqModuleManager->Destroy();
		OexAllocDelete( g_psqModuleManager );
	} // end if

	return 0;
}

int run(int argc, char* argv[])
{
	// Parse the command line
	oex::CPropertyBag pbCmdLine = oex::CParser::ParseCommandLine( argc, (const char**)argv );

	// Check for version request
	if ( pbCmdLine.IsKey( oexT( "version" ) ) )
	{	oexEcho( oexVersion().Ptr() );		
		pbCmdLine.Destroy();
	    oexUNINIT();
		return 0;					  
	} // end if
	
	// Enable crash reporting
	_STT_SET_NAME( oexT( "Main Thread" ) );
	oexEnableCrashReporting( oexNULL, oexT( "logs" ) );

	// Initialize sockets
    if ( !oex::os::CIpSocket::InitSockets() )
    	oexERROR( 0, oexT( "Unable to initialize sockets" ) );

	// Start a log file
	oexNOTICE( 0, oexT( "Application startup" ) );

	// Run the app
	int nRet = run( pbCmdLine );

	oexNOTICE( 0, oexT( "Shutting down..." ) );

	// Uninitialize sockets
    oex::os::CIpSocket::UninitSockets();

	return nRet;
}

int main(int argc, char* argv[])
{
    // Initialize the oex library
	oexINIT();

	// Run the app
	int nRet = run( argc, argv );

	// Uninitialize the oex library
    oexUNINIT();

	return nRet;
}

