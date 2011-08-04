
_self.load_module( "irrlicht", "" );
_self.load_module( "live555", "" );
_self.load_module( "ffmpeg", "" );


class CGlobal
{
	quit = 0;

	irr = 0;
	video = 0;
	tex = 0;

	rtsp = 0;
	dec = 0;
	adec = 0;

	w = 0;
	h = 0;

	rec_avi = 0;
//	rec_avi = CFfContainer();

};

local _g = CGlobal();

function _init() : ( _g )
{
	_g.irr = CSqIrrlicht();

//	_g.irr.SetStereo( 1 );
//	_g.irr.SetREyeKey( 0x000000ff );
//	_g.irr.SetLEyeKey( 0x00ffff00 );
//	_g.irr.Init( "FFMPEG Test", 240, 320, 1 );
	_g.irr.Init( "FFMPEG Test", 480, 640, 1 );
//	_g.irr.Init( "FFMPEG Test", 1024, 768, 1 );

	_g.irr.SetAmbientLight( CSqirrColorf( 0.5, 0.5, 0.5 ) );

	_g.irr.AddLight( CSqirrVector3d( 0., 0., 100. ), CSqirrColorf( 1., 1., 1. ), 500. );
//	_g.irr.AddLight( CSqirrVector3d( 50., 100., 100. ), CSqirrColorf( 0.5, 0.5, 0.5 ), 200. );

	local cam = _g.irr.AddCamera( CSqirrVector3d( 0, 0, 100 ), CSqirrVector3d( 0, 0, 0 ) );
//	cam.SetLens( 1., 2.4, 3.2 );

    _g.video = _g.irr.AddGrid( 100., 100., 1, 1, 0., 2, CSqirrColor( 255, 255, 255 ), 2 );
	_g.video.SetPosition( CSqirrVector3d( -50, -50, 0 ) );

//    local rotate = _g.irr.AddRotateAnimator( CSqirrVector3d( 0, 0.5, 0 ) );
//    _g.video.AddAnimator( rotate );

	local rtsp_video =
		{
			nasa 		= [ "NASA", 		"rtsp://a1352.l1857053128.c18570.g.lq.akamaistream.net/D/1352/18570/v0001/reflector:53128" ],
			adventure 	= [ "Adventure", 	"rtsp://video3.multicasttech.com/AFTVAdventure3GPP296.sdp" ],
			cartoons 	= [ "Cartoons", 	"rtsp://video2.multicasttech.com/AFTVCartoons3GPP296.sdp" ],
			classic 	= [ "Classic", 		"rtsp://video3.multicasttech.com/AFTVClassics3GPP296.sdp" ],
			comedy 		= [ "Comedy", 		"rtsp://video3.multicasttech.com/AFTVComedy3GPP296.sdp" ],
			crime 		= [ "Crime", 		"rtsp://video2.multicasttech.com/AFTVCrime3GPP296.sdp" ],
			halloween	= [ "Halloween", 	"rtsp://video3.multicasttech.com/AFTVHalloween3GPP296.sdp" ],
			horror		= [ "Horror", 		"rtsp://video2.multicasttech.com/AFTVHorror3GPP296.sdp" ],
			indy		= [ "Indy Movies", 	"rtsp://video2.multicasttech.com/AFTVIndyMovies3GPP296.sdp" ],
			mystery		= [ "Mystery", 		"rtsp://video2.multicasttech.com/AFTVMystery3GPP296.sdp" ],
			scifi		= [ "SciFi", 		"rtsp://video2.multicasttech.com/AFTVSciFi3GPP296.sdp" ],
			western		= [ "Westerns", 	"rtsp://video2.multicasttech.com/AFTVWesterns3GPP296.sdp" ],
			espana		= [ "Espana", 		"rtsp://video3.multicasttech.com/EspanaFree3GPP296.sdp" ],

			utube1		= [ "utube1",		"rtsp://v2.cache1.c.youtube.com/CkgLENy73wIaPwlnoDu0pt7zDRMYDSANFEIJbXYtZ29vZ2xlSARSB3Jlc3VsdHNaDkNsaWNrVGh1bWJuYWlsYOmkotHXgfvJRgw=/0/0/0/video.3gp" ],
		};

	local channel = _self.get( "/", "cmdline.channel" );
	if ( !channel.len() )
	{	channel = "utube1";
		local link = _self.get( "/", "cmdline.rtsp" );
		if ( link.len() )
		{	channel = "user";
			rtsp_video[ "user" ] <- [ "User", link ];
		} // end if
	} // end if

	// Check TCP Port 554 and UDP ports 6970-9999
	StartStream( rtsp_video[ channel ], 0, 0 );

	_self.set_timer( ".", 15, "OnTimer" );
	
	return 0;
}

function StartStream( inf, w, h ) : ( _g )
{
	_self.echo( "\nConnecting to : " + inf[ 1 ] + "\n" );

	_g.rtsp = CLvRtspClient();
	if ( !_g.rtsp.Open( inf[ 1 ], 1, 0, CSqMulti() ) )
	{	_self.echo( "Failed to open RTSP stream " + inf[ 1 ] );
		return 0;
	} // end if

	_g.w = w;
	_g.h = h;

	return 1;
}

function UpdateVideo() : ( _g )
{
	// Has it connected?
	if ( !_g.rtsp.waitInit( 0 ) )
	{	_self.print( "." ); _self.flush();
		return 0;
	} // end if

	if ( !_g.rtsp.isOpen() )
	{	_g.quit = 1;
		return 0;
	} // end if

	// Do we need to create the video decoder?
	if ( !_g.dec && _g.rtsp.isVideo() )
	{
		// Insert H264 Header if needed
		if ( "H264" == _g.rtsp.getVideoCodecName() )
		{	local header = CSqBinary();
			header.setString( "\x00\x00\x01" );
			_g.rtsp.setVideoHeader( header );
		} // end if

//		_self.echo( "Video Size = " + _g.rtsp.getWidth() + " x " + _g.rtsp.getHeight() + " - " + _g.rtsp.getFps() + "fps" );
//		_self.echo( "Video Size = " + _g.w + " x " + _g.h );

		_self.echo( "Creating video decoder for " + _g.rtsp.getVideoCodecName() );
		_g.dec = CFfDecoder();
		_g.dec.setExtraData( _g.rtsp.getExtraVideoData() );
		if ( !_g.dec.Create( CFfDecoder().LookupCodecId( _g.rtsp.getVideoCodecName() ), CFfConvert().PIX_FMT_YUV420P,
							 _g.rtsp.getWidth(), _g.rtsp.getHeight(), 5, 2000000, CSqMulti( "cmp=-2" ) ) )
			_self.echo( "!!! Failed to create decoder for " + _g.rtsp.getVideoCodecName() ), _g.quit = 1;

		_self.echo( " !!! STARTING RTSP STREAM !!!" );
		_g.rtsp.Play();

//		_g.dec.Decode( _g.rtsp.getExtraVideoData(), CFfConvert().PIX_FMT_RGB32, CSqBinary(), CSqMulti() );

	} // end if

	if ( !_g.adec && _g.rtsp.isAudio() )
	{
		_self.echo( "Creating audio decoder for " + _g.rtsp.getAudioCodecName() );

		_g.adec = CFfAudioDecoder();
		if ( !_g.adec.Create( CFfAudioDecoder().LookupCodecId( _g.rtsp.getAudioCodecName() ), CFfConvert().PIX_FMT_YUV420P,
							 0, 0, 0 ) )
			_self.echo( "!!! Failed to create decoder for " + _g.rtsp.getAudioCodecName() ), _g.quit = 1;

	} // end if

	if ( _g.quit )
		return 0;

	if ( !_g.dec && !_g.adec )
	{	_self.echo( "Closing stream because there are no decoders" );
		_g.rtsp.Destroy();
		return 0;
	} // end if

	local frame = CSqBinary();

	if ( _g.dec && _g.rtsp.LockVideo( frame, 1000 ) )
	{
//		_self.echo( "Frame Size : " + frame.getUsed() );
//		_self.echo( frame.AsciiHexStr( 16, 16 ) );

		// Saving to avi?
		if ( _g.rec_avi )
		{
			if ( !_g.rec_avi.isOpen() && 0 < _g.dec.getWidth() && 0 < _g.dec.getHeight() )
			{
				if ( !_g.rec_avi.Create( "irr_live555.avi", "", CSqMulti() ) )
					_self.echo( "Failed to create avi" );
				else if ( 0 > _g.rec_avi.AddVideoStream( CFfDecoder().LookupCodecId( _g.rtsp.getVideoCodecName() ),
														 _g.dec.getWidth(), _g.dec.getHeight(), 30 ) )
					_self.echo( "Failed to add video stream" );
				else if ( !_g.rec_avi.InitWrite() )
					_self.echo( "Failed to initiailze avi" );

			} // end if

			// Save frame to avi
			if ( !_g.rec_avi.WriteFrame( frame, CSqMulti() ) )
				_self.echo( "Failed to write to avi file" );

		} // end if

		_g.dec.BufferData( frame, CSqMulti() );

		_g.rtsp.UnlockVideo();

	} // end if

	//	if ( _g.dec.Decode( CSqBinary(), CFfConvert().PIX_FMT_RGB32, CSqBinary(), CSqMulti(), 0 ) )
		if ( 0 < _g.dec.getBufferSize() )
		{
//			::_self.echo( "buffered : " + _g.dec.getBufferSize() );

			if ( !_g.tex )
			{
				// Decode a frame to get the width / height
				_g.dec.Decode( CSqBinary(), CFfConvert().PIX_FMT_RGB32, CSqBinary(), CSqMulti(), 0 );
				if ( 0 < _g.dec.getWidth() && 0 < _g.dec.getHeight() )
				{	_g.tex = _g.irr.CreateTexture( _g.dec.getWidth(), _g.dec.getHeight(), 0 );
					_g.video.SetTexture( 0, _g.tex );
				} // end if

				_self.echo( _g.dec.getWidth() + " x " + _g.dec.getHeight() );

			} // end if

			else
			{
				local tex = _g.tex.Lock();
				if ( tex.getUsed() )
				{
					// dts=-9223372036854775808,duration=0,flags=1,pos=-1,pts=-9223372036854775808,size=165698,stream_index=0
//					if ( !_g.dec.Decode( frame, CFfConvert().PIX_FMT_RGB32, tex, CSqMulti( "dts=-9223372036854775808,duration=0,flags=1,pos=-1,pts=-9223372036854775808" ) ) )
//					if ( !_g.dec.Decode( frame, CFfConvert().PIX_FMT_RGB32, tex, CSqMulti(), 0 ) )
					if ( !_g.dec.Decode( CSqBinary(), CFfConvert().PIX_FMT_RGB32, tex, CSqMulti(), 0 ) )
						;
//						_self.echo( "failed to decode frame" );

					_g.tex.Unlock();

				} // end if

			} // end else

		} // end if

	// Is there audio?
	if ( _g.adec && _g.rtsp.LockAudio( frame, CSqMulti() ) )
	{
//		_self.echo( "Audio data : " + frame.getUsed() );

		_g.rtsp.UnlockAudio();

	} // end if

	return 1;
}

function OnTimer() : ( _g )
{
	if ( _g.quit )
		return -1;

	UpdateVideo();
	_g.quit = _g.irr.Draw( CSqirrColor( 100, 100, 100 ) );

//	if ( UpdateVideo() )
//		_g.quit = _g.irr.Draw( CSqirrColor( 100, 100, 100 ) );

	return 0;
}


function _idle() : ( _g )
{
	return _g.quit;
}

/*
-	RFC 3550 for RTCP
-	RFC 1889 for RTP
-	RFC 2327 for SDP
-	RFC 2326 for RTSP
-	RFC 3984 for the data format of H.264 streaming over RTP
*/
