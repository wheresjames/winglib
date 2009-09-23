
_self.load_module( "http", "" );

class CGlobal
{
	server = 0;
};

local _g = CGlobal();

function OnServerEvent( server, data )
{

}

function OnProcessRequest( params )
{
	local mParams = CSqMulti();
	mParams.deserialize( params );
	_self.echo( mParams[ "REQUEST" ][ "REMOTE_ADDR" ].str() + " : " + mParams[ "REQUEST" ][ "REQUEST_STRING" ].str() );

	local page = "<pre>" + mParams.print_r( 1 ) + "</pre>";

	local mReply = CSqMap();
	mReply.set( "content", page );
	return mReply.serialize();
}

function _init() : ( _g )
{
	_g.server = CHttpServer();

	_g.server.SetSessionCallback( _self.queue(), "OnProcessRequest" );

	if ( !_g.server.Start( 1234 ) )
		_self.alert( "Unable to start http server" );
}

function _idle()
{
//	_self.alert( "Waiting..." );

	return 0;
}

