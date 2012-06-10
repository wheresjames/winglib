
_self.load_module( "portaudio", "" );
_self.load_module( "fftw", "" );

class CGlobal
{
	quit = 1;

	// Local web server
	server = 0;

	// Remote sdr server
	remote = 0;

	po = 0;

	buf = CSqBinary();
	dat = CSqBinary();
	fft = CSqBinary();
	img = CSqImage();

	fftw = 0;

	fftsize = 64;
	imgwidth = 800;

	// Input data type
	in_type = _self.tInt8;
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
	if ( !_g.img.getWidth() || !_g.img.getHeight() )
		if ( !_g.img.Create( _g.imgwidth, _g.fftsize * 4 ) )
			return;

	local pix = CSqBinary();
	if ( _g.img.refPixels( pix ) )
		pix.Zero();

	if ( _g.dat.getUsed() )
	{

		// Create graph
		_g.fft.Graph( 0, _g.fftsize / 2, _self.tFloat | _g.dat.eGtHorzFft,
					  _g.img, CSqColor( 0, 0, 128 ), CSqColor( 255, 128, 0 ),
					  "mag=4" );

	//	_g.dat.Graph( 0, _g.fftsize, _self.tFloat | _g.dat.eGtHorzFft,
	//				  _g.img, CSqColor( 0, 0, 0 ), CSqColor( 255, 255, 255 ),
	//				  "mag=2,ho=" + _g.fftsize * 2 );

		_g.dat.Graph( 0, _g.fftsize, _g.in_type | _g.dat.eGtHorzFft,
					  _g.img, CSqColor( 0, 0, 0 ), CSqColor( 255, 255, 255 ),
					  "mag=2,ho=" + _g.fftsize * 2 );

	} // end if

	// Encode the image
	local enc = _g.img.Encode( "png" );
	if ( !enc.getUsed() )
	{	mReply[ "content" ] <- "#err=Error encoding image";
		return mReply.serialize();
	} // end if

	// Set global buffer
	local id = _self.unique();
	_self.set_binshare( id, enc );

	// Set image info
	mReply[ "binary" ] <- id;
	mReply[ "binary_type" ] <- "png";
}

function setCmd( mParams ) : ( _g )
{
	local res = CSqMulti();

	local cmds = CSqMulti(); cmds.setJSON( mParams[ "GET" ][ "cmds" ].str() );
	_self.echo( cmds.print_r( 1 ) );
	foreach( k,v in cmds )
	{
		switch( k )
		{
			// CONNECT
			case "connect" :
			{	if ( !v[ "server" ].len() )
					res[ k ][ "error" ] <- "Server address must be specified";
				else
				{	_g.remote = CSqSocket();
					local port = ( 0 < v[ "port" ].toint() ) ? v[ "port" ].toint() : 80;
					if ( !_g.remote.Connect( v[ "server" ].str(), port ) )
						_g.remote = 0, res[ k ][ "error" ] <- _g.remote.getLastError();
					else
						_g.remote = 0, res[ k ][ "status" ] <- "Connecting to " + v[ "server" ].str();
				} // end else 
			} break;
		} // end switch

	} // end foreach

	return res.getJSON();
}


function OnProcessRequest( params ) : ( _g )
{
	local mParams = CSqMulti();
	mParams.deserialize( params );
	_self.echo( mParams[ "REQUEST" ][ "REMOTE_ADDR" ].str() + " : " + mParams[ "REQUEST" ][ "REQUEST_STRING" ].str() );

	local mReply = CSqMulti();

	local path = mParams[ "REQUEST" ][ "path" ].str();
	switch ( path )
	{
		case "/img" :
			getImage( mReply );
			break;

		case "/cmd" :
			mReply[ "content" ] <- setCmd( mParams );
			break;

		default :
			local content = 0;
			if ( CSqFile().exists( _self.path( path ) ) )
				content = CSqFile().get_contents( _self.path( path ) ); 
			mReply[ "content" ] <- ( ( content && content.len() ) ? content 
								   : _self.include_inline( "test_sdr.gui.squ", mParams ) );
			break;

	} // end switch

	return mReply.serialize();
}

function _init() : ( _g )
{
	local url = _self.get( "/", "cmdline.1" );
	if ( !url.len() )
	{	_self.echo( " You must specify the sdr server" );
		_g.quit = 1; return 0;
	} // end if

	// Parse the url
	local link = _self.parse_url( url );
	if ( 0 >= link[ "port" ].toint() ) link[ "port" ] <- "80";
	_self.echo( link.print_r( 1 ) );

	_g.remote = CSqSocket();
	if ( !_g.remote.Connect( link[ "host" ].str(), link[ "port" ].toint() ) )
	{	_self.echo( "Connect() : " + _g.remote.getLastError() ); return; }

	// Wait for connect
	if ( !_g.remote.WaitEvent( CSqSocket().EVT_CONNECT, 3000 ) )
	{	_self.echo( "WaitEvent( EVT_CONNECT ) : " + _g.remote.getLastError() ); return; }

//	local link = "/~~stream";
//	local link = "/live?id=Lobby&video=MJPG";

	// Write data
//	local req = link[ "path" ].str() + ( link[ "extra" ].len() ? ( "?" + link[ "extra" ].str() ) : "" );
//	if ( !_g.remote.Write( "GET " + req + " HTTP/1.1\r\nHost:\r\n\r\n", 0 ) )
//	{	_self.echo( "Write() : " + _g.remote.getLastError() ); return; }

	_g.remote.Write( "GET /~~stream?format=2\r\n\r\n", 0 );
//	_g.remote.Write( "GET /~~param?ppm=0&f=7023.71&band=0&lo=-2.7&hi=-0.3&mode=0&name=\r\n\r\n", 0 );
//	_g.remote.Write( "GET /~~param?ppm=1352&ppml=1367&f=7179.9125&band=3&lo=-2.7&hi=0.825&mode=0&name=\r\n\r\n", 0 );
	_g.remote.Write( "GET /~~param?ppm=1200&ppml=1202&f=14192.3375&band=4&lo=0.3&hi=2.7&mode=0&name=\r\n\r\n", 0 );

//	_g.remote.Write( "GET /~~waterstream0\r\n\r\n", 0 );
//	_g.remote.Write( "GET /~~waterparam?slow=4\r\n\r\n", 0 );
//	_g.remote.Write( "GET /~~waterparam?band=4&zoom=0&start=0\r\n\r\n", 0 );

	// Start web server
	_g.server = CSqHttpServer();
	_g.server.SetSessionCallback( _self.queue(), "OnProcessRequest" );
	if ( !_g.server.Start( 1234 ) )
		_self.echo( "Unable to start http server" );
	else
		_self.echo( "server started on http://localhost:1234" );

	_g.fftw = CSqFftw();
	if ( !_g.fftw.Plan( _g.fftsize, 0, 0 ) )
	{   _self.echo( "!!! Failed to create fft plan" );
		WaitKey(); _g.quit = 1; return 0;
	} // end if

	_self.echo( "Streaming sdr data..." );

	_self.set_timer( ".", 100, "Update" );


	local sps = 9600.;
	local bsize = ( sps / 10 ).tointeger();

	_g.po = CPaOutput();
	_self.echo( "*** Opening output device : " + _g.po.getDefaultOutputDevice() );
	if ( !_g.po.Open( 0, _g.po.getDefaultOutputDevice(), 1, _g.in_type, 0.2, sps, bsize ) )
//	if ( !_g.po.Open( 0, _g.po.getDefaultOutputDevice(), 1, _self.tFloat, 0.2, sps, bsize ) )
	{   _self.echo( "!!! Failed to open output stream : " + _g.po.getLastError() ); return; }

	if ( !_g.po.Start() )
	{   _self.echo( "!!! Failed to start output stream : " + _g.po.getLastError() ); return; }

	_self.echo( "Audio is open" );


	_g.quit = 0;

	return 1;
}

function _idle() : ( _g )
{
	return _g.quit;
}

function Update() : ( _g )
{
	if ( !_g.remote )
		return;

	// Wait for data
	if ( !_g.remote.WaitEvent( CSqSocket().EVT_READ, 0 ) )
		return 0;

	local newdata = _g.remote.AppendBin( _g.buf, 0 );
//	_self.echo( "new = " + newdata + ", total = " + _g.buf.getUsed() );

	if ( 0 >= newdata || !_g.buf.getUsed() )
		return 0;

	if ( _g.po )
	{	local sub = _g.buf.getSub( _g.buf.getUsed() - newdata, newdata );
		_g.po.Write( sub, newdata );
	} // end if

	local fft = CSqBinary();
//	local used = _g.fftw.CalcMagnitudes( _self.tFloat, 0, _g.buf, _self.tFloat, 0, fft );
	local used = _g.fftw.CalcMagnitudes( _g.in_type, 0, _g.buf, _self.tFloat, 0, fft );

	// Accumulate data
	_g.fft.InsertBin( fft, 0 );
	if ( used ) _g.dat.InsertBin( _g.buf, 0 ), _g.buf.LShift( used );

//	_self.echo( ": " + used + ", fft=" + _g.fft.getUsed() + ", dat=" + _g.dat.getUsed() );

	// Limit buffer sizes
	local max = ( _g.fftsize / 2 ) * _g.imgwidth * 4;
	local sz = _g.fft.getUsed();
	if ( sz > max ) _g.fft.setUsed( max );

	max = _g.fftsize * _g.imgwidth * 4;
	sz = _g.dat.getUsed();
	if ( sz > max ) _g.dat.setUsed( max );

	return 0;
}


