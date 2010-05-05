
class CGlobal
{
	quit = 1;
	http = CSqHttpServer();
	session = CSqSocket();
	session_addr = CSqSockAddress();
	session_mode = 0;

	connect = "";
	disconnect = 0;

	io = CSqMulti();
};

local _g = CGlobal();

function _init() : ( _g )
{
	for ( local i = 0; i < 8; i++ )
		_g.io[ "inputs" ][ i.tostring() ] <- "",
		_g.io[ "outputs" ][ i.tostring() ] <- "";

	if ( !StartWebServer( 1235 ) )
		return;

	_self.set_timer( ".", 1000, "UpdateModbusServer" );

	_g.quit = 0;
}

function _idle() : ( _g )
{
	if ( _g.quit )
		return -1;

	if ( _g.connect.len() )
	{	_self.echo( "Connecting to : " + _g.connect );

		local ip = _g.connect;
		_g.connect = "";
		_g.session_mode = 0;

		if ( !_g.session.Connect( ip, 1502 ) )
		{	_self.echo( "Connect() : " + _g.session.getLastError() );
			return 0;
		} // end if

		if ( !_g.session.WaitEvent( CSqSocket().EVT_CONNECT, 3000 ) )
		{	_self.echo( "WaitEvent( EVT_CONNECT ) : " + _g.session.getLastError() );
			return 0;
		} // end if

		_g.io[ "connected" ] <- ip;

	} // end if

	else if ( _g.disconnect && _g.io.isset( "connected" ) )
	{	_self.echo( "Disconnecting from : " + _g.io[ "connected" ].str() );
		_g.io.unset( "connected" );
		_g.disconnect = 0;
		_g.session.Destroy();
		for ( local i = 0; i < 8; i++ )
			_g.io[ "inputs" ][ i.tostring() ] <- "",
			_g.io[ "outputs" ][ i.tostring() ] <- "";
	} // end else if

	return 0;
}

//-------------------------------------------------------------------
// Modbus
//-------------------------------------------------------------------

function UpdateModbusServer() : ( _g )
{
	if ( !_g.session.isActivity() )
		return;

	if ( 0 == _g.session_mode )
	{	_g.session_mode++;

		_self.echo( "sending packet" );

		local pkt_size = 8 + 4;
		local pkt = CSqBinary();
		if ( !pkt.Allocate( pkt_size ) )
		{	_self.echo( "Failed to allocate memory for packet" );
			return;
		} // end if

		pkt.Zero();
		pkt.setUsed( pkt_size );

		pkt.BE_setAbsUSHORT( 0, 0 );
		pkt.BE_setAbsUSHORT( 2, 0 );
		pkt.BE_setAbsUSHORT( 4, pkt_size - 6 );
		pkt.setAbsUCHAR( 6, 255 );
		pkt.setAbsUCHAR( 7, 1 );

		// Read coils
		pkt.BE_setAbsUSHORT( 8, 0 );
		pkt.BE_setAbsUSHORT( 10, 8 );

		if ( !_g.session.WriteBin( pkt, 0 ) )
		{	_self.echo( "Write() : " + _g.session.getLastError() );
			_g.disconnect = 1;
		} // end if

	} // end if

	else if ( 1 == _g.session_mode )
	{
		if ( _g.session.WaitEvent( CSqSocket().EVT_READ, 0 ) )
		{	_g.session_mode++;

			local pkt = CSqBinary();

			if ( _g.session.ReadFromBin( pkt, 0 ) )
				ProcessReply( pkt );

		} // end if

	} // end else if

	else if ( 5 < _g.session_mode++ )
		_g.session_mode = 0;

}

function ProcessReply( pkt ) : ( _g )
{
	_self.echo( "Modbus : Data Rx'd : " + pkt.getUsed() );

	_self.echo( pkt.AsciiHexStr( 8, 4 ) );

	local cmd = pkt.getAbsUCHAR( 7 );
	if ( cmd & 0x80 )
	{	_self.echo( "!!! ERROR : " + pkt.getAbsUCHAR( 8 ) );
		return;
	} // end if

	switch ( cmd )
	{
		case 1 :

			local bytes = pkt.getAbsUCHAR( 8 );
			local bits = bytes * 8;

			local pos = 9;
			for ( local c = 0; c < bytes; c++ )
			{
				local data = pkt.getAbsUCHAR( pos++ );

				local mask = 1;
				for( local b = 0; b < 8 && bits; b++, bits-- )
				{	_g.io[ "outputs" ][ b.tostring() ] <- ( ( data & mask ) ? 1 : 0 ).tostring();
					mask = mask << 1;
				} // end for

			} // end for

			break;

		default :
			_self.echo( "!!! Unknown command : " + cmd );
			break;

	} // end switch

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
			if ( mParams[ "GET" ].isset( "connect" ) )
				_g.connect = mParams[ "GET" ][ "connect" ].str();
			if ( mParams[ "GET" ].isset( "disconnect" ) )
				_g.disconnect = 1;
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
	local page = "<table>";

	page += "<tr><td colspan='99'>";
	page += @"
			<table><tr>
				<td><input id='ip' type='text' style='width:100%'></td>
				<td><input type='submit' value='Connect' onclick='doConnect()'></td>
				<td><input type='submit' value='Disconnect' onclick='doDisconnect()'></td>
			</tr></table>
		";
	page += "</td>";

	page += "</tr><tr><td><b>Status</b></td><td colspan='99'><div id='status' align='center' style='background-color:#b0b0b0'></div>";

	page += "</tr><tr><td><b>Inputs</b></td>";
	if ( _g.io.isset( "inputs" ) )
		foreach ( k,v in _g.io[ "inputs" ] )
			div_ids += ( div_ids.len() ? ", " : "" ) + "'i_" + k + "'",
			page += "<td><div id='i_" + k + "' style='" + border + "background-color:#b0b0b0;' align='center' >"
					+ "&nbsp;" + k + "&nbsp;</div></td>";

	page += "</tr><tr><td><b>Outputs</b></td>";
	if ( _g.io.isset( "outputs" ) )
		foreach ( k,v in _g.io[ "outputs" ] )
			div_ids += ( div_ids.len() ? ", " : "" ) + "'o_" + k + "'",
			page += "<td><div id='o_" + k + "' style='" + border + "background-color:#b0b0b0;' align='center' >"
					+ "&nbsp;" + k + "&nbsp;</div></td>";

	page += "</table>";

	page += @"
		<script type='text/javascript'>

			var g_req = '';
			var g_div_ids = Array( " + div_ids + @" );

			function doConnect()
			{	g_req += ( g_req.length ? '&' : '' ) + 'connect=' + $('#ip').attr( 'value' );
			}

			function doDisconnect()
			{	g_req += ( g_req.length ? '&' : '' ) + 'disconnect=1';
			}

			function setState( id, state )
			{	var cols = Array( '#400000', '#ff0000', '#b0b0b0' );
				if ( $('#'+id).css( 'backgroundColor' ) != cols[ parseInt( state ) ] )
					$('#'+id).animate( { backgroundColor: cols[ parseInt( state ) ] }, 500 );
			}

			function data_error()
			{	$('#status').html( '' );
				$('#status').animate( { backgroundColor: '#b0b0b0' }, 500 );
				for ( var i in g_div_ids ) setState( g_div_ids[ i ], 2 );
			}

			function data_success( data )
			{	if ( !data || !data[ 'inputs' ] || !data[ 'outputs' ] ) return data_error();
				if ( data[ 'connected' ] && data[ 'connected' ].length )
				{
					$('#status').animate( { backgroundColor: '#00a000' }, 500 ), $('#status').html( data[ 'connected' ] );
					for( var i in data[ 'inputs' ] ) setState( 'i_' + i, data[ 'inputs' ][ i ] );
					for( var i in data[ 'outputs' ] ) setState( 'o_' + i, data[ 'outputs' ][ i ] );
				} // end if
				else
				{	for ( var i in g_div_ids ) setState( g_div_ids[ i ], 2 );
					$('#status').animate( { backgroundColor: '#a00000' }, 500 ), $('#status').html( 'Disconnected' );
				} // end else

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
