
_self.load_module( "ssh2", "" );

class CGlobal
{
};

local _g = CGlobal();

function WaitKey()
{	_self.echo( "\n...press any key...\n" );
	_self.get_key();
}

function _init() : ( _g )
{
	// Connection information
	local server = "192.168.2.33", port = 22;
	local username = "ru", password = "Ru3026)";

	_self.echo( "\nConnecting to : " + server + ":" + port +"\n" );

	local socket = CSqSocket();
	if ( !socket.Connect( server, port ) )
	{	_self.echo( "Connect() : " + socket.getLastError() );
		WaitKey();
		return;
	} // end if

	// Wait for connect
	if ( !socket.WaitEvent( CSqSocket().EVT_CONNECT, 3000 ) )
	{	_self.echo( "WaitEvent( EVT_CONNECT ) : " + socket.getLastError() );
		WaitKey();
		return;
	} // end if

	// SSH2 object
	local ssh2 = CSqSsh2();

	// Start SSH connection
	if ( !ssh2.Connect( socket ) )
	{	_self.echo( "Failed to start SSH session : " + ssh2.getLastError() );
		WaitKey();
		return;
	} // end if

	_self.echo( "Fingerprint\n" + ssh2.getFingerprint().Fingerprint( 32, 10, 1 ) + "\n" );

	_self.echo( "Authoriaztion Methods : " + ssh2.getAuthorizationMethods() );

	if ( !ssh2.authUsernamePassword( username, password ) )
	{	_self.echo( "!!! Failed to authenticate SSH session : " + ssh2.getLastError() );
		WaitKey();
		return;
	} // end if
	else
		_self.echo( "\n*** Successfull authentication using username : " + username );

	if ( !ssh2.OpenChannelDirectTcpip( "test", "74.14.204.99", 80, "", 0 ) )
	{	_self.echo( "!!! Failed to open channel : " + ssh2.getLastError() );
		WaitKey();
		return;
	} // end if

	if ( !ssh2.ChannelWrite( "test", 0, "GET / HTTP/1.1\r\n\r\n" ) )
	{	_self.echo( "!!! ChannelWrite() failed : " + ssh2.getLastError() );
		WaitKey();
		return;
	} // end if

	_self.echo( "\n...done...\n" );

	WaitKey();
}

function _idle() : ( _g )
{
	return -1;
}

