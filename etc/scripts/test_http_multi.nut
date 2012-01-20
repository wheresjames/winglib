
_self.load_module( "curl", "" );

function _init()
{
	local url = _self.get( "/", "cmdline.1" );
	local num = _self.tolong( _self.get( "/", "cmdline.2" ) );
	local head = _self.tolong( _self.get( "/", "cmdline.h" ) );
	local body = 1;
	if ( _self.isset( "/", "cmdline.b" ) )
		body = _self.tolong( _self.get( "/", "cmdline.b" ) );
		
	_self.echo( _self.get( "/", "cmdline" ) );

	if ( 0 >= num )
		num = 2;

	_self.echo( "\nNUM: " + num );
	_self.echo( "URL: " + url );

	local addr = _self.parse_url( url );
	_self.echo( addr.print_r( 1 ) );
//	::_self.echo( "SVR: " + addr[ "host" ].str() + ":" + addr[ "port" ].str() );
//	::_self.echo( "PTH: " + addr[ "path" ].str() );


	::_self.echo( "\n --- RAW SOCKET ---\n" );

	// Connect
	local socket = CSqSocket();
	socket.Connect( addr[ "host" ].str(), addr[ "port" ].toint() );
	if ( !socket.WaitEvent( CSqSocket().EVT_CONNECT, 8000 ) )
	{	_self.echo( "WaitEvent( EVT_CONNECT ) : " + socket.getLastError() );
		return;
	} // end if

	for ( local i = 0; i < num; i++ )
	{
		::_self.echo( " --- Attempt " + ( i + 1 ) + " ---" );

		if ( !socket.Write( "GET " + addr[ "path" ].str() + "?" + addr[ "extra" ].str() + " HTTP/1.1\r\n"
							+ "Host: 127.0.0.1\r\n"
							+ "Connection: Keep-Alive\r\n"
							+ "\r\n", 0 ) )
		{	_self.echo( "Write() : " + socket.getLastError() );
			return 0;
		} // end if
		
		if ( !socket.WaitEvent( CSqSocket().EVT_READ, 8000 ) )
		{	_self.echo( "WaitEvent( EVT_READ ) : " + socket.getLastError() );
			return 0;
		} // end if

		local data = CSqBinary(), len = -1;
		do 
		{
			// Append data
			data.appendString( socket.Read( 0 ) );

			// Do we need headers?
			if ( 0 > len )
			{
				// Find the end of headers
				local eh = data.FindBin( CSqBinary( "\r\n\r\n" ), 0, 0 );
				if ( data.failed() != eh )
				{
					if ( head )
						_self.echo( data.getSubString( 0, eh ) );

					// Parse the headers
					local headers = CSqMulti();
					headers.setMIME( data.getSubString( 0, eh ), 0 );
					
					// Grab the content length
					len = headers[ "content-length" ].toint();

					// Drop the headers
					data.LShift( eh + 4 );

				} // end if

			} // end if

		} while ( ( 0 > len || data.getUsed() < len )
				  && socket.WaitEvent( CSqSocket().EVT_READ, 8000 ) );

		_self.echo( "Len : " + len );
		_self.echo( "Rx  : " + data.getUsed() );
		if ( body )
			_self.echo( data.getString() );

	} // end while


	// Same thing again, but with curl
	::_self.echo( "\n\n --- CURL ---\n" );

	local curl = CSqCurl();
	curl.setHeader( "Connection: Keep-Alive" );

	for ( local i = 0; i < num; i++ )
	{
		::_self.echo( " --- Attempt " + ( i + 1 ) + " ---" );

		local data = CSqBinary();
		if ( curl.GetUrl( url, 0, data ) )
		{	_self.echo( "Rx : " + data.getUsed() );
			if ( body )
				_self.echo( data.getString() );
		} // end if
		else
			_self.echo( "Error\n" );

	} // end for

	_self.echo( "\n... done ...\n" );
}

function _idle()
{
	return -1;
}


