
class CGlobal
{
};

local _g = CGlobal();

function _init() : ( _g )
{
	local server = "time-a.nist.gov";
//	local server = "nist1.columbiacountyga.gov";
//	local server = "localhost";
	
	local port = 123;
//	local port = 12345;
	local NTP_EPOCH = ( 86400 * ( 365 * 70 + 17 ) );

	_self.echo( "\n Getting Time from : " + server + ":" + port +"\n" );

	local socket = CSqSocket();
	if ( !socket.CreateUDP() )
	{	_self.echo( "CreateUDP() : " + socket.getLastError() );
		return;
	} // end if

	if ( !socket.Connect( server, port ) )
	{	_self.echo( "Connect() : " + socket.getLastError() );
		return;
	} // end if

	// Wait for connect
	if ( !socket.WaitEvent( CSqSocket().EVT_CONNECT, 3000 ) )
	{	_self.echo( "WaitEvent( EVT_CONNECT ) : " + socket.getLastError() );
		return;
	} // end if

	local pkt_size = 12 * 4;
	local pkt = CSqBinary();
	if ( !pkt.Allocate( pkt_size ) )
	{	_self.echo( "Failed to allocate memory for packet" );
		return;
	} // end if

	pkt.Zero();
	pkt.setUsed( pkt_size );

	// Get local time
	local dw0 = 0;
	dw0 = dw0 | ( ( 0 & 0x03 ) << 30 );	// LI
	dw0 = dw0 | ( ( 3 & 0x07 ) << 27 );	// VN
	dw0 = dw0 | ( ( 3 & 0x07 ) << 24 );	// Mode
//	dw0 = dw0 | ( ( 0 & 0x0f ) << 16 );	// Stratum
//	dw0 = dw0 | ( ( 0 & 0xff ) << 8 );	// Poll
//	dw0 = dw0 | ( ( 0 & 0xff ) << 0 );	// Precision
	pkt.setUINT( 0, socket.htonl( dw0 ) );

	// Set current time
	pkt.setUINT( 6, socket.htonl( _self.gmt_time() + NTP_EPOCH ) );

	// Write data
	if ( !socket.SendToBin( pkt, 0 ) )
	{	_self.echo( "SendToBin() : " + socket.getLastError() );
		return;
	} // end if

	// Wait for data
	if ( !socket.WaitEvent( CSqSocket().EVT_READ, 8000 ) )
	{	_self.echo( "WaitEvent( EVT_READ ) : " + socket.getLastError() );
		return;
	} // end if

	if ( !socket.ReadFromBin( pkt, 0 ) )
	{	_self.echo( "ReadFromBin() : " + socket.getLastError() );
		return;
	} // end if

	if ( pkt.getUsed() < pkt_size )
	{	_self.echo( "Too few bytes returned from server : " + pkt.getUsed() );
		return;
	} // end if

	// Get timestamp from server packet
	local ts_server = socket.ntohl( pkt.getUINT( 10 ) ) - NTP_EPOCH;
	local ft = socket.ntohl( pkt.getUINT( 11 ) ) / 4295;

	// Local time
	local ts_client = _self.gmt_time();

	local tm = CSqTime();
	tm.SetUnixTime( ts_client );
	_self.echo( "Local  Time : " + tm.FormatTime( "%W, %B %D, %Y - %h:%m:%s %A" ) );
	tm.SetUnixTime( ts_server );
	_self.echo( "Server Time : " + tm.FormatTime( "%W, %B %D, %Y - %h:%m:%s %A" ) );

	_self.echo( "\nLocal time is off by " + ( ts_client - ts_server ) + " seconds" );

	// Correct time
//	if ( ts_client != ts_server )
//		tm.SetSystemTime();

//	CSqFile().put_contents_bin( "sntp." + server + ".dat", pkt );


	_self.echo( "\n...done...\n" );
}

function _idle() : ( _g )
{
	return 1;
}

