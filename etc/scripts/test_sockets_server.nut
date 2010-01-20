
class CGlobal
{
	server = CSqSocket();
};

local _g = CGlobal();

function _init() : ( _g )
{
	_self.echo( "\n... Starting server ...\n" );

	if ( !_g.server.Bind( 9876 ) )
	{	_self.echo( "Bind() : " + _g.server.getLastError() );
		return;
	} // end if

	if ( !_g.server.Listen( 0 ) )
	{	_self.echo( "Listen() : " + _g.server.getLastError() );
		return;
	} // end if
/*
	while( 1 )
	{

	// Wait for connect
	if ( !_g.server.WaitEvent( CSqSocket().EVT_ACCEPT, 30000 ) )
	{	_self.echo( "WaitEvent( EVT_ACCEPT ) : " + _g.server.getLastError() );
		return;
	} // end if

	{ // Handle session inline

		local session = CSqSocket();
		if ( !_g.server.Accept( session ) )
		{	_self.echo( "Accept() : " + _g.server.getLastError() );
			return;
		} // end if

		_self.echo( "\n... Writing data to synchronous socket..." );

		local data = "Synchronous!";
		local write = "HTTP/1.1 200\r\nContent-Length: " + data.len() + "\r\n\r\n" + data;
		if ( !session.Write( write, 0 ) )
		{	_self.echo( "Write() : " + session.getLastError() );
			return;
		} // end if
		_self.echo( write );
	}
	}
*/

	while ( 1 )
	{

	// Wait for connect
	if ( !_g.server.WaitEvent( CSqSocket().EVT_ACCEPT, 30000 ) )
	{	_self.echo( "WaitEvent( EVT_ACCEPT ) : " + _g.server.getLastError() );
		return;
	} // end if

	{ // Handle session in own thread

		local session = CSqSocket();

		// Accept the session
		if ( !_g.server.Accept( session ) )
		{	_self.echo( "Accept() : " + _g.server.getLastError() );
			return;
		} // end if

		// Set the script that will handle the session
		_self.echo( "... Spawning asynchronous session..." );
		session.setScript( _self.queue(), "session", "test_sockets_session.nut", "SetSocket" );

		// Wait for session to handle connection
//		session.WaitEvent( CSqSocket().EVT_CLOSE, 30000 );

//		_self.sleep( 1000 );

	}

	}

	_self.echo( "\n...done...\n" );
}

function _idle() : ( _g )
{
	return -1;
}

