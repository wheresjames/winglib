
_self.load_module( "live555", "" );
_self.load_module( "ffmpeg", "" );

class CGlobal
{
	running = 0;

	l555 = CLvRtspServer();

	vid = CFfContainer();

};

local _g = CGlobal();

function _init() : ( _g )
{
	if ( !StartServer() )
		return 0;

	_self.echo( "RTSP Server running..." );

	return 1;
}

function StartServer() : ( _g )
{
	local file = _self.path( "../media/nurse_shark.avi" );

	if ( !_g.vid.Open( file, CSqMulti() ) )
	{	_self.echo( "failed to open file" );
		return;
	} // end if

	_self.echo( "Video File : " + _g.vid.getWidth() + "x" + _g.vid.getHeight() );

//	local dec = CFfDecoder();
//	if ( !dec.Create( vid.getVideoCodecId(), vid.getVideoFormat(),
//					  vid.getWidth(), vid.getHeight(), 0 ) )
//	{	_self.echo( "failed to create decoder" );
//		return;
//	} // end if

	_g.l555.setServerCallback( _self.queue(), "doGetNextFrame" );

	if ( !_g.l555.StartServer( CSqMulti() ) )
	{	_self.echo( "Failed to open RTSP stream " + inf[ 1 ] );
		return 0;
	} // end if

	return 1;
}

function doGetNextFrame() : ( _g )
{
	local stream = -1;
	local frame = CSqBinary();
	local frame_info = CSqMulti();
	while ( 0 <= ( stream = _g.vid.ReadFrame( frame, frame_info ) ) )
	{
		if ( _g.vid.getVideoStream() == stream )
		{
			_self.echo( "delivering frame" );
//			_g.l555.setNextFrame( frame );
			_g.l555.deliverFrame( frame );

			return;

		} // end if

	} // end while

}

function _idle() : ( _g )
{
	if ( !_g.l555.isThread() )
	{	_self.echo( "Server thread has stopped" );
		return 1;
	} // end if

	return 0;
}

