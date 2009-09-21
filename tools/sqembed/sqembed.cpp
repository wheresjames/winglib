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
int IncludeScript( const sqbind::stdString &sScript, sqbind::stdString &sData, sqbind::stdString &sName )
{
	// Sanity checks
	if ( !sScript.length() )
		return -1;

	// Data container
	oex::CStr s;
	
	// Possible script override folders
	const char *szSub[] = { oexT( "config" ), oexT( "scripts" ), oexT( "sq" ), 0 };

	// Is there an override script?
	oex::CStr sRoot = oexGetModulePath();
	for ( int i = 0; !s.Length() && szSub[ i ]; i++ )
	{	oex::CStr sSub = oexBuildPath( sRoot, oexBuildPath( szSub[ i ], sScript.c_str() ) );
		if ( oexExists( sSub.Ptr() ) )
		{	s = oexMbToStr( oexFileGetContents( sSub.Ptr() ) );
			sName.assign( sSub.Ptr(), sSub.Length() ); 
		} // end if
	} // end for

	// Embedded version?
	if ( !s.Length() )
	{	s = oexMbToStr( oexGetResource( oexBuildPath( oexT( "sq" ), sScript.c_str() ) ) );
		sName = ( oexGetModuleFileName() << oexT( ":" ) << sScript.c_str() ).Ptr();
	} // end if

	// Assign data if any
	if ( s.Length() )
		sData.assign( s.Ptr(), s.Length() );

	return 0;
}

int main(int argc, char* argv[])
{
    // Initialize the oex library
	oexINIT();

	// Initialize sockets
    if ( !oex::os::CIpSocket::InitSockets() )
    	oexERROR( 0, oexT( "Unable to initialize sockets" ) );

	// Start a log file
	oexNOTICE( 0, oexT( "Application startup" ) );

	// Get main script file
	oex::CStr sScript = oexMbToStr( oexGetResource( oexT( "sq/main.nut" ) ) );
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

	g_psqScriptThread->SetModuleManager( g_psqModuleManager );

	g_psqScriptThread->SetScript( sScript.Ptr(), oex::oexFALSE );

	g_psqScriptThread->SetExportFunction( SQBIND_Export_Symbols, oexNULL );

	g_psqScriptThread->SetIncludeScriptFunction( &IncludeScript );

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

