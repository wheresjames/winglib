
_self.load_module( "irrlicht", "" );
_self.load_module( "live555", "" );
_self.load_module( "ffmpeg", "" );


class CGlobal
{
	irr = 0;
	video = 0;
	tex = 0;

	rtsp = 0;
	dec = 0;
	adec = 0;
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

			live555		= [ "live555", 		"rtsp://192.168.2.200:8554/vid1" ]

//			ser			= [ "ser", 			"rtsp://192.168.2.251" ]
			ser			= [ "ser", 			"rtsp://192.168.2.251/h264.sdp?res=half&x0=0&y0=0&x1=1600&y1=1200&qp=30&ssn=1&doublescan=0" ]
			

		};

	// Check UDP ports 6970-9999

	StartStream( rtsp_video[ "ser" ] );

}

function StartStream( inf ) : ( _g )
{
	_self.echo( "\nConnecting to : " + inf[ 1 ] + "...\n" );

	_g.rtsp = CLvRtspClient();
	if ( !_g.rtsp.Open( inf[ 1 ], 1, 0, CSqMulti() ) )
	{	_self.echo( "Failed to open RTSP stream " + inf[ 1 ] );
		return 0;
	} // end if

	return 1;
}

function UpdateVideo() : ( _g )
{
	// Has it connected?
	if ( !_g.rtsp.waitInit( 0 ) )
	{	_self.print( "." ); _self.flush();
		return;
	} // end if

	if ( !_g.rtsp.isOpen() )
		return;

	// Do we need to create the video decoder?
	if ( !_g.dec && _g.rtsp.isVideo() )
	{
		_self.echo( "Creating video decoder for " + _g.rtsp.getVideoCodecName() );
		_g.dec = CFfDecoder();
		if ( !_g.dec.Create( CFfDecoder().LookupCodecId( _g.rtsp.getVideoCodecName() ), CFfConvert().PIX_FMT_YUV420P,
							 0, 0, 0, 0, CSqMulti() ) )
			_self.echo( "!!! Failed to create decoder for " + _g.rtsp.getVideoCodecName() );

	} // end if

	if ( !_g.adec && _g.rtsp.isAudio() )
	{
		_self.echo( "Creating audio decoder for " + _g.rtsp.getAudioCodecName() );

		_g.adec = CFfAudioDecoder();
		if ( !_g.adec.Create( CFfAudioDecoder().LookupCodecId( _g.rtsp.getAudioCodecName() ), CFfConvert().PIX_FMT_YUV420P,
							 0, 0, 0 ) )
			_self.echo( "!!! Failed to create decoder for " + _g.rtsp.getAudioCodecName() );

	} // end if

	if ( !_g.dec && !_g.adec )
	{	_self.echo( "Closing stream because there are no decoders" );
		_g.rtsp.Destroy();
		return 0;
	} // end if

	local frame = CSqBinary();

	// Are we doing video?
	if ( _g.dec && _g.rtsp.LockVideo( frame, CSqMulti() ) )
	{
		_self.echo( "Video data : " + frame.getUsed() );

		if ( !_g.tex )
		{
			// Decode a frame to get the width / height
			_g.dec.Decode( frame, CFfConvert().PIX_FMT_RGB32, CSqBinary(), CSqMulti() );
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
				_g.dec.Decode( frame, CFfConvert().PIX_FMT_RGB32, tex, CSqMulti() ),
				_g.tex.Unlock();

		} // end else

		_g.rtsp.UnlockVideo();

	} // end if

	// Is there audio?
	if ( _g.adec && _g.rtsp.LockAudio( frame, CSqMulti() ) )
	{
//		_self.echo( "Audio data : " + frame.getUsed() );

		_g.rtsp.UnlockAudio();

	} // end if

}

function _idle() : ( _g )
{
	UpdateVideo();
	return _g.irr.Draw( CSqirrColor( 100, 100, 100 ) );
}

/*
-	RFC 3550 for RTCP
-	RFC 1889 for RTP
-	RFC 2327 for SDP
-	RFC 2326 for RTSP
-	RFC 3984 for the data format of H.264 streaming over RTP
*/
