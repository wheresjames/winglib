
_self.load_module( "irrlicht", "" );

class CGlobal
{
	irr = 0;
};

local _g = CGlobal();

function _init() : ( _g )
{
	_g.irr = CSqIrrlicht();

	_g.irr.Init( 320, 240 );

}

function _idle() : ( _g )
{
//	_self.alert( "Waiting..." );

	return _g.irr.Draw();
}

