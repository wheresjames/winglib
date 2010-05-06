
class CGlobal
{
	quit = 1;
	http = CSqHttpServer();
	modbus = CSqSocket();
	session = CSqSocket();
	session_addr = CSqSockAddress();

	io = CSqMulti();
};

local _g = CGlobal();

function _init() : ( _g )
{
	for ( local i = 0; i < 8; i++ )
		_g.io[ "inputs" ][ i.tostring() ] <- "0",
		_g.io[ "outputs" ][ i.tostring() ] <- ( i & 1).tostring();

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

function SendError( session, pkt, err )
{
	// Signal error
	pkt.setAbsUCHAR( 7, pkt.getAbsUCHAR( 7 ) | 0x80 );
	pkt.setAbsUCHAR( 8, err );
	pkt.setUsed( 9 );

	// Write error
	if ( !session.WriteBin( pkt, 0 ) )
		_self.echo( "Write() : " + session.getLastError() );
}

function ProcessRequest( session, pkt ) : ( _g )
{
	_self.echo( "Modbus : Data Rx'd : " + pkt.getUsed() );

	// Signal error
//	pkt.setAbsUCHAR( 7, pkt.getAbsUCHAR( 7 ) | 0x80 );

	local tx = CSqBinary();
	if ( !tx.Allocate( 1024 ) )
	{	_self.echo( "Failed to allocate memory for packet" );
		return;
	} // end if
	tx.Zero();

	switch( pkt.getAbsUCHAR( 7 ) )
	{
		case 1 :

			local bits = pkt.BE_getAbsUSHORT( 10 );
			if ( !bits )
				return SendError( session, pkt, 3 );

			local bytes = bits / 8 + ( ( bits % 8 ) ? 1 : 0 );

			_self.echo( "requested: " + bits + "/" + bytes );

			local tx_size = 8 + 1 + bytes;
			tx.setUsed( tx_size );

			tx.BE_setAbsUSHORT( 0, 0 );
			tx.BE_setAbsUSHORT( 2, 0 );
			tx.BE_setAbsUSHORT( 4, tx_size - 6 );
			tx.BE_setAbsUCHAR( 6, 255 );
			tx.BE_setAbsUCHAR( 7, 1 );
			tx.BE_setAbsUCHAR( 8, bytes );

			local pos = 9;
			for ( local c = 0; c < bytes; c++ )
			{
				local mask = 1, data = 0;
				for( local b = 0; b < 8 && bits; b++, bits-- )
				{	if ( _g.io[ "outputs" ][ b.tostring() ].toint() )
						data = data | mask;
					mask = mask << 1;
				} // end for

				tx.BE_setAbsUCHAR( pos++, data );

			} // end for

			break;

		default :
			SendError( session, pkt, 1 );
			break;

	} // end switch

	if ( !session.WriteBin( tx, 0 ) )
		_self.echo( "Write() : " + session.getLastError() );

}

function UpdateModbusServer() : ( _g )
{
	if ( !_g.session.isActivity() )
	{
		if ( !_g.modbus.WaitEvent( CSqSocket().EVT_ACCEPT, 100 ) )
			return 0;

		if ( !_g.modbus.Accept( _g.session ) )
		{	_self.echo( "Accept() : " + _g.modbus.getLastError() );
			return 0;
		} // end if

		_g.session.getPeerAddress( _g.session_addr );
		_self.echo( "Modbus connection from : " + _g.session_addr.getDotAddress() );
		_g.io[ "clients" ] <- _g.session_addr.getDotAddress();

	} // end if

	if ( _g.session.isActivity() )
	{
		if ( _g.session.WaitEvent( CSqSocket().EVT_READ, 0 ) )
		{
			local pkt = CSqBinary();

			if ( _g.session.ReadFromBin( pkt, 0 ) )
				ProcessRequest( _g.session, pkt );

			else
			{
				_g.session.Destroy();
				_g.io.unset( "clients" );
				_self.echo( "Modbus connection closed : " _g.session_addr.getDotAddress() );

			} // end if

		} // end while

	} // end if


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
	_self.echo( _self.gmt_time() + " : " + mParams[ "REQUEST" ][ "REMOTE_ADDR" ].str() + " : " + mParams[ "REQUEST" ][ "REQUEST_STRING" ].str() );

	local page = "Access Denied", raw = "";
	switch ( mParams[ "REQUEST" ][ "path" ].str() )
	{
		case "" : case "/" :
			page = pgHome();
			break;

		case "/test" :
			page = "<pre>" + mParams.print_r( 1 ) + "</pre>";
			break;

		case "/data" :

			if ( mParams[ "GET" ].isset( "toggle_output" ) )
			{	local n = mParams[ "GET" ][ "toggle_output" ].str();
				if ( _g.io[ "outputs" ].isset( n ) )
					_g.io[ "outputs" ][ n ] <- ( _g.io[ "outputs" ][ n ].toint() ? "0" : "1" );
			} // end if

			raw = _g.io.getJSON();
			break;

	} // end switch

	page = raw.len() ? raw : @"<html>
		<head>
				<script src='/js/jquery.min.js' type='text/javascript'></script>
				<script src='/js/jquery.color.js' type='text/javascript'></script>
		</head>
		<body>
			" + page + @"
		</body>
	</html>";

	local mReply = CSqMap();
	mReply.set( "content", page );
	return mReply.serialize();
}

function pgHome() : ( _g )
{
	local border = "border-left: 1px solid #000000;border-top: 1px solid #000000;" +
				   "border-bottom: 1px solid #000000;border-right: 1px solid #000000;";

	local div_ids = "";
	local page = "<table><tr><td><b>Inputs</b></td>";

	if ( _g.io.isset( "inputs" ) )
		foreach ( k,v in _g.io[ "inputs" ] )
			div_ids += ( div_ids.len() ? ", " : "" ) + "'i_" + k + "'",
			page += "<td><div id='i_" + k + "' style='" + border + "background-color:#b0b0b0;' >"
					+ "&nbsp;" + k + "&nbsp;</div></td>";

	page += "</tr><tr><td><b>Outputs</b></td>";

	if ( _g.io.isset( "outputs" ) )
		foreach ( k,v in _g.io[ "outputs" ] )
			div_ids += ( div_ids.len() ? ", " : "" ) + "'o_" + k + "'",
			page += "<td><div id='o_" + k + "' style='" + border + "background-color:#b0b0b0;' onclick='toggleOutput( " + k + " )' >"
					+ "&nbsp;" + k + "&nbsp;</div></td>";

	page += "</tr><tr><td><b>Clients</b></td><td colspan='99'><div id='clients'></div>";

	page += "</table>";

	page += @"
		<script type='text/javascript'>

			var g_req = '';
			var g_div_ids = Array( " + div_ids + @" );

			function toggleOutput( n )
			{	g_req += ( g_req.length ? '&' : '' ) + 'toggle_output=' + n;
			}

			function setState( id, state )
			{	var cols = Array( '#400000', '#ff0000', '#b0b0b0' );
				if ( $('#'+id).css( 'backgroundColor' ) != cols[ parseInt( state ) ] )
					$('#'+id).animate( { backgroundColor: cols[ parseInt( state ) ] }, 500 );
			}

			function data_error()
			{	$('#clients').html( '' );
				for ( var i in g_div_ids ) setState( g_div_ids[ i ], 2 );
			}

			function data_success( data )
			{	if ( !data || !data[ 'inputs' ] || !data[ 'outputs' ] ) return data_error();
				for( var i in data[ 'inputs' ] ) setState( 'i_' + i, data[ 'inputs' ][ i ] );
				for( var i in data[ 'outputs' ] ) setState( 'o_' + i, data[ 'outputs' ][ i ] );
				$('#clients').html( data[ 'clients' ] ? data[ 'clients' ] : '' );
			}

			function poll()
			{	$.ajax( { url: '/data', dataType: 'json', success: data_success, error: data_error, data: g_req })
				g_req = '';
			}

			setInterval( poll, 1000 );

		</script>
	";

	return page;
}
