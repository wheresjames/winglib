// sqembed.cpp

#include "stdafx.h"
#include "stdio.h"

// Include statically linked modules
#include "sqbind_static_build.hpp"

/// Pointer to script thread
sqbind::CScriptThread	*g_psqScriptThread = oexNULL;

/// Pointer to module manager
sqbind::CModuleManager	*g_psqModuleManager = oexNULL;

// Custom include script
int IncludeScript( const sqbind::stdString &sScript, oex::CStr8 &sData, sqbind::stdString &sName )
{_STT();
	// Sanity checks
	if ( !sScript.length() )
		return -1;

	sData.clear();
	
	// Possible script override folders
	oex::oexCSTR szSub[] = { oexT( "config" ), oexT( "scripts" ), oexT( "sq" ), 0 };

	// Is there an override script?
	oex::CStr sRoot = oexGetModulePath();
	for ( int i = 0; !sData.Length() && szSub[ i ]; i++ )
	{	oex::CStr sSub = oexBuildPath( sRoot, oexBuildPath( szSub[ i ], sScript.c_str() ) );
		if ( oexExists( sSub.Ptr() ) )
		{	sData = oexFileGetContents( sSub.Ptr() );
			sName.assign( sSub.Ptr(), sSub.Length() );
		} // end if
	} // end for

	// Embedded version?
	if ( !sData.Length() )
	{	sData = oexGetResource( oexBuildPath( oexT( "sq" ), sScript.c_str() ) );
		sName = ( oexGetModuleFileName().GetFileName() << oexT( ":" ) << sScript.c_str() ).Ptr();
	} // end if

	return 0;
}

int run( int argc, char* argv[] )
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
	
	else if ( pbCmdLine.IsKey( oexT( "build" ) ) )
	{	oexEcho( oexBuild().Ptr() );		
		pbCmdLine.Destroy();
	    oexUNINIT();
		return 0;					  
	} // end if
	
	// Enable crash reporting
	_STT_SET_NAME( oexT( "Main Thread" ) );
	oexEnableCrashReporting( oexNULL, oexT( "logs" ) );

	// Initialize resources
	oexInitResources();

	// Initialize sockets
    if ( !oex::os::CIpSocket::InitSockets() )
    	oexERROR( 0, oexT( "Unable to initialize sockets" ) );

	// Start a log file
	oexNOTICE( 0, oexT( "Application startup" ) );

	// Get main script file
	oex::CStr8 sScript = oexGetResource( oexT( "sq/main.nut" ) );
	if ( !sScript.Length() )
		return oexERROR( -2, oexT( "embedded:main.nut not found" ) );

	// Create objects
	g_psqScriptThread = OexAllocConstruct< sqbind::CScriptThread >();
	if ( !oexCHECK_PTR( g_psqScriptThread ) )
		return oexERROR( -3, oexT( "Out of memory!" ) );

	g_psqModuleManager = OexAllocConstruct< sqbind::CModuleManager >();
	if ( !oexCHECK_PTR( g_psqModuleManager ) )
		return oexERROR( -4, oexT( "Out of memory!" ) );

	// Log the script name
	oexNOTICE( 0, oexT( "Running script : embedded:main.nut" ) );

	sqbind::CScriptThread::SetAppInfo( oexAppNamePtr(), oexAppNameProcPtr(), oexAppLongNamePtr(), oexAppDescPtr() );

	g_psqScriptThread->SetScriptName( oexT( "embedded:main.nut" ) );

	g_psqScriptThread->SetModuleManager( g_psqModuleManager );

	g_psqScriptThread->SetScript( sScript, oex::oexFALSE );

	g_psqScriptThread->SetExportFunction( SQBIND_Export_Symbols, oexNULL );

	g_psqScriptThread->SetIncludeScriptFunction( &IncludeScript );

	g_psqScriptThread->Pb()[ oexT( "cmdline" ) ] = pbCmdLine;

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

int main( int argc, char* argv[] )
{
    // Initialize the oex library
	oexINIT();

	// Run the application
	int res = run( argc, argv );

	oexNOTICE( 0, oexT( "Shutting down..." ) );

	// Uninitialize sockets
    oex::os::CIpSocket::UninitSockets();

	// Uninitialize the oex library
    oexUNINIT();

	return res;
}
