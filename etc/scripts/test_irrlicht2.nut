
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

	local cam = _g.irr.AddCamera( CSqirrVector3d( 0, 100, 100 ), CSqirrVector3d( 0, 0, 0 ) );
//	cam.SetLens( 1., 2.4, 3.2 );

    local node = _g.irr.AddGrid( 10., 10., 10, 10, 0.2, 2, CSqirrColor( 255, 255, 255 ), 2 );
    _g.irr.AddMeshAnimator( _self, node, OnAnimate, 1 );

    local rotate = _g.irr.AddRotateAnimator( CSqirrVector3d( 0, 0.5, 0 ) );
    node.AddAnimator( rotate );
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

	local clk = _mod_fp( _self.ticks() / 5000., 11. );

	for ( local i = 0; i < n; i++ )
	{
        // Normalize space
        local u = o.x( i ) * m;
        local v = o.y( i ) * m;
        local x = 0, y = 0, z = 0;
        local _u = u, _v = v;

		// Flag
		if ( 1 > clk )
		{
			local p = 1 - clk;

			x += p * u;
			y += p * v;
			z += p * 0;

		} // end if

		// Cylinder
		local q = 0;
		u = _u; v = _v;
		if ( q < clk && ( q + 2 ) > clk )
		{
			local p = clk - q;
			if ( 1 < p ) p = 2 - p;

	 		x += p * sin( u );
			y += p * v;
			z += p * cos( u );

		} // end if

		// Cone
		q++;
		u = _u; v = _v;
		if ( q < clk && ( q + 2 ) > clk )
		{
			local p = clk - q;
			if ( 1 < p ) p = 2 - p;

			x += p * sin( u ) * ( pi - v ) * 0.2;
			y += p * v;
			z += p * cos( u ) * ( pi - v ) * 0.2;

		} // end if

		// Sphere
		q++;
		u = _u; v = _v;
		if ( q < clk && ( q + 2 ) > clk )
		{
			local p = clk - q;
			if ( 1 < p ) p = 2 - p;

			x += p * sin( u ) * cos( v / 2 );
			y += p * sin( v / 2 );
			z += p * cos( u ) * cos( v / 2 );

		} // end if

		// Torus
		q++;
		u = _u; v = _v;
		if ( q < clk && ( q + 2 ) > clk )
		{
			local p = clk - q;
			if ( 1 < p ) p = 2 - p;

			x += p * ( 2 + cos( v ) ) * cos( u );
			y += p * sin( v );
			z += p * ( 2 + cos( v ) ) * sin( u );

		} // end if

		// Toroid
		q++;
		u = _u; v = _v;
		if ( q < clk && ( q + 2 ) > clk )
		{
			local p = clk - q;
			if ( 1 < p ) p = 2 - p;

			u += pi; u *= 1.5;
			x += p * ( cos( u ) * ( u / ( 3 * pi ) * cos( v ) + 2 ) );
			y += p * ( u * sin( v ) / ( 3 * pi ) );
			z += p * ( sin( u ) * ( u / ( 3 * pi ) * cos( v ) + 2 ) );

		} // end if

		// Nautilus
		q++;
		u = _u; v = _v;
		if ( q < clk && ( q + 2 ) > clk )
		{
			local p = clk - q;
			if ( 1 < p ) p = 2 - p;

			u += pi;
			x += p * ( 0.5 * u * cos( u ) * ( cos( v ) + 1 ) );
			y += p * ( 0.5 * u * sin( v ) );
			z += p * ( 0.5 * u * sin( u ) * ( cos( v ) + 1 ) );

		} // end if

		// Coil
		q++;
		u = _u; v = _v;
		if ( q < clk && ( q + 2 ) > clk )
		{
			local p = clk - q;
			if ( 1 < p ) p = 2 - p;

			x += p * ( cos( u * 1.5 ) * ( cos( v ) + 2 ) );
			y += p * ( sin( u * 1.5 ) * ( cos( v ) + 2 ) );
			z += p * ( sin( v ) + u );

		} // end if

		// Trefoil Knot
		q++;
		u = _u; v = _v;
		if ( q < clk && ( q + 2 ) > clk )
		{
			local p = clk - q;
			if ( 1 < p ) p = 2 - p;

			local a = 0.5;
			u *= 2;
			x += p * ( a * ( cos( u ) * cos( v ) + 3 * cos( u ) * ( 1.5 + sin( 1.5 * u ) / 2 ) ) );
			y += p * ( a * ( sin( v ) + 2 * cos( 1.5 * u ) ) );
			z += p * ( a * ( sin( u ) * cos( v ) + 3 * sin( u ) * ( 1.5 + sin( 1.5 * u ) / 2 ) ) );

		} // end if

		// Mobius Strip
		q++;
		u = _u; v = _v;
		if ( q < clk && ( q + 2 ) > clk )
		{
			local p = clk - q;
			if ( 1 < p ) p = 2 - p;

			u += pi; // 0 <= u < 2pi
			v *= 0.2; // -t <= v < t
			x += p * ( cos( u ) + v * cos( u / 2 ) * cos( u ) );
			y += p * ( sin( u ) + v * cos( u / 2 ) * sin( u ) );
			z += p * ( v * sin( u / 2 ) );

		} // end if

		// Klein Bottle
		q++;
		u = _u; v = _v;
		if ( q < clk && ( q + 2 ) > clk )
		{
			local p = clk - q;
			if ( 1 < p ) p = 2 - p;

			local a = 2.0;
			v += pi; // 0 <= v < 2pi
			x += p * ( ( a + cos( u / 2 ) * sin( v ) - sin( u / 2 ) * sin( 2 * v ) ) * cos( u ) );
			y += p * ( sin( u / 2 ) * sin( v ) + cos( u / 2 ) * sin( 2 * v ) );
			z += p * ( ( a + cos( u / 2 ) * sin( v ) - sin( u / 2 ) * sin( 2 * v ) ) * sin( u ) );

		} // end if

		// Flag
		q++;
		u = _u; v = _v;
		if ( q < clk && ( q + 2 ) > clk )
		{
			local p = clk - q;
			if ( 1 < p ) p = 2 - p;

			x += p * u;
			y += p * v;
			z += p * 0;

		} // end if

		// Waving
//        z += sin( u * 2. + clk / 2 ) + sin( u + ( clk * 2.2 ) );
//        z += sin( v * 1. + clk ) + sin( v * 1.1 + ( clk * 1.2 ) );
//        z *= ( ( u - pi ) / pi2 ) / attn;

        c.set( i, x / m, y / m, z / m );

    } // end for

}
