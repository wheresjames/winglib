
#include "stdafx.h"
#include "stdio.h"

// sqengine_d.dll ../../winglib/etc/scripts/test_cell.nut

int main(int argc, char* argv[])
{
    // Initialize the oex library
	oexINIT();

	// Start a log file
	oexNOTICE( 0, "Application startup" );

	oex::CStr sModule;

	if ( argc > 1 && oexCHECK_PTR( argv[ 1 ] ) )
		sModule = argv[ 1 ];

	// Calculate a module name if not specified
	if ( !sModule.Length() )
	{
		// Look for a .cfg file
		oex::CStr sSettings = oexGetModuleFileName() << oexT( ".cfg" );
		if ( oex::CFile::Exists( sSettings.Ptr() ) )
		{
			oex::CPropertyBag pb = oex::CParser::DecodeIni( oex::CFile().OpenExisting( sSettings.Ptr() ).Read() );
			if ( pb.IsKey( oexT( "module" ) ) )
				sModule = pb[ oexT( "module" ) ].ToString().DecorateName( oex::oexTRUE, oex::oexTRUE );

		} // end if

		// Use the name of the default module
		else if ( !sModule.Length() )
			sModule = oexMks( oexT( "srvmod" ) ).DecorateName( oex::oexTRUE, oex::oexTRUE );

	} // end if

	oex::os::CSys::Printf( "Starting...\n" );

	oex::CStr sCommandLine;
	if ( argc > 2 && oexCHECK_PTR( argv[ 2 ] ) )
		sCommandLine = argv[ 2 ];

	int nRet = oex::os::CService::Run( oexGetModulePath().BuildPath( sModule.Ptr() ), sCommandLine, oexNULL, oexNULL );

	if ( 0 > nRet )
	{	oexERROR( nRet, "Failed to start service module..." );
		oex::os::CSys::Printf( "Failed to start service module %s\n", oexStrToMb( sModule ).Ptr() );
	} // end if

	else if ( 0 < nRet )
	{	oexNOTICE( 0, "Return from parent..." );
		oex::os::CSys::Printf( "Return from parent...\n" );
	} // end else if

	else
	{	oexNOTICE( 0, "Return from child..." );
		oex::os::CSys::Printf( "Return from child...\n" );
	} // end else

	// Uninitialize the oex library
    oexUNINIT();

	return 0;
}

