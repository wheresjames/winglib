
#include "stdafx.h"
#include "stdio.h"

#define NUM_TEMPLATE	"%6u "

#	define DBG_PRINT( s )	( lDbg ? oexPrintf( s ), os::CSys::Flush_stdout() : 0 )
#	define DBGBLOCKSIZE	1
#	define BLOCKSIZE		50

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
						 " -e = Extended information" oexNL8
						 " -r = Read until socket closes" oexNL8
						 " -c = Strictly limit number of connections to -a:" oexNL8
						 " -l = Log socket errors" oexNL8
						 oexNL8
						 "Example: iptest google.com -u:/index.html" oexNL8
						 "Example: iptest 127.0.0.1 -p:8080 -a:100 -s:2 -u:/big_image.jpg" oexNL8
						 oexNL8
						 "--- What the output symbols mean ---" oexNL8
						 "  . = Successful read" oexNL8
						 "  ! = Socket error" oexNL8
						 "  x = Port closed without reading data" oexNL8
						 "  < = Read error" oexNL8
						 "  > = Write error" oexNL8
						 oexNL8
						 "--- Following apply to the -e option ---"	oexNL8
						 "  * = Data block read, continuing to wait for data" oexNL8
						 "  _ = Socket is idle, waiting for data" oexNL8
						 "  ( = Operation is starting" oexNL8
						 " (c = Conecting" oexNL8
						 " (w = Writing" oexNL8
						 " (r = Reading" oexNL8
						 " (! = Socket error" oexNL8
						 "  ) = Operation completed" oexNL8
						 "  ~ = Closing lingering socket, if -c is not specified" oexNL8
						 oexNL8
						 ) );
	else
		oexERROR( 0, pErr );
	return nRet;
}

static oexDOUBLE g_dStartTime = os::CHqTimer::GetTimerSeconds();
static oexUINT g_uCounts = 0, g_uOk = 0, g_uBad = 0;
static oexLONG g_lLastShow = 0;
static oexUINT g_uCountsTotal = 0, g_uOkTotal = 0, g_uBadTotal = 0;

void show_position( oexBOOL bEnd, oexLONG &lShow, oexLONG lBlockSize )
{
	// Time to show?
	if ( !bEnd && ( lShow == g_lLastShow || ( lShow % lBlockSize ) ) )
		return;

	g_lLastShow = lShow;

	// Do we have enough to show statistics?
	if ( 10 <= g_uCounts && ( g_uOk || g_uBad ) )
	{
		oexPrintf( oexFmt( oexT( " %3u %%%%" ), g_uOk * 100 / ( g_uOk + g_uBad ) ).Ptr() );

		// Collect totals
		g_uCountsTotal += g_uCounts;
		g_uOkTotal += g_uOk;
		g_uBadTotal += g_uBad;

		// Reset counters
		g_uCounts = 0, g_uOk = 0, g_uBad = 0;

	} // end if

	if ( !bEnd )

		// Show new line
		oexPrintf( oexFmt( oexTEXT( oexNL8 NUM_TEMPLATE ), lShow ).Ptr() );

	else
	{
		oexDOUBLE dSecs = os::CHqTimer::GetTimerSeconds() - g_dStartTime;
		if ( 0 == dSecs ) dSecs = 1;

		// Show summary
		oexEcho( oexMks( oexTEXT( oexNL8
							      "================================================" oexNL8 ),
						    oexT( "Total   : " ), g_uCountsTotal, oexNL,
						    oexT( "Success : " ), g_uOkTotal, oexNL,
						    oexT( "Failed  : " ), g_uBadTotal, oexNL,
				 oexFmt( oexTEXT( "Time    : %.2f seconds" oexNL8 ), dSecs ),
				 oexFmt( oexTEXT( "Speed   : %.2f / second" oexNL8 ), (oexDOUBLE)g_uOkTotal / dSecs ),
				 oexFmt( oexTEXT( "Quality : %3u %%" oexNL8 ),
								( ( g_uOkTotal + g_uBadTotal )
						 		? ( g_uOkTotal * 100 / ( g_uOkTotal + g_uBadTotal ) )
						 		: 0 ) ),
						 oexTEXT( "================================================" oexNL8 )
					   ).Ptr() );

	} // end if

}

void show_progress( oexCSTR pShow, oexLONG &lShow )
{
	if ( !pShow )
		return;

	if ( oexT( '.' ) == *pShow )
		g_uOk++;
	else
		g_uBad++;

	oexPrintf( pShow );

	os::CSys::Flush_stdout();

	lShow++;
	g_uCounts++;
}

int iptest(int argc, char* argv[])
{
	if ( 2 > argc )
		return show_use( -2, oexT( "Command line variables are invalid" ), 1 );

	// Parse the command line info
	CPropertyBag pbCmdLine = oex::CParser::ParseCommandLine( argc, (const char**)argv );

	// Get params
	CStr sAddress		= pbCmdLine.IsKey( oexT( "0" ) ) ? pbCmdLine[ oexT( "0" ) ].ToString() : oexT( "" );
	oexLONG lPort		= pbCmdLine.IsKey( oexT( "p" ) ) ? pbCmdLine[ oexT( "p" ) ].ToLong() : 80;
	oexLONG lAttempts	= pbCmdLine.IsKey( oexT( "a" ) ) ? pbCmdLine[ oexT( "a" ) ].ToLong() : 1000;
	oexLONG lSockets	= pbCmdLine.IsKey( oexT( "s" ) ) ? pbCmdLine[ oexT( "s" ) ].ToLong() : 10;
	oexLONG lDbg		= pbCmdLine.IsKey( oexT( "e" ) ) ? 1 : 0;
	oexLONG lReadAll	= pbCmdLine.IsKey( oexT( "r" ) ) ? 1 : 0;
	oexLONG lLog		= pbCmdLine.IsKey( oexT( "l" ) ) ? 1 : 0;
	oexLONG lLimit		= pbCmdLine.IsKey( oexT( "c" ) ) ? 1 : 0;
	oexLONG lBlockSize	= pbCmdLine.IsKey( oexT( "b" ) ) ? pbCmdLine[ oexT( "b" ) ].ToLong() : ( lDbg ? DBGBLOCKSIZE : BLOCKSIZE );
	CStr sUrl			= pbCmdLine.IsKey( oexT( "u" ) ) ? pbCmdLine[ oexT( "u" ) ].ToString() : oexT( "/" );
	CStr8 sData			= oexStrToStr8( pbCmdLine.IsKey( oexT( "d" ) ) ? pbCmdLine[ oexT( "d" ) ].ToString() : oexT( "" ) );

	if ( !sAddress.Length() )
		return show_use( -3, oexT( "IP Address not specified" ), 1 );

	os::CIpAddress addr;
    if ( !lPort && !addr.LookupUrl( sAddress.Ptr() ) )
		return show_use( -3, oexT( "Network address lookup failed" ), 1 );
    else if ( !addr.LookupHost( sAddress.Ptr(), lPort ) )
		return show_use( -3, oexT( "Network address lookup failed" ), 1 );

	if ( 0 >= lAttempts )
		return show_use( -4, oexT( "Invalid number of attempts 0" ), 1 );

	if ( 0 >= lSockets )
		return show_use( -5, oexT( "Invalid number of sockets 0" ), 1 );

	if ( !sData.Length() )
		sData = CStr8() << "GET " << oexStrToStr8( sUrl ) << " HTTP/1.1\r\n\r\n";

	TMem< os::CIpSocket > mSockets;

	if ( mSockets.OexConstructArray( lSockets ).Size() != (oexSIZE_T)lSockets )
		return show_use( -6, oexT( "Unable to create socket array" ) );

	oexEcho( oexMks( oexTEXT( oexNL8 "Connecting to : " ), addr.GetDotAddress(), oexT( ":" ), lPort,
					 oexT( " - attempts = " ), lAttempts,
					 oexT( ", sockets = " ), lSockets,
					 oexT( ", url = " ), sUrl,
					 oexNL ).Ptr() );

	oexPrintf( oexFmt( oexTEXT( oexNL8 NUM_TEMPLATE ) , 0 ).Ptr() );

	oexLONG lShow = 0, lConnects = 0;
	oexUINT uConnected = 0;
	while( lShow < lAttempts )
	{
		// Reset connected count
		uConnected = 0;

		// Start all the sockets we have
		for ( oexULONG i = 0; i < mSockets.Size(); i++ )
		{
			// Ditch stalled sockets
			if ( mSockets[ i ].IsSocket() && !mSockets[ i ].IsRunning( 1 ) )
			{
				DBG_PRINT( "(" );

				show_progress( oexT( "!" ), lShow );

				DBG_PRINT( oexFmt( oexT( " writes= flags=%x to=%f " ),
								   mSockets[ i ].getNumWrites(),
								   mSockets[ i ].GetConnectState(),
								   mSockets[ i ].GetActivityTimeout()
								 ).Ptr() );

				if ( mSockets[ i ].GetLastError() )
				{	if ( lLog ) oexERROR( mSockets[ i ].GetLastError(), mSockets[ i ].GetLastErrorMsg() );
					DBG_PRINT( mSockets[ i ].GetLastErrorMsg().Ptr() );
				} // end if

				mSockets[ i ].Destroy();

				DBG_PRINT( ")" );

			} // end if

			// Connect if it's not doing anything
			if ( ( lConnects < lAttempts || ( !lLimit && lShow < lAttempts ) ) && !mSockets[ i ].IsSocket() )
			{
				DBG_PRINT( "(c" );

				lConnects++;
				if ( !mSockets[ i ].Connect( addr ) )
				{
					show_progress( oexT( "x" ), lShow );

					if ( mSockets[ i ].GetLastError() )
					{	if ( lLog ) oexERROR( mSockets[ i ].GetLastError(), mSockets[ i ].GetLastErrorMsg() );
						DBG_PRINT( mSockets[ i ].GetLastErrorMsg().Ptr() );
					} // end if

					mSockets[ i ].Destroy();

				} // end if

				DBG_PRINT( ")" );

			} // end if

			// Wait for connection
			else if ( mSockets[ i ].IsConnected() )
			{
				// Do we need to write the data?
				if ( !mSockets[ i ].getNumWrites() )
				{
					DBG_PRINT( "(w" );

					if ( !mSockets[ i ].Write( sData ) )
					{
						if ( !lDbg )
							show_progress( oexT( ">" ), lShow );
						else
							show_progress( oexT( ":" ), lShow );

						if ( mSockets[ i ].GetLastError() )
						{	if ( lLog ) oexERROR( mSockets[ i ].GetLastError(), mSockets[ i ].GetLastErrorMsg() );
							DBG_PRINT( mSockets[ i ].GetLastErrorMsg().Ptr() );
						} // end if

						mSockets[ i ].Destroy();

					} // end if

					DBG_PRINT( ")" );

				} // end else if

				// Wait for data
				else if ( mSockets[ i ].WaitEvent( oex::os::CIpSocket::eReadEvent, 0 ) )
				{
					DBG_PRINT( "(r" );

					if ( !lReadAll )
					{
						if ( mSockets[ i ].Read().Length() )
							show_progress( oexT( "." ), lShow );
						else
						{
							show_progress( oexT( "<" ), lShow );

							if ( mSockets[ i ].GetLastError() )
							{	if ( lLog ) oexERROR( mSockets[ i ].GetLastError(), mSockets[ i ].GetLastErrorMsg() );
								DBG_PRINT( mSockets[ i ].GetLastErrorMsg().Ptr() );
							} // end if

						} // end else

						// Close the socket
						mSockets[ i ].Destroy();

					} // end if

					else
					{
						oexINT nRes = mSockets[ i ].Read().Length();

						if ( 0 < nRes )
							DBG_PRINT( "*" );

						else
						{
							if ( mSockets[ i ].GetLastError() )
							{
								if ( !lDbg )
									show_progress( oexT( "<" ), lShow );
								else
									show_progress( oexT( ":" ), lShow );

								if ( lLog ) oexERROR( mSockets[ i ].GetLastError(), mSockets[ i ].GetLastErrorMsg() );
								DBG_PRINT( mSockets[ i ].GetLastErrorMsg().Ptr() );

							} // end if

							else
								show_progress( oexT( "." ), lShow );

							// Close the socket
							mSockets[ i ].Destroy();

						} // end if

					} // end else

					DBG_PRINT( ")" );

				} // end else if

				// Wait for data
				else if ( mSockets[ i ].WaitEvent( oex::os::CIpSocket::eCloseEvent, 0 ) )
				{
					// Did we read anything
					if ( lReadAll && mSockets[ i ].getNumReads() )
						show_progress( oexT( "." ), lShow );

					// No data
					else
						show_progress( oexT( "x" ), lShow );

					// Close the socket
					mSockets[ i ].Destroy();

				} // end else if

				// Waiting...
				else
					DBG_PRINT( "_" );

			} // end if

			// Track the number of sockets still working
			if ( mSockets[ i ].IsConnected() || mSockets[ i ].IsConnecting() )
				uConnected++;

			// Show our position
			show_position( 0, lShow, lBlockSize );

		} // end for

		// Wait a bit
		if ( uConnected )
			oexSleep( 15 );

	} // end for

	// Show totals
	show_position( 1, lShow, lBlockSize );

	if ( lDbg )
	{
		// Show the closing of lingering sockets
		for ( oexULONG i = 0; i < mSockets.Size(); i++ )
			if ( mSockets[ i ].IsSocket() )
				oexPrintf( oexT( "~" ) ), os::CSys::Flush_stdout(),
				mSockets[ i ].Destroy();

		oexEcho( oexTEXT( oexNL8 oexNL8 ) );

	} // end if

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

