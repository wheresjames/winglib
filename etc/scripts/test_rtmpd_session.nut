
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

function sendVideoHeaders( data ) : ( _g )
{
	// Decode header
	local p = CSqMulti();
	p.setJSON( _g.share.getHeader() );

	// Create SEI string
	local sei = "", pps = CSqBinary(), sps = CSqBinary();
	if ( p[ "codec" ].str() == "H264" && p[ "extra" ].len() )
	{	local extra = p[ "extra" ].bin(), s1 = extra.Find( "\x0\x0\x0\x1", 0, 0 );
		if ( extra.failed() != s1 )
		{	local s2 = extra.Find( "\x0\x0\x0\x1", s1 + 4, 0 );
			if ( extra.failed() != s2 )
			{	pps = extra.getSub( s1 + 4, s2 - s1 - 4 );
				sps = extra.getSub( s2 + 4, 0 );
				sei = extra.getSub( s1 + 4, s2 - s1 - 4 ).base64_encode()
					  + "," + extra.getSub( s2 + 4, 0 ).base64_encode();
			} // end if
		} // end if
	} // end if

	local e = data.Find( "onMetaData", 0, 0 ), eh = 0;
	if ( data.failed() != e )
	{
		_self.echo( " ********************** HEADER AT : " + e );

		eh = data.Find( "\x0\x0\x9\x0\x0", e, 0 );

		if ( data.failed() != eh )
		{
			_self.echo( " ********************** END HEADER AT : " + eh );

			local body = data.getSub( e - 3, eh - e + 3 + 3 );
			body.Unshare();
			_self.echo( body.AsciiHexStr( 16, 16 ) );

			local h = CSqMulti();
			h[ "pkt" ][ "format" ] <- "1";
			h[ "pkt" ][ "chunk_stream_id" ] <- "5";
			h[ "pkt" ][ "type_id" ] <- "18";
			h[ "pkt" ][ "timestamp" ] <- "0";
			h[ "pkt" ][ "info" ] <- "1";
			h[ "pkt" ][ "has_abs_timestamp" ] <- "0";

			h[ "body" ] <- body.getString();

			_self.echo( "onMetaData : " + _g.rtmp.SendPacket( h, 0 ) );

		} // end if

	} // end if

	{ // Send video frame headers

		local h = CSqMulti(), body = CSqBinary();
		h[ "pkt" ][ "format" ] <- "0";
		h[ "pkt" ][ "chunk_stream_id" ] <- "7";
		h[ "pkt" ][ "type_id" ] <- "9";
		h[ "pkt" ][ "timestamp" ] <- "0";
		h[ "pkt" ][ "info" ] <- "1";
		h[ "pkt" ][ "has_abs_timestamp" ] <- "0";

		body.Allocate( 1024 );
		body.setString( "\x17\x00\x00\x00\x00" );
		body.appendString( "\x01\x42\x80\x0d\xFF" );
//					body.appendString( "\x01\x42\x80\x1f\xFF" );

_self.echo( "pps = " + pps.getUsed() );
		body.appendString( "\xE1" );
		body.Resize( body.getUsed() + 2 );
		body.setUsed( body.getUsed() + 2 );
		body.BE_setAbsUSHORT( 11, pps.getUsed() );
		body.Append( pps );

_self.echo( "sps = " + sps.getUsed() );
		body.appendString( "\x1" );
		body.Resize( body.getUsed() + 2 );
		body.setUsed( body.getUsed() + 2 );
		body.BE_setAbsUSHORT( 11 + 2 + 1 + pps.getUsed(), sps.getUsed() );
		body.Append( sps );
		//body.Append( p[ "extra" ].bin() );

		h[ "body" ] <- body.getString();

		_self.echo( "Video Headers : " + _g.rtmp.SendPacket( h, 0 ) );

	} // end

	return eh;
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
	{
		// Attempt to open share
		if ( !_g.share.isOpen() )
			_g.share.Open( _g.sid );

		else
		{
			local data;
			do
			{
				data = _g.share.ReadData();

				if ( data.getUsed() )
				{
					data.Unshare();

//					_self.echo( " --- FLV " + _g.count + " ---\n" + data.AsciiHexStr( 16, 16 ) );

					// Skip file headers
					local e = 0;
					if ( !_g.count++ )
					{
						// Send video headers
						e = sendVideoHeaders( data );

						// Skip to first key frame
						e = data.findUSHORT( 0x0117, e, 0 );

					} // end if

					// Brute force skip FLV header
					else 
						e = 11;

					// If we found what we wanted
					if ( data.failed() != e )
					{
						data.LShift( e );
						data.Resize( data.getUsed() - 4 );

						data.BE_setAbsUINT( 5, data.getUsed() - 9 );

						local h = CSqMulti(), body = CSqBinary();
						h[ "pkt" ][ "format" ] <- "1";
						h[ "pkt" ][ "chunk_stream_id" ] <- "7";
						h[ "pkt" ][ "type_id" ] <- "9";
						h[ "pkt" ][ "timestamp" ] <- "0";
						h[ "pkt" ][ "info" ] <- "1";
						h[ "pkt" ][ "has_abs_timestamp" ] <- "0";

//						_self.echo( " --- FRAME ---\n" + data.AsciiHexStr( 16, 16 ) );
						h[ "body" ] <- data.getString();

						if ( !_g.rtmp.SendPacket( h, 0 ) )
						{	_g.quit = 1;
							_self.echo( " *** CLIENT CLOSED SOCKET *** " );
						} // end if

						else
							_self.echo( "*** Send Video frame : " + data.getUsed() );

					} // end if

					_g.share.incReadPtr();

				} // end if

			} while ( data.getUsed() );

		} // end else

		return 0;

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
	r[ "pkt" ][ "format" ] <- "0";
	r[ "pkt" ][ "chunk_stream_id" ] <- "3";
	r[ "pkt" ][ "type_id" ] <- "20";
	r[ "pkt" ][ "timestamp" ] <- "0";
	r[ "pkt" ][ "info" ] <- "0";
	r[ "pkt" ][ "has_abs_timestamp" ] <- "0";
	r[ "0" ] <- "_result";
	r[ "1" ] <- pkt[ "1" ].str();

	switch( pkt[ "0" ].str() )
	{
		case "connect" :

			{ // WinAckSize

				local h = CSqMulti(), body = CSqBinary( 4, 4 );
				h[ "pkt" ][ "format" ] <- "0";
				h[ "pkt" ][ "chunk_stream_id" ] <- "2";
				h[ "pkt" ][ "type_id" ] <- "5";
				h[ "pkt" ][ "timestamp" ] <- "0";
				h[ "pkt" ][ "info" ] <- "0";
				h[ "pkt" ][ "has_abs_timestamp" ] <- "0";

				// Set window acknowledgement size
				body.BE_setUINT( 0, 2500000 );

				h[ "body" ] <- body.getString();
				_self.echo( "WinAck : " + _g.rtmp.SendPacket( h, 0 ) );

			} // end

			{ // Set peer bandwidth size

				local h = CSqMulti(), body = CSqBinary( 5, 5 );
				h[ "pkt" ][ "format" ] <- "0";
				h[ "pkt" ][ "chunk_stream_id" ] <- "2";
				h[ "pkt" ][ "type_id" ] <- "6";
				h[ "pkt" ][ "timestamp" ] <- "0";
				h[ "pkt" ][ "info" ] <- "0";
				h[ "pkt" ][ "has_abs_timestamp" ] <- "0";

				// Window acknowledgement size
				body.BE_setUINT( 0, 2500000 );

				// Limit type : dynamic
				body.setCHAR( 4, 2 );

				h[ "body" ] <- body.getString();
				_self.echo( "Peer bandwidth size : " + _g.rtmp.SendPacket( h, 0 ) );

			} // end

			{ // Begin stream

				local h = CSqMulti(), body = CSqBinary( 6, 6 );
				h[ "pkt" ][ "format" ] <- "0";
				h[ "pkt" ][ "chunk_stream_id" ] <- "2";
				h[ "pkt" ][ "type_id" ] <- "4";
				h[ "pkt" ][ "timestamp" ] <- "0";
				h[ "pkt" ][ "info" ] <- "0";
				h[ "pkt" ][ "has_abs_timestamp" ] <- "0";

				body.Zero();

				// Stream Begin
				body.BE_setSHORT( 0, 0 );

				// Stream ID
				body.BE_setAbsUINT( 2, 0 );

				h[ "body" ] <- body.getString();
				_self.echo( "Begin stream 0 : " + _g.rtmp.SendPacket( h, 0 ) );

			} // end

			// Connect response
			r[ "2" ][ "fmsVer" ] <- "FMS/3,5,5,2004";
			r[ "2" ][ "capabilities" ] <- "31.0";
			r[ "2" ][ "mode" ] <- "1.0";

			r[ "3" ][ "level" ] <- "status";
			r[ "3" ][ "code" ] <- "NetConnection.Connect.Success";
			r[ "3" ][ "description" ] <- "Connection succeeded.";
			r[ "3" ][ "objectEncoding" ] <- pkt[ "2" ][ "objectEncoding" ].str();
			r[ "3" ][ "clientid" ] <- "1978006327";

			r[ "3" ][ "data" ][ "version" ] <- "3,5,5,2004";

			break;

		case "createStream" :
			r[ "2" ] <- "";
			r[ "3" ] <- ( _g.nStreamId++ ).tostring() + ".";
			break;

		case "getStreamLength" :
			r[ "2" ] <- "";
			r[ "3" ] <- "0.";
			break;

		case "checkBandwidth" :
			r[ "pkt" ][ "chunk_stream_id" ] <- "3";

			r[ "0" ] <- "onBWDone";
//			r[ "2" ] <- "";
			r[ "1" ] <- "0."; // 8192.";
			r[ "2" ] <- "";

			// r[ "0" ] <- "onBWCheck";
			// local bin = CSqBinary( 32767, 32767 );
			// _self.srand( 41 );
			// if ( bin.Randomize( 0, 0 ) )
			// {	r[ "2" ] <- "";
				// r[ "3" ] <- bin.getString();
				// _g.nBandwidthTime = _self.gmt_time_useconds().tostring();
			// } // end if
			// else r.clear(), _g.nBandwidthTime = 0;

			break;

		case "play" :

			{ // Begin stream

				local h = CSqMulti(), body = CSqBinary( 6, 6 );
				h[ "pkt" ][ "format" ] <- "0";
				h[ "pkt" ][ "chunk_stream_id" ] <- "2";
				h[ "pkt" ][ "type_id" ] <- "4";
				h[ "pkt" ][ "timestamp" ] <- "0";
				h[ "pkt" ][ "info" ] <- "0";
				h[ "pkt" ][ "has_abs_timestamp" ] <- "0";

				body.Zero();

				// Stream Begin
				body.BE_setSHORT( 0, 0 );

				// Stream ID
				body.BE_setAbsUINT( 2, 1 );

				h[ "body" ] <- body.getString();
				_self.echo( "Begin stream 1 : " + _g.rtmp.SendPacket( h, 0 ) );

			} // end

			{ // NetStream.Play.Reset

				local h = CSqMulti();
				h[ "pkt" ][ "format" ] <- "0";
				h[ "pkt" ][ "chunk_stream_id" ] <- "5";
				h[ "pkt" ][ "type_id" ] <- "20";
				h[ "pkt" ][ "timestamp" ] <- "0";
				h[ "pkt" ][ "info" ] <- "1";
				h[ "pkt" ][ "has_abs_timestamp" ] <- "0";

				h[ "0" ] <- "onStatus";
				h[ "1" ] <- "0.";
				h[ "2" ] <- "";
				h[ "3" ][ "level" ] <- "status";
				h[ "3" ][ "code" ] <- "NetStream.Play.Reset";
				h[ "3" ][ "description" ] <- "Playing and resetting " + pkt[ "3" ].str();
				h[ "3" ][ "clientid" ] <- "1978006327";

				_self.echo( "NetStream.Play.Reset : " + _g.rtmp.SendPacket( h, 0 ) );

			} // end

			{ // NetStream.Play.Start

				local h = CSqMulti();
				h[ "pkt" ][ "format" ] <- "0";
				h[ "pkt" ][ "chunk_stream_id" ] <- "5";
				h[ "pkt" ][ "type_id" ] <- "20";
				h[ "pkt" ][ "timestamp" ] <- "0";
				h[ "pkt" ][ "info" ] <- "1";
				h[ "pkt" ][ "has_abs_timestamp" ] <- "0";

				h[ "0" ] <- "onStatus";
				h[ "1" ] <- "0.";
				h[ "2" ] <- "";
				h[ "3" ][ "level" ] <- "status";
				h[ "3" ][ "code" ] <- "NetStream.Play.Start";
				h[ "3" ][ "description" ] <- "Started Playing " + pkt[ "3" ].str();
				h[ "3" ][ "clientid" ] <- "1978006327";
				h[ "3" ][ "isFastPlay" ] <- "0";
				h[ "3" ][ "timecodeOffset" ] <- ( _self.gmt_time_useconds() / 1000 ) + "L";

				_self.echo( "NetStream.Play.Start : " + _g.rtmp.SendPacket( h, 0 ) );

			} // end

			{ // |RtmpSampleAccess

				local h = CSqMulti();
				h[ "pkt" ][ "format" ] <- "1";
				h[ "pkt" ][ "chunk_stream_id" ] <- "5";
				h[ "pkt" ][ "type_id" ] <- "18";
				h[ "pkt" ][ "timestamp" ] <- "0";
				h[ "pkt" ][ "info" ] <- "0";
				h[ "pkt" ][ "has_abs_timestamp" ] <- "0";

				h[ "0" ] <- "|RtmpSampleAccess";
				h[ "1" ] <- "0";
				h[ "2" ] <- "0";

				_self.echo( "|RtmpSampleAccess : " + _g.rtmp.SendPacket( h, 0 ) );

			} // end

			{ // NetStream.Data.Start

				local h = CSqMulti();
				h[ "pkt" ][ "format" ] <- "0";
				h[ "pkt" ][ "chunk_stream_id" ] <- "5";
				h[ "pkt" ][ "type_id" ] <- "18";
				h[ "pkt" ][ "timestamp" ] <- "0";
				h[ "pkt" ][ "info" ] <- "1";
				h[ "pkt" ][ "has_abs_timestamp" ] <- "0";

				h[ "0" ] <- "onStatus";
				h[ "1" ][ "code" ] <- "NetStream.Data.Start";

				_self.echo( "NetStream.Data.Start : " + _g.rtmp.SendPacket( h, 0 ) );

			} // end

			{ // User control message

				local h = CSqMulti(), body = CSqBinary( 6, 6 );
				h[ "pkt" ][ "format" ] <- "0";
				h[ "pkt" ][ "chunk_stream_id" ] <- "2";
				h[ "pkt" ][ "type_id" ] <- "4";
				h[ "pkt" ][ "timestamp" ] <- "0";
				h[ "pkt" ][ "info" ] <- "0";
				h[ "pkt" ][ "has_abs_timestamp" ] <- "0";

				body.Zero();

				// Stream Begin
				body.BE_setSHORT( 0, 0x20 );

				// Stream ID
				body.BE_setAbsUINT( 2, 1 );

				h[ "body" ] <- body.getString();
				_self.echo( "User control message : " + _g.rtmp.SendPacket( h, 0 ) );

			} // end

			r.clear();

			{ // Start video stream

				local p = CSqMulti();
				p[ "codec" ] <- _g.codec;
				p[ "type" ] <- "flv";
				//p[ "type" ] <- "mp4";
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

			} // end start video stream

			break;

		default :
			r.clear();
			break;

	} // end switch

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
