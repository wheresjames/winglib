
_self.load_module( "http", "" );
_self.load_module( "cell", "" );

class CGlobal
{
	quit = 0;
	server = 0;
	tc = CCellConnection();
};

local _g = CGlobal();

function OnServerEvent( server, data )
{

}

function OnProcessRequest( get ) : ( _g )
{
	local mGet = CSqMap();
	mGet.deserialize( get );

	local mReply = CSqMap();

	if ( mGet[ "quit" ] )
	{	_g.quit = 1;
		mReply.set( "content", "Quitting" );
		return mReply.serialize();
	} // end if

	if ( mGet[ "close" ] )
		_g.tc.Destroy();

	if ( !_g.tc.IsConnected() && !mGet[ "ip" ] )
	{	mReply.set( "content", "Not connected and no ip specified" );
		return mReply.serialize();
	} // end if

	if ( !_g.tc.IsConnected() || ( mGet[ "ip" ] && mGet[ "ip" ] != _g.tc.GetIp() ) )
		if ( !_g.tc.Connect( mGet[ "ip" ], 1 ) )
		{	mReply.set( "content", "Unable to connect to " + mGet[ "ip" ] );
			return mReply.serialize();
		} // end if

	if ( !_g.tc.IsConnected() )
		mReply.set( "content", "Not connected" );

	else
	{
		if ( mGet[ "tag" ] )
		{
			if ( !_g.tc.tags()[ mGet[ "tag" ] ] )
				mReply.set( "content", "Tag not found " + mGet[ "tag" ] );
			else
				mReply.set( "content", _g.tc.ReadTag( mGet[ "tag" ] ).urlencode() );

		} // end if

		else if ( mGet[ "all" ] )
		{
			local s = "Tags:\r\n";
			local tags = _g.tc.tags();
			foreach( k,v in tags )
			{
				s += k + " : " + v + "\r\n";

			} // end foreach

			mReply.set( "content", s );

		} // end if

/*
		if ( mGet[ "tag" ] )
			mReply.set( "content", _g.tc.ReadTag( "", mGet[ "tag" ] ).urlencode() );
		else if ( mGet[ "all" ] )
			mReply.set( "content", _g.tc.GetBackplaneData() );
		else
			mReply.set( "content", "Connected to " + _g.tc.GetIp() + ", so, what else do you want?" );
*/
	} // end else

	return mReply.serialize();
}

function _init() : ( _g )
{
	_g.server = CHttpServer();

	_g.server.SetCallback( _self.queue(), "OnServerEvent", "OnProcessRequest" );

	if ( !_g.server.Start( 1234 ) )
		_self.alert( "Unable to start http server" );
}

function _idle() : ( _g )
{
	return _g.quit;
}

