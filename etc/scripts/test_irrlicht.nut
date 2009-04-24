
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
	_self.echo( "...Initializing...\n" );
	_g.irr.Init( "Irr Test", 240, 320, 1 );

	_self.echo( "...setting ambient light...\n" );
	_g.irr.SetAmbientLight( CSqirrColorf( 0.5, 0.5, 0.5 ) );

	_self.echo( "...adding lights...\n" );
	_g.irr.AddLight( CSqirrVector3d( 0., 100., 0. ), CSqirrColorf( 1., 1., 1. ), 100. );
	_g.irr.AddLight( CSqirrVector3d( 0., 100., 50. ), CSqirrColorf( 1., 1., 1. ), 100. );

//	_self.echo( "adding sky...\n" );
//	_g.irr.AddSkyDome( _self.path( "../imgs/sky.png" ), 16, 16, 100., 100. );

	_self.echo( "...adding camera...\n" );
	local cam = _g.irr.AddCamera( CSqirrVector3d( 0, 30, -40 ), CSqirrVector3d( 0, 0, 0 ) );
//	cam.SetLens( 1., 2.4, 3.2 );

	_self.echo( "...adding cube...\n" );
	local cube = _g.irr.AddCube( 10. );

	_self.echo( "...adding animator...\n" );
	local ani = _g.irr.AddRotateAnimator( CSqirrVector3d( 0, 0.4, 0 ) );
	cube.AddAnimator( ani );
}

function _idle() : ( _g )
{
//	_self.alert( "Waiting..." );

	return _g.irr.Draw( CSqirrColor( 100, 100, 100 ) );
}

