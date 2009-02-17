
_self.load_module( "http", "" );

class CGlobal
{
	server = 0;
};

local _g = CGlobal();

function OnProcessRequest( session, data )
{

}

function _init() : ( _g )
{
	_g.server = CHttpServer();

	_g.server.SetQueue( _self.queue() );

	if ( !_g.server.Start( 1234 ) )
		_self.alert( "Unable to start http server" );
}

function _idle()
{
	_self.alert( "Waiting..." );

	return 1;
}

