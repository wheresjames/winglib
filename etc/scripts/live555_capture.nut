
_self.load_module( "live555", "" );
_self.load_module( "ffmpeg", "" );

class CGlobal
{
	running = 0;

	fps = 15;
	brate = 2000000;

	use_ffmpeg_decode = 0;
	save_avi = ""; //capture_test.avi";

	l555 = CLvRtspServer();

	cap = CSqCapture();
//	cap = CFfCapture();

	enc = CFfEncoder();
	tc = CFfTranscode();
	frame = CSqBinary();
	tframe = CSqBinary();

	frame_count = 0;

	img = CSqImage();
	avi = CFfContainer();
};

local _g = CGlobal();

function _init() : ( _g )
{
	if ( !StartServer() )
		return 0;

	return 1;
}

function StartServer() : ( _g )
{

	if ( 0 > _g.cap.Init( 0, 0, 320, 240, _g.fps, _g.use_ffmpeg_decode ? "JPEG" : "RGB3", 1 ) )
	{	_self.echo( "Unable to start capture device" ); return 0; }

	// Create empty image
	if ( !_g.img.Create( _g.cap.getWidth(), _g.cap.getHeight() ) )
	{	_self.echo( "Unable to create image buffer" ); return 0; }

/*
	local disp = "";
	local formats = split( _g.cap.GetSupportedFormats(), "," );
	foreach( f in formats )
		disp += f + " - " + _g.cap.GetFormatDescription( f ) + "\n";

	_self.echo( disp );
*/

	if ( _g.use_ffmpeg_decode )
	{
		// Create transcoder
		if ( !_g.tc.Init( _g.cap.getWidth(), _g.cap.getHeight(), _g.fps, _g.brate,
						  CFfDecoder().LookupCodecId( "MJPG" ), CFfDecoder().LookupCodecId( "FMP4" ) ) )
		{	_self.echo( "failed to create transcoder" ); return 0; }

	} // end if

	else
	{
		if ( !_g.enc.Create( CFfDecoder().LookupCodecId( "FMP4" ), CFfFmt().AV_PIX_FMT_YUV420P,
							 _g.cap.getWidth(), _g.cap.getHeight(), _g.fps, _g.brate, CSqMulti() ) )
		{	_self.echo( "failed to create encoder" ); return 0; }

	} // end else

	if ( _g.save_avi.len() )
	{
		// Create avi file
		if ( !_g.avi.Create( _g.save_avi, "", CSqMulti() ) )
		{	_self.echo( "failed to create avi" ); return 0; }

		if ( 0 > _g.avi.AddVideoStream( CFfDecoder().LookupCodecId( "FMP4" ), _g.cap.getWidth(), _g.cap.getHeight(), _g.fps ) )
		{	_self.echo( "failed to add video stream" ); return 0; }

		if ( !_g.avi.InitWrite() )
		{	_self.echo( "failed to initialize file" ); return 0; }

	} // end if

	// Start the server
	if ( !_g.l555.StartServer( 554 ) )
	{	_self.echo( "Failed to open RTSP stream " + inf[ 1 ] );
		return 0;
	} // end if

	// Create the stream
	_g.l555.Msg( "cmd=CreateStream,id=cam,desc=cam,fps=" + _g.cap.getFps() );

	_self.set_timer( ".", 1000 / _g.cap.getFps().tointeger() / 2, "DeliverFrame" )
	
	return 1;
}

function DeliverFrame() : ( _g )
{
	if ( !_g.running )
	{	if ( !_g.l555.isThread() ) return; _g.running = 1;
		_self.echo( "RTSP Server running : " + _g.l555.getUrl( "cam" ) );
	} // end if

	// Attempt a capture
	if ( 0 >= _g.cap.Capture( _g.frame, 100 ) )
		return;

//	_self.echo( "Frame : " + _g.frame_count++ );

	local frame_info = CSqMulti(), tframe = CSqBinary();
	if ( _g.use_ffmpeg_decode )
	{
		// Transcode the frame
		if ( !_g.tc.Transcode( _g.frame, tframe, frame_info ) )
		{	_self.echo( "Transcode() failed" ); }

	} // end if

	else
	{
//		if ( !_g.img.Decode( "jpg", _g.frame )
		if ( !_g.img.setPixels( _g.frame )
			 || ! _g.enc.EncodeImage( _g.img, tframe, frame_info ) )
		{	_self.echo( "Transcode() failed" ); }

	} // end else

	// Release the capture buffer
	_g.cap.ReleaseFrame();
	
	// If we got a frame
	if ( tframe.getUsed() )
	{	
		// Share with friends
		local fid = _self.unique();
		_self.set_binshare( fid, tframe );
		_g.l555.Msg( "cmd=DeliverFrame,stream_id=cam,frame_id=" + fid );
		
		// Save frame to avi
		if ( _g.avi.isOpen() )
			if ( !_g.avi.WriteFrame( tframe, frame_info ) )
				_self.echo( "failed to write frame to avi" );
				
	} // end if

}

function _idle() : ( _g )
{
	if ( _g.running && !_g.l555.isThread() )
	{	_self.echo( "Server thread has stopped" );
		return 1;
	} // end if

	return 0;
}

