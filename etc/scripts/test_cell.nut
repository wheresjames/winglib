
_self.load_module( "http", "" );
_self.load_module( "cell", "" );

class CGlobal
{
	server = 0;
};

local _g = CGlobal();

function OnServerEvent( server, data )
{

}

function OnProcessRequest( get )
{
	local mGet = CSqMap();
	mGet.deserialize( get );

	local mReply = CSqMap();
	local tc = CCellConnection();

	if ( !mGet[ "ip" ] )
		mReply.set( "content", "You didn't specify an ip address" );

	else if ( !tc.Connect( mGet[ "ip" ] ) )
		mReply.set( "content", "Failed to connect to device at " + mGet[ "ip" ] );

	else if ( !mGet[ "tag" ] )
		mReply.set( "content", "Data : " + tc.GetBackplaneData() );

	else
		mReply.set( "content", mGet[ "tag" ] + " = " + tc.ReadTag( "CSA", mGet[ "tag" ] ) );

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


/*

function _init()
{
	_self.load_module( "cell", "" );

	local tc = CCellConnection();

	if ( !tc.Connect( "172.17.2.20" ) )
		_self.alert( "Failed to connect to device" );

	_self.alert( tc.GetBackplaneData() );

}

function _idle()
{
	_self.alert( "Waiting..." );

	return 1;
}


*/
