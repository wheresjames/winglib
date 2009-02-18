
#include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "errno.h"

extern "C" oexDECLARE_SRV_FUNCTION( SRV_GetModuleInfo );
extern "C" oex::oexRESULT SRV_GetModuleInfo( oex::os::service::SSrvInfo *pDi )
{
	if ( !oexCHECK_PTR( pDi ) )
	{	oexERROR( EINVAL, oexT( "Invalid function argument" ) );
		return 0;
	} // end if

	// Clear structure
	oexZeroMemory( pDi, sizeof( oex::os::service::SSrvInfo ) );

	// Module name
	strncpy( pDi->szName, oexT( "Squirrel Engine" ), sizeof( pDi->szName ) - 1 );

	// Module description
	strncpy( pDi->szDesc, oexT( "Supplies Squirrel-Script processing capabilities" ), sizeof( pDi->szDesc ) - 1 );

	// Set the Squirrel engine type
	pDi->guidType = sqbind::SQBIND_ENGINE_IID;

	// pDi->guidId = ;

	// Create random guid
	oexUniqueGuid( &pDi->guidInstance );

	// Set version
	pDi->lVer = oexVERSION( 1, 0 );

	return 0;
}

//sqbind::CSqEngine		g_sqEngine;
//CSqMsgQueue				g_sqMsgQueue;

sqbind::CScriptThread	g_sqScriptThread;
sqbind::CModuleManager	g_sqModuleManager;


extern "C" oexDECLARE_SRV_FUNCTION( SRV_Start );
extern "C" oex::oexRESULT SRV_Start( oex::oexCSTR x_pPath, oex::oexCSTR x_pCommandLine, oex::oexINT x_nCommandLine, oex::oexCPVOID x_pData )
{
    // Initialize the oex library
	oexINIT();

	if ( oexCHECK_PTR( x_pPath ) && *x_pPath )
		oex::CLog::GlobalLog().OpenLogFile( oexNULL, oex::CStr( x_pPath ).GetFileName().Ptr(), oexT( ".module.debug.log" ) );
	else
		oex::CLog::GlobalLog().OpenLogFile( oexNULL, oexNULL, oexT( ".module.debug.log" ) );

	// Start a log file
	oexNOTICE( 0, oexT( "Module startup" ) );

	if ( !x_nCommandLine || !oexCHECK_PTR( x_pCommandLine ) )
		oexERROR( 0, oexT( "Script not specified" ) );

	else
	{
		// Attempt to find the file
		oex::CStr sFile = x_pCommandLine;
		if ( !oex::CFile::Exists( sFile.Ptr() ) )
			sFile = oexGetModulePath( x_pCommandLine );

		if ( !oex::CFile::Exists( sFile.Ptr() ) )
			oexERROR( 0, oexMks( oexT( "File not found : " ), sFile ) );

		else
		{
			// Log the script name
			oexNOTICE( 0, oexMks( "Running script : ", x_pCommandLine ) );

			g_sqScriptThread.SetModuleManager( &g_sqModuleManager );

			g_sqScriptThread.SetScript( sFile.Ptr(), oex::oexTRUE );

			if ( g_sqScriptThread.Start() )
				oexERROR( 0, "Failed to start script thread" );

			else
				return 0;

		} // end else

	} // end else

	// Uninitialize the oex library
	oexUNINIT();

	return -1;
}

extern "C" oexDECLARE_SRV_FUNCTION( SRV_Idle );
extern "C" oex::oexRESULT SRV_Idle()
{
	// Attempt to execute idle function
	if ( !g_sqScriptThread.IsRunning() )
	{	oexNOTICE( 0, "Script thread has terminated" );
		return 1;
	} // end if

	return 0;
}

extern "C" oexDECLARE_SRV_FUNCTION( SRV_Stop );
extern "C" oex::oexRESULT SRV_Stop()
{
	// Attempt to execute idle function
	g_sqScriptThread.Destroy();

	// Lose the modules
	g_sqModuleManager.Destroy();

	// Uninitialize the oex library
	oexUNINIT();

	return 0;
}

// !!! Never call this function !!!
// This is just a dummy function to pull in these function definitions
extern "C" void dummy_to_import_symbols()
{	HSQUIRRELVM 	*hvm = 0;
	SQVM 			*svm = 0;
	sqstd_seterrorhandlers( *hvm );
	sqstd_register_iolib( svm );
	sqstd_register_stringlib( *hvm );
	sqstd_register_mathlib( *hvm );
}
