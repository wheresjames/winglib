
_self.load_module( "http", "" );

class CGlobal
{
	server = 0;
	cap = 0;
};

local _g = CGlobal();

function OnServerEvent( server, data )
{

}

function OnProcessRequest( params ) : ( _g )
{
	local mReply = CSqMulti();

	local mParams = CSqMulti();
	mParams.deserialize( params );
	_self.echo( mParams[ "REQUEST" ][ "REMOTE_ADDR" ].str() + " : " + mParams[ "REQUEST" ][ "REQUEST_STRING" ].str() );

	local err = "";
	local pix = CSqBinary();

	// Lock the capture frame
	if ( 0 < _g.cap.Capture( pix, 3000 ) )
	{
		// Create an image to do the encoding
		local img = CSqImage();
		if ( img.Create( _g.cap.getWidth(), _g.cap.getHeight() ) )
		{
			// Copy image pixels
			img.setPixels( pix );

			// Encoded the image into a shared buffer
			local id = _self.unique();
			_self.set_binshare( id, img.Encode( "jpg" ) )

			// Return to web server
			mReply[ "binary" ] <- id,
			mReply[ "binary_type" ] <- "jpg";
			return mReply.serialize();

		} // end if
		else
			err = img.getLastErrorStr();

	} // end if

	else
		err = _g.cap.getLastErrorStr();


	mReply[ "content" ] <- "Capture Failed : " + err;

	return mReply.serialize();
}

function _init() : ( _g )
{
	// Start the web server
	_g.server = CSqHttpServer();
	_g.server.SetSessionCallback( _self.queue(), "OnProcessRequest" );
	if ( !_g.server.Start( 1234 ) )
		_self.echo( "Unable to start http server" );
	else
		_self.echo( "server started on : http://localhost:1234/" );

	// Start the video capture device
	_g.cap = CSqCapture();
	if ( 0 > _g.cap.Init( 0, 0, 320, 240, 15, "RGB3", 1 ) )
		_self.echo( "Unable to start capture device" );

	// Show supported formats
	local disp = "";
	local formats = split( _g.cap.GetSupportedFormats(), "," );
	foreach( f in formats )
		disp += f + " - " + _g.cap.GetFormatDescription( f ) + "\n";
	_self.echo( disp );

}

function _idle()
{
//	_self.alert( "Waiting..." );

	return 0;
}

