
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
	_g.irr.Init( "Irr Test", 320, 240 );
	_g.irr.AddSkyDome( _self.path( "../imgs/sky.png" ), 16, 16, 100., 100. );

}

function _idle() : ( _g )
{
//	_self.alert( "Waiting..." );

	return _g.irr.Draw();
}

