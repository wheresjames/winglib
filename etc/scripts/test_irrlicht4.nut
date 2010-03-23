
_self.load_module( "irrlicht", "" );

class CGlobal
{
	irr = 0;
};

local _g = CGlobal();

function _init() : ( _g )
{
	_g.irr = CSqIrrlicht();

	_g.irr.SetStereo( 1 );
//	_g.irr.SetREyeKey( 0x000000ff );
//	_g.irr.SetLEyeKey( 0x00ffff00 );
	_g.irr.Init( "Irr Test", 240, 320, 1 );
//	_g.irr.Init( "Irr Test", 640, 480, 1 );

	_g.irr.SetAmbientLight( CSqirrColorf( 0.5, 0.5, 0.5 ) );

	_g.irr.AddLight( CSqirrVector3d( 0., 200., 0. ), CSqirrColorf( 0.5, 0.5, 0.5 ), 100. );
	_g.irr.AddLight( CSqirrVector3d( 50., 200., 100. ), CSqirrColorf( 0.5, 0.5, 0.5 ), 100. );

	local cam = _g.irr.AddCamera( CSqirrVector3d( 0, 150, 150 ), CSqirrVector3d( 0, 0, 0 ) );
//	cam.SetLens( 1., 2.4, 3.2 );

    local node = _g.irr.AddGrid( 10., 10., 10, 20, 0., 2, CSqirrColor( 255, 255, 255 ), 2 );
    _g.irr.AddMeshAnimator( _self, node, OnAnimate, 1 );

    local rotate = _g.irr.AddRotateAnimator( CSqirrVector3d( 0, 0.1, 0 ) );
    node.AddAnimator( rotate );

	local tex = _g.irr.LoadTexture( _self.path( "../media/wheresjames_logo.png" ), 1 );
    node.SetTexture( 0, tex );
}

function _idle() : ( _g )
{
	return _g.irr.Draw( CSqirrColor( 100, 100, 100 ) );
}

function _mod_fp( _x, _y )
{	local n = ( _x / _y ).tointeger();
	return _x - n * _y;
}

function OnAnimate( n, o, c )
{
    local pi = 3.141592654;
    local pi2 = pi * 2.;

    local attn = 6;
    local m = pi2 / 100;

	local clk = _self.ticks() / 200.;

	for ( local i = 0; i < n; i++ )
	{
        // Normalize space
        local u = o.x( i ) * m;
        local v = o.y( i ) * m;
        local x = 0, y = 0, z = 0;
        local _u = u, _v = v;

		// Flag
		x += u;
		y += v;
		z += 0;

		// Waving
        z += sin( u * 2. + clk / 2 ) + sin( u + ( clk * 2.2 ) );
        z += sin( v * 1. + clk ) + sin( v * 1.1 + ( clk * 1.2 ) );
        z *= ( ( u - pi ) / pi2 ) / attn;

        c.set( i, x / m, y / m, z / m );

    } // end for

}


