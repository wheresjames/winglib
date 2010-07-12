
class CGlobal
{
	share = CSqBinary();
};

local _g = CGlobal();

function _init() : ( _g )
{
	local k = _self.get( "/", "cmdline.k" );
	if ( !k.len() ) k = "1234";
	_self.echo( "key = " + k );

	_g.share.SetName( k );
	_g.share.PlainShare( 1 );
	_g.share.Allocate( 8 );
	_g.share.setUsed( 8 );
}

function _idle() : ( _g )
{
	if ( _self.get( "/", "cmdline.s" ).len() )
		_g.share.setUINT( 0, _g.share.getUINT( 0 ) + 1 );

	_self.echo( _g.share.AsciiHexStr( 16, 16 ) );

	return 0;
}

