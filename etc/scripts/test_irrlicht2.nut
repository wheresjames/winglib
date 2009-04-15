
_self.load_module( "irrlicht", "" );

class CGlobal
{
	irr = 0;
};

local _g = CGlobal();

function _init() : ( _g )
{
	_g.irr = CSqIrrlicht();

//	_g.irr.SetStereo( 1 );
//	_g.irr.Init( "Irr Test", 240, 320 );
	_g.irr.Init( "Irr Test", 640, 480 );

	_g.irr.SetAmbientLight( CSqirrColorf( 0.5, 0.5, 0.5 ) );

	_g.irr.AddLight( CSqirrVector3d( 0., 200., 0. ), CSqirrColorf( 1., 1., 1. ), 200. );
	_g.irr.AddLight( CSqirrVector3d( 0., 200., 50. ), CSqirrColorf( 1., 1., 1. ), 200. );

//	_self.echo( "adding sky...\n" );
//	_g.irr.AddSkyDome( _self.path( "../imgs/sky.png" ), 16, 16, 100., 100. );

	local cam = _g.irr.AddCamera( CSqirrVector3d( 0, -100, 100 ), CSqirrVector3d( 0, 0, 0 ) );
//	cam.SetLens( 1., 2.4, 3.2 );

    local node = _g.irr.AddGrid( 10., 10., 10, 10, 0.2, 2, CSqirrColor( 255, 255, 255 ), 2 );

    _g.irr.AddMeshAnimator( _self, node, OnAnimate, 0 );

}

function _idle() : ( _g )
{
//	_self.alert( "Waiting..." );

	return _g.irr.Draw( CSqirrColor( 100, 125, 150 ) );
}


function _mod_fp( _x, _y )
{	local n = ( _x / _y ).tointeger();
	return _x - n * _y;
}

function OnAnimate( n, o, c )
{
//    _self.alert( n.tostring() );

	_self.echo( "animating...\n" );

    local pi = 3.141592654;
    local pi2 = pi * 2.;

    local attn = 6;
    local m = pi2 / 100;

    local clk = _irr.clock();

	for ( local i = 0; i < n; i++ )
	{
        // Normalize space
        local u = o.x( i ) * m;
//        local u = _mod_fp( o.x( i ) * m + clk, pi );
        local v = o.y( i ) * m;
//        local v = _mod_fp( o.y( i ) * m + clk, pi );
        local x = 0, y = 0, z = 0;

		// Cylinder
// 		x = sin( u );
//		y = ty;
//		z = cos( u );

		// Cone
//		x = sin( u ) * ( pi - v ) * 0.2;
//		y = v;
//		z = cos( u ) * ( pi - v ) * 0.2;

		// Sphere
//		x = sin( u ) * cos( v / 2 );
//		y = sin( v / 2 );
//		z = cos( u ) * cos( v / 2 );

		// Torus
//		x = ( 2 + cos( v ) ) * cos( u );
//		y = sin( v );
//		z = ( 2 + cos( v ) ) * sin( u );

		// Toroid
//		u += pi; u *= 1.5;
//		x = cos( u ) * ( u / ( 3 * pi ) * cos( v ) + 2 );
//		y = u * sin( v ) / ( 3 * pi );
//		z = sin( u ) * ( u / ( 3 * pi ) * cos( v ) + 2 );

		// Coil
//		x = cos( u * 1.5 ) * ( cos( v ) + 2 );
//		y = sin( u * 1.5 ) * ( cos( v ) + 2 );
//		z = sin( v ) + u;

		// Trefoil Knot
		local a = 0.5;
		u *= 2;
		x = a * ( cos( u ) * cos( v ) + 3 * cos( u ) * ( 1.5 + sin( 1.5 * u ) / 2 ) );
		y = a * ( sin( v ) + 2 * cos( 1.5 * u ) );
		z = a * ( sin( u ) * cos( v ) + 3 * sin( u ) * ( 1.5 + sin( 1.5 * u ) / 2 ) );

		// Nautilus
//		u += pi;
//		x = 0.5 * u * cos( u ) * ( cos( v ) + 1 );
//		y = 0.5 * u * sin( v );
//		z = 0.5 * u * sin( u ) * ( cos( v ) + 1 );

		// Mobius Strip
//		u += pi; // 0 <= u < 2pi
//		v *= 0.2; // -t <= v < t
//		x = cos( u ) + v * cos( u / 2 ) * cos( u );
//		y = sin( u ) + v * cos( u / 2 ) * sin( u );
//		z = v * sin( u / 2 );

		// Klein Bottle
//		local a = 2.0;
//		v += pi; // 0 <= v < 2pi
//		x = ( a + cos( u / 2 ) * sin( v ) - sin( u / 2 ) * sin( 2 * v ) ) * cos( u );
//		y = sin( u / 2 ) * sin( v ) + cos( u / 2 ) * sin( 2 * v );
//		z = ( a + cos( u / 2 ) * sin( v ) - sin( u / 2 ) * sin( 2 * v ) ) * sin( u );

		// Dini's surface
//		u += pi; u *= 2; // 0 <= u < 4pi
//		x = cos( u ) * sin( v );
//		y = -2 - ( cos( v ) + log( tan( v / 2 ) ) + 0.2 * u - 4 );
//		z = sin( u ) * sin( v );

		// Flag
//		 x = u;
//		 y = v;
//		 z = 0;

		// Waving
//        z += sin( u * 2. + clk / 2 ) + sin( u + ( clk * 2.2 ) );
//        z += sin( v * 1. + clk ) + sin( v * 1.1 + ( clk * 1.2 ) );
//        z *= ( ( u - pi ) / pi2 ) / attn;

        c.set( i, x / m, y / m, z / m );

    } // end for

}
