
class CGlobal
{
};

local _g = CGlobal();

function _init() : ( _g )
{
	local server = "google.com";
	local port = 80;

	_self.echo( "\n" + server + ":" + port +"\n" );

	local socket = CSqSocket();
	if ( !socket.Connect( server, port ) )
	{	_self.echo( "Connect() : " + socket.getLastError() );
		return;
	} // end if

	// Wait for connect
	if ( !socket.WaitEvent( CSqSocket().EVT_CONNECT, 3000 ) )
	{	_self.echo( "WaitEvent( EVT_CONNECT ) : " + socket.getLastError() );
		return;
	} // end if

	// Write data
	if ( !socket.Write( "GET / HTTP/1.1\r\n\r\n", 0 ) )
	{	_self.echo( "Write() : " + socket.getLastError() );
		return;
	} // end if

	// Wait for data
	if ( !socket.WaitEvent( CSqSocket().EVT_READ, 3000 ) )
	{	_self.echo( "WaitEvent( EVT_READ ) : " + socket.getLastError() );
		return;
	} // end if

	// Show the data that was read
	_self.echo( " Data: " + socket.Read( 0 ) );

	_self.echo( "\n...done...\n" );
}

function _idle() : ( _g )
{
	return -1;
}

