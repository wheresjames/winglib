
class CModbusClient
{
	socket = CSqSocket();
	addr = CSqSockAddress();
	mode = 0;
	io = CSqMulti();
	lio = CSqMulti();
	quit = 0;
	trans = 0;
	reply = 0;
	dev = 5;
	pto = 3;
	inputs = 8;
	outputs = 8;
	analogs = 2;

	constructor()
	{
		InitIo();
	}

	function InitIo()
	{
		io.clear();
		lio.clear();
		for ( local i = 0; i < inputs; i++ )
			io[ "inputs" ][ i.tostring() ] <- "";

		for ( local i = 0; i < outputs; i++ )
			io[ "outputs" ][ i.tostring() ] <- "";

		for ( local i = 0; i < analogs; i++ )
			io[ "analogs" ][ i.tostring() ] <- "";
	}

	function getInput( i )
	{	return !io[ "inputs" ].isset( i.tostring() ) 
				? 0 : io[ "inputs" ][ i.tostring() ].toint();
	}

	function getOutput( i )
	{	return !io[ "outputs" ].isset( i.tostring() ) 
				? 0 : io[ "outputs" ][ i.tostring() ].toint();
	}

	function setOutput( i, v )
	{	io[ "soutputs" ][ i.tostring() ] <- v.tostring();
	}

	function getAnalog( i )
	{	return !io[ "analogs" ].isset( i.tostring() ) 
				? 0 : io[ "analogs" ][ i.tostring() ].toint();
	}

	function RefreshOutput( b, v )
	{	io[ "outputs" ][ b.tostring() ] <- ( v ? 1 : 0 ).tostring();
		if ( io[ "soutputs" ].isset( b.tostring() )
			     && io[ "outputs" ][ b.tostring() ].str() == io[ "soutputs" ][ b.tostring() ].str() )
				io[ "soutputs" ].unset( b.tostring() );
	}

	function Disconnect()
	{
		socket.Destroy();
		mode = 0;
		quit = 0;
		reply = 0;
		InitIo();
	}

	function Connect( ip )
	{
		Disconnect();

		if ( !socket.Connect( ip, 502 ) )
		{	::_self.echo( "Connect() : " + ip + " : " + socket.getLastError() );
			quit = 1;
			return 0;
		} // end if

		if ( !socket.WaitEvent( CSqSocket().EVT_CONNECT, 3000 ) )
		{	::_self.echo( "WaitEvent( EVT_CONNECT ) : " + ip + " : " + socket.getLastError() );
			quit = 1;
			return 0;
		} // end if

		if ( socket.getLastErrorValue() )
		{	::_self.echo( "WaitEvent( EVT_CONNECT ) : " + ip + " : " + socket.getLastError() );
			quit = 1;
			return 0;
		} // end if

		::_self.echo( "Connected to : " + ip );
		io[ "connected" ] <- ip;

		return 1;
	}

	function isConnected() { return io.isset( "connected" ); }

	function getPeerAddress() { return io.isset( "connected" ) ? io[ "connected" ].str() : ""; }

	function SendPacket( cmd, pkt )
	{
		local tx_size = 8;
		local tx = CSqBinary();
		if ( !tx.Allocate( tx_size ) )
		{	::_self.echo( "Failed to allocate memory for packet" );
			return 0;
		} // end if

		tx.Zero();
		tx.setUsed( tx_size );

		tx.BE_setAbsUSHORT( 0, trans++ );
		tx.BE_setAbsUSHORT( 2, 0 );
		tx.BE_setAbsUSHORT( 4, tx_size + pkt.getUsed() - 6 );
		tx.BE_setAbsUCHAR( 6, dev );
		tx.BE_setAbsUCHAR( 7, cmd );

		tx.Append( pkt );

//		::_self.echo( tx.AsciiHexStr( 8, 4 ) );
		if ( !socket.WriteBin( tx, 0 ) )
		{	::_self.echo( "Write() : " + socket.getLastError() );
			quit = 1;
			return 0;
		} // end if

		reply = ::_self.gmt_time() + pto;

		return 1;
	}

	function Update()
	{
		if ( !socket.isActivity() )
			return;

		if ( quit )
		{	Disconnect();
			return;
		} // end if

		if ( reply >= ::_self.gmt_time() )
		{
			if ( !socket.WaitEvent( CSqSocket().EVT_READ, 0 ) )
				return;

			reply = 0;
			local pkt = CSqBinary();
			if ( socket.ReadFromBin( pkt, 0 ) )
				ProcessReply( pkt );

		} // end if

		switch( mode )
		{
			case 0 :
//				::_self.echo( "Updating outputs" );
				if ( !UpdateOutputs() )
					mode++;
				break;

			case 1 :
				ReadOutputs();
				mode++;
				break;

			case 2 :
				ReadInputs();
				mode++;
				break;

			case 3 :
				if ( analogs )
				{	ReadAnalogs();
					mode++;
				} else mode = 0;
				break;

			default :
				mode = 0;
				break;

		} // end switch

	}

	function UpdateOutputs()
	{	if ( io.isset( "soutputs" ) )
			foreach ( k,v in io[ "soutputs" ] )
				if ( v.str() != io[ "outputs" ][ k ].str() )
				{	io[ "outputs" ][ k ] <- v.str();
					local pkt = CSqBinary( 4, 4 );
					pkt.BE_setAbsUSHORT( 0, k.tointeger() );
					pkt.BE_setAbsUSHORT( 2, ( v.toint() ? 0xff00 : 0 ) );
					return SendPacket( 5, pkt );
				} // end if
		return 0;
	}

	function ReadOutputs()
	{	local pkt = CSqBinary( 4, 4 );
		pkt.BE_setAbsUSHORT( 0, 0 );
		pkt.BE_setAbsUSHORT( 2, outputs );
		return SendPacket( 1, pkt );
	}

	function ReadInputs()
	{	local pkt = CSqBinary( 4, 4 );
		pkt.BE_setAbsUSHORT( 0, 0 );
		pkt.BE_setAbsUSHORT( 2, inputs );
		return SendPacket( 2, pkt );
	}

	function ReadAnalogs()
	{	local pkt = CSqBinary( 4, 4 );
		pkt.BE_setAbsUSHORT( 0, 0 );
		pkt.BE_setAbsUSHORT( 2, analogs );
		return SendPacket( 4, pkt );
	}

	function ProcessReply( pkt )
	{
//		::_self.echo( "Modbus : Data Rx'd : " + pkt.getUsed() );
//		::_self.echo( pkt.AsciiHexStr( 8, 4 ) );

		local cmd = pkt.getAbsUCHAR( 7 );
		if ( cmd & 0x80 )
		{	::_self.echo( "!!! ERROR : " + pkt.getAbsUCHAR( 8 ) );
			::_self.echo( pkt.AsciiHexStr( 8, 4 ) );
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
					{	RefreshOutput( b, data & mask );
						mask = mask << 1;
					} // end for

				} // end for

				break;

			case 2 :

				local bytes = pkt.getAbsUCHAR( 8 );
				local bits = bytes * 8;

				local pos = 9;
				for ( local c = 0; c < bytes; c++ )
				{
					local data = pkt.getAbsUCHAR( pos++ );

					local mask = 1;
					for( local b = 0; b < 8 && bits; b++, bits-- )
					{	io[ "inputs" ][ b.tostring() ] <- ( ( data & mask ) ? 1 : 0 ).tostring();
						mask = mask << 1;
					} // end for

				} // end for

				break;

			case 4 :

				local bytes = pkt.getAbsUCHAR( 8 ) / 2;
				local pos = 9;
				for ( local b = 0; b < bytes; b++ )
				{	local data = pkt.getAbsUSHORT( pos++ ); pos++;
					io[ "analogs" ][ b.tostring() ] <- data.tostring();
				} // end for

				break;

			case 5 :
				RefreshOutput( pkt.getAbsUSHORT( 8 ), pkt.getAbsUSHORT( 10 ) );
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

	_self.set_timer( ".", 250, "UpdateModbusServer" );

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
				if ( _g.modbus.io[ "soutputs" ].isset( n ) )
					_g.modbus.io[ "soutputs" ][ n ] <- ( _g.modbus.io[ "soutputs" ][ n ].toint() ? "0" : "1" );
				else if ( _g.modbus.io[ "outputs" ].isset( n ) )
					_g.modbus.io[ "soutputs" ][ n ] <- ( _g.modbus.io[ "outputs" ][ n ].toint() ? "0" : "1" );
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

	local div_ids = "", div_as = "";
	local page = "<table width='400'>";

	page += "<tr><td colspan='99'>";
	page += @"
			<table width='100%'><tr>
				<td><input id='ip' type='text' style='width:100%'></td>
				<td><input type='submit' value='Connect' onclick='doConnect()'></td>
				<td><input type='submit' value='Disconnect' onclick='doDisconnect()'></td>
			</tr></table>
		";
	page += "</td></tr>";

	page += "<tr><td><b>Status</b></td><td colspan='99'><div id='status' align='center' style='background-color:#b0b0b0'></div></td></tr>";

	page += "<tr><td><b>Inputs</b></td>";

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

	page += "</tr><tr><td><b>Analogs</b></td><td colspan='99' align='left'>";
	if ( _g.modbus.io.isset( "analogs" ) )
		foreach ( k,v in _g.modbus.io[ "analogs" ] )
			div_as += ( div_as.len() ? ", " : "" ) + "'a_" + k + "'",
			page += "&nbsp;" + k + "&nbsp;<input id='a_" + k + "' type='input' style='width:40%;" + border + "background-color:#b0b0b0;' align='left' >";

	page += "</td></tr></table>";

	page += @"
		<script type='text/javascript'>

			var g_req = '';
			var g_div_ids = Array( " + div_ids + @" );
			var g_div_as = Array( " + div_as + @" );

			function doConnect()
			{	g_req += ( g_req.length ? '&' : '' ) + 'connect=' + $('#ip').attr( 'value' );
			}

			function doDisconnect()
			{	g_req += ( g_req.length ? '&' : '' ) + 'disconnect=1';
			}

			function toggleOutput( n )
			{	g_req += ( g_req.length ? '&' : '' ) + 'toggle_output=' + n;
			}

			function setState( id, state, setstate )
			{	var cols = Array( '#400000', '#ff0000', '#b0b0b0' );
				if ( setstate ) cols = Array( '#404000', '#ffff00', '#b0b0b0' ), state = setstate;
				if ( $('#'+id).css( 'backgroundColor' ) != cols[ parseInt( state ) ] )
					$('#'+id).css( 'backgroundColor', cols[ parseInt( state ) ] );
					// $('#'+id).animate( { backgroundColor: cols[ parseInt( state ) ] }, 500 );
			}

			function setAnalog( id, value )
			{	var cols = Array( '#b0b0b0', '#00ff00' );
				if ( !value )
					$('#'+id).attr( 'value', '' ),
					$('#'+id).css( 'backgroundColor', cols[ 0 ] );
				else
					$('#'+id).attr( 'value', value ),
					$('#'+id).css( 'backgroundColor', cols[ 1 ] );
			}

			function data_error()
			{	$('#status').html( '' );
				$('#status').css( 'backgroundColor', '#b0b0b0' );
				for ( var i in g_div_ids ) setState( g_div_ids[ i ], 2 );
				for ( var i in g_div_as ) setAnalog( g_div_as[ i ], undefined );
			}

			function data_success( data )
			{	if ( !data ) return data_error();
				if ( data[ 'connected' ] && data[ 'connected' ].length )
				{	$('#status').animate( { backgroundColor: '#00a000' }, 500 ), $('#status').html( data[ 'connected' ] );
					if ( data[ 'inputs' ] ) for( var i in data[ 'inputs' ] ) setState( 'i_' + i, data[ 'inputs' ][ i ], 0 );
					if ( data[ 'outputs' ] ) for( var i in data[ 'outputs' ] ) setState( 'o_' + i, data[ 'outputs' ][ i ], ( data[ 'soutputs' ] ) ? data[ 'soutputs' ][ i ] : 0 );
					if ( data[ 'analogs' ] ) for( var i in data[ 'analogs' ] ) setAnalog( 'a_' + i, data[ 'analogs' ][ i ] );
				} // end if
				else
				{	for ( var i in g_div_ids ) setState( g_div_ids[ i ], 2 );
					for ( var i in g_div_as ) setAnalog( g_div_as[ i ], undefined );
					$('#status').animate( { backgroundColor: '#a00000' }, 500 ), $('#status').html( 'Disconnected' );
				} // end else

			}
			function poll()
			{	$.ajax( { url: '/data', dataType: 'json', success: data_success, error: data_error, data: g_req })
				g_req = '';
			}

			setInterval( poll, 500 );

		</script>
	";

	return page;
}

