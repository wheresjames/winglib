
_self.load_module( "irrlicht", "" );

class CGlobal
{
	// 3D engine
	irr = 0;

	// Ball node
	ball = 0;

	// x position / velocity
	xpos = 0;
	xvel = 1;

	// y position / velocity / friction
	ypos = 0;
	yvel = 0;
	yfrc = 0.005
};

local _g = CGlobal();

function _init() : ( _g )
{
	_g.irr = CSqIrrlicht();

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

	// Add a flat grid
//    _g.ball = _g.irr.AddGrid( 10., 10., 10, 10, 0., 2, CSqirrColor( 255, 255, 255 ), 2 );
	_g.ball = _g.irr.AddSphere( 10., 10., 32 );

	// Shape into a ball
    _g.irr.AddMeshAnimator( _self, _g.ball, make_ball, 1 );

	// Set texture
    _g.ball.SetTexture( 0, _g.irr.LoadTexture( "../../lib2/winglib/etc/media/tennis.jpg", 1 ) );

	// Add a rotator
	local ani = _g.irr.AddRotateAnimator( CSqirrVector3d( 0, 0.4, 0 ) );
	_g.ball.AddAnimator( ani );
}

function _idle() : ( _g )
{
	// Bounce off sides
	if ( -50 > _g.xpos || 50 < _g.xpos )
		_g.xvel = -_g.xvel;
	
	// Gravity
	_g.yvel -= 0.5

	// Friction
	_g.yvel -= ( _g.yvel * _g.yfrc );

	// Bounce off ground
	if ( -100 > _g.ypos )
		_g.yvel = -_g.yvel;

	// Update ball position
	_g.xpos += _g.xvel;
	_g.ypos += _g.yvel;

	// Set the balls new position
	_g.ball.SetPosition( CSqirrVector3d( _g.xpos, _g.ypos, 0 ) );

	return _g.irr.Draw( CSqirrColor( 100, 100, 100 ) );
}

// Complex math crap that bends a grid into a ball
function make_ball( n, o, c )
{
    local pi = 3.141592654;
    local pi2 = pi * 2.;

    local m = pi2 / 100;

	for ( local i = 0; i < n; i++ )
	{
        // Normalize space
        local u = o.x( i ) * m;
        local v = o.y( i ) * m;
        local x = 0, y = 0, z = 0;

		x = sin( u ) * cos( v / 2 );
		y = sin( v / 2 );
		z = cos( u ) * cos( v / 2 );

        c.set( i, x / m, y / m, z / m );

    } // end for

}



