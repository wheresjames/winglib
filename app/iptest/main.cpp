
#include "stdafx.h"
#include "stdio.h"

using namespace oex;

int main(int argc, char* argv[])
{
    // Initialize the oex library
	oexINIT();

	// Start a log file
	oexNOTICE( 0, oexT( "Application startup" ) );

    if ( !oexVERIFY( oex::os::CIpSocket::InitSockets() ) )
        return -1;

	if ( 3 > argc || !oexCHECK_PTR( argv[ 1 ] ) || !oexCHECK_PTR( argv[ 2 ] ) )
	{	oexPrintf( oexT( "" oexNL8 "Usage <IP Address> <IP Port> [Attempts:1000] [Sockets:100]" oexNL8 oexNL8 ) );
		oexERROR( 0, oexT( "IP Address not specified" ) );
		return -2;
	} // end if

	oexCSTR	pAddress = oexMbToStrPtr( argv[ 1 ] );
	oexINT  nPort = os::CSys::StrToLong( oexMbToStrPtr( argv[ 2 ] ) );
	oexINT	nAttempts = 1000;
	oexINT 	nNumSockets = 100;
//	oexCSTR pWrite = oexNULL;
	oexCSTR8 pWrite = "GET /\r\n\r\n";

	if ( 3 < argc && oexCHECK_PTR( oexMbToStrPtr( argv[ 3 ] ) ) )
		nAttempts = os::CSys::StrToLong( oexMbToStrPtr( argv[ 3 ] ) );

	if ( 4 < argc && oexCHECK_PTR( oexMbToStrPtr( argv[ 4 ] ) ) )
		nNumSockets = os::CSys::StrToLong( oexMbToStrPtr( argv[ 4 ] ) );

	if ( 5 < argc && oexCHECK_PTR( oexMbToStrPtr( argv[ 5 ] ) ) )
		pWrite = argv[ 5 ];

	CStr sAttempting = CStr().Fmt( oexT( "address = %s, port = %d, attempts = %d, sockets = %d" ),
								   pAddress, nPort, nAttempts, nNumSockets );
	if ( pWrite )
		sAttempting += CStr().Fmt( oexT( " writing = '%s'" ), pWrite );
	oexPrintf( oexT( "%s" oexNL8 oexNL8 ), sAttempting.Ptr() );
	oexNOTICE( 0, sAttempting );

	os::CIpSocket *pSockets = OexAllocConstructArray< os::CIpSocket >( nNumSockets );
	if ( !pSockets )
	{	oexERROR( 0, oexT( "Out of memory" ) );
		return -1;
	} // end if

	oexINT nFlushCount = 0;
	oexINT nTotalCount = 0;
	oexBOOL bLooped = oexFALSE;
	oexINT nCurSocket = 0;
	for ( oexINT i = 0; i < nAttempts; i++ )
	{
		nTotalCount++;
    	if ( !nTotalCount || 50 <= ++nFlushCount )
    	{	nFlushCount = 0;
    		oexPrintf( oexT( "" oexNL8 "%d : " ), nTotalCount );
    		os::CSys::Flush_stdout();
		} // end if

		if ( bLooped )
			pSockets[ nCurSocket ].Destroy();

    	if ( !pSockets[ nCurSocket ].Connect( pAddress, nPort ) )
    		oexPrintf( oexT( "x" ) ), os::CSys::Flush_stdout();
    	else
    		oexPrintf( oexT( "." ) ), os::CSys::Flush_stdout();

		if ( pWrite )
			pSockets[ nCurSocket ].Write( pWrite );

    	if ( ++nCurSocket >= nNumSockets )
    		bLooped = oexTRUE, nCurSocket = 0;

	} // end for

	// End line
	oexPrintf( oexT( "" oexNL8 oexNL8 ) );

	// Kill all sockets
    OexAllocDestruct( pSockets );

	// Initialize the oex library
    oexUNINIT();

	return 0;
}

