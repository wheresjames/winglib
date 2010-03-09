
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

		if ( !_g.socket.ReadFromBin( pkt, 0 ) )
		{	_self.echo( "ReadFromBin() : " + socket.getLastError() );
			continue;
		} // end if

		local addr = CSqSockAddress();
		_g.socket.getPeerAddress( addr );
		local server = addr.getDotAddress();
		_self.echo( "Data read from client : " + server) );

//		CSqFile().put_contents_bin( "sntp.in." + server + ".dat", pkt );

		if ( pkt.getUsed() < pkt_size )
		{	_self.echo( "Too few bytes sent from client : " + pkt.getUsed() );
			continue;
		} // end if

		// What is the clients time
		local ts_client = _g.socket.ntohl( pkt.getUINT( 6 ) );
		ts_client = ( ts_client >= NTP_EPOCH ) ? ts_client -= NTP_EPOCH : 0;
		local ts_server = _self.gmt_time();

		local tm = CSqTime();
		tm.SetUnixTime( ts_server );
		_self.echo( "Server Time : " + tm.FormatTime( "%W, %B %D, %Y - %h:%m:%s %A" ) );
		tm.SetUnixTime( ts_client );
		_self.echo( "Local  Time : " + tm.FormatTime( "%W, %B %D, %Y - %h:%m:%s %A" ) );
		_self.echo( "\nClient time is off by " + ( ts_client - ts_server ) + " seconds" );

		pkt.Zero();
		pkt.setUsed( pkt_size );

		local dw0 = 0;
		dw0 = dw0 | ( ( 0 & 0x03 ) << 30 );	// LI
		dw0 = dw0 | ( ( 3 & 0x07 ) << 27 );	// VN
		dw0 = dw0 | ( ( 4 & 0x07 ) << 24 );	// Mode
		dw0 = dw0 | ( ( 2 & 0x0f ) << 16 );	// Stratum
		dw0 = dw0 | ( ( 6 & 0xff ) << 8 );	// Poll
//		dw0 = dw0 | ( ( 0 & 0xff ) << 0 );	// Precision
		pkt.setUINT( 0, _g.socket.htonl( dw0 ) );

		// 3	= Reference Identifier
		pkt.setUINT( 3, _g.socket.htonl( ts_server ) );

		// 4-5	= Reference Timestamp 
		pkt.setUINT( 4, _g.socket.htonl( ts_server + NTP_EPOCH ) );

		// 6-7	= Originate Timestamp
		if ( ts_client ) pkt.setUINT( 6, _g.socket.htonl( ts_client + NTP_EPOCH ) );

		// 8-9	= Receive Timestamp
		pkt.setUINT( 8, _g.socket.htonl( ts_server + NTP_EPOCH ) );

		// 10-11= Transmit Timestamp
		pkt.setUINT( 10, _g.socket.htonl( ts_server + NTP_EPOCH ) );

//		CSqFile().put_contents_bin( "sntp.out." + server + ".dat", pkt );

		// Write data
		if ( !_g.socket.SendToBin( pkt, 0 ) )
		{	_self.echo( "SendToBin() : " + _g.socket.getLastError() );
			return;
		} // end if

	} // end while

	return 0;
}

