
_self.load_module( "irrlicht", "" );
_self.load_module( "freenect", "" );
_self.load_module( "ffmpeg", "" );

class CGlobal
{
	quit = 1;
	irr = 0;
	fn = 0;
	video = 0;
	tex = 0;
	cnv = 0;
	vidx = 0;
};

local _g = CGlobal();

function WaitKey()
{	_self.echo( "\n...press any key...\n" );
	_self.get_key();
}

function _init() : ( _g )
{
	_g.irr = CSqIrrlicht();

//	_g.irr.SetStereo( 1 );
	_self.echo( "...Initializing...\n" );
	_g.irr.Init( "libfreenect Test", 640, 480, 1 );

	_self.echo( "...setting ambient light...\n" );
	_g.irr.SetAmbientLight( CSqirrColorf( 0.5, 0.5, 0.5 ) );

	_self.echo( "...adding lights...\n" );
	_g.irr.AddLight( CSqirrVector3d( 0., 100., 0. ), CSqirrColorf( 1., 1., 1. ), 100. );
	_g.irr.AddLight( CSqirrVector3d( 0., 100., 50. ), CSqirrColorf( 1., 1., 1. ), 100. );

//	_self.echo( "adding sky...\n" );
//	_g.irr.AddSkyDome( _self.path( "../imgs/sky.png" ), 16, 16, 100., 100. );

	_self.echo( "...adding camera...\n" );
//	local cam = _g.irr.AddCamera( CSqirrVector3d( 0, 50, 100 ), CSqirrVector3d( 0, 0, 0 ) );
	local cam = _g.irr.AddCamera( CSqirrVector3d( 0, 0, 100 ), CSqirrVector3d( 0, 0, 0 ) );
//	cam.SetLens( 1., 2.4, 3.2 );

//	_self.echo( "...adding cube...\n" );
//	_g.video = _g.irr.AddCube( 50. );
//	_self.echo( "...adding animator...\n" );
//	local ani = _g.irr.AddRotateAnimator( CSqirrVector3d( 0, 0.4, 0 ) );
//	_g.video.AddAnimator( ani );

	_self.echo( "...adding video plane...\n" );
	_g.video = _g.irr.AddGrid( 100., 100., 1, 1, 0., 2, CSqirrColor( 255, 255, 255 ), 2 );
	_g.video.SetPosition( CSqirrVector3d( -50, -50, 0 ) );

	_g.fn = CSqFreenect();

	if ( !_g.fn.Init() )
	{	_self.echo( "Failed to initialize freenect library" );
		WaitKey();
		return 0;
	} // end if

	_self.echo( "...freenect library initialized...\n" );

	_g.fn.OpenDevice( 0 );
	_g.fn.StartVideo( 0 );

//	_g.fn.setTilt( 0. );
	_g.fn.setLED( 2 );
	

	// Set draw function
	_self.set_timer( ".", 30, "OnDraw" )

	_g.quit = 0;
}

function _idle() : ( _g )
{
	if ( _g.quit )
		_g.fn.setLED( 0 ),
		_g.fn.Destroy();

	return _g.quit;
}

function OnDraw() : ( _g )
{
	// Wait for video
	local vidx = _g.fn.getVideoFrameIdx();
	if ( vidx != _g.vidx )
	{	_g.vidx = vidx;

		if ( !_g.tex )
		{
			// Decode a frame to get the width / height
			if ( 0 <= _g.fn.getOpenDevice() )
			{	_g.tex = _g.irr.CreateTexture( _g.fn.getVideoWidth(), _g.fn.getVideoHeight(), 0 );
				if ( _g.tex ) _g.video.SetTexture( 0, _g.tex );
				_g.cnv = CFfConvert();
			} // end if

		} // end if

		else
		{
			local buffer = _g.tex.Lock();
			local video = _g.fn.getVideo( vidx );

			if ( buffer.getUsed() )
				_g.cnv.ConvertColorBB( _g.fn.getVideoWidth(), _g.fn.getVideoHeight(),
									   video, CFfConvert().PIX_FMT_RGB24, buffer, CFfConvert().PIX_FMT_RGB32, 
	//								   video, CFfConvert().PIX_FMT_UYVY422, buffer, CFfConvert().PIX_FMT_RGB32, 
									   CFfConvert().SWS_FAST_BILINEAR ),
				_g.tex.Unlock();

		} // end if

	} // end else

	_g.quit = _g.irr.Draw( CSqirrColor( 100, 100, 100 ) );

	return 0;
}


