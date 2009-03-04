
_self.load_module( "http", "" );
_self.load_module( "cell", "" );

class CGlobal
{
	quit = 0;
	server = 0;
	tc = CCellConnection();
	tmpl = {};
};

local _g = CGlobal();

function OnServerEvent( server, data )
{

}

function pg_data( request, headers, get, post ) : ( _g )
{
	local mGet = CSqMap();
	mGet.deserialize( get );

	local mReply = CSqMap();

	if ( mGet[ "quit" ] )
	{	_g.quit = 1;
		mReply.set( "content", "Quitting" );
		return mReply.serialize();
	} // end if

	if ( mGet[ "close" ] )
		_g.tc.Destroy();

	if ( !_g.tc.IsConnected() && !mGet[ "ip" ] )
	{	mReply.set( "content", "Not connected and no ip specified" );
		return mReply.serialize();
	} // end if

	if ( !_g.tc.IsConnected() || ( mGet[ "ip" ] && mGet[ "ip" ] != _g.tc.GetIp() ) )
		if ( !_g.tc.Connect( mGet[ "ip" ], 1 ) )
		{	mReply.set( "content", "Unable to connect to " + mGet[ "ip" ] );
			return mReply.serialize();
		} // end if

	if ( !_g.tc.IsConnected() )
		mReply.set( "content", "Not connected" );

	else
	{
		if ( mGet[ "tag" ] )
		{
			if ( !_g.tc.tags()[ mGet[ "tag" ] ] )
				mReply.set( "content", "Tag not found " + mGet[ "tag" ] );
			else
				mReply.set( "content", _g.tc.ReadTag( mGet[ "tag" ] ).serialize() );

		} // end if

		else if ( mGet[ "all" ] )
		{
			local s = "<table border=1><tr><td><b>Tag</b></td><td><b>Items</b></td><td><b>Type</b></td><td><b>Dim</b></td><td><b>Struct</b><td><b>a1_size</b></td><td><b>Value</b></td></tr>";
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
					s += "<td><small>" + rep[ "items" ] + "</small></td>";
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
		return "Can't ready tag, not connected!!!";

	if ( !_g.tc.tags()[ name ] )
		return "Tag not found : " + name;

	local tag = _g.tc.ReadTag( name );
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

function edit_template( name, request, headers, get, post ) : ( _g )
{
	local mPost = CSqMap();
	mPost.deserialize( post );

	switch ( mPost[ "etmp" ] )
	{
		case "add" :
			local i = _g.tmpl.len();
			_g.tmpl[ i ] <-
				{
					[ "name" ] = mPost[ "etmp_name" ],
					[ "type" ] = mPost[ "etmp_type" ],
					[ "size" ] = mPost[ "etmp_size" ]
				};
			break;

	} // end if

	local content = @"
		<form action='admin' method='post'>
		<input type='hidden' name='etmp' id='etmp' value='add' >
		<input type='hidden' name='do' id='do' value='read_tag' >
		<input type='hidden' name='tag' id='tag' value='" + name + @"' >
		<table>
			<tr>
				<td colspan='99'>
					<b>Template for " + name + @"</b>
				</td>
			</tr>
			<tr>
				<td>
					<b><small>Name</small></b>
				</td>
				<td>
					<b><small>Type</small></b>
				</td>
				<td>
					<b><small>Size</small></b>
				</td>
				<td>
					<b><small>Value</small></b>
				</td>
			</tr>
		";

	foreach( k,v in _g.tmpl )
	{
		content += @"
			<tr>
				<td bgcolor='#ffffff'>
					" + v[ "name" ] + @"
				</td>
				<td bgcolor='#ffffff'>
					" + v[ "type" ] + @"
				</td>
				<td bgcolor='#ffffff'>
					" + v[ "size" ] + @"
				</td>
				<td bgcolor='#ffffff'>
				</td>
			</tr>
			";
	} // end foreach

	content += @"
			<tr>
				<td colspan='99'>
					<hr>
				</td>
			</tr>
			<tr>
				<td>
					<input name='etmp_name' id='etmp_name' type='text'>
				</td>
				<td>
					<input name='etmp_type' id='etmp_type' type='text'>
				</td>
				<td>
					<input name='etmp_size' id='etmp_size' type='text'>
				</td>
				<td>
					<input type='submit' value='Add'>
				</td>
			</tr>
		</form>
		</table>
		";

	content += @"
		</table>
		";

	return content;
}

function pg_admin( request, headers, get, post ) : ( _g )
{
	local mGet = CSqMap();
	mGet.deserialize( get );

	local mPost = CSqMap();
	mPost.deserialize( post );

	switch ( mPost[ "do" ] )
	{
		case "connect" :
			_g.tc.Connect( mPost[ "server" ], 1 );

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
					<input name='tag' id='tag' type='text' value='" + ( mPost[ "tag" ] ? mPost[ "tag" ] : "" ) + @"'>
					</td>
					<td>
						<input type='submit' value='Read Tag'>
					</td>
				</tr>
			</table>
		</form>
	";

	if ( mPost[ "do" ] && mPost[ "do" ] == "read_tag" && mPost[ "tag" ] )
	{
		content += @"
			<table>
				<tr valign='top'>
					<td>
						" + show_tag( mPost[ "tag" ] ) + @"
					</td>
					<td>
						" + edit_template( mPost[ "tag" ], request, headers, get, post ) + @"
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

function OnProcessRequest( request, headers, get, post ) : ( _g )
{
	local mRequest = CSqMap();
	mRequest.deserialize( request );

	switch ( mRequest[ "path" ] )
	{
		case "/data" :
			return pg_data( request, headers, get, post );

		case "/admin" :
			return pg_admin( request, headers, get, post );

		case "/test" :

			local m = CSqMulti();

//			m[ "hello" ] <- "world";

			m.get( "a" ).value().set( "1" );

			m.get( "b" ).get( "ba" ).value().set( "1" );

			m.get( "c" ).get( "ca" ).get( "caa" ).value().set( "1" );

			local mReply = CSqMap();
			mReply.set( "content", m.print_r( 1 ) );
			return mReply.serialize();
			break

	} // end if

	local mReply = CSqMap();
	mReply.set( "content", "Access Denied" );
	return mReply.serialize();
}

function _init() : ( _g )
{
	_g.server = CHttpServer();

	_g.server.SetCallback( _self.queue(), "OnServerEvent", "OnProcessRequest" );

	if ( !_g.server.Start( 1234 ) )
		_self.alert( "Unable to start http server" );
}

function _idle() : ( _g )
{
	return _g.quit;
}

