
_self.load_module( "live555", "" );
_self.load_module( "ffmpeg", "" );

class CGlobal
{
	file = _self.path( "../media/nurse_shark.avi" );

	fps = 15;
	brate = 2000000;

	running = 0;

	l555 = CLvRtspServer();

	vid = CFfContainer();

	tc = CFfTranscode();
	frame = CSqBinary();
	
};

local _g = CGlobal();

function _init() : ( _g )
{
	if ( !StartServer() )
		return 0;

	_self.echo( "Started RTSP Server" );
		
	return 1;
}

function StartServer() : ( _g )
{
	if ( !_g.vid.Open( _g.file, CSqMulti() ) )
	{	_self.echo( "failed to open file" );
		return;
	} // end if

	_self.echo( "Video File : " + _g.vid.getWidth() + "x" + _g.vid.getHeight() );

	// Init transcoder
	if ( !_g.tc.Init( _g.vid.getWidth(), _g.vid.getHeight(), _g.fps, _g.brate,
				      _g.vid.getVideoCodecId(), CFfDecoder().LookupCodecId( "FMP4" ) ) )
	{	_self.echo( "failed to create transcoder" );
		return;
	} // end if

	// Start the server
	if ( !_g.l555.StartServer( 554 ) )
	{	_self.echo( "Failed to open RTSP stream " + inf[ 1 ] );
		return 0;
	} // end if

	// Create the stream
	_g.l555.Msg( "cmd=CreateStream,id=shark,desc=shark,fps=" + _g.vid.getFps() );

	_self.set_timer( ".", 1000 / _g.vid.getFps().tointeger(), "DeliverFrame" )

	return 1;
}

function DeliverFrame() : ( _g )
{
	if ( !_g.running )
	{	if ( !_g.l555.isThread() ) return; _g.running = 1;
		_self.echo( "RTSP Server running : " + _g.l555.getUrl( "shark" ) );
	} // end if
	else if ( !_g.running ) return;

	local frame_info = CSqMulti();
	while ( 0 <= ( stream = _g.vid.ReadFrame( _g.frame, frame_info ) ) )
	{
		if ( _g.vid.getVideoStream() == stream )
		{
//			_self.echo( "input image = " + _g.frame.getUsed() );
			local tframe = CSqBinary();
			if ( !_g.tc.Transcode( _g.frame, tframe, frame_info, 0 ) )
			{	_self.echo( "Transcode() failed" );
				return;
			} // end if		
		
			// Send encoded frame to rtsp server
			local fid = _self.unique();
			_self.set_binshare( fid, tframe );
			_g.l555.Msg( "cmd=DeliverFrame,stream_id=shark,frame_id=" + fid );
			
			return;
		
		} // end if
		
	} // end while

	// Reopen file
	_g.vid.Open( _g.file, CSqMulti() );

}

function _idle() : ( _g )
{
	if ( _g.running && !_g.l555.isThread() )
	{	_self.echo( "Server thread has stopped" );
		return 1;
	} // end if

	return 0;
}

