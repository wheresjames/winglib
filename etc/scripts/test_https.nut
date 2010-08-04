
_self.load_module( "http", "" );
_self.load_module( "openssl", "" );

class CGlobal
{
	server = 0;
	ssl = 0;
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
	local port = 1234;

	_self.echo( "Starting web server on port " + port );

	_g.server = CSqHttpServer();

	_g.ssl = CSqSSLPortFactory();
	if ( !_g.ssl.Initialize() || !_g.ssl.LoadCerts( "cert", "pkey" ) )
	{	_self.echo( _g.ssl.getLastError() ); return 0; }

	_g.server.setPortFactory( _g.ssl.getFactory() );

	_g.server.SetSessionCallback( _self.queue(), "OnProcessRequest" );

	if ( !_g.server.Start( port ) )
		_self.alert( "Unable to start http server" );
}

function _idle()
{
//	_self.alert( "Waiting..." );

	return 0;
}

function _exit()
{
	_g.server.Destroy();
}

