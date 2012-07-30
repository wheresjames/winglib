
_self.load_module( "asio", "" );

local asio = CAsioDrv();

// Show a list of available devices
_self.echo( " --- ASIO DEVICES ---\n" + asio.getDevices().getJSON() );

if ( !asio.LoadDriver( 0 ) )
{	_self.echo( "Error : " + asio.getLastErrorStr() );
	return 0;
} // end if

_self.echo( " --- DRIVER INFO ---\n" + asio.getInfo().getJSON() );

local ch = asio.OpenChannels( CSqMulti( "s1=0,s2={1,2},s3={1,2,3}" ), 0, 0 );
_self.echo( "Opened channels : " + ch );

if ( 0 >= ch )
{	_self.echo( "Error : " + asio.getLastErrorStr() );
	return 0;
} // end if

_self.echo( "... Starting audio capture ..." );

if ( !asio.Start() )
{	_self.echo( "Error : " + asio.getLastErrorStr() );
	return 0;
} // end if

_self.echo( "... Capturing ..." );
_self.sleep( 3000 );

_self.echo( "... Stopping audio capture ..." );

if ( !asio.Stop() )
{	_self.echo( "Error : " + asio.getLastErrorStr() );
	return 0;
} // end if
