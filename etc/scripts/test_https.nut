
_self.load_module( "http", "" );
_self.load_module( "openssl", "" );

class CGlobal
{
	quit = 0;
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

	_self.echo( "Starting web server at https://localhost:" + port + "/" );

	_g.server = CSqHttpServer();

	_g.ssl = CSqSSLPortFactory();
	if ( !_g.ssl.Initialize() || !_g.ssl.LoadCerts( _self.root( "cert" ), _self.root( "pkey" ) ) )
	{	_self.echo( _g.ssl.getLastError() ); _g.quit = 1; return 0; }

	_g.server.setPortFactory( _g.ssl.getFactory() );

	_g.server.SetSessionCallback( _self.queue(), "OnProcessRequest" );

	if ( !_g.server.Start( port ) )
		_g.quit = 1,
		_self.alert( "Unable to start http server" );
}

function _idle() : ( _g )
{
//	_self.alert( "Waiting..." );

	return _g.quit;
}

function _exit() : ( _g )
{
	_g.server.Stop();
	_g.ssl.Destroy();
}

