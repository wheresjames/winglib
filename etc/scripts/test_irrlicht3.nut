
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

//	_g.irr.SetAmbientLight( CSqirrColorf( 1.0, 1.0, 1.0, 1.0 ) );

	_g.irr.AddLight( CSqirrVector3d( 0., 200., 0. ), CSqirrColorf( 0.5, 0.5, 0.5 ), 100. );
	_g.irr.AddLight( CSqirrVector3d( 50., 200., 100. ), CSqirrColorf( 0.5, 0.5, 0.5 ), 100. );

	local cam = _g.irr.AddCamera( CSqirrVector3d( 0, 100, 100 ), CSqirrVector3d( 0, 0, 0 ) );
//	cam.SetLens( 1., 2.4, 3.2 );

    local node = _g.irr.AddMesh( "../../lib2/winglib/etc/media/compass.obj", 10.0, 0 );

    local rotate = _g.irr.AddRotateAnimator( CSqirrVector3d( 0, 0.5, 0 ) );
    node.AddAnimator( rotate );
}

function _idle() : ( _g )
{
	return _g.irr.Draw( CSqirrColor( 100, 100, 100 ) );
}


