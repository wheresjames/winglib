
#include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "errno.h"

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
	oexNOTICE( 0, "Module startup" );

//	if ( !x_nCommandLine || !oexCHECK_PTR( x_pCommandLine ) || !oex::CFile::Exists( x_pCommandLine ) )
//		oexERROR( 0, "Script not specified" );
//	else
	{
		sqbind::CSqEngine sq;

		if ( !sq.Init() )
			oexERROR( 0, "Unable to initialize Squirrel-Script engine" );
		else
		{
//			sq.Load( "MessageBox( \"Hello World!\" );", oex::oexFALSE );
			if ( !sq.Run( "_self.MessageBox( \"Hello World!\" );" ) )
				oexERROR( 0, oex::CStr().Fmt( oexT( "Squirrel-Script : %s" ), sq.GetLastError().c_str() ) );

		} // end else

	} // end else

	// Uninitialize the oex library
	oexUNINIT();

	return 0;
}

extern "C" oexDECLARE_SRV_FUNCTION( SRV_GetModuleInfo );
extern "C" oex::oexRESULT SRV_GetModuleInfo( oex::os::service::SSrvInfo *pDi )
{
	if ( !oexCHECK_PTR( pDi ) )
	{	oexERROR( EINVAL, "Invalid function argument" );
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
	oexStringToGuid( &pDi->guidInstance );

	// Set version
	pDi->lVer = oexVERSION( 1, 0 );

	return 0;
}
/*
extern "C" void sqstd_seterrorhandlers(HSQUIRRELVM v)
{
}

extern "C" SQRESULT sqstd_register_iolib(HSQUIRRELVM v)
{
	return 0;
}
*/
