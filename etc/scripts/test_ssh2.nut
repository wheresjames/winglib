
if ( _self.load_module( "openssl", "" ) )
{	_self.echo( "Failed to load openssl module" ); return; }

if ( _self.load_module( "ssh2", "" ) )
{	_self.echo( "Failed to load ssh2 module" ); return; }

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
	local server = "", port = 22;
	local username = "", password = "";
	local pubkey = "", prvkey = "";
//	local pubkey = _self.root( "pubkey" ), prvkey = _self.root( "prvkey" );
	local thost = "google.com", tport = 80;

	_self.echo( "\nConnecting to : " + username + "@" + server + ":" + port +"\n" );

	local socket = CSqSocket();
	if ( !socket.Connect( server, port ) )
	{	_self.echo( "Connect() : " + socket.getLastError() ); WaitKey(); return; }

	// Wait for connect
	if ( !socket.WaitEvent( CSqSocket().EVT_CONNECT, 30000 ) )
	{	_self.echo( "WaitEvent( EVT_CONNECT ) : " + socket.getLastError() ); WaitKey(); return; }

	// Turn on socket blocking
//	socket.setBlockingMode( 1 );

	// SSH2 object
	local ssh2 = CSqSsh2();
	ssh2.setUserInfo( username, "" );
//	ssh2.print_debug_information( 1 );

	// Start SSH connection
	if ( !ssh2.Connect( socket ) )
	{	_self.echo( "Failed to start SSH session : " + ssh2.getLastError() ); WaitKey(); return; }

	_self.echo( "Fingerprint\n" + ssh2.getFingerprint().Fingerprint( 32, 10, 1 ) + "\n" );

	_self.echo( "Authoriaztion Methods : " + ssh2.getAuthorizationMethods() );

	if ( pubkey.len() && prvkey.len() )
	{
		// Create keys if needed
		if ( !CSqFile().exists( prvkey ) )
			if ( !CreateKeys( pubkey, prvkey ) )
			{	_self.echo( "!!! Failed to create key pair" ); WaitKey(); return; }

		// Load from private key
		local key = COsslKey();
		if ( !key.LoadPrivateKey( prvkey ) )
		{	_self.echo( "LoadPrivateKey() failed" ); WaitKey(); return; }

		// Authenticate
//		if ( !ssh2.authPublicKeyBin( username, key.getPrivateKeyRaw(), key.getPublicKeyRaw() ) )
		if ( !ssh2.authPublicKey( username, password, prvkey, "" ) )
		{	_self.echo( "!!! Failed to authenticate SSH session : " + ssh2.getLastError() ); WaitKey(); return; }
		else _self.echo( "\n*** Successfull authentication using username : " + username );

	} // end else if

	// Username password?
	else
	{
		if ( !ssh2.authUsernamePassword( username, password ) )
		{	_self.echo( "!!! Failed to authenticate SSH session : " + ssh2.getLastError() ); WaitKey(); return; }
		else
			_self.echo( "\n*** Successfull authentication using username : " + username );

	} // end if

	_self.echo( "\nTesting SSH tunnel using " + thost + ":" + tport );

	if ( !ssh2.OpenChannelDirectTcpip( "test", thost, tport, "", 0 ) )
	{	_self.echo( "!!! Failed to open channel : " + ssh2.getLastError() ); WaitKey(); return; }

	if ( !ssh2.ChannelWrite( "test", 0, CSqBinary( "GET / HTTP/1.1\r\n\r\n" ), 8 ) )
	{	_self.echo( "!!! ChannelWrite() failed : " + ssh2.getLastError() ); WaitKey(); return; }

	local buf = CSqBinary();
	if ( 0 > ssh2.ChannelRead( "test", 0, buf, 8 ) )
	{	_self.echo( "!!! ChannelRead() failed : " + ssh2.getLastError() ); WaitKey(); return; }

	_self.echo( "\n Read " + buf.getUsed() + " bytes \n" );
	_self.echo( buf.AsciiHexStr( 16, 16 ) );

	_self.echo( "\n...done...\n" );

	WaitKey();
}

function _idle() : ( _g )
{
	return -1;
}

function CreateKeys( pub, prv ) : ( _g )
{
	_self.echo( "Creating new keys" );

	local key = COsslKey();

	if ( !key.CreateRsa( 1024 ) )
	{	_self.echo( "CreateRsa() failed" ); return 0; }

	if ( !key.SaveKeys( prv, pub ) )
	{	_self.echo( "SaveKeys() failed" ); return 0; }

	return 1;
}

