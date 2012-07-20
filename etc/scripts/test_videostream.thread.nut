
if ( _self.load_module( "ffmpeg", "" ) )
	_self.echo( "FFMPEG Failed to load" );

if ( _self.load_module( "freetype2", "" ) )
	_self.echo( "Freetype library failed to load" );

class CGlobal
{
	quit = 0;

	cap = CSqBinary();

	w = 0;
	h = 0;

	vid = 0;
	enc = 0;
	share = 0;
	frame = 0;

	ft = 0;
	font = 0;

	img = 0;
	pix = 0;

	t = 0;

	fps = 0;

	delay = 0;

	// Maximum number of seconds to buffer before dropping frames
	maxbuf = 2;

};

local _g = CGlobal();

function _init() : ( _g )
{
	// Initialize free type library
	_g.ft = CFtLibrary();
	if ( _g.ft.getLastError() )
	{	_self.echo( "freetype2 failed to initialize : " + _g.ft.getLastError() );
		return -1;
	} // end if

	// Load font
	local fname = "cour.ttf";
	_g.font = CFtFace();
	if ( !_g.ft.LoadFont( _g.font, fname, 0 ) )
	{	_self.echo( "freetype2 failed to load font '" + fname + "' : " + ft.getLastError() );
		return -1;
	} // end if

	// Set font size
	_g.font.setCharSize( 25 * 64, 15 * 64, 100, 0 );

	// Frame buffer
	_g.frame = CSqBinary();

	return 0;
}

function _idle() : ( _g )
{
	return _g.quit;
}

function _exit() : ( _g )
{
	if ( _g.cap.getUsed() )
		_self.screen_release_capture( _g.cap );

	_g.font = 0;
	_g.ft = 0;
	return 0;
}

function StartStream( params ) : ( _g )
{
	local p = CSqMulti( params );
	_self.echo( params );

	// Frame rate
	_g.fps = p[ "fps" ].toint();
	if ( 0 >= _g.fps ) _g.fps = 15;

	_g.w = p[ "w" ].toint();
	_g.h = p[ "h" ].toint();
	_g.delay = p[ "delay" ].toint();

	if ( p[ "cap" ].toint() )
	{
		if ( !_self.screen_init_capture( _g.cap, 0, 0, _g.w, _g.h ) )
		{	_self.echo( "Failed to initialize screen capture" ); _g.quit = 1; return 0; }

		local inf = CSqMulti();
		if ( !_self.screen_get_info( _g.cap, inf ) )
		{	_self.echo( "Failed to get screen info" ); _g.quit = 1; return 0; }

		if( 0 >= inf[ "w" ].toint() || 0 >= inf[ "h" ].toint() )
		{	_self.echo( "Invalid screen info" ); _g.quit = 1; return 0; }

		_g.w = inf[ "w" ].toint();
		_g.h = inf[ "h" ].toint();
		_g.pix = CSqBinary();

	} // end if

	else
	{
		if ( 0 >= _g.w ) _g.w = 320;
		if ( 0 >= _g.h ) _g.h = 240;

		// Create a raw frame buffer
		_g.img = CSqImage();
		if ( !_g.img.Create( _g.w, _g.h ) )
		{	_self.echo( "Failed to create image buffer" ); _g.quit = 1; return 0; }

		// Get a pointer to the raw data
		_g.pix = CSqBinary();
		if ( !_g.img.refPixels( _g.pix ) )
		{	_self.echo( "Failed to obtain image buffer pointer" ); _g.quit = 1; return 0; }

	} // end else

//	_self.echo( "w = " + _g.w + ", h = " + _g.h );

	// Codec
	local fmt = p[ "codec" ].str();
	local pix = CFfConvert().PIX_FMT_YUV420P;
	if ( fmt == "MJPG" ) pix = CFfConvert().PIX_FMT_YUVJ420P;
	else if ( fmt == "MPG4" ) fmt = "FMP4";

	// Create the encoder
	_g.enc = CFfEncoder();
//	_g.enc.setTimeBase( 90000 );
	local q = p[ "q" ].toint(); if ( 0 >= q ) q = 5;
	p[ "encoder_params" ][ "quality" ] <- q.tostring();
	if ( !_g.enc.Create( CFfDecoder().LookupCodecId( fmt ),
						 pix, _g.w, _g.h, _g.fps 0, p[ "encoder_params" ] ) )
	{	::_self.echo( "Unable to create codec : " + fmt );
		_g.quit = 1; return 0;
	} // end if

	// Packet header?
	if ( p[ "header" ].len() )
		_g.enc.setHeader( p[ "header" ].bin() );

	// Set extra data into header
	p[ "extra" ] <- _g.enc.getExtraData().getString();

	// Encoding level
	p[ "level" ] <- _g.enc.getLevel().tostring();

	// Choose reasonable buffer sizes
	local bufsz = _g.w * _g.h * _g.fps * _g.maxbuf * 2;
	local buffers = _g.fps * _g.maxbuf * 3;

	// Do we need a container?
	if ( p[ "type" ].len() )
	{
		// Video container
		_g.vid = CFfContainer();
		local url = "memshare://" + p[ "sid" ].str()
					+ "?size=" + bufsz
					+ "&buffers=" + buffers
					+ "&max_packet_size=1024"
					+ "&headers=" + _self.urlencode( p.getJSON() );
		if ( !_g.vid.Create( url, p[ "type" ].str(), CSqMulti() ) )
		{	::_self.echo( "Failed to create video container : " + p[ "type" ].str() );
			_g.quit = 1; return 0;
			return 0
		} // end if

		// Add video stream
		_g.vid.setTimeBase( _g.enc.getTimeBase() );
//		_g.vid.setVideoExtraData( _g.enc.getExtraData() );
		if ( 0 > _g.vid.AddVideoStream( CFfDecoder().LookupCodecId( fmt ),
										_g.img.getWidth(), _g.img.getHeight(), _g.fps ) )
		{	::_self.echo( "Failed to add video stream : " + fmt );
			_g.quit = 1; return 0;
			return 0
		} // end if

		// Initilalize write
		if ( !_g.vid.InitWrite() )
		{	::_self.echo( "Failed to initialize video container : " + p[ "type" ].str() );
			_g.quit = 1; return 0;
			return 0
		} // end if

	} // end if

	// Raw codec output
	else if ( p[ "sid" ].len() )
	{
		// Memory share
		_g.share = CSqFifoShare();
		if ( !_g.share.Create( p[ "sid" ].str(), "", bufsz, buffers, p.getJSON() ) )
		{	::_self.echo( "Failed to open output share : " + p[ "share" ].str() );
			_g.quit = 1; return 0;
		} // end if

	} //end if

	// Set frame timer
	_self.set_timer( ".", 1000 / _g.fps, "Run" );

	// Let everyone know we're off and running
	_self.echo( "Started stream : " + p[ "sid" ].str() );

	return 1;
}

function Run() : ( _g )
{
	if ( _g.quit )
		return 0;

	// Some protocols require this
	if ( 0 < _g.delay )
		if ( _g.delay-- % _g.fps )
			return 0;

	// Monitor read / writes
	local reads = _g.vid ? _g.vid.getFifoReads() : _g.share.getReads();
	local writes = _g.vid ? _g.vid.getFifoWrites() : _g.share.getWrites();
	local diff = writes - reads;
	if ( diff > _g.fps * _g.maxbuf )
	{	//_self.print( " " + reads + "/" + writes );
		_self.echo( "Reader quit caring :(" );
		_g.quit = 1;
		return 0;
	} // end if

//	_self.print( " " + reads + "/" + writes );

	// Calculate frame timers
	local gmt = _self.gmt_time_useconds() / 1000;
	local hrs = ( gmt / ( 60 * 60 * 1000 ) % 24 ).tointeger();
	local mins = ( gmt / ( 60 * 1000 ) % 60 ).tointeger();
	local secs = ( ( gmt / 1000 ) % 60 ).tointeger();
	local msecs = ( gmt % 1000 ).tointeger()
	local str = format( "%0.2d:%0.2d:%0.2d.%0.3d", hrs, mins, secs, msecs );

	if ( _g.cap.getUsed() )
	{
		_self.screen_lock( _g.cap, _g.pix );
	} // end if
	else
	{
		// Paing black background
		_g.pix.Zero();

		// Draw
		_g.font.setColor( 64, 0, 0 );
		_g.font.DrawImg( str, CSqPos( 9, 59 ), _g.img, 1 );
		_g.font.setColor( 255, 0, 0 );
		_g.font.DrawImg( str, CSqPos( 10, 60 ), _g.img, 1 );

	} // end else

	// Attempt to encode the frame
	local inf = CSqMulti();
	if ( 0 >= _g.enc.Encode( CFfConvert().PIX_FMT_BGR24,
							 _g.w, -_g.h, _g.pix, _g.frame, inf ) )
	{	_self.echo( "??? = " + inf[ "pts" ].toint() );
		return 0;
	} // end if

//_self.echo( "pts = " + inf[ "pts" ].toint() + ", fsz = " + _g.frame.getUsed() );
//_self.echo( "pts = " + _g.enc.calcPts() + ", fsz = " + _g.frame.getUsed() );

//_self.echo( inf.print_r( 1 ) );
	// Write to container
	if ( _g.vid )
	{
		// Attempt to write the frame
//		if ( !_g.vid.WriteVideoFrame( _g.frame, _g.enc.calcPts(), _g.enc.calcPts(), inf ) )
		if ( !_g.vid.WriteVideoFrame( _g.frame, inf[ "pts" ].toint(), inf[ "pts" ].toint(), inf ) )
//		if ( !_g.vid.WriteVideoFrame( _g.frame, -1, -1, inf ) )
			_g.quit = 1;

		// Flush data buffers
		_g.vid.FlushBuffers();

	} // end if

	// Write to shared memory
	else if ( _g.share )
		if ( !_g.share.Write( _g.frame, "", 0, 0 ) )
			_g.quit = 1;

	return 1;
}

