
_self.load_module( "irrlicht", "" );
_self.load_module( "ffmpeg", "" );

class CGlobal
{
	irr = 0;
	cam = 0;
	cam_pos = CSqirrVector3d( 0, 500, 0 );
	t = 0;

	rec_avi = 0;
	rec_enc = 0;
	rec_frame = 0;

	// Range
	x = -100;
	max = 100;

	// Scale
	xs = 4;
	ys = 1;
};

local _g = CGlobal();


function OnEvent( e, x, y, w ) : ( _g )
{
//	_self.echo( e + ", " + x + ", " + y + ", " + w );

	if ( e.tointeger() == _g.irr.EMIE_LMOUSE_PRESSED_DOWN )
	{

	} // end if

	else if ( e.tointeger() == _g.irr.EMIE_RMOUSE_PRESSED_DOWN )
	{

	} // end else if
}


function _init() : ( _g )
{
	_g.irr = CSqIrrlicht();
	_g.irr.SetCallback( _self.queue(), "OnEvent" );

//	_g.irr.SetStereo( 1 );

	// Create 3d window
	_g.irr.Init( "Yahoo Data", 640, 480, 1 );

	// Set ambient light level/color
	_g.irr.SetAmbientLight( CSqirrColorf( 0.4, 0.4, 0.4, 0.4 ) );

	// Add point lights
//	_g.irr.AddLight( CSqirrVector3d( 0., 10000., -10000. ), CSqirrColorf( 1., 1., 1. ), 10000. );
	_g.irr.AddLight( CSqirrVector3d( -1000., 10000., -5000. ), CSqirrColorf( 1., 1., 1. ), 20000. );

	// Add default camera
	_g.cam = _g.irr.AddCamera( _g.cam_pos, CSqirrVector3d( 0, 0, 1 ) );
//	_g.cam.SetLens( 1., 2.4, 3.2 );

	// Save to file
	RecordToFile( _self.root( "yahoo-graph.avi" ) );

}

function RecordToFile( file ) : ( _g )
{
	_g.rec_avi = CFfContainer();
	_g.rec_enc = CFfEncoder();
	_g.rec_frame = CSqBinary();

	local width = _g.irr.getWidth(), height = _g.irr.getHeight();

	if ( !_g.rec_avi.Create( file, "", CSqMulti() ) )
		_self.echo( "Failed to create avi" );
	else if ( 0 > _g.rec_avi.AddVideoStream( CFfDecoder().LookupCodecId( "H264" ), width, height, 10 ) )
		_self.echo( "Failed to add video stream" );
	else if ( !_g.rec_avi.InitWrite() )
		_self.echo( "Failed to initiailze avi" );

	else if ( !_g.rec_enc.Create( _g.rec_avi.getVideoCodecId(), CFfConvert().PIX_FMT_YUV420P,
								  width, height, 10, 800000, CSqMulti() ) )
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

		else
			if ( !_g.rec_avi.WriteFrame( _g.rec_frame, inf ) )
				_self.echo( "Failed to write to avi file" );

	} // end if

	else
		_self.echo( "Failed to capture image buffer" );
}

function addPoint( x, y, size, color ) : ( _g )
{
	local pt;
	pt = _g.irr.AddSphereMesh( size, size, 8 );
	_g.irr.SetVertexColors( pt, color );
	pt.SetMaterialFlag( CSqirrNode().EMF_LIGHTING, 0 );
	pt.SetPosition( CSqirrVector3d( x, 0., y ) );
}

function Fx( x ) : ( _g )
{
	return 100. * cos( x * 0.1 );
}

function _idle() : ( _g )
{
	if ( _g.x < _g.max )
	{
		local y = Fx( _g.x );
		addPoint( _g.x * _g.xs, y * _g.ys, 10., CSqirrColor( 255, 0, 0 ) );
		_g.x++;

	} // end if

//	_g.t++;
//	_g.cam_pos.set( 500 * cos( _g.t * 0.1 ), _g.cam_pos.y(), 500 * sin( _g.t * 0.1 ) );
//	_g.cam.node().SetPosition( _g.cam_pos );

	RenderFile();
	return _g.irr.Draw( CSqirrColor( 100, 100, 100 ) );
}

