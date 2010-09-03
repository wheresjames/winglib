
local file = _self.get( "/", "cmdline.1" );
local server = _self.get( "/", "cmdline.2" );
local port = _self.tolong( _self.get( "/", "cmdline.3" ) );

if ( !CSqFile().exists( file ) )
{	_self.echo( "Invalid file : " + file );
	return 0;
} // end if

_self.echo( "tx : " + file + "\nto : " + server + ":" + port +"\n" );

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
if ( !socket.Write( CSqFile().get_contents( file ), 0 ) )
{	_self.echo( "Write() : " + socket.getLastError() );
	return;
} // end if

// Wait for data
if ( !socket.WaitEvent( CSqSocket().EVT_CLOSE, 30000 ) )
{	_self.echo( "WaitEvent( EVT_CLOSE ) : " + socket.getLastError() );
	return;
} // end if

_self.echo( "\n --- DONE ---\n" );
