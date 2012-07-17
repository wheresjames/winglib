
class CGlobal
{
	quit = 0;
	session = 0;
	fps = 30;
	rtmp = 0;

	sid = "";
	share = 0;
	codec = "H264";
//	codec = "VP80";
//	codec = "MPG4";

	count = 0;

	format = 0;

	frame = CSqBinary();
	
	pps = 0;
	sps = 0;
	sei = "";
	level = 0;
	sei_sent = 0;

	name = "";
	params = 0;

	// RTMP streamid
	nStreamId = 1;

	nBandwidthTime = 0;
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

function createSeiHeaders() : ( _g )
{
	// Decode header
	local p = CSqMulti();
	p.setJSON( _g.share.getHeader() );

	// Create SEI string
	local sei = "", pps = CSqBinary(), sps = CSqBinary();
	if ( p[ "codec" ].str() != "H264" || !p[ "extra" ].len() )
		return 0;

	local extra = p[ "extra" ].bin(), s1 = extra.Find( "\x0\x0\x0\x1", 0, 0 );
	if ( extra.failed() == s1 )
		return 0;

	local s2 = extra.Find( "\x0\x0\x0\x1", s1 + 4, 0 );
	if ( extra.failed() == s2 )
		return 0;

	// Get sei data
	_g.pps = extra.getSub( s1 + 4, s2 - s1 - 4 );
	_g.sps = extra.getSub( s2 + 4, 0 );
	_g.sei = _g.pps.base64_encode() + "," +_g.sps.base64_encode();
	_g.level = p[ "level" ].toint();

	return 1;
}

function sendSeiHeaders() : ( _g )
{
	// Create headers if needed
	if ( !_g.pps || !_g.sps )
		if ( !createSeiHeaders() )
			return 0;

	_g.sei_sent = 1;

	local body = CSqBinary( 128 );
	body.setString( "\x17\x00\x00\x00\x00" );
	body.appendString( "\x01\x42\x00\x0d\xFF" );
	body.setUCHAR( 8, _g.level );

	body.appendString( "\xE1" );
	body.Resize( body.getUsed() + 2 );
	body.setUsed( body.getUsed() + 2 );
	body.BE_setAbsUSHORT( 11, _g.pps.getUsed() );
	body.Append( _g.pps );

	body.appendString( "\x1" );
	body.Resize( body.getUsed() + 2 );
	body.setUsed( body.getUsed() + 2 );
	body.BE_setAbsUSHORT( 11 + 2 + 1 + _g.pps.getUsed(), _g.sps.getUsed() );
	body.Append( _g.sps );

	_g.rtmp.SendPacketBin( 0, 7, 9, 1, body, 0 );

	return 1;
}

function SendVideo() : ( _g )
{
	if ( !_g.share )
		return 0;

	// Attempt to open share
	if ( !_g.share.isOpen() )
		return _g.share.Open( _g.sid );

	local data;
	do
	{
		// Grab the input data
		data = _g.share.ReadData();
		if ( !data.getUsed() )
			return 1;

//if ( !_g.count++ )
//	_self.echo( "--- VIDEO FRAME ---\n" + data.AsciiHexStr( 16, 32 ) );

		// Append packet fragment
		_g.frame.Append( data );

		// Parse FLV packet
		local m = CSqMulti(), used = _g.rtmp.ParseFlv( _g.frame, m );
		if ( 0 > used )
			_self.echo( "--- VIDEO FRAME ---\n !!! err = " + _g.rtmp.getLastErrorStr() ),
			_self.echo( _g.frame.AsciiHexStr( 16, 64 ) ),
			_g.frame.setUsed( 0 );

//_self.echo( "used = " + used );
//_self.echo( "data.getUsed() = " + data.getUsed() );
//_self.echo( m.print_r( 1 ) );

		// Process each packet
		for ( local i = 0; m.isset( i.tostring() ); i++ )
		{
			local si = i.tostring();

//_self.echo( "m[ si ] = " + m[ si ].print_r( 1 ) );

			local pkt = _g.frame.getSub( m[ si ][ "dat" ].toint(),
										 m[ si ][ "dsz" ].toint() );

//_self.echo( "--- PKT ---\n" + pkt.AsciiHexStr( 16, 64 ) );
//_self.echo( "PKT TYPE = " + m[ si ][ "id" ].toint() );

			switch( m[ si ][ "id" ].toint() )
			{
				case 0x12 :
					local meta = CSqMulti();
					if ( _g.rtmp.DeserializePacket( pkt, meta, 0 ) )
						_self.echo( " === onMetaData ===\n" + meta.print_r( 1 ) );
/*
_self.echo( "--- onMetaData Packet ---\n" + pkt.AsciiHexStr( 16, 64 ) );

					local meta = CSqMulti();
					if ( _g.rtmp.DeserializePacket( pkt, meta, 0 ) )
					{
//						meta[ "1" ].unset( "duration" );
//						meta[ "1" ].unset( "filesize" );

						meta[ "1" ][ "trackinfo" ][ "_arraytype" ] <- "strict";
						meta[ "1" ][ "trackinfo" ][ "timescale" ] <- _g.fps.tostring();
						meta[ "1" ][ "trackinfo" ][ "language" ] <- "eng";
						meta[ "1" ][ "trackinfo" ][ "type" ] <- "video";
						meta[ "1" ][ "trackinfo" ][ "profile-level-id" ] <- "42000d";

						// Create sei headers
						if ( !_g.pps || !_g.sps ) createSeiHeaders();
						if ( _g.sei.len() ) meta[ "1" ][ "trackinfo" ][ "sprop-parameter-sets" ] <- _g.sei;

						pkt.setUsed( 0 ); pkt.Unshare();
						//_g.rtmp.SerializePacket( pkt, meta, _g.rtmp.eFlagEmcaArray );
						_g.rtmp.SerializePacket( pkt, meta, 0 );

_self.echo( " === onMetaData Parsed ===\n" + meta.print_r( 1 ) );

_self.echo( "--- Seialized Packet ---\n" + pkt.AsciiHexStr( 16, 64 ) );

					} // end if
*/
					if ( !_g.rtmp.SendPacketBin( 1, 5, 18, 1, pkt, 0 ) )
						_g.quit = 1;
					break;

				case 0x09 :

					if ( !_g.sei_sent )
						sendSeiHeaders();

					pkt.BE_setAbsUINT( 5, pkt.getUsed() - 9 );
					if ( !_g.rtmp.SendPacketBin( 1, 7, 9, 1, pkt, 0 ) )
						_g.quit = 1;

					break;

			} // end switch

		} // end for

		// Shift out used bytes
		if ( 0 < used )
			_g.frame.LShift( used );

	} while ( _g.share.incReadPtr() );

	return 1;
}

function ProcessCommands() : ( _g )
{
	// Attempt to read a packet
	local ret = _g.rtmp.ReadPacket();

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

	switch( pkt[ "0" ].str() )
	{
		case "connect" :

			local url = _self.parse_url( pkt[ "2" ][ "tcUrl" ].str() );
			_g.params = CSqMulti( url[ "extra" ].str() );

			// Get device name
			_g.name = _g.params[ "id" ].str();
			if ( !_g.name.len() && url[ "path" ].len() )
				_g.name = url[ "path" ].str().slice( 1 );

			// Did the user specify a valid device
			if ( !_g.name.len() )
			{	_self.echo( "!!! RTMP : Invalid device : " + pkt.getJSON() );
				_g.quit = 1;
				return 0;
			} // end if

			_self.echo( "RTMP Client connected to device : " + _g.name );

			local body = CSqBinary();

			// WinAckSize
			_g.rtmp.SendPacketBin( 0, 2, 5, 0, 
								   body.Using( 4, 4 )
								   .BE_setUINT( 0, 2500000 ), 0 );

			// Set peer bandwidth
			_g.rtmp.SendPacketBin( 0, 2, 6, 0, 
								   body.Using( 5, 5 )
								   .BE_setUINT( 0, 2500000 )
								   .setCHAR( 4, 2 ), 0 );

			// Sream Begin 0
			_g.rtmp.SendPacketBin( 0, 2, 4, 0, 
								   body.Using( 6, 6 )
								   .BE_setSHORT( 0, 0 )
								   .BE_setAbsUINT( 2, 0 ), 0 );

			// Send chunk size
			_g.rtmp.SendPacketBin( 0, 2, 1, 0, 
								   body.Using( 4, 4 )
								   .BE_setUINT( 0, _g.rtmp.getDefaultChunkSize() ), 0 );

			// Send Connect reply
			_g.rtmp.SendPacket2( 0, 3, 20, 0, CSqMulti(
								 "0=_result"
								 + ",1=" + pkt[ "1" ].str_urlenc()
								 + ",2={fmsVer=FMS/3%2C5%2C5%2C2004,capabilities=31.0,mode=1.0}"
								 + ",3={level=status,code=NetConnection.Connect.Success"
									+ ",description=Connection Succeeded."
									+ ",objectEncoding=" + pkt[ "2" ][ "objectEncoding" ].str_urlenc()
									+ ",clientid=" + _self.urlencode( _g.sid )
									+ ",data={version=" + _self.urlencode( "3,5,5,2004" ) + "}"
									+ "}"
								 ), 0 );

			break;

		case "createStream" :
			return _g.rtmp.SendPacket2( 0, 3, 20, 0, CSqMulti(
										"0=_result"
										+ ",1=" + pkt[ "1" ].str()
										+ ",2="
										+ ",3=1."
										), 0 );

		case "checkBandwidth" :
			return _g.rtmp.SendPacket2( 0, 3, 20, 0, CSqMulti(
										"0=onBWDone"
										+ ",1=0."
										+ ",2="
										), 0 );

		case "getStreamLength" :
			return _g.rtmp.SendPacket2( 0, 3, 20, 0, CSqMulti(
										"0=_result"
										+ ",1=" + pkt[ "1" ].str()
										+ ",2="
										+ ",3=0."
										), 0 );

		case "play" :

			local body = CSqBinary();

			// Sream Begin 1
			_g.rtmp.SendPacketBin( 0, 2, 4, 0, 
								   body.Using( 6, 6 )
								   .BE_setSHORT( 0, 0 )
								   .BE_setAbsUINT( 2, 1 ), 0 );

  
			// NetStream.Play.Reset
			_g.rtmp.SendPacket2( 0, 5, 20, 1, CSqMulti(
								 "0=onStatus"
								 + ",1=0."
								 + ",2="
								 + ",3={level=status,code=NetStream.Play.Reset"
									+ ",description=Connection Succeeded."
									+ ",clientid=" + _self.urlencode( _g.sid )
									+ "}"
								), 0 );

			// NetStream.Play.Start
			_g.rtmp.SendPacket2( 0, 5, 20, 1, CSqMulti(
								 "0=onStatus"
								 + ",1=0."
								 + ",2="
								 + ",3={level=status,code=NetStream.Play.Start"
									+ ",description=Started Playing " + _self.urlencode( pkt[ "3" ].str() )
									+ ",clientid=" + _self.urlencode( _g.sid )
									+ ",timecodeOffset=" + ( _self.gmt_time_useconds() / 1000 ) + "L"
									+ "}"
								), 0 );

			// |RtmpSampleAccess
			_g.rtmp.SendPacket2( 1, 5, 18, 0, CSqMulti(
								 "0=|RtmpSampleAccess"
								 + ",1=0"
								 + ",2=0"
								), 0 );

			// NetStream.Data.Start
			_g.rtmp.SendPacket2( 0, 5, 18, 1, CSqMulti(
								 "0=onStatus,1={code=NetStream.Data.Start}"
								), 0 );

			// User control message
			_g.rtmp.SendPacketBin( 0, 2, 4, 0, 
								   body.Using( 6, 6 )
								   .BE_setSHORT( 0, 0x20 )
								   .BE_setAbsUINT( 2, 1 ), 0 );

			{ // Start video stream

				local p = CSqMulti();
				p[ "codec" ] <- _g.codec;
				p[ "type" ] <- "flv";
				p[ "w" ] <- "320";
				p[ "h" ] <- "240";
				p[ "fps" ] <- _g.fps.tostring();
				p[ "encoder_params" ][ "global_headers" ] <- "1";

				// Generate a stream id
				_g.sid = _self.unique();

				// Start the streaming thread
				p[ "sid" ] <- _g.sid;
				::_self.spawn( 1, ".", _g.sid, "test_videostream.thread.nut", 1 );
				::_self.execute1( 0, _g.sid, "StartStream", p.serialize() );

				// Create fifo share object
				_g.share = CSqFifoShare();

				// Don't skip initial frames
				_g.share.setFlag( _g.share.eFlagNoInitSync, 1 );

			} // end start video stream

			break;

	} // end switch

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

	// Are we streaming video
	if ( _g.share )
		return SendVideo();

	// Process commands
	return ProcessCommands();
}
