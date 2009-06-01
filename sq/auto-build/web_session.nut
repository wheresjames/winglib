
dofile( _self.path( "config.nut" ) );
local _cfg = CConfig();

class CGlobal
{
};

local _g = CGlobal();

function OnProcessRequest( request, headers, get, post ) : ( _g, _cfg )
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

	switch ( mRequest[ "path" ] )
	{
		case "" :
		case "/" :
		case "/home" :
			dofile( _self.path( "pg_home.nut" ) );
			page = pg_run( mRequest, mHeaders, mGet, mPost );
			break;

		case "/build" :
			dofile( _self.path( "pg_build.nut" ) );
			page = pg_run( mRequest, mHeaders, mGet, mPost );
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
			<body bgcolor='#000000' text='" + _cfg.col_text + "' link='" + _cfg.col_link + "' vlink='" + _cfg.col_link + @"'>
		"
		+ show_menu( mRequest[ "path" ] )
		+ page
		+ @"
	</body>
</html>";

	mReply.set( "content", content );
	return mReply.serialize();
}

function _init() : ( _g )
{
}

function _idle()
{
	return 0;
}


function show_menu( sel )
{
	local menu_items =
	[
		[ "home", "Home" ],
		[ "build", "Build" ]
	];

// style='border-left:solid 1px;border-top:solid 1px;border-right:solid 1px;border-bottom:solid 1px'

	local ret = @"
		<table width='100%'>
			<tr><td colspan='00'><nobr><b>Auto&nbsp;Build&nbsp;&nbsp;&nbsp;</b></nobr></td>
			<td width='100%'><table><tr>
		";

	local count = 0;
	foreach( v in menu_items )
	{
		local btn = "";
		if ( v[ 0 ] == sel.slice( 1 ) )
			btn = "<b><small><a style='color:#808080' href='/" + v[ 0 ] + @"'>[" + v[ 1 ] + "]</a></small></b>";
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
			</tr>
		</table>
		<hr>
	";

	return ret;
}


