
_self.load_module( "live555", "" );
_self.load_module( "ffmpeg", "" );

class CGlobal
{
	running = 0;

	count = 0;

	// Maximum number of seconds to stream video
	max_count = 0;
//	max_count = 30;

	l555 = CLvRtspServer();

};

local _g = CGlobal();

function _init() : ( _g )
{
	local avail = CSqMulti();
	avail[ "shark" ].deserialize( "fps=15,w=320,h=240" );

	// Set available streams
	_g.l555.setParam( "sources", avail );

	// Start the server
	if ( !_g.l555.StartServer( 554 ) )
	{	_self.echo( "RTSP server failed : " + _g.l555.getLastErrorStr() );
		return 0;
	} // end if

//	_self.set_timer( ".", 1000 / _g.vid.getFps().tointeger(), "DeliverFrame" );

	_self.set_timer( ".", 1000, "UpdateServer" );

	_self.echo( "Started RTSP Server : " + _g.l555.getParamStr( "url" ) );

	return 1;
}

function _idle() : ( _g )
{
	if ( _g.running && !_g.l555.isThread() )
	{	_self.echo( "Server thread has stopped" );
		return 1;
	} // end if

	return 0;
}

function UpdateServer() : ( _g )
{
	local streams = _g.l555.getParam( "streams" );
	if ( streams.size() )
	{
		_g.count++;

//		_self.echo( " --- STREAMS --- " );
//		_self.echo( streams.print_r( 1 ) );

		foreach( k,v in streams )
		{
			if ( v[ "run" ].toint() && !v[ "quit" ].len() )
				if ( !_self.is_path( k ) )
				{
					_self.echo( "Creating : " + k );

					// Share name
					local sid = v[ "share" ].str();
					if ( !sid.len() ) share = k;

					// Codec
					local codec = v[ "params" ][ "codec" ].str();
					if ( !codec.len() ) codec = "FMP4";

					// Start the streaming thread
					local p = CSqMulti();
					p[ "sid" ] <- sid;
					p[ "codec" ] <- codec;
					::_self.spawn( 1, ".", sid, "test_videostream.thread.nut", 1 );
					::_self.execute1( 0, sid, "StartStream", p.serialize() );

				} // end if

				else
				{
//					_self.echo( "Running : " + k );

					// Quit stream after 10 seconds
					if ( _g.max_count && _g.max_count < _g.count )
					{	_g.count = 0;
						_g.l555.setParamStr( "streams." + k + ".quit", "1" );
						_self.echo( "Quitting : " + k );
					} // end if

				} // end else

			else if ( v[ "run" ].toint() )
				_g.l555.setParamStr( "streams." + k + ".run", "0" ),
				_self.echo( "Stopped : " + k );

//			else
//				_self.echo( "Idle : " + k );

		} // end foreach

	} // end if

	return 0;
}

