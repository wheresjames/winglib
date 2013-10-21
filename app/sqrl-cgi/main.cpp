
#include "stdafx.h"
#include "stdio.h"

// Terminal comes up in text mode on windows
#if defined( _WIN32 ) || defined( WIN32 )
#	include <io.h>
#	include <fcntl.h>
#endif

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

	oex::oexBOOL bFile = oex::oexTRUE;
	oex::CStr sCmd = pbCmdLine[ 0 ].ToString();
	oex::oexBOOL bInline = !pbCmdLine.IsKey( oexT( "I" ) );

	// Calculate a module name if not specified
	if ( !sCmd.Length() )
	{
		// Raw script on the command line?
		if ( pbCmdLine.IsKey( oexT( "s" ) ) )
			bFile = oex::oexFALSE, sCmd = pbCmdLine[ oexT( "s" ) ].ToString();

		else if ( pbCmdLine.IsKey( oexT( "script" ) ) )
			bFile = oex::oexFALSE, sCmd = pbCmdLine[ oexT( "script" ) ].ToString();

		else
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

		} // end else

	} // end if

	// Stdin?
	if ( !sCmd.Length() )
		sCmd = oexReadStdin(), bFile = oex::oexFALSE;

	// Do we have a script
	if ( !sCmd.Length() )
		return oexERROR( -1, oexT( "Script not specified" ) );

	// Ensure the file exists
	if ( bFile && !oexExists( sCmd.Ptr() ) )
	{
		if ( oexExists( oexGetModulePath( oexT( "scripts" ) ).BuildPath( sCmd ).Ptr() ) )
			sCmd = oexGetModulePath( oexT( "scripts" ) ).BuildPath( sCmd );

		else
		{	oexEcho( oexMks( oexT( "Script not found : " ), sCmd ).Ptr() );
			return oexERROR( -2, oexMks( oexT( "Script not found : " ), sCmd ) );
		} // end else

	} // end if

	// Check for non-inline type
	if ( bFile && sCmd.GetFileExtension().ToLower() == oexT( "nut" ) )
		bInline = oex::oexFALSE;

	// Create objects
	g_psqScriptThread = OexAllocConstruct< sqbind::CScriptThread >();
	if ( !oexCHECK_PTR( g_psqScriptThread ) )
		return oexERROR( -3, oexT( "Out of memory!" ) );

	g_psqModuleManager = OexAllocConstruct< sqbind::CModuleManager >();
	if ( !oexCHECK_PTR( g_psqModuleManager ) )
		return oexERROR( -4, oexT( "Out of memory!" ) );

	// Log the script name
	oexNOTICE( 0, oexMks( oexT( "Running script : " ), sCmd ) );

	sqbind::CScriptThread::SetAppInfo( oexAppNamePtr(), oexAppNameProcPtr(), oexAppLongNamePtr(), oexAppDescPtr() );

	g_psqScriptThread->setInline( bInline );

	g_psqScriptThread->SetScriptName( bFile ? sCmd.Ptr() : oexT( "buffer" ) );

	g_psqScriptThread->SetModuleManager( g_psqModuleManager );

	g_psqScriptThread->SetScript( oexStrToMb( sCmd ), bFile );

	g_psqScriptThread->SetExportFunction( SQBIND_Export_Symbols, oexNULL );

	g_psqScriptThread->Pb()[ oexT( "cmdline" ) ] = pbCmdLine.Copy();

	// Process the script
	if ( bInline )
	{
		// Initialize the engine
		if ( !g_psqScriptThread->InitEngine() || !g_psqScriptThread->GetEngine()->Init() )
			return oexERROR( -5, oexT( "Engine failed to initialize" ) );

		sqbind::stdString sRet;
		if ( bFile )
			sRet = g_psqScriptThread->GetEngine()->include_inline( sqbind::oex2std( sCmd ), oexNULL );
		else
			sRet = g_psqScriptThread->GetEngine()->run_inline( sqbind::oex2std( sCmd ), oexNULL );

		// Echo output
		oexEcho( sRet.c_str(), sRet.length() );

	} // end if

	else
	{
		// Start the thread
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

	} // end else

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

	// Check for version request
	else if ( pbCmdLine.IsKey( oexT( "build" ) ) )
	{	oexEcho( oexBuild().Ptr() );
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

	// Show command line input
	if ( pbCmdLine.IsKey( oexT( "show" ) ) )
		oexEcho( pbCmdLine.PrintR().Ptr() );

	// Pause before starting
	if ( pbCmdLine.IsKey( oexT( "pause" ) ) )
		oexSleep( pbCmdLine[ oexT( "pause" ) ].ToInt() );

	// Run the app
	int nRet = run( pbCmdLine );

	// Wait at the end
	if ( pbCmdLine.IsKey( oexT( "pause_end" ) ) )
		oexSleep( pbCmdLine[ oexT( "pause_end" ) ].ToInt() );

	oexNOTICE( 0, oexT( "Shutting down..." ) );

	// Uninitialize sockets
    oex::os::CIpSocket::UninitSockets();

	return nRet;
}

int main(int argc, char* argv[])
{
#if defined( _WIN32 ) || defined( WIN32 )

	// Ensure stdout is in binary mode
	_setmode( fileno( stdout ), O_BINARY );

#endif

    // Initialize the oex library
	oexINIT();

	// Run the app
	int nRet = run( argc, argv );

	// Uninitialize the oex library
    oexUNINIT();

	return nRet;
}

