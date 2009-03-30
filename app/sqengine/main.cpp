
#include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "errno.h"

/// Pointer to script thread
sqbind::CScriptThread	*g_psqScriptThread = oexNULL;

/// Pointer to module manager
sqbind::CModuleManager	*g_psqModuleManager = oexNULL;

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
	strncpy( pDi->szName, "Squirrel Engine", sizeof( pDi->szName ) - 1 );

	// Module description
	strncpy( pDi->szDesc, "Supplies Squirrel-Script processing capabilities", sizeof( pDi->szDesc ) - 1 );

	// Set the Squirrel engine type
	pDi->guidType = sqbind::SQBIND_ENGINE_IID;

	// pDi->guidId = ;

	// Create random guid
	oexUniqueGuid( &pDi->guidInstance );

	// Set version
	pDi->lVer = oexVERSION( 1, 0 );

	return 0;
}

extern "C" oexDECLARE_SRV_FUNCTION( SRV_Start );
extern "C" oex::oexRESULT SRV_Start( oex::os::SRawAllocator x_sRawAllocator, oex::oexCSTR x_pPath, oex::oexCSTR x_pCommandLine, oex::oexINT x_nCommandLine, oex::oexCPVOID x_pData )
{
	// Set our allocator
	oex::os::CMem::SetRawAllocator( x_sRawAllocator );

    // Initialize the oex library
	oexINIT();

	// Create objects
	g_psqScriptThread = OexAllocConstruct< sqbind::CScriptThread >();
	if ( !oexCHECK_PTR( g_psqScriptThread ) )
		return oexERROR( -1, oexT( "Out of memory!" ) );

	g_psqModuleManager = OexAllocConstruct< sqbind::CModuleManager >();
	if ( !oexCHECK_PTR( g_psqModuleManager ) )
		return oexERROR( -1, oexT( "Out of memory!" ) );

	if ( oexCHECK_PTR( x_pPath ) && *x_pPath )
		oex::CLog::GlobalLog().OpenLogFile( oexNULL, oex::CStr( x_pPath ).GetFileName().Ptr(), oexT( ".module.debug.log" ) );
	else
		oex::CLog::GlobalLog().OpenLogFile( oexNULL, oexNULL, oexT( ".module.debug.log" ) );

	// Start a log file
	oexNOTICE( 0, oexT( "Module startup" ) );

	oex::CStr sScript;

	if ( oexCHECK_PTR( x_pCommandLine ) && *x_pCommandLine )
		sScript = x_pCommandLine;

	else
		sScript = oexT( "main.nut" );

	if ( sScript.Length() )
	{
		// Attempt to find the file
		oex::CStr sFile = sScript;
		if ( !oex::CFile::Exists( sFile.Ptr() ) )
		{
			// Look relative to current folder
			if ( oex::CFile::Exists( oexGetModulePath( sFile.Ptr() ).Ptr() ) )
				sFile = oexGetModulePath( sFile.Ptr() );

			// Relative to scripts folder
			else if ( oex::CFile::Exists( oexGetModulePath( oexT( "scripts" ) ).BuildPath( sFile ).Ptr() ) )
				sFile = oexGetModulePath( oexT( "scripts" ) ).BuildPath( sFile );

		} // end if

		if ( !oex::CFile::Exists( sFile.Ptr() ) )
			oexERROR( 0, oexMks( oexT( "File not found : " ), sFile ) );

		else
		{
			// Log the script name
			oexNOTICE( 0, oexMks( oexT( "Running script : " ), sFile ) );

			g_psqScriptThread->SetModuleManager( g_psqModuleManager );

			g_psqScriptThread->SetScript( sFile.Ptr(), oex::oexTRUE );

			if ( g_psqScriptThread->Start() )
				oexERROR( 0, oexT( "Failed to start script thread" ) );

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
	// Ensure pointer
	if ( !oexCHECK_PTR( g_psqScriptThread ) )
		return -1;

	// Attempt to execute idle function
	if ( !g_psqScriptThread->IsRunning() )
	{	oexNOTICE( 0, oexT( "Script thread has terminated" ) );
		return 1;
	} // end if

	return 0;
}

extern "C" oexDECLARE_SRV_FUNCTION( SRV_Stop );
extern "C" oex::oexRESULT SRV_Stop()
{
	if ( oexCHECK_PTR( g_psqScriptThread ) )
	{	g_psqScriptThread->Destroy();
		OexAllocDelete( g_psqScriptThread );
	} // end if

	if ( oexCHECK_PTR( g_psqModuleManager ) )
	{	g_psqModuleManager->Destroy();
		OexAllocDelete( g_psqModuleManager );
	} // end if

	// Uninitialize the oex library
	oexUNINIT();

	// Switch back to default allocator
	oex::os::CMem::SetDefaultRawAllocator();

	return 0;
}

