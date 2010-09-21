
_self.load_module( "irrlicht", "" );
_self.load_module( "ffmpeg", "" );

class CGlobal
{
	quit = 0;

	irr = 0;
	video = 0;
	tex = 0;

	ffmpeg = 0;

	frames = 0;
	frame = CSqBinary();
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
			utube2		= [ "utube2",		"rtsp://rtsp2.youtube.com/ChoLENy73wIaEQmJv18x7xfevhMYESARFEgGDA==/0/0/0/video.3gp" ],

		};

	StartStream( rtsp_video[ "utube1" ] );

	_self.set_timer( ".", 60, "OnTimer" );

}

function StartStream( inf ) : ( _g )
{
	_g.ffmpeg = CFfContainer();
	if ( !_g.ffmpeg.Open( inf[ 1 ], CSqMulti() ) )
	{	_self.echo( "Failed to open link : " + inf[ 1 ] );
		return 0;
	} // end if

	_self.echo( "Video File : " + _g.ffmpeg.getWidth() + "x" + _g.ffmpeg.getHeight() );

	_g.tex = _g.irr.CreateTexture( _g.ffmpeg.getWidth(), _g.ffmpeg.getHeight(), 0 );
    _g.video.SetTexture( 0, _g.tex );

	return 1;
}

function UpdateVideo() : ( _g )
{
	if ( !_g.ffmpeg.isOpen() )
		return;

	_g.frames++;

//	if ( 1 < _g.frames )
//		return;

	local buf = _g.tex.Lock();
	if ( buf.getUsed() )
	{
//		_self.echo( "Texture locked : " + buf.getUsed() );

		local inf = CSqMulti();

		if ( _g.ffmpeg.getVideoStream() != _g.ffmpeg.ReadFrame( _g.frame, inf ) )
			_self.echo( "Not a video frame" );

		else
		{
			_self.echo( "Frame Size : " + _g.frame.getUsed() );

//			_self.echo( inf.serialize() );

//			_self.echo( _g.frame.AsciiHexStr( 16, 16 ) );

			if ( !_g.ffmpeg.DecodeFrameBin( _g.frame, CFfConvert().PIX_FMT_RGB32, buf, inf ) )
				_self.echo( "Error decoding frame" );

		} // end else

		_g.tex.Unlock();

	} // end if
}

function OnTimer() : ( _g )
{
	if ( _g.quit )
		return;

	UpdateVideo();

	_g.quit = _g.irr.Draw( CSqirrColor( 100, 100, 100 ) );
}


function _idle() : ( _g )
{
	return _g.quit;
}

