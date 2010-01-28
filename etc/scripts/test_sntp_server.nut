
class CGlobal
{
	socket = CSqSocket();
};

local _g = CGlobal();

function _init() : ( _g )
{
	if ( !_g.socket.CreateUDP() )
	{	_self.echo( "CreateUDP() : " + _g.socket.getLastError() );
		return;
	} // end if

	if ( !_g.socket.Bind( 12345 ) )
	{	_self.echo( "Bind() : " + _g.socket.getLastError() );
		return;
	} // end if

	_self.echo( "SNTP server is running..." );
}

function _idle() : ( _g )
{
	local NTP_EPOCH = ( 86400 * ( 365 * 70 + 17 ) );

	// Wait for connect request
	while ( _g.socket.WaitEvent( CSqSocket().EVT_READ, 1000 ) )
	{
		local pkt_size = 12 * 4;
		local pkt = CSqBinary();

		if ( !socket.ReadFromBin( pkt, 0 ) )
		{	_self.echo( "ReadFromBin() : " + socket.getLastError() );
			continue;
		} // end if

		if ( pkt.getUsed() < pkt_size )
		{	_self.echo( "Too few bytes sent from client : " + pkt.getUsed() );
			continue;
		} // end if

		// What is the clients time
		local ts_client = socket.ntohl( pkt.getUINT( 10 ) ) - NTP_EPOCH;
		local ts_server = _self.gmt_time();

		local tm = CSqTime();
		tm.SetUnixTime( ts_server );
		_self.echo( "Server Time : " + tm.FormatTime( "%W, %B %D, %Y - %h:%m:%s %A" ) );
		tm.SetUnixTime( ts_client );
		_self.echo( "Local  Time : " + tm.FormatTime( "%W, %B %D, %Y - %h:%m:%s %A" ) );
		_self.echo( "\Client time is off by " + ( ts_client - ts_server ) + " seconds" );

		pkt.Zero();
		pkt.setUsed( pkt_size );

		local dw0 = 0;
	//	dw0 = dw0 | ( 0 & 0x03 );			// LI
		dw0 = dw0 | ( ( 3 & 0x07 ) << 2 );	// VN
		dw0 = dw0 | ( ( 4 & 0x07 ) << 5 );	// Mode
	//	dw0 = dw0 | ( ( 0 & 0xff << 8 ) );	// Stratum
	//	dw0 = dw0 | ( ( 0 & 0xff << 16 ) );	// Poll
	//	dw0 = dw0 | ( ( 0 & 0xff << 24 ) );	// Precision

		// Set up request
		pkt.setUINT( 0, dw0 );
		pkt.setUINT( 6, _g.socket.htonl( ts_client + NTP_EPOCH ) );

		pkt.setUINT( 10, _g.socket.htonl( ts_server + NTP_EPOCH ) );

		// Write data
		if ( !_g.socket.SendToBin( pkt, 0 ) )
		{	_self.echo( "SendToBin() : " + _g.socket.getLastError() );
			return;
		} // end if

	} // end while

	return 0;
}

