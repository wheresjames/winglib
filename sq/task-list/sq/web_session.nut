
_self.include( "config.nut" );

function CreateCookie( params )
{
	local c = CSqMap();
	c[ "SID" ] <- params;

	local cookie = c.serialize() + "; path=/";
	return cookie;
}

function DecodeCookie( cookie )
{
	local c = CSqMap();
	local l = split( cookie, "; " );

	foreach( i in l )
	{	c.deserialize( i );
		if ( c.isset( "SID" ) )
			return c[ "SID" ];
	} // end foreach
	return "";
}

function RestoreSession( mHeaders, mReply )
{
	local id = "";
	local mSession = CSqMap();
	if ( mHeaders.isset( "Cookie" ) )
	{
		id = DecodeCookie( mHeaders[ "Cookie" ] );

		if ( id.len() )
			mSession.deserialize( _self.get( "/web_server", id ) );

	} // end if

	// Validate the entry
	if ( !id.len() || !mSession.isset( "id" ) || mSession[ "id" ] != id )
	{
		id = _self.unique();
		mSession[ "id" ] <- id;
		local mRHeaders = CSqMap();
		mRHeaders.set( "Set-Cookie", CreateCookie( mSession[ "id" ] ) );
		mReply.set( "headers", mRHeaders.serialize() );

	} // end if

	return mSession;
}

function DoLogin( mGet, mPost, mSession )
{
	if ( mGet.isset( "login" ) )
		mSession[ "user" ] <- "1";
	else if ( mGet.isset( "logout" ) )
		mSession.clear();

	return mSession.isset( "user" );
}

function OnProcessRequest( request, headers, get, post )
{
	_self.kill( "." );

	local mRequest = CSqMap();
	mRequest.deserialize( request );

	local mHeaders = CSqMap();
	mHeaders.deserialize( headers );

	local mGet = CSqMap();
	mGet.deserialize( get );

	local mPost = CSqMap();
	mPost.deserialize( post );

	local mReply = CSqMap();
	local page = "Access Denied";

	// Restore session
	local mSession = RestoreSession( mHeaders, mReply );

	local login_menu = "";
	local loggedin = DoLogin( mGet, mPost, mSession );
	local login_menu = loggedin ? "<a href='?logout=1'>Logout</a>" : "<a href='?login=1'>Login</a>";

	local menu_items =
	[
		[ "home", "Home" ]
	];

	if ( loggedin )
		menu_items.append( [ "tasks", "Tasks" ] );

	switch ( mRequest[ "path" ] )
	{
		case "" :
		case "/" :
		case "/home" :
			_self.include( "pg/home.nut" );
			page = pg_run( mRequest, mHeaders, mGet, mPost, mSession );
			break;

		case "/signup" :
			_self.include( "pg/signup.nut" );
			page = pg_run( mRequest, mHeaders, mGet, mPost, mSession );
			break;

		case "/test" :
			page = "<font color='" + _cfg( "col_text" ) + "'><pre><b>Request</b><br>\r\n" + mRequest.print_r( 1 ) + "<br>\r\n"
					+ "<b>Headers</b><br>\r\n" + mHeaders.print_r( 1 ) + "<br>\r\n"
					+ "<b>POST</b><br>\r\n" + mPost.print_r( 1 ) + "<br>\r\n"
					+ "<b>GET</b><br>\r\n" + mGet.print_r( 1 ) + "</pre></font>";
			CSqFile().put_contents( _self.root( "test.htm" ), page );
			break;

	} // end if

	if ( !page.len() )
	{	mReply.set( "content", "Access Denied" );
		return mReply.serialize();
	} // end if
	local content = "<html>\r\n"
		+ "<!--Remote:" + mRequest[ "REMOTE_ADDR" ] + "-->\r\n"
		+ "<!--Local:" + mRequest[ "SERVER_ADDR" ] + "-->\r\n"
		+ @"
			<body bgcolor='" + _cfg( "col_bg" ) + "' text='" + _cfg( "col_fg" ) + "' link='" + _cfg( "col_link" ) + "' vlink='" + _cfg( "col_link" ) + @"'>
		"
		+ show_menu( mRequest[ "path" ], menu_items, login_menu )
		+ page
		+ @"
	</body>
</html>";

	// Save session data
	if ( mSession.isset( "id" ) )
		_self.set( "/web_server", mSession[ "id" ], mSession.serialize() );

	mReply.set( "content", content );
	return mReply.serialize();
}

function _init()
{
}

function _idle()
{
	return 0;
}

function show_menu( sel, menu_items, right )
{
// style='border-left:solid 1px;border-top:solid 1px;border-right:solid 1px;border-bottom:solid 1px'

	local ret = @"
		<table width='100%' style='" + _cfg( "obj_border" ) + @"' bgcolor='" + _cfg( "col_shade" ) + @"'>
			<tr><td colspan='0'><nobr><b>Task&nbsp;List&nbsp;&nbsp;&nbsp;</b></nobr></td>
			<td width='100%'><table><tr>
		";

	local count = 0;
	foreach( v in menu_items )
	{
		local btn = "";
		if ( v[ 0 ] == sel.slice( 1 ) )
			btn = "<b><small><a style='color:" + _cfg( "col_slink" ) + "' href='/" + v[ 0 ] + @"'>[" + v[ 1 ] + "]</a></small></b>";
		else
			btn = "<b><small><a href='/" + v[ 0 ] + @"'>[" + v[ 1 ] + "]</a></small></b>";

		if ( count++ > 4 )
		{	count = 0;
			ret += "</tr><tr>";
		} // end if

		ret += @"
				<td>
					<nobr>" + btn + @"</nobr>
				</td>
				<td>
					&nbsp;&nbsp;&nbsp;
				</td>
			";

	} // end foreach

	ret += @"	</td></tr></table></td>
				<td>&nbsp;<nobr>" + right + @"</nobr>&nbsp;</td>
			</tr>
		</table>
	";

	return ret;
}


