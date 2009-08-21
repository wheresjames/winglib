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
int IncludeScript( const sqbind::stdString &sScript, sqbind::stdString &sData )
{
	// Sanity checks
	if ( !sScript.length() )
		return -1;

	// Data container
	oex::CStr s;

	// Check for override
	if ( oexExists( oexBuildPath( oexT( "scripts" ), sScript.c_str() ).Ptr() ) )
		s = oexMbToStr( oexFileGetContents( oexBuildPath( oexT( "scripts" ), sScript.c_str() ).Ptr() ) );

	else if ( oexExists( oexBuildPath( oexT( "sq" ), sScript.c_str() ).Ptr() ) )
		s = oexMbToStr( oexFileGetContents( oexBuildPath( oexT( "sq" ), sScript.c_str() ).Ptr() ) );

	// Attempt to get the embedded script
	else
		s = oexMbToStr( oexGetResource( sScript.c_str() ) );

	// Assign data
	sData.assign( s.Ptr(), s.Length() );

	return 0;
}

class CSqService : public oex::os::CServiceImpl
{
public:

	CSqService()
	{
		SetName( oexT( OEX_PROJECT_NAME ) );
	}

	virtual int OnRun() 
	{
		// Initialize sockets
		if ( !oex::os::CIpSocket::InitSockets() )
    		oexERROR( 0, oexT( "Unable to initialize sockets" ) );

		// Start a log file
		oexNOTICE( 0, oexT( "Application startup" ) );

		// Get main script file
		oex::CStr sScript = oexMbToStr( oexGetResource( oexT( "main.nut" ) ) );
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

		// Set pointer to module manager
		g_psqScriptThread->SetModuleManager( g_psqModuleManager );

		// Path to the script we will run
		g_psqScriptThread->SetScript( sScript.Ptr(), oex::oexFALSE );

		// Function to export embedded squirrel functions
		g_psqScriptThread->SetExportFunction( SQBIND_Export_Symbols, oexNULL );

		// Function that will find script data
		g_psqScriptThread->SetIncludeScriptFunction( &IncludeScript );

		// Start the script
		if ( g_psqScriptThread->Start() )
			return oexERROR( -5, oexT( "Failed to start script thread" ) );

		// Wait on service to shutdown or thread to exit
		while ( IsRunning() && g_psqScriptThread->IsRunning() )			    
			oexSleep( 100 );

		if ( IsRunning() )
			oexNOTICE( 0, oexT( "Script thread has terminated" ) );
		else
			oexNOTICE( 0, oexT( "Service shutdown requested" ) );

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

		return 0; 
	}

};

// Run the service
oexRUN_SERVICE( CSqService );

