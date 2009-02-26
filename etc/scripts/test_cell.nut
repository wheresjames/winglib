
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
				mReply.set( "content", _g.tc.ReadTag( mGet[ "tag" ] ).serialize() );

		} // end if

		else if ( mGet[ "all" ] )
		{
			local s = "<table border=1><tr><td><b>Tag</b></td><td><b>Items</b></td><td><b>Type</b></td><td><b>Dim</b></td><td><b>Struct</b><td><b>a1_size</b></td><td><b>Value</b></td></tr>";
			local tags = _g.tc.tags();

			local i = 0;
			foreach( k,v in tags )
			{
				local rep = _g.tc.ReadTag( k );

				if ( ++i & 1 )
					s += "<tr valign=top>";
				else
					s += "<tr valign=top bgcolor=#d0e0ff>";
				s += "<td><small>" + k + "</small></td>";

				if ( rep[ "err" ] )
					s += "<td colspan=99>" + rep[ "err" ] + "</small></td>";
				else
				{
					s += "<td><small>" + rep[ "items" ] + "</small></td>";
					s += "<td><small>" + format( "0x%04x:", rep[ "type" ].tointeger() ) + rep[ "type_name" ] + "</small></td>";
					s += "<td><small>" + rep[ "dim" ] + "</small></td>";
					s += "<td><small>" + rep[ "struct" ] + "</small></td>";
					s += "<td><small>" + rep[ "a1_size" ] + "</small></td>";
					if ( rep[ "value" ] )
						s += "<td><small>" + rep[ "value" ] + "</small></td>";
					else
						s += "<td><small>[n/a]</small></td>";

				} // end else

				s += "</tr>";

			} // end foreach

			s += "</table>";

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

