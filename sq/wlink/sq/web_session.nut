
_self.include( "config.nut" );
_self.include( "font.nut" );

function Exe( mParams, mReply )
{
	local font = CFont(), fn = "arial.ttf";
	if ( !font.Create( fn ) )
	{	mReply[ "content" ] <- "Error creating font : " + fn;
		return 0;
	} // end if

	local img = CSqImage();
	local w = mParams[ "GET" ][ "w" ].toint(); if ( 0 >= w ) w = 400;
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

	font.Draw( img, mParams[ "GET" ][ "cmd" ].str() );

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
			mReply[ "content" ] <- ::_self.include_inline( "pg/home.nut", mParams );
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

