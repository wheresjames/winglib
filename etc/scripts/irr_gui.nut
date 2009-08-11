
_self.load_module( "irrlicht", "" );

class CGlobal
{
	// 3D engine
	irr = 0;

	window = 0;
};

local _g = CGlobal();

function _irr_event( e, x, y, w ) : ( _g )
{
//	_self.echo( e + ", " + x + ", " + y + ", " + w );

/*
	if ( e.tointeger() == _g.irr.EMIE_LMOUSE_PRESSED_DOWN )
	{
		_g.btn = 1;

		// Drop old selection
		if ( _g.sel.IsValid() )
			_g.sel.SetDebugDataVisible( CSqirrNode().EDS_OFF );

		// Hilight new selection
		_g.sel = _g.irr.NodeAtScreen( CSqirrVector2d( x.tofloat(), y.tofloat() ), 1 );
		if ( _g.sel.IsValid() )
			_g.sel.SetDebugDataVisible( CSqirrNode().EDS_FULL );
	} // end if

	else if ( e.tointeger() == _g.irr.EMIE_LMOUSE_LEFT_UP )
		_g.btn = 0;

	else if ( e.tointeger() == _g.irr.EMIE_MOUSE_MOVED )
	{
		if ( _g.btn && _g.sel.IsValid() )
		{
			local pos = CSqirrVector3d();
			if ( _g.irr.screenToWorld( CSqirrVector2d( x.tofloat(), y.tofloat() ), pos, 100. ) )
				_g.sel.SetPosition( pos );

		} // end if

	} // end if
*/
}

function _init() : ( _g )
{
	_g.irr = CSqIrrlicht();
	_g.irr.SetCallback( _self.queue(), "_irr_event" );

	// Stereo mode / if you have the colored 3d glasses
//	_g.irr.SetStereo( 1 );

	// Uncomment for super bowl blue/orange glasses
//	_g.irr.SetREyeKey( 0x000000ff );
//	_g.irr.SetLEyeKey( 0x00ffff00 );

	// Create 3d window
	_g.irr.Init( "GUI", 240, 320, 1 );

	// Set ambient light level/color
	_g.irr.SetAmbientLight( CSqirrColorf( 0.5, 0.5, 0.5 ) );

	// Add point lights
	_g.irr.AddLight( CSqirrVector3d( 0., 200., -100. ), CSqirrColorf( 1., 1., 1. ), 800. );
	_g.irr.AddLight( CSqirrVector3d( 50., 200., -100. ), CSqirrColorf( 1., 1., 1. ), 800. );

	// Add default camera
	local cam = _g.irr.AddCamera( CSqirrVector3d( 0, 0, -200 ), CSqirrVector3d( 0, 0, 0 ) );
//	cam.SetLens( 1., 2.4, 3.2 );

//	_g.window = _g.irr.AddGrid( 100., 100., 1, 1, 0., 2, CSqirrColor( 255, 255, 255 ) );
//  _g.window = _g.irr.AddGrid( 10., 10., 10, 20, 0., 2, CSqirrColor( 255, 255, 255 ) );

	_g.window = _g.irr.AddSphere( 10., 16 );
//	_g.window = _g.irr.AddPlane( 160., 120., 2, CSqirrColor( 255, 255, 255 ) );
//	_g.window.SetPosition( CSqirrVector3d( -80., -60., 0. ) );

	local tex = _g.irr.LoadTexture( _self.path( "../media/car.png" ), 1 );
    _g.window.SetTexture( 0, tex );

	local posScreen = CSqirrBoundingBox3d();
//	local posScreen = CSqirrBoundingBox3d( 0, 0, 0, 50, 50, 50 )

	_g.irr.screenToWorldBox( CSqirrRect2d( 20., 50., 220., 100. ), posScreen, 200. );
	_g.irr.FillVolume( _g.window, posScreen );

	// Add a rotator
	local ani = _g.irr.AddRotateAnimator( CSqirrVector3d( 0, 0.4, 0 ) );
	_g.window.AddAnimator( ani );
}

function _idle() : ( _g )
{
	// Set the balls new position
//	_g.ball.SetPosition( CSqirrVector3d( _g.xpos, _g.ypos, _g.zpos ) );

	return _g.irr.Draw( CSqirrColor( 100, 100, 100 ) );
}


