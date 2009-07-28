
_self.load_module( "irrlicht", "" );

class CGlobal
{
	// 3D engine
	irr = 0;

	// Ball node
	ball = 0;

	// x position / velocity
	xpos = 0;
	xvel = 2;
	xfrc = 0.005;

	// y position / velocity / friction
	ypos = 0;
	yvel = 0;
	yfrc = 0.01;

	zpos = 0;
};

local _g = CGlobal();

function _irr_event( e, x, y, w ) : ( _g )
{
//	_self.echo( e + ", " + x + ", " + y + ", " + w );

	if ( e.tointeger() == _g.irr.EMIE_LMOUSE_PRESSED_DOWN )
	{
		local pos = CSqirrVector3d();
		_g.irr.screenToWorld( CSqirrVector2d( x.tofloat(), y.tofloat() ), pos, 100. );
		_g.xpos = pos.x();
		_g.ypos = pos.y();
		_g.zpos = pos.z();
		_g.yvel = 0;
		_g.xvel = 2;
//		_g.ball.SetPosition( pos );

	} // end if
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
	_g.irr.Init( "Bouncing Ball", 240, 320, 1 );

	// Set ambient light level/color
	_g.irr.SetAmbientLight( CSqirrColorf( 0.5, 0.5, 0.5 ) );

	// Add point lights
	_g.irr.AddLight( CSqirrVector3d( 0., 200., 0. ), CSqirrColorf( 0.5, 0.5, 0.5 ), 100. );
	_g.irr.AddLight( CSqirrVector3d( 50., 200., 100. ), CSqirrColorf( 0.5, 0.5, 0.5 ), 100. );

	// Add default camera
	local cam = _g.irr.AddCamera( CSqirrVector3d( 0, 50, 100 ), CSqirrVector3d( 0, 0, 0 ) );
//	cam.SetLens( 1., 2.4, 3.2 );

	// Add a ball
	_g.ball = _g.irr.AddSphere( 10., 16 );

	// Set texture
    _g.ball.SetTexture( 0, _g.irr.LoadTexture( _self.path( "../media/tennis.jpg" ), 1 ) );

	// Add a rotator
	local ani = _g.irr.AddRotateAnimator( CSqirrVector3d( 0, 0.4, 0 ) );
	_g.ball.AddAnimator( ani );
}

function _idle() : ( _g )
{
	// Bounce off sides
	if ( -50 > _g.xpos || 50 < _g.xpos )
		_g.xvel = -_g.xvel;
	
	// Friction
	_g.yvel -= ( _g.yvel * _g.yfrc );
	_g.xvel -= ( _g.xvel * _g.xfrc );

	// Bounce off ground
	if ( -100 > _g.ypos )
	{	if ( 0  > _g.yvel )
			_g.yvel = -_g.yvel;
	} // end if

	// Gravity
	else if ( -98 < _g.ypos )
		_g.yvel -= 0.5

	// Update ball position
	_g.xpos += _g.xvel;
	_g.ypos += _g.yvel;

	// Set the balls new position
	_g.ball.SetPosition( CSqirrVector3d( _g.xpos, _g.ypos, _g.zpos ) );

	return _g.irr.Draw( CSqirrColor( 100, 100, 100 ) );
}


