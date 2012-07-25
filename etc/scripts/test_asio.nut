
_self.load_module( "asio", "" );

local asio = CAsioDrv();

// Show a list of available devices
_self.echo( " --- ASIO DEVICES ---\n" + asio.getDevices().getJSON() );

if ( !asio.LoadDriver( 0 ) )
{	_self.echo( "Error : " + asio.getLastErrorStr() );
	return 0;
} // end if

_self.echo( " --- DRIVER INFO ---\n" + asio.getInfo().getJSON() );
				
if ( !asio.OpenChannels( CSqMulti( "0,1" ), "myshare", 0, 0 ) )
{	_self.echo( "Error : " + asio.getLastErrorStr() );
	return 0;
} // end if

_self.echo( "... Starting audio capture ..." );
asio.Start();

_self.echo( "... Capturing ..." );
_self.sleep( 10000 );

_self.echo( "... Stopping audio capture ..." );
asio.Stop();
