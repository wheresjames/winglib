
_self.load_module( "curl", "" );

_self.include( "config.nut" );
_self.include( "font.nut" );

function webpath( root, name )
{	local path = "", hash = ::_self.md5( name );
	for ( local p = 0, i = 0; i < 3; i++, p += 3 )
		path = ::_self.build_path( path, hash.slice( p, p + 3 ) );
	return ::_self.build_path( ::_self.build_path( root, path ), hash );
}

function Stop( name )
{
	if ( name == "all" )
		name = "";

	local devlist = split( _self.get_children( "." ), "," );
	foreach( k in devlist )
	{	local n = _self.urldecode( k );
		if ( !name.len() || name == k )
			_self.set( "/", "running." + n, "" ),
			_self.kill_wait( n, 3, 1 );
	} // end foreach

	return 1;	
}

function Start( name )
{
	// Stop all
	Stop( "" );

	if ( !name.len() ) 
		return;

	// Attempt to start the script
	local id = _self.md5( name );
	local url = webpath( _cfg( "code_server" ), name );
	if ( !CSqCurl().urlSpawn( _self.queue(), "", id, name, url ) )
		return 0;

	// Map name
	_self.set( "/", "running." + id, name );

	return 1;
}

function Cmd( mParams, font, img )
{
	// Starting script?
	if ( mParams[ "GET" ][ "start" ].len() )
	{	local name = mParams[ "GET" ][ "start" ].str();
		_self.echo( "Starting : " + name );
		font.Draw( 1, img, ( Start( name ) ? "Starting : " : "Failed to start : " ) + name );
	} // end if

	else if ( mParams[ "GET" ][ "stop" ].len() )
	{	local name = mParams[ "GET" ][ "stop" ].str();
		_self.echo( "Stoping : " + name );
		font.Draw( 1, img, ( Stop( name ) ? "Stopping : " : "Failed to stop : " ) + name );
	} // end if

	else
	{
		local devlist = split( _self.get_children( "." ), "," );
		if ( !devlist.len() )
			font.Draw( 1, img, "Engine is idle" );
		else
		{	local running = "";
			foreach( k in devlist )
			{	local id = _self.urldecode( k );
				local name = _self.get( "/", "running." + id );
				if ( running.len() ) running += ", ";
				if ( name.len() ) running += name;
				else running += id;
			} // end foreach
			font.Draw( 1, img, "Running : " + running );
		} // end else

	} // end else

}

function Console( mParams, font, img )
{
	local lines = split( _self.get_output( 0 ), "\r\n" ), max = font.getLines();
	local off = ( lines.len() > max ) ? ( lines.len() - max ) : 0;
	foreach( l in lines ) 
		if ( 0 < off ) 
			off--; 
		else
			font.Draw( 0, img, _self.replace( l, "\t", "  " ) );
}

function Exe( mParams, mReply )
{
//	_self.echo( "Fonts folder : '" + _self.get_sys_folder( "fonts" ) + "'" );

	local img = CSqImage(), bConsole = mParams[ "GET" ].isset( "console" );
	local w = mParams[ "GET" ][ "w" ].toint(); if ( 0 >= w ) w = 600;
	local h = mParams[ "GET" ][ "h" ].toint(); if ( 0 >= h ) h = bConsole ? 400 : 24;

	local font = CFont(), fn = "courbd.ttf";
	if ( !font.Create( fn, h ) )
	{	font = CFont(), fn = "cour.ttf";
		if ( !font.Create( fn, h ) )
		{	mReply[ "content" ] <- "Error creating font : " + fn;
			return 0;
		} // end if
	} // end if

	// Create image
	if ( !img.Create( w, h ) )
	{	mReply[ "content" ] <- "Error creating image";
		return 0;
	} // end if

	// Init background
	local pix = CSqBinary();
	if ( img.refPixels( pix ) )
		pix.Zero();

	if ( bConsole )
		Console( mParams, font, img );
	else
		Cmd( mParams, font, img );

	// Encode the image
	local enc = img.Encode( "png" );
	if ( !enc.getUsed() )
	{	mReply[ "content" ] <- "Error encoding image";
		return 0;
	} // end if

	// Set global buffer
	local id = _self.unique();
	_self.set_binshare( id, enc );

	// Set image info
	mReply[ "binary" ] <- id;
	mReply[ "binary_type" ] <- "png";

	return 1;
}

function OnProcessRequest( params )
{
	local mParams = CSqMulti();
	mParams.deserialize( params );
//	_self.echo( mParams[ "REQUEST" ][ "REMOTE_ADDR" ].str()
//				+ " : " + mParams[ "REQUEST" ][ "REQUEST_STRING" ].str() );

	local mReply = CSqMulti();
	local content = "";

	local path = split( mParams[ "REQUEST" ][ "path" ].str(), "/" );
	switch( path.len() ? path[ 0 ] : "" )
	{
		case "exe" :
			Exe( mParams, mReply );
			break;

		default :
			mReply[ "content" ] <- ::_self.include_inline( "pg/home.squ", mParams );
			break;

	} // end switch

	mReply[ "session" ] <- mParams[ "SESSION" ].serialize();
	return mReply.serialize();
}

function _init()
{
}

function _idle()
{
	return 0;
}

