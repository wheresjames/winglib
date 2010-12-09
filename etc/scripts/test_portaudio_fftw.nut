
_self.load_module( "http", "" );
_self.load_module( "portaudio", "" );
_self.load_module( "fftw", "" );

class CGlobal
{
	quit = 0;
	server = 0;
	pa = 0;
	dat = CSqBinary();
	img = CSqImage();
};

local _g = CGlobal();

function WaitKey()
{	_self.echo( "\n...press any key...\n" );
	_self.get_key();
}

function OnServerEvent( server, data )
{

}

function getImage( mReply ) : ( _g )
{
	if ( !_g.dat.getUsed() )
		return;

	if ( !_g.img.getWidth() || !_g.img.getHeight() )
		if ( !_g.img.Create( 320, 240 ) )
			return;

	// Create graph
	_g.dat.Graph( 0, 128, _self.tFloat | _g.dat.eGtHorzFft, 
				  _g.img, CSqColor( 0, 0, 0 ), CSqColor( 255, 255, 255 ),
				  "" );

	// Encode the image
	local enc = _g.img.Encode( "png" );
	if ( !enc.getUsed() )
	{	mReply[ "content" ] <- "#err=Error encoding chart";
		return mReply.serialize();
	} // end if

	// Set global buffer
	local id = _self.unique();
	_self.set_binshare( id, enc );

	// Set image info
	mReply[ "binary" ] <- id;
	mReply[ "binary_type" ] <- "png";
}

function getPage( mReply )
{
	local page = @"<html>
		<head>
			<script src='/js/jquery.min.js' type='text/javascript'></script>
		</head>
		<body>

			<img name='image' onload='onloadimg();' onabort='onerrorimg();' onerror='onerrorimg();'>

			<script>
				function onloadimg()
				{	setTimeout( 'RefreshImage()', 100 ); }
				function onerrorimg()
				{	setTimeout( 'RefreshImage()', 1000 ); }
				function RefreshImage()
				{	var t = document.getElementsByName( 'image' )[ 0 ];
					if ( t ) t.src = '/img?_random_=' + Math.random();
				} RefreshImage();
			</script>
			
		</body>
	</html>";

	mReply[ "content" ] <- page;
}

function OnProcessRequest( params ) : ( _g )
{
	local mParams = CSqMulti();
	mParams.deserialize( params );
	_self.echo( mParams[ "REQUEST" ][ "REMOTE_ADDR" ].str() + " : " + mParams[ "REQUEST" ][ "REQUEST_STRING" ].str() );

	local mReply = CSqMulti();

	switch ( mParams[ "REQUEST" ][ "path" ].str() )
	{
		case "/img" :
			getImage( mReply );
			break;
		
		default :
			getPage( mReply );

	} // end switch

	return mReply.serialize();
}

function _init() : ( _g )
{
	_g.server = CSqHttpServer();

	_g.server.SetSessionCallback( _self.queue(), "OnProcessRequest" );

	if ( !_g.server.Start( 1234 ) )
		_self.echo( "Unable to start http server" );
	else
		_self.echo( "server started on http://localhost:1234" );

	_g.pa = CPaInput();
	if ( !_g.pa.Open( 0, _g.pa.getDefaultInputDevice(), 1, 
				   CPaInput().paFloat32, 0.2, 44100., 0 ) )
	{   _self.echo( "!!! Failed to open input stream : " + _g.pa.getLastError() );
		WaitKey();
		_g.quit = 1;
		return 0;
	} // end if

	if ( !_g.pa.Start() )
	{   _self.echo( "!!! Failed to start input stream : " + _g.pa.getLastError() );
		WaitKey();
		_g.quit = 1;
		return 0;
	} // end if

	_self.echo( "Capturing audio data..." );

	return 1;
}

function _idle() : ( _g )
{
	// Read the latest data
	_g.pa.Read( _g.dat, 0 );

	local sz = _g.dat.getUsed(), max = 256 * 1024;
	if ( sz > max ) _g.dat.LShift( sz - max );

	return _g.quit;
}

