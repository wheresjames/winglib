
_self.load_module( "irrlicht", "" );
_self.load_module( "ffmpeg", "" );

class CGlobal
{
	irr = 0;
	video = 0;
	tex = 0;

	rtsp = 0;
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

//	StartStream( "rtsp://a1352.l1857053128.c18570.g.lq.akamaistream.net/D/1352/18570/v0001/reflector:53128", 30 );
	StartStream( "rtsp://192.168.2.130/Mediainput/mpeg4", 30 );
//	StartStream( "rtsp://10.10.211.26/Mediainput/mpeg4", 30 );


}

function StartStream( url, fps ) : ( _g )
{
	_g.rtsp = CFfContainer();
	if ( !_g.rtsp.Open( url, CSqMulti() ) )
		return 0;

	_self.echo( "Video File : " + _g.rtsp.getWidth() + "x" + _g.rtsp.getHeight() );

	_g.tex = _g.irr.CreateTexture( _g.rtsp.getWidth(), _g.rtsp.getHeight(), 0 );
    _g.video.SetTexture( 0, _g.tex );

	return 1;
}

function UpdateVideo() : ( _g )
{
	local buf = _g.tex.Lock();
	if ( buf.getUsed() )
		_g.rtsp.DecodeFrame( _g.rtsp.getVideoStream(), CFfConvert().PIX_FMT_RGB32, buf, CSqMulti() ),
		_g.tex.Unlock();
}

function _idle() : ( _g )
{
	UpdateVideo();
	return _g.irr.Draw( CSqirrColor( 100, 100, 100 ) );
}

