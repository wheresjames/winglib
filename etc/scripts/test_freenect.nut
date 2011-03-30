
_self.load_module( "irrlicht", "" );
_self.load_module( "freenect", "" );
_self.load_module( "ffmpeg", "" );

class CGlobal
{
	quit = 1;
	irr = 0;
	fn = 0;
	cnv = 0;

	video = 0;
	vtex = 0;
	vidx = 0;

	depth = 0;
	dtex = 0;
	didx = 0;

	video2 = 0;
	vtex2 = 0;
	vidx2 = 0;

	rec_avi = 0;
	rec_enc = 0;
	rec_frame = 0;
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
//	_g.irr.Init( "libfreenect Test", 640, 480, 1 );
	_g.irr.Init( "libfreenect Test", 1024, 800, 1 );

	_self.echo( "...setting ambient light...\n" );
	_g.irr.SetAmbientLight( CSqirrColorf( 0.5, 0.5, 0.5 ) );

	_self.echo( "...adding lights...\n" );
	_g.irr.AddLight( CSqirrVector3d( 0., 1000., 0. ), CSqirrColorf( 1., 1., 1. ), 2000. );
	_g.irr.AddLight( CSqirrVector3d( 0., 1000., 500. ), CSqirrColorf( 1., 1., 1. ), 2000. );

//	_self.echo( "adding sky...\n" );
//	_g.irr.AddSkyDome( _self.path( "../imgs/sky.png" ), 16, 16, 100., 100. );

	_self.echo( "...adding camera...\n" );
//	local cam = _g.irr.AddCamera( CSqirrVector3d( 0, 30, 60 ), CSqirrVector3d( 0, 0, 0 ) );
	local cam = _g.irr.AddCamera( CSqirrVector3d( 0, 0, 400 ), CSqirrVector3d( 0, 0, 0 ) );
//	cam.SetLens( 1., 2.4, 3.2 );

//	_self.echo( "...adding cube...\n" );
//	_g.video = _g.irr.AddCube( 50. );
//	_self.echo( "...adding animator...\n" );
//	local ani = _g.irr.AddRotateAnimator( CSqirrVector3d( 0, 0.4, 0 ) );
//	_g.video.AddAnimator( ani );

	_self.echo( "...adding video plane...\n" );
	_g.video = _g.irr.AddGrid( 600., 400., 1, 1, 0., 2, CSqirrColor( 255, 255, 255 ), 2 );
	_g.video.SetPosition( CSqirrVector3d( 100, 100, -400 ) );

	_self.echo( "...adding depth plane...\n" );
	_g.depth = _g.irr.AddGrid( 600., 400., 1, 1, 0., 2, CSqirrColor( 255, 255, 255 ), 2 );
	_g.depth.SetPosition( CSqirrVector3d( -600, 100, -400 ) );

	_self.echo( "...adding 3d plane...\n" );
//	_g.video2 = _g.irr.AddGrid( 64., 48., 10, 10, 0., 2, CSqirrColor( 255, 255, 255 ), 2 );
	_g.video2 = _g.irr.AddGrid( 10., 10., 64, 48, 0., 2, CSqirrColor( 255, 255, 255 ), 2 );
	_g.video2.SetPosition( CSqirrVector3d( 0, -100, -300 ) );
    local rotate = _g.irr.AddRotateAnimator( CSqirrVector3d( 0, 0.2, 0 ) );
    _g.video2.AddAnimator( rotate );

	_g.fn = CSqFreenect();
	_g.cnv = CFfConvert();

	if ( !_g.fn.Init() )
	{	_self.echo( "Failed to initialize freenect library" );
		WaitKey();
		return 0;
	} // end if

	_self.echo( "...freenect library initialized...\n" );

	_g.fn.OpenDevice( 0 );
	_g.fn.StartVideo( 0 );
	_g.fn.StartDepth( 0 );

//	_g.fn.setTilt( 0. );
	_g.fn.setLED( 2 );

	// Set draw function
	_self.set_timer( ".", 30, "OnDraw" )

    _g.irr.AddMeshAnimator( _self, _g.video2, OnAnimate, 1 );

	// Save to file
	RecordToFile( _self.root( "freenect.avi" ) );

	_g.quit = 0;
}

function OnAnimate( n, o, c ) : ( _g )
{
	if ( !_g.dtex ) return;
	local depth = _g.dtex.Lock();
	if ( !depth.getUsed() ) return;

	local w = 64, w2 = w / 2, h = 48, h2 = h / 2, sw = 64 * 4;
	for ( local i = 0; i < n; i++ )
	{
	    local x = o.x( i ), y = o.y( i );
	    local _x = x.tointeger() / 10 + w2, _y = ( y.tointeger() ) / 10 + h2;
		local _o = ( ( ( h - _y ) * sw ) + ( ( w - _x ) * 4 ) ) + 1;
		local z = o.z( i ) + ( depth.getCHAR( _o ) * 3 );
	    c.set( i, x, y, z );

	} // end for

	_g.dtex.Unlock();
}

function _idle() : ( _g )
{
	if ( _g.quit )
		_g.fn.setLED( 0 );

	return _g.quit;
}

function OnDraw() : ( _g )
{
	// Wait for video
	local vidx = _g.fn.getVideoFrameIdx();
	if ( vidx != _g.vidx )
	{	_g.vidx = vidx;

		if ( !_g.vtex )
		{
			// Decode a frame to get the width / height
			if ( 0 <= _g.fn.getOpenDevice() )
			{	_g.vtex = _g.irr.CreateTexture( _g.fn.getVideoWidth(), _g.fn.getVideoHeight(), 0 );
				if ( _g.vtex ) 
					_g.video.SetTexture( 0, _g.vtex ),
					_g.video2.SetTexture( 0, _g.vtex );
			} // end if

		} // end if

		else
		{
			local buffer = _g.vtex.Lock();
			local video = _g.fn.getVideo( vidx );
			if ( buffer.getUsed() )
				_g.cnv.ConvertColorBB( _g.fn.getVideoWidth(), _g.fn.getVideoHeight(),
									   video, CFfConvert().PIX_FMT_RGB24, buffer, CFfConvert().PIX_FMT_RGB32, 
									   CFfConvert().SWS_FAST_BILINEAR ),
				_g.vtex.Unlock();

		} // end if

	} // end else

	local didx = _g.fn.getDepthFrameIdx();
	if ( didx != _g.didx )
	{	_g.didx = didx;

		if ( !_g.dtex )
		{
			// Decode a frame to get the width / height
			if ( 0 <= _g.fn.getOpenDevice() )
			{	_g.dtex = _g.irr.CreateTexture( _g.fn.getDepthWidth() / 10, _g.fn.getDepthHeight() / 10, 0 );
				if ( _g.dtex ) _g.depth.SetTexture( 0, _g.dtex );
			} // end if

		} // end if

		else
		{
			local buffer = _g.dtex.Lock();
			local depth = _g.fn.getDepth( vidx );
			depth.Scale( _self.tUInt16, 0, 0, 1, 10. );
			depth.Threshold( _self.tUInt16, 0, 0, 1, 8000., 10000. 0. 0. );
			if ( buffer.getUsed() )
				_g.cnv.ConvertColorBB2( _g.fn.getDepthWidth(), _g.fn.getDepthHeight(),
									    depth, CFfConvert().PIX_FMT_GRAY16LE, 
										_g.fn.getDepthWidth() / 10, _g.fn.getDepthHeight() / 10,
										buffer, CFfConvert().PIX_FMT_RGB32, 
									    CFfConvert().SWS_FAST_BILINEAR ),
				_g.dtex.Unlock();

		} // end if

	} // end else

	RenderFile();
	_g.quit = _g.irr.Draw( CSqirrColor( 100, 100, 100 ) );

	return 0;
}

function RecordToFile( file ) : ( _g )
{
	_g.rec_avi = CFfContainer();
	_g.rec_enc = CFfEncoder();
	_g.rec_frame = CSqBinary();

	local width = _g.irr.getWidth(), height = _g.irr.getHeight();

	if ( !_g.rec_avi.Create( file, "", CSqMulti() ) )
		_self.echo( "Failed to create avi" );
	else if ( 0 > _g.rec_avi.AddVideoStream( CFfDecoder().LookupCodecId( "MP42" ), width, height, 10 ) )
		_self.echo( "Failed to add video stream" );
	else if ( !_g.rec_avi.InitWrite() )
		_self.echo( "Failed to initiailze avi" );

	else if ( !_g.rec_enc.Create( _g.rec_avi.getVideoCodecId(), CFfConvert().PIX_FMT_YUV420P, 
								  width, height, 10, 2000000, CSqMulti() ) )
		_self.echo( "Failed to create encoder" );

}

function RenderFile() : ( _g )
{
	// Render to file if needed
	if ( !_g.rec_enc || !_g.rec_avi )
		return;

	local img = CSqBinary();
	if ( _g.irr.Capture( img ) )
	{
		local inf = CSqMulti();
		
		if ( !_g.rec_enc.Encode( CFfConvert().PIX_FMT_BGR32, 
								 _g.irr.getWidth(), -_g.irr.getHeight(),
								 img, _g.rec_frame, inf ) )
			_self.echo( "Failed to encode frame" );

		else if ( !_g.rec_avi.WriteFrame( _g.rec_frame, inf ) )
			_self.echo( "Failed to write to avi file" );

	} // end if

	else
		_self.echo( "Failed to capture image buffer" );
}



