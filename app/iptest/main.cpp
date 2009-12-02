
#include "stdafx.h"
#include "stdio.h"

using namespace oex;

int show_use( int nRet, oexCSTR pErr, int bShowUse = 0 )
{
	if ( pErr )
		oexEcho( pErr );

	if ( bShowUse )
		oexPrintf( oexT( "" oexNL8 "Usage <IP Address>" oexNL8
						 " -p:<num> = TCP port, default is 80" oexNL8
						 " -a:<num> = Number of attempts, default is 1000" oexNL8
						 " -s:<num> = Number of sockets, default is 10" oexNL8
						 " -b:<num> = Block size, default is 50" oexNL8
						 " -u:<string> = URL address for HTTP" oexNL8
						 " -d:<string> = data to send" oexNL8
						 oexNL8 
						 "Example: iptest google.com -u:/index.html" oexNL8
						 "Example: iptest 127.0.0.1 -p:8080 -a:100 -s:2 -u:/big_image.jpg" oexNL8
						 oexNL8 
						 ) );
	else
		oexERROR( 0, pErr );
	return nRet;
}

void show_progress( oexCSTR pShow, oexULONG &uShow, oexLONG lBlockSize )
{
	static oexUINT uOk = 0, uBad = 0;

	if ( !pShow || !( uShow % lBlockSize ) )
	{
		// Show progress
		if ( uOk || uBad )
			oexPrintf( oexFmt( oexT( " %3u %%%%" ), uOk * 100 / ( uOk + uBad ) ).Ptr() );

		// Reset counters
		uOk = 0, uBad = 0;

		if ( pShow )

			// Show new line
			oexPrintf( oexFmt( oexT( "\r\n%6u " ), uShow ).Ptr() );

	} // end if

	if ( !pShow )
		return;

	if ( oexT( '.' ) == *pShow )
		uOk++;
	else
		uBad++;

	oexPrintf( pShow );

	os::CSys::Flush_stdout();

	uShow++;
}

int iptest(int argc, char* argv[])
{
	if ( 2 > argc )
		return show_use( -2, oexT( "Command line variables are invalid" ), 1 );

	// Parse the command line info
	CPropertyBag8 pbCmdLine = oex::CParser::ParseCommandLine( argc, (const char**)argv );

	// Get params
	CStr8 sAddress		= pbCmdLine.IsKey( oexT( "0" ) ) ? pbCmdLine[ oexT( "0" ) ].ToString() : oexT( "" );
	oexLONG lPort		= pbCmdLine.IsKey( oexT( "p" ) ) ? pbCmdLine[ oexT( "p" ) ].ToLong() : 80;
	oexLONG lAttempts	= pbCmdLine.IsKey( oexT( "a" ) ) ? pbCmdLine[ oexT( "a" ) ].ToLong() : 1000;
	oexLONG lSockets	= pbCmdLine.IsKey( oexT( "s" ) ) ? pbCmdLine[ oexT( "s" ) ].ToLong() : 10;
	oexLONG lBlockSize	= pbCmdLine.IsKey( oexT( "b" ) ) ? pbCmdLine[ oexT( "b" ) ].ToLong() : 50;
	CStr8 sUrl			= pbCmdLine.IsKey( oexT( "u" ) ) ? pbCmdLine[ oexT( "u" ) ].ToString() : oexT( "/" );
	CStr8 sData			= pbCmdLine.IsKey( oexT( "d" ) ) ? pbCmdLine[ oexT( "d" ) ].ToString() : oexT( "" );

	if ( !sAddress.Length() )
		return show_use( -3, oexT( "IP Address not specified" ), 1 );

	if ( 0 >= lPort )
		return show_use( -3, oexT( "Invalid TCP port 0" ), 1 );

	if ( 0 >= lAttempts )
		return show_use( -4, oexT( "Invalid number of attempts 0" ), 1 );

	if ( 0 >= lSockets )
		return show_use( -5, oexT( "Invalid number of sockets 0" ), 1 );

	if ( !sData.Length() )
		sData = CStr8() << "GET " << sUrl << " HTTP/1.1\r\n\r\n";

	TMem< os::CIpSocket > mSockets;

	if ( mSockets.OexConstructArray( lSockets ).Size() != lSockets )
		return show_use( -6, oexT( "Unable to create socket array" ) );

	oexEcho( oexMks( oexT( "Connecting to : " ), sAddress, oexT( ":" ), lPort,
					 oexT( " - attempts = " ), lAttempts, 
					 oexT( ", sockets = " ), lSockets, 
					 oexT( ", url = " ), sUrl, 
					 oexNL ).Ptr() );

	oexULONG uShow = 0, uConnected = 0;
	while( 0 < lAttempts || uConnected )
	{
		// Reset connected count
		uConnected = 0;

		// Start all the sockets we have
		for ( oexLONG i = 0; i < mSockets.Size(); i++ )
		{
			// Ditch err'd sockets
			if ( mSockets[ i ].IsError() )
			{	mSockets[ i ].Destroy();
				show_progress( oexT( "e" ), uShow, lBlockSize );
			} // end if

			// Connect if it's not doing anything
			if ( 0 < lAttempts && !mSockets[ i ].IsConnected() && !mSockets[ i ].IsConnecting() )
			{	
				lAttempts--;
				
				if ( !mSockets[ i ].Connect( sAddress.Ptr(), lPort ) )
					show_progress( oexT( "x" ), uShow, lBlockSize );
//				else
//					show_progress( oexT( "c" ), uShow, lBlockSize );

			} // end if

			// Wait for connection
			else if ( mSockets[ i ].IsConnected() )
			{
				// Do we need to write the data?
				if ( !mSockets[ i ].getNumWrites() )
				{	mSockets[ i ].Write( sData );
//					show_progress( oexT( "w" ), uShow, lBlockSize );
				} // end else if

				// Wait for data
				else if ( mSockets[ i ].WaitEvent( oex::os::CIpSocket::eReadEvent, 0 ) )
				{
					if ( mSockets[ i ].Read().Length() )
						show_progress( oexT( "." ), uShow, lBlockSize );
					else
						show_progress( oexT( "0" ), uShow, lBlockSize );

					// Close the socket
					mSockets[ i ].Destroy();

				} // end else if

			} // end if

			// Track the number of sockets still working
			if ( mSockets[ i ].IsConnected() || mSockets[ i ].IsConnecting() )
				uConnected++;

		} // end for

		// Wait a bit
		if ( uConnected )
			oexSleep( 15 );

	} // end for

	// Finish off the block
	show_progress( oexNULL, uShow, lBlockSize );

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
		return show_use( -1, oexT( "Failed to initialize sockets" ) );

	// Run the test
	iptest( argc, argv );

	// Uninit sockets
	oex::os::CIpSocket::UninitSockets();

	// Initialize the oex library
    oexUNINIT();

	return 0;
}

