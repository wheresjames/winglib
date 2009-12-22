
_self.include( "config.nut" );

function DoLogin( mParams )
{
	if ( mParams[ "GET" ].isset( "login" ) )
		mParams[ "SESSION" ][ "user" ] <- "1";

	else if ( mParams[ "GET" ].isset( "logout" ) )
		mParams[ "SESSION" ].unset( "user" ),
		mParams[ "SESSION" ].unset( "_ts" );

	return mParams[ "SESSION" ].isset( "user" );
}

function OnProcessRequest( params )
{
	local mParams = CSqMulti();
	mParams.deserialize( params );
	_self.echo( mParams[ "REQUEST" ][ "REMOTE_ADDR" ].str()
				+ " : " + mParams[ "REQUEST" ][ "REQUEST_STRING" ].str() );

	local mReply = CSqMap();
	local page = "Access Denied";

	local login_menu = "";
	local loggedin = DoLogin( mParams );
	local login_menu = loggedin ? "<a href='?logout=1'>Logout</a>" : "<a href='?login=1'>Login</a>";
/*
	// Is it a file request?
	local share_name = "/jscript";
	if ( 0 <= _self.find( mParams[ "REQUEST" ][ "path" ].str(), share_name ) )
	{	//local file = _self.build_path( cc[ "media_root" ], CSqFile().get_filename( mParams[ "REQUEST" ][ "path" ].str() ) );
		local file = mParams[ "REQUEST" ][ "path" ].str();
		file = _self.replace( file, share_name, _self.root( "tmp/jscript" ) );
		mReply.set( "file", file );
		return mReply.serialize();
	} // end if
*/
	if ( !loggedin )
		page = CSqFile().get_contents( _self.root( "tmp/html/login.html" ) );

	else
	{
		local menu_items =
		[
			[ "home", "Home" ],
			[ "signup", "Sign Up" ]
		];

		if ( loggedin )
			menu_items.append( [ "tasks", "Tasks" ] );

		switch ( mParams[ "REQUEST" ][ "path" ].str() )
		{
			case "" :
			case "/" :
			case "/home" :
				_self.include( "pg/home.nut" );
				page = pg_home( mParams );
				break;

			case "/signup" :
				_self.include( "pg/signup.nut" );
				page = pg_signup( mParams );
				break;

			case "/test" :
				page = "<font color='" + _cfg( "col_text" ) + "'><pre>"
						+ mParams.print_r( 1 )
						+ "</pre></font>";
				CSqFile().put_contents( _self.root( "test.htm" ), page );
				break;

		} // end if

		page = show_menu( mParams[ "REQUEST" ][ "path" ].str(), menu_items, login_menu ) + page;

	} // end else

	if ( !page.len() )
	{	mReply.set( "content", "Access Denied" );
		return mReply.serialize();
	} // end if

	local content = "<html>\r\n"
		+ "<head>\r\n"
	    + " <script type='text/javascript' src='jscript/jquery-1.3.2.min.js'></script>\r\n"
		+ " <!--Remote:" + mParams[ "REQUEST" ][ "REMOTE_ADDR" ].str() + "-->\r\n"
		+ " <!--Local:" + mParams[ "REQUEST" ][ "SERVER_ADDR" ].str() + "-->\r\n"
		+ "</head>\r\n"
		+ @"
			<body bgcolor='" + _cfg( "col_bg" ) + "' text='" + _cfg( "col_fg" ) + "' link='" + _cfg( "col_link" ) + "' vlink='" + _cfg( "col_link" ) + @"'>
		"
		+ page
		+ @"
	</body>
</html>";

	mReply.set( "session", mParams[ "SESSION" ].serialize() );
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


