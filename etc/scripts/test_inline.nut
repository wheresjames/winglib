
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
	local mParams = CSqMulti( params );
	_self.echo( mParams[ "REQUEST" ][ "REMOTE_ADDR" ].str() + " : " + mParams[ "REQUEST" ][ "REQUEST_STRING" ].str() );

	// The result
	local page = _self.include_inline( "test_inline.squ", CSqMulti( "p1=hello,p2=world" ) );

	// Show the raw code
	local script = _self.prepare_inline( _self.path( "test_inline.squ" ), 1 );
	script = _self.htmlencode( script );
	script = _self.replace( script, "&#13;&#10;", "&#10;" );	// windows
	script = _self.replace( script, "&#13;", "&#10;" );			// mac
	script = _self.replace( script, "&#10;", "\n" );
	page += "<hr><br><br><pre>" + script + "</pre>";

	local mReply = CSqMulti();
	mReply[ "content" ] <- page;
	return mReply.serialize();
}

function _init() : ( _g )
{
	local port = 1234;

	_self.echo( "Starting web server on port " + port );

	_g.server = CSqHttpServer();

	_g.server.SetSessionCallback( _self.queue(), "OnProcessRequest" );

	if ( !_g.server.Start( port ) )
		_self.alert( "Unable to start http server" );
}

function _idle()
{
//	_self.alert( "Waiting..." );

	return 0;
}

