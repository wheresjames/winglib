
class CGlobal
{
	quit = 0;
	session = 0;
	fps = 0;
	rtmp = 0;

	// RTMP streamid
	nStreamId = 0;
};

local _g = CGlobal();

function _idle() : ( _g )
{
	return _g.quit;
}

function _init() : ( _g )
{
	// Start the update timer
	if ( 0 >= _g.fps ) _g.fps = 30;
	_self.set_timer( ".", 1000 / _g.fps, "Run" );

	_self.echo( "... Starting session ..." );

	return 0;
}

function _exit() : ( _g )
{
	if ( _g.rtmp )
		_g.rtmp.Destroy(), _g.rtmp = 0;

	if ( _g.session )
		_g.session.Destroy(), _g.session = 0;

	return 0;
}

function SetSocket( socket ) : ( _g )
{
	// Load rtmpd module
	if ( _self.load_module( "rtmpd", "" ) )
	{	_self.echo( "!!! Unable to load RTMPD module !!!" );
		_g.quit = 1;
		return 0;
	} // end if

	// Important! must do a share here
	_g.session = CSqSocket();
	_g.session.Share( socket );

	// Show remote client address
	local addr = CSqSockAddress();
	if ( _g.session.getPeerAddress( addr ) )
		_self.echo( "... Connecting to : " + addr.getDotAddress() );
	else
		_self.echo( "... FAILED TO CONNECT ..." );

	_self.echo( "... Connected" );

	return 1;
}

function Run() : ( _g )
{
	if ( _g.quit || !_g.session )
		return 0;

	if ( !_g.rtmp )
	{
		// Create rtmpd session object
		_g.rtmp = CRtmpdSession();

		// Initialize RTMP stuff
		if ( !_g.rtmp.Init( _g.session ) )
		{	_self.echo( "!!! RTMP Session Init() Failed : " + _g.rtmp.getLastErrorStr() );
			_g.quit = 1;
			return 0;
		} // end if

	} // end if

	local ret = _g.rtmp.ReadPacket();

	_self.echo( "... ReadPacket() : " + ret );

	// Connection closed?
	if ( 0 > ret )
	{	_g.quit = 1;
		return 0;
	} // end if

	// No packet
	else if ( !ret )
		return 0;

	_self.echo( "... Packet Type : " + ret + format( " : 0x%x", ret ) );
	_self.echo( "... Packet Size : " + _g.rtmp.getPacketSize() );

	local data = _g.rtmp.getPacketData( 0 );
	_self.echo( "\n --- CMD BINARY ---\n" + data.AsciiHexStr( 16, 32 ) );

	local pkt = _g.rtmp.getPacket( 0 );
	_self.echo( "\n --- CMD PACKET ---\n" + pkt.print_r( 1 ) );

	// Generic reply packet
	local r = CSqMulti();
	r[ "pkt" ][ "channel" ] <- "3";
	r[ "pkt" ][ "header_type" ] <- "1";
	r[ "pkt" ][ "packet_type" ] <- "20";
	r[ "pkt" ][ "timestamp" ] <- "0";
	r[ "pkt" ][ "infofield2" ] <- "0";
	r[ "pkt" ][ "has_abs_timestamp" ] <- "0";
	r[ "0" ] <- "_result";
	r[ "1" ] <- pkt[ "1" ].str();

	// *** connect
	if ( pkt[ "0" ].str() == "connect" )
	{
		r[ "2" ][ "fmsVer" ] <- "FMS/3,5,1,525";
		r[ "2" ][ "capabilities" ] <- "31.0";
		r[ "2" ][ "mode" ] <- "1.0";

		r[ "3" ][ "level" ] <- "status";
		r[ "3" ][ "code" ] <- "NetConnection.Connect.Success";
		r[ "3" ][ "description" ] <- "Connection succeeded.";
		r[ "3" ][ "objectEncoding" ] <- pkt[ "2" ][ "objectEncoding" ].str();
		r[ "3" ][ "version" ] <- "3,5,1,525";

	} // end if

	// *** createStream
	else if ( pkt[ "0" ].str() == "createStream" )
		r[ "2" ] <- "", 
		r[ "3" ] <- ( _g.nStreamId++ ).tostring() + ".";

	// *** getStreamLength
	else if ( pkt[ "0" ].str() == "getStreamLength" )
		r[ "2" ] <- "", 
		r[ "3" ] <- "10.";

	// Do we have a reply packet to send?
	if ( r.size() )
	{
		// Send the reply packet
		_self.echo( "SendPacket() : " + _g.rtmp.SendPacket( r, 0 ) );

		// Show reply packet
		_self.echo( "\n --- REPLY PACKET ---\n" + r.print_r( 1 ) );

		// Show packet encoding
		local snd = CSqBinary();
		if ( !_g.rtmp.SerializePacket( snd, r, 0 ) )
			_self.echo( "!!! Failed to serialize packet" );
		else _self.echo( "\n --- REPLY BINARY ---\n" + snd.AsciiHexStr( 16, 32 ) );

	} // end if

	return 1;
}
