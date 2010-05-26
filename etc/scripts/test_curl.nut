
_self.load_module( "curl", "" );
_self.load_module( "tinyxml", "" );

class CGlobal
{
};

local _g = CGlobal();

function _init() : ( _g )
{
	local url = "http://www.google.com/";
	local curl = CSqCurl();

	_self.echo( "\n" + url + "\n" );

	local data = CSqString();
	if ( curl.GetUrl( url, 0, data ) )
		_self.echo( data.str() );
	else
		_self.echo( "Error\n" );

	_self.echo( "\n...done...\n" );

}

function _idle() : ( _g )
{
	return -1;
}

