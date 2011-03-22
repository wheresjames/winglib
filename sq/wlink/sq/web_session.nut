
_self.load_module( "curl", "" );

_self.include( "config.nut" );
_self.include( "font.nut" );

function webpath( root, name )
{	local path = "", hash = ::_self.md5( name );
	for ( local p = 0, i = 0; i < 3; i++, p += 3 )
		path = ::_self.build_path( path, hash.slice( p, p + 3 ) );
	return ::_self.build_path( ::_self.build_path( root, path ), hash );
}


function Exe( mParams, mReply )
{
//	_self.echo( "Fonts folder : '" + _self.get_sys_folder( "fonts" ) + "'" );

	local font = CFont(), fn = "arial.ttf";
	if ( !font.Create( fn ) )
	{	mReply[ "content" ] <- "Error creating font : " + fn;
		return 0;
	} // end if

	local img = CSqImage();
	local w = mParams[ "GET" ][ "w" ].toint(); if ( 0 >= w ) w = 600;
	local h = mParams[ "GET" ][ "h" ].toint(); if ( 0 >= h ) h = 24;

	// Create image
	if ( !img.Create( w, h ) )
	{	mReply[ "content" ] <- "Error creating image";
		return 0;
	} // end if

	// Init background
	local pix = CSqBinary();
	if ( img.refPixels( pix ) )
		pix.Zero();

	local name = mParams[ "GET" ][ "start" ].str();	
	local url = webpath( "http://localhost/ed/prj", name );
	local res = CSqCurl().urlSpawn( _self.queue(), "", _self.md5( url ), url );
	if ( !res )
		font.Draw( img, "Failed to start : " + name );
	else
		font.Draw( img, "Running : " + name );

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

