
class CGlobal
{
	quit = 0;
	session = 0;
	fps = 0;
	rtmp = 0;

	sid = "";
	share = 0;
//	codec = "FLV1";
	codec = "H264";
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
		{
			if ( _g.share.Open( _g.sid ) )
			{
				// Decode header
				local p = CSqMulti(); 
				p.setJSON( _g.share.getHeader() );

				// Create SEI string
				local sei = "";
				if ( p[ "codec" ].str() == "H264" && p[ "extra" ].len() )
				{	local extra = p[ "extra" ].bin(), s1 = extra.Find( "\x0\x0\x0\x1", 0, 0 );
					if ( extra.failed() != s1 )
					{	local s2 = extra.Find( "\x0\x0\x0\x1", s1 + 4, 0 );
						if ( extra.failed() != s2 )
							sei = extra.getSub( s1 + 4, s2 - s1 - 4 ).base64_encode()
								  + "," + extra.getSub( s2 + 4, 0 ).base64_encode();
					} // end if
				} // end if

				{ // onMetaData

					local h = CSqMulti();
					h[ "pkt" ][ "format" ] <- "1";
					h[ "pkt" ][ "chunk_stream_id" ] <- "5";
					h[ "pkt" ][ "type_id" ] <- "18";
					h[ "pkt" ][ "timestamp" ] <- "0";
					h[ "pkt" ][ "info" ] <- "1";
					h[ "pkt" ][ "has_abs_timestamp" ] <- "0";

					h[ "0" ] <- "onMetaData";
					h[ "1" ][ "videocodecid" ] <- "avc1";
					h[ "1" ][ "videoframerate" ] <- p[ "fps" ].str();
					h[ "1" ][ "width" ] <- p[ "w" ].str(); 
					h[ "1" ][ "height" ] <- p[ "h" ].str();
					h[ "1" ][ "avcprofile" ] <- "66";
					h[ "1" ][ "avclevel" ] <- "30";
//					h[ "1" ][ "frameWidth" ] <- "320";
//					h[ "1" ][ "frameHeight" ] <- "240";
//					h[ "1" ][ "displayWidth" ] <- "320";
//					h[ "1" ][ "displayHeight" ] <- "240";
					h[ "1" ][ "trackinfo" ][ "timescale" ] <- "15";
					h[ "1" ][ "trackinfo" ][ "language" ] <- "eng";
					h[ "1" ][ "trackinfo" ][ "sampledescription" ][ "sampletype" ] <- _g.codec;
					h[ "1" ][ "trackinfo" ][ "type" ] <- "video";

					if ( sei.len() )
						h[ "1" ][ "trackinfo" ][ "sprop-parameter-sets" ] <- sei;

					// http://tools.ietf.org/html/rfc3984
					// http://en.wikipedia.org/wiki/H.264#Levels
//					h[ "1" ][ "trackinfo" ][ "profile-level-id" ] <- "4d800d"; // 66 | 00 | 13

//					h[ "1" ][ "trackinfo" ][ "description" ] <- "";
//					h[ "1" ][ "rtpsessioninfo" ][ "name" ] <- "RTSP Server";
//					h[ "1" ][ "rtpsessioninfo" ][ "timing" ] <- "0 0";
//					h[ "1" ][ "rtpsessioninfo" ][ "protocolversion" ] <- "0";
//					h[ "1" ][ "rtpsessioninfo" ][ "attributes" ][ "range" ] <- "npt=now-";

					_self.echo( "onMetaData " + h.print_r( 1 ) );
					_self.echo( "onMetaData : " + _g.rtmp.SendPacket( h, 0 ) );

				} // end

			} // end if

		} // end if

		else
		{
			local data;
			do
			{
				data = _g.share.ReadData();
//_self.echo( "*** data = " + data.getUsed() );
 
				if ( data.getUsed() )
				{
					local h = CSqMulti(), body = CSqBinary( 4, 4 );
					h[ "pkt" ][ "format" ] <- "0";
	//				h[ "pkt" ][ "chunk_stream_id" ] <- "2";
					h[ "pkt" ][ "chunk_stream_id" ] <- "7";
	//				h[ "pkt" ][ "type_id" ] <- "5";
					h[ "pkt" ][ "type_id" ] <- "9";
					h[ "pkt" ][ "timestamp" ] <- "0";
					h[ "pkt" ][ "info" ] <- "1";
					h[ "pkt" ][ "has_abs_timestamp" ] <- "0";

					data.Unshare();
					local e = data.failed();
					if ( !_g.count++ )
						e = data.findUCHAR( 0x17, 0, 0 );
					else if ( data.failed() == ( e = data.findUCHAR( 0x17, 0, 16 ) ) )
						e = data.findUCHAR( 0x27, 0, 16 );
					if ( data.failed() != e )
						data.LShift( e );

					h[ "body" ] <- data.getString();

					_self.echo( "*** Send Video : " + data.getUsed() + " : " + _g.rtmp.SendPacket( h, 0 ) );

					_g.format = 1;

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
//				body.BE_setUINT( 0, 0x140000 );
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

/*
			{ // Set chunk size

				local h = CSqMulti(), body = CSqBinary( 4, 4 );
				h[ "pkt" ][ "chunk_stream_id" ] <- "2";
				h[ "pkt" ][ "format" ] <- "0";
				h[ "pkt" ][ "type_id" ] <- "1";
				h[ "pkt" ][ "timestamp" ] <- "0";
				h[ "pkt" ][ "info" ] <- "0";
				h[ "pkt" ][ "has_abs_timestamp" ] <- "0";

				body.Zero();

				// Window acknowledgement size 
				body.BE_setUINT( 0, 0x1000 );

				h[ "body" ] <- body.getString();
				_self.echo( "Begin stream : " + _g.rtmp.SendPacket( h, 0 ) );

			} // end
*/

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
				_self.echo( "Begin stream : " + _g.rtmp.SendPacket( h, 0 ) );

			} // end
/*
			{ // Set chunk size

				local h = CSqMulti(), body = CSqBinary( 4, 4 );
				h[ "pkt" ][ "format" ] <- "0";
				h[ "pkt" ][ "chunk_stream_id" ] <- "2";
				h[ "pkt" ][ "type_id" ] <- "1";
				h[ "pkt" ][ "timestamp" ] <- "0";
				h[ "pkt" ][ "info" ] <- "0";
				h[ "pkt" ][ "has_abs_timestamp" ] <- "0";

				body.Zero();

				// Stream ID
				body.BE_setUINT( 0, 4096 );

				h[ "body" ] <- body.getString();
				_self.echo( "Set chunk size : " + _g.rtmp.SendPacket( h, 0 ) );

			} // end
*/
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
				p[ "fps" ] <- "15";
				// p[ "header" ] <- "\x17"; //\x0\x0"; //\x0\x0";

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
