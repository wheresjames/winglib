
#include "stdafx.h"
#include "stdio.h"

using namespace oex;

int iptest(int argc, char* argv[])
{
	if ( 3 > argc || !oexCHECK_PTR( argv[ 1 ] ) || !oexCHECK_PTR( argv[ 2 ] ) )
	{	oexPrintf( oexT( "" oexNL8 "Usage <IP Address> <IP Port> [Attempts:1000] [Sockets:100]" oexNL8 oexNL8 ) );
		oexERROR( 0, oexT( "IP Address not specified" ) );
		return -2;
	} // end if

	oexCSTR	pAddress = oexMbToStrPtr( argv[ 1 ] );
	oexINT  nPort = os::CSys::StrToLong( oexMbToStrPtr( argv[ 2 ] ) );
	oexINT	nAttempts = 1000;
	oexINT 	nNumSockets = 100;
//	oexCSTR pWrite = oexNULL; HTTP/1.1
	oexCSTR8 pWrite = "GET / HTTP/1.1\r\n\r\n";
	CStr8 sWrite;

	if ( 3 < argc && oexCHECK_PTR( oexMbToStrPtr( argv[ 3 ] ) ) )
		nAttempts = os::CSys::StrToLong( oexMbToStrPtr( argv[ 3 ] ) );

	if ( 4 < argc && oexCHECK_PTR( oexMbToStrPtr( argv[ 4 ] ) ) )
		nNumSockets = os::CSys::StrToLong( oexMbToStrPtr( argv[ 4 ] ) );

	if ( 5 < argc && oexCHECK_PTR( oexMbToStrPtr( argv[ 5 ] ) ) )
	{	sWrite << "GET " << argv[ 5 ] << " HTTP/1.1\r\n\r\n";
		pWrite = sWrite.Ptr();
	} // end if

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
		{
			// Give it a second to return data
			if ( pSockets[ nCurSocket ].WaitEvent( oex::os::CIpSocket::eReadEvent, 2000 ) )
			{		if ( pSockets[ nCurSocket ].Read().Length() )
					oexPrintf( oexT( "." ) ), os::CSys::Flush_stdout();
				else
					oexPrintf( oexT( "0" ) ), os::CSys::Flush_stdout();
			} // end if
	    	else
	    		oexPrintf( oexT( "x" ) ), os::CSys::Flush_stdout();

			// Ditch the socket
			pSockets[ nCurSocket ].Destroy();

		} // end if

    	if ( !pSockets[ nCurSocket ].Connect( pAddress, nPort ) )
    		oexPrintf( oexT( "X" ) ), os::CSys::Flush_stdout();
//    	else
//    		oexPrintf( oexT( "." ) ), os::CSys::Flush_stdout();

		if ( pWrite )
			pSockets[ nCurSocket ].Write( pWrite );

    	if ( ++nCurSocket >= nNumSockets )
    		bLooped = oexTRUE, nCurSocket = 0;

	} // end for

	// Give each socket a chance to receive data
	for ( oexINT i = 0; i < nNumSockets; i++ )
	{
		// Give it a second to return data
		if ( pSockets[ i ].IsSocket() )
		{
			if ( pSockets[ i ].WaitEvent( oex::os::CIpSocket::eReadEvent, 2000 ) )
			{	if ( pSockets[ nCurSocket ].Read().Length() )
					oexPrintf( oexT( "." ) ), os::CSys::Flush_stdout();
				else
					oexPrintf( oexT( "0" ) ), os::CSys::Flush_stdout();
			} // end if
			else
				oexPrintf( oexT( "x" ) ), os::CSys::Flush_stdout();

		} // end if

		// Ditch the socket
		pSockets[ i ].Destroy();

	} // end for

	// End line
	oexPrintf( oexT( "" oexNL8 oexNL8 ) );

	// Kill all sockets
    OexAllocDestruct( pSockets );

	return 0;
}


int main(int argc, char* argv[])
{
    // Initialize the oex library
	oexINIT();

	// Start a log file
	oexNOTICE( 0, oexT( "Application startup" ) );

	// Initialize sockets
    if ( !oexVERIFY( oex::os::CIpSocket::InitSockets() ) )
        return -1;

	// Run the test
	iptest( argc, argv );

	// Uninit sockets
	oex::os::CIpSocket::UninitSockets();

	// Initialize the oex library
    oexUNINIT();

	return 0;
}

