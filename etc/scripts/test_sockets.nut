
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
	local server = _self.get( "/", "cmdline.1" );
	if ( !server.len() ) server = "google.com";
	local port = _self.tolong( _self.get( "/", "cmdline.2" ) );
	if ( !port ) port = 80;
	local link = _self.get( "/", "cmdline.3" );
	if ( !link.len() ) link = "/";

	_self.echo( "\n" + server + ":" + port +"\n" );

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

	local tx = "GET " + link + " HTTP/1.1\r\nHost:\r\n\r\n";
	_self.echo( " --- REQUEST --- \r\n\r\n" + tx );
	
	// Write data
	if ( !socket.Write( "GET " + link + " HTTP/1.1\r\nHost:\r\n\r\n", 0 ) )
	{	_self.echo( "Write() : " + socket.getLastError() );
		WaitKey();
		return;
	} // end if

	// Do we want sleep before read?
	local sl = _self.tolong( _self.get( "/", "cmdline.sleep" ) )
	if ( 0 < sl ) _self.sleep( sl );

	// Wait for data
	if ( !socket.WaitEvent( CSqSocket().EVT_READ, 3000 ) )
	{	_self.echo( "WaitEvent( EVT_READ ) : " + socket.getLastError() );
		WaitKey();
		return;
	} // end if

	// Show the data that was read
	_self.echo( " --- RESPONSE --- \r\n\r\n" + socket.Read( 0 ) );

	WaitKey();
}

function _idle() : ( _g )
{
	return -1;
}

