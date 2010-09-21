
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

	local data = CSqBinary();
	if ( curl.GetUrl( url, 0, data ) )
		_self.echo( data.getString() );
	else
		_self.echo( "Error\n" );

	_self.echo( "\n...press any key...\n" );
	_self.get_key();
}

function _idle() : ( _g )
{
	return -1;
}

