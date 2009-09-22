
_self.load_module( "http", "" );
_self.load_module( "cell", "" );

class CGlobal
{
	quit = 0;
	server = 0;
	tc = CCellConnection();
	tmpl_cfg = "tmpl.cfg";
};

local _g = CGlobal();

function OnServerEvent( server, data )
{

}

function parse_tag( name )
{
	local parts = split( name, "." );
	if ( 1 in parts && parts[ 1 ].len() )
		return parts[ 0 ];

	parts = split( name, ":" );
	if ( 1 in parts && parts[ 1 ].len() )
		return parts[ 0 ];

	return name;
}

function pg_data( mParams ) : ( _g )
{
	local mReply = CSqMap();

	if ( mParams[ "GET" ].isset( "quit" ) )
	{	_g.quit = 1;
		mReply.set( "content", "Quitting" );
		return mReply.serialize();
	} // end if

	if ( mParams[ "GET" ].isset( "close" ) )
		_g.tc.Destroy();

	if ( !_g.tc.IsConnected() && !mParams[ "GET" ][ "ip" ].len() )
	{	mReply.set( "content", "Not connected and no ip specified" );
		return mReply.serialize();
	} // end if

	if ( !_g.tc.IsConnected() || 
		 ( mParams[ "GET" ][ "ip" ].len() && mParams[ "GET" ][ "ip" ].str() != _g.tc.GetIp() ) )
		if ( !_g.tc.Connect( mParams[ "GET" ][ "ip" ].str(), 1 ) )
		{	mReply.set( "content", "Unable to connect to " + mParams[ "GET" ][ "ip" ].str() );
			return mReply.serialize();
		} // end if

	if ( !_g.tc.IsConnected() )
		mReply.set( "content", "Not connected" );

	else
	{
		if ( mParams[ "GET" ][ "tag" ].len() )
		{
			local tag_name = parse_tag( mParams[ "GET" ][ "tag" ].str() );
			if ( !_g.tc.tags()[ tag_name ] )
				mReply.set( "content", "Base tag not found " + tag_name );
			else
				mReply.set( "content", _g.tc.ReadTag( mParams[ "GET" ][ "tag" ].str() ).serialize() );

		} // end if

		else if ( mParams[ "GET" ].isset( "all" ) )
		{
			local s = "<table border=1><tr><td><b>Tag</b></td><td><b>Bytes</b></td><td><b>Type</b></td><td><b>Dim</b></td><td><b>Struct</b><td><b>a1_size</b></td><td><b>Value</b></td></tr>";
			local tags = _g.tc.tags();

			local i = 0;
			foreach( k,v in tags )
			{
				local rep = _g.tc.ReadTag( k );

				if ( ++i & 1 )
					s += "<tr valign=top>";
				else
					s += "<tr valign=top bgcolor=#d0e0ff>";
				s += "<td><small>" + k + "</small></td>";

				if ( rep[ "err" ] )
					s += "<td colspan=99>" + rep[ "err" ] + "</small></td>";
				else
				{
					s += "<td><small>" + rep[ "bytes" ] + "</small></td>";
					s += "<td><small>" + format( "0x%04x:", rep[ "type" ].tointeger() ) + rep[ "type_name" ] + "</small></td>";
					s += "<td><small>" + rep[ "dim" ] + "</small></td>";
					s += "<td><small>" + rep[ "struct" ] + "</small></td>";
					s += "<td><small>" + rep[ "a1_size" ] + "</small></td>";
					if ( rep[ "value" ] )
						s += "<td><small>" + rep[ "value" ] + "</small></td>";
					else
						s += "<td><small>[n/a]</small></td>";

				} // end else

				s += "</tr>";

			} // end foreach

			s += "</table>";

			mReply.set( "content", s );

		} // end if

	} // end else

	return mReply.serialize();
}

function show_tag( name ) : ( _g )
{
	if ( !_g.tc.IsConnected() )
		return "[ Not Connected ]";

	local base_name = parse_tag( name );

	if ( !_g.tc.tags()[ base_name ] )
		return "Base tag not found : " + base_name;

	local tag = _g.tc.ReadTag( name );
	if ( tag[ "err" ] )
		return tag[ "err" ];


	local content = @"
				<table>
					<tr valign='top'>
						<td colspan='99'>
							<nobr><b>" + name + @"</b></nobr>
						</td>
					<tr>
			";

	foreach( k,v in tag )
	{
		content += @"
						<tr valign='top'>
							<td bgcolor='#f0f0f0'>
								" + k + @"
							</td>
							<td bgcolor='#ffffff'>
								" + v + @"
							</td>
						</tr>
			";
	} // end foreach

	content += @"
				</table>
		";

	return content;
}

function edit_template( mParams ) : ( _g )
{
	// Cut off template stuff
	local full_tag = name;
	name = parse_tag( name );

	switch ( mParams[ "POST" ][ "etmp" ].str() )
	{
		case "add" :

			if ( mParams[ "POST" ][ "etmp_name" ].len() )
			{
				local i = _g.tc.tmpl().get( name ).size();
				local tmpl = _g.tc.tmpl().get( name ).get( i.tostring() );
				tmpl[ "name" ].set( mParams[ "POST" ][ "etmp_name" ].str() );
				tmpl[ "type" ].set( mParams[ "POST" ][ "etmp_type" ].str() );
				if ( mParams[ "POST" ][ "etmp_type" ].str() == "USER" )
					tmpl[ "bytes"].set( mParams[ "POST" ][ "etmp_bytes" ].str() );

				_g.tc.VerifyTemplate();
				CSqFile().put_contents( _g.tmpl_cfg, _g.tc.tmpl().serialize() );

			} // end if

			break;
	};

	switch ( mParams[ "GET" ][ "etmp" ].str() )
	{
		case "del" :
			local tmpl = _g.tc.tmpl().get( name ).unset( mParams[ "GET" ][ "etmp_idx" ].str() );
			_g.tc.VerifyTemplate();
			CSqFile().put_contents( _g.tmpl_cfg, _g.tc.tmpl().serialize() );
			break;

		case "up" :
			_g.tc.tmpl().get( name ).move_up( mParams[ "GET" ][ "etmp_idx" ].str() );
			_g.tc.VerifyTemplate();
			CSqFile().put_contents( _g.tmpl_cfg, _g.tc.tmpl().serialize() );
			break;

		case "down" :
			_g.tc.tmpl().get( name ).move_down( mParams[ "GET" ][ "etmp_idx" ].str() );
			_g.tc.VerifyTemplate();
			CSqFile().put_contents( _g.tmpl_cfg, _g.tc.tmpl().serialize() );
			break;

	} // end if

	local content = @"
		<form action='admin' method='post'>
		<input type='hidden' name='etmp' id='etmp' value='add' >
		<input type='hidden' name='tag' id='tag' value='" + name + @"' >
		<table>
			<tr>
				<td colspan='99'>
					<b>Template for " + name + @"</b>
				</td>
			</tr>
			<tr>
				<td>
				</td>
				<td>
					<b><small>Name</small></b>
				</td>
				<td>
					<b><small>Type</small></b>
				</td>
				<td>
					<b><small>Bytes</small></b>
				</td>
				<td>
					<b><small>Offset</small></b>
				</td>
				<td>
					<b><small>Value</small></b>
				</td>
			</tr>
		";


	if ( _g.tc.tmpl().isset( name ) )
	{
		local tmpl = _g.tc.tmpl().get( name );
		foreach( k,v in tmpl  )
		{
			content += @"
				<tr>
					<td>
						<a href='admin?etmp=up&etmp_idx=" + k + @"&tag=" + full_tag + @"'>[<b><small>up</small></b>]</a>
						<a href='admin?etmp=down&etmp_idx=" + k + @"&tag=" + full_tag + @"'>[<b><small>down</small></b>]</a>
						<a href='admin?etmp=del&etmp_idx=" + k + @"&tag=" + full_tag + @"'>[<b><small>delete</small></b>]</a>
					</td>
					<td bgcolor='#ffffff'>
						" + v[ "name" ].str() + @"
					</td>
					<td bgcolor='#ffffff'>
						" + v[ "type" ].str() + @"
					</td>
					<td bgcolor='#ffffff'>
						" + v[ "bytes" ].str() + @"
					</td>
					<td bgcolor='#ffffff'>
						" + v[ "offset" ].str() + @"
					</td>
					<td bgcolor='#ffffff'>
					</td>
				</tr>
				";

		} // end foreach

	} // end if

	local jscript_enable = "document.getElementsByName( \"etmp_bytes\" )[ 0 ].disabled = ( this.value != \"USER\" );";

	content += @"
			<tr>
				<td colspan='99'>
					<hr>
				</td>
			</tr>
			<tr>
				<td>
				</td>
				<td>
					<input name='etmp_name' id='etmp_name' type='text'>
				</td>
				<td>
					<select name='etmp_type' id='etmp_type' onchange='" + jscript_enable + @"'>
						<option >USER</option>
						<option >BOOL</option>
						<option >SINT</option>
						<option >INT</option>
						<option >DINT</option>
						<option >LINT</option>
						<option >USINT</option>
						<option >UINT</option>
						<option >UDINT</option>
						<option >ULINT</option>
						<option >REAL</option>
						<option >LREAL</option>
					</select>
				</td>
				<td>
					<input name='etmp_bytes' id='etmp_bytes' type='text' style='width:40'>
				</td>
				<td colspan='2'>
					<input type='submit' value='Add'>
				</td>
			</tr>
		</form>
		</table>
		";

	return content;
}

function pg_admin( mParams ) : ( _g )
{
	local tag = "";
	if ( mParams[ "POST" ][ "tag" ].len() )
		tag = mParams[ "POST" ][ "tag" ].str();
	else if ( mParams[ "GET" ][ "tag" ].len() )
		tag = mParams[ "GET" ][ "tag" ].str();

	switch ( mParams[ "POST" ][ "do" ].str() )
	{
		case "connect" :
			_g.tc.Connect( mParams[ "POST" ][ "server" ].str(), 1 );

	} // end if

	local content = @"
<html>
	<body bgcolor='#c0c0c0'>
		<table>
			<tr>
				<td>
					<b>Status : </b>
				</td>
				<td>
					" + ( _g.tc.IsConnected() ? ( "Connected To " +  _g.tc.GetIp() ) : "Not Connected" ) + @"
				</td>
			</tr>
		</table>

		<form action='admin' method='post'>
			<input type='hidden' name='do' id='do' value='connect' >
			<table>
				<tr>
					<td>
						<b>Server : </b>
					</td>
					<td>
						<input name='server' id='server' type='text'>
					</td>
					<td>
						<input type='submit' value='Connect'>
					</td>
				</tr>
			</table>
		</form>

		<form action='admin' method='post'>
			<input type='hidden' name='do' id='do' value='read_tag' >
			<table>
				<tr>
					<td>
						<b>Tag : </b>
					</td>
					<td>
					<input name='tag' id='tag' type='text' value='" + tag + @"'>
					</td>
					<td>
						<input type='submit' value='Read Tag'>
					</td>
				</tr>
			</table>
		</form>
	";

	if ( tag.len() )
	{
		content += @"
			<table>
				<tr valign='top'>
					<td>
						" + edit_template( tag, request, headers, get, post ) + @"
					</td>
					<td style='border-right:2px inset black'>
						&nbsp;
					</td>
					<td>
						" + show_tag( tag ) + @"
					</td>
				</tr>
			</table>
			";

	} // end if

	content += @"
	</body>
</html>
		";

	local mReply = CSqMap();
	mReply.set( "content", content );
	return mReply.serialize();
}

function OnProcessRequest( params ) : ( _g )
{
	local mParams = CSqMulti();
	mParams.deserialize( params );
	_self.echo( mParams[ "REQUEST" ][ "REMOTE_ADDR" ].str() + " : " + mParams[ "REQUEST" ][ "REQUEST_STRING" ].str() );

	switch ( mParams[ "REQUEST" ][ "path" ].str() )
	{
		case "/data" :
			return pg_data( mParams );

		case "/admin" :
			return pg_admin( mParams );

	} // end if

	local mReply = CSqMap();
	mReply.set( "content", "Access Denied" );
	return mReply.serialize();
}

function _init() : ( _g )
{
	// Load the data templates
	_g.tc.tmpl().deserialize( CSqFile().get_contents( _g.tmpl_cfg ) );
	_g.tc.VerifyTemplate();

	_g.server = CHttpServer();

	_g.server.SetSessionCallback( _self.queue(), "OnProcessRequest" );

	if ( !_g.server.Start( 1234 ) )
		_self.alert( "Unable to start http server" );
}

function _idle() : ( _g )
{
	return _g.quit;
}

