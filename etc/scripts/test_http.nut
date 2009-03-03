
_self.load_module( "http", "" );

class CGlobal
{
	server = 0;
};

local _g = CGlobal();

function OnServerEvent( server, data )
{

}

function OnProcessRequest( request, headers, get, post )
{
//	_self.alert( get[ "str" ] );

//	_self.alert( "hi" );

	local mGet = CSqMap();
	mGet.deserialize( get );

	local mReply = CSqMap();
	mReply.set( "content", mGet[ "str1" ] + " " + mGet[ "str2" ] );
	return mReply.serialize();
}

function _init() : ( _g )
{
	_g.server = CHttpServer();

	_g.server.SetCallback( _self.queue(), "OnServerEvent", "OnProcessRequest" );

	if ( !_g.server.Start( 1234 ) )
		_self.alert( "Unable to start http server" );
}

function _idle()
{
//	_self.alert( "Waiting..." );

	return 0;
}

