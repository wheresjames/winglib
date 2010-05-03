
class CGlobal
{
	quit = 1;
	http = CSqHttpServer();

	modbus = CSqSocket();
};

local _g = CGlobal();

function _init() : ( _g )
{
	if ( !StartWebServer( 1234 ) )
		return;

	if ( !StartModbusServer( 1502 ) )
		return;

	_g.quit = 0;
}

function _idle() : ( _g )
{
	if ( _g.quit )
		return -1;

	UpdateModbusServer();

	return 0;
}

//-------------------------------------------------------------------
// Modbus
//-------------------------------------------------------------------

function StartModbusServer( port ) : ( _g )
{
	if ( !_g.modbus.Bind( port ) )
	{	_self.echo( "Bind() : " + _g.modbus.getLastError() );
		return 0;
	} // end if

	if ( !_g.modbus.Listen( 0 ) )
	{	_self.echo( "Listen() : " + _g.modbus.getLastError() );
		return 0;
	} // end if

	return 1;
}

function UpdateModbusServer() : ( _g )
{
	if ( !_g.modbus.WaitEvent( CSqSocket().EVT_ACCEPT, 1000 ) )
		return 0;

	local session = CSqSocket();
	if ( !_g.modbus.Accept( session ) )
	{	_self.echo( "Accept() : " + _g.modbus.getLastError() );
		return 0;
	} // end if

	while ( session.WaitEvent( CSqSocket().EVT_READ, 10000 ) )
	{
		local pkt_size = 12 * 4;
		local pkt = CSqBinary();

		if ( !session.ReadFromBin( pkt, 0 ) )
		{	_self.echo( "ReadFromBin() : " + session.getLastError() );
			return 0;
		} // end if

		_self.echo( "Modbus : Data Rx'd : " + pkt.getUsed() );



	} // end while

	return 1;
}

//-------------------------------------------------------------------
// Web Server
//-------------------------------------------------------------------

function StartWebServer( port ) : ( _g )
{
	_g.http.SetSessionCallback( _self.queue(), "OnProcessRequest" );
	_g.http.MapFolder( "js", _self.path( "js" ) );
	_g.http.EnableMultiThreading( 0 );

	if ( !_g.http.Start( port ) )
	{	_self.echo( "Start() : " + _g.http.getLastError() );
		return 0;
	} // end if

	_self.echo( "Web server running at http://localhost:" + port + "/" );

	return 1;
}

function OnProcessRequest( params ) : ( _g )
{
	local mParams = CSqMulti();
	mParams.deserialize( params );
	_self.echo( mParams[ "REQUEST" ][ "REMOTE_ADDR" ].str() + " : " + mParams[ "REQUEST" ][ "REQUEST_STRING" ].str() );

	local page = "Access Denied", raw = "";
	switch ( mParams[ "REQUEST" ][ "path" ].str() )
	{
		case "" : case "/" :
			page = @"
				<div id='data'>loading...</div>
				<script type='text/javascript'>
					setTimeout( '$(data).load( \'/data\' );', 1000 );
				</script>
			";
			break;

		case "/test" :
			page = "<pre>" + mParams.print_r( 1 ) + "</pre>";
			break;

		case "/data" :
			raw = "12345";
			break;

	} // end switch

	page = raw.len() ? raw : @"<html>
		<head>
				<script src='/js/jquery.min.js' type='text/javascript'></script>
		</head>
		<body>
			" + page + @"
		</body>
	</html>";

	local mReply = CSqMap();
	mReply.set( "content", page );
	return mReply.serialize();
}

