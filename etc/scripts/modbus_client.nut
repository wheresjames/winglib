
class CModbusClient
{
	socket = CSqSocket();
	addr = CSqSockAddress();
	mode = 0;
	io = CSqMulti();
	lio = CSqMulti();
	quit = 0;

	constructor()
	{
		InitIo();
	}

	function InitIo()
	{
		io.clear();
		lio.clear();
		for ( local i = 0; i < 8; i++ )
			io[ "inputs" ][ i.tostring() ] <- "",
			io[ "outputs" ][ i.tostring() ] <- "";
	}

	function Disconnect()
	{
		socket.Destroy();
		mode = 0;
		quit = 0;
		InitIo();
	}

	function Connect( ip )
	{
		Disconnect();

		if ( !socket.Connect( ip, 1502 ) )
		{	::_self.echo( "Connect() : " + socket.getLastError() );
			return 0;
		} // end if

		if ( !socket.WaitEvent( CSqSocket().EVT_CONNECT, 3000 ) )
		{	::_self.echo( "WaitEvent( EVT_CONNECT ) : " + socket.getLastError() );
			return 0;
		} // end if

		io[ "connected" ] <- ip;

		return 1;
	}

	function isConnected() { return io.isset( "connected" ); }

	function getPeerAddress() { return io.isset( "connected" ) ? io[ "connected" ].str() : ""; }

	function Update()
	{
		if ( !socket.isActivity() )
			return;

		if ( quit )
		{	Destroy();
			return;
		} // end if

		if ( 0 == mode )
		{	mode++;

			::_self.echo( "sending packet" );

			local pkt_size = 8 + 4;
			local pkt = CSqBinary();
			if ( !pkt.Allocate( pkt_size ) )
			{	::_self.echo( "Failed to allocate memory for packet" );
				return;
			} // end if

			pkt.Zero();
			pkt.setUsed( pkt_size );

			pkt.BE_setAbsUSHORT( 0, 0 );
			pkt.BE_setAbsUSHORT( 2, 0 );
			pkt.BE_setAbsUSHORT( 4, pkt_size - 6 );
			pkt.BE_setAbsUCHAR( 6, 255 );
			pkt.BE_setAbsUCHAR( 7, 1 );

			// Read coils
			pkt.BE_setAbsUSHORT( 8, 0 );
			pkt.BE_setAbsUSHORT( 10, 8 );

			if ( !socket.WriteBin( pkt, 0 ) )
			{	::_self.echo( "Write() : " + socket.getLastError() );
				quit = 1;
			} // end if

		} // end if

		else if ( 1 == mode )
		{
			if ( socket.WaitEvent( CSqSocket().EVT_READ, 0 ) )
			{	mode++;

				local pkt = CSqBinary();

				if ( socket.ReadFromBin( pkt, 0 ) )
					ProcessReply( pkt );

			} // end if

		} // end else if

		else //if ( 5 < _g.session_mode++ )
			mode = 0;
	}

	function ProcessReply( pkt )
	{
		::_self.echo( "Modbus : Data Rx'd : " + pkt.getUsed() );

		::_self.echo( pkt.AsciiHexStr( 8, 4 ) );

		local cmd = pkt.getAbsUCHAR( 7 );
		if ( cmd & 0x80 )
		{	::_self.echo( "!!! ERROR : " + pkt.getAbsUCHAR( 8 ) );
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
					{	io[ "outputs" ][ b.tostring() ] <- ( ( data & mask ) ? 1 : 0 ).tostring();
						mask = mask << 1;
					} // end for

				} // end for

				break;

			default :
				::_self.echo( "!!! Unknown command : " + cmd );
				break;

		} // end switch

	}

};

class CGlobal
{
	quit = 1;
	http = CSqHttpServer();

	connect = "";
	disconnect = 0;

	modbus = CModbusClient();

};

local _g = CGlobal();

function _init() : ( _g )
{
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

		if ( !_g.modbus.Connect( ip ) )
		{	_self.echo( "Connect() failed "  );
			return 0;
		} // end if


	} // end if

	else if ( _g.disconnect && _g.modbus.isConnected() )
	{
		_self.echo( "Disconnecting from : " + _g.modbus.getPeerAddress() );
		_g.disconnect = 0;
		_g.modbus.Disconnect();
	} // end else if

	return 0;
}

//-------------------------------------------------------------------
// Modbus
//-------------------------------------------------------------------

function UpdateModbusServer() : ( _g )
{
	_g.modbus.Update();
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

			if ( mParams[ "GET" ].isset( "toggle_output" ) )
			{	local n = mParams[ "GET" ][ "toggle_output" ].str();
				if ( _g.modbus.io[ "outputs" ].isset( n ) )
					_g.modbus.io[ "outputs" ][ n ] <- ( _g.modbus.io[ "outputs" ][ n ].toint() ? "0" : "1" );
			} // end if

			raw = _g.modbus.io.getJSON();

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

	if ( _g.modbus.io.isset( "inputs" ) )
		foreach ( k,v in _g.modbus.io[ "inputs" ] )
			div_ids += ( div_ids.len() ? ", " : "" ) + "'i_" + k + "'",
			page += "<td><div id='i_" + k + "' style='" + border + "background-color:#b0b0b0;' align='center' >"
					+ "&nbsp;" + k + "&nbsp;</div></td>";

	page += "</tr><tr><td><b>Outputs</b></td>";
	if ( _g.modbus.io.isset( "outputs" ) )
		foreach ( k,v in _g.modbus.io[ "outputs" ] )
			div_ids += ( div_ids.len() ? ", " : "" ) + "'o_" + k + "'",
			page += "<td><div id='o_" + k + "' style='" + border + "background-color:#b0b0b0;' align='center' onclick='toggleOutput( " + k + " )' >"
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

			function toggleOutput( n )
			{	g_req += ( g_req.length ? '&' : '' ) + 'toggle_output=' + n;
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
			{	if ( !data ) return data_error();
				if ( data[ 'connected' ] && data[ 'connected' ].length )
				{
					$('#status').animate( { backgroundColor: '#00a000' }, 500 ), $('#status').html( data[ 'connected' ] );
					if ( data[ 'inputs' ] ) for( var i in data[ 'inputs' ] ) setState( 'i_' + i, data[ 'inputs' ][ i ] );
					if ( data[ 'outputs' ] ) for( var i in data[ 'outputs' ] ) setState( 'o_' + i, data[ 'outputs' ][ i ] );
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

