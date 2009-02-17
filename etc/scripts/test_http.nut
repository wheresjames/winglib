
_self.load_module( "http", "" );

class CGlobal
{
	server = 0;
};

local _g = CGlobal();

function _init() : ( _g )
{

	_g.server = CHttpServer();

	if ( !_g.server.Start( 1234 ) )
		_self.alert( "Unable to start http server" );

}

function _idle()
{
	_self.alert( "Waiting..." );

	return 1;
}

