
_self.load_module( "freetype2", "" );
_self.load_module( "ffmpeg", "" );
//_self.load_module( "portaudio", "" );

function create_font( name, ft )
{
	local font = CFtFace();
	if ( !ft.LoadFont( font, name, 0 ) )
	{	_self.echo( "freetype2 failed to load font '" + name + "' : " + ft.getLastError() );
		return 0;
	} // end if

	// Set font size
	font.setCharSize( 25 * 64, 15 * 64, 100, 0 );

	return font;
}

function create_clip( fname, fmt, w, h, fps, hz, sps, secs, font )
{
	// Audio format
	local afmt = _self.tFloat;

	// What are we doing?
	::_self.echo( "Creating : " + fname + " - " + w + "x" + h + "x" + fps + "x" + hz + "x" + sps + "x" + secs );

	// *** AVI ***

	local avi = CFfContainer();
	local frame = CSqBinary();

	if ( !avi.Create( fname, "", CSqMulti() ) )
	{	_self.echo( "Failed to create avi : " + fname ); return; }

	if ( 0 > avi.AddVideoStream( CFfDecoder().LookupCodecId( fmt ), w, h, fps ) )
	{	_self.echo( "Failed to add video stream" ); return; }

	if ( 0 > avi.AddAudioStream( CFfAudioDecoder().LookupCodecId( "AC3_" ), afmt, 1, sps.tointeger(), 0 ) )
	{	_self.echo( "Failed to add audio stream" ); return; }

	if ( !avi.InitWrite() )
	{	_self.echo( "Failed to initiailze avi" ); return; }

	// *** VIDEO ***

	local venc = CFfEncoder();
	if ( !venc.Create( avi.getVideoCodecId(), CFfConvert().PIX_FMT_YUV420P,
					  w, h, fps, 1000000, CSqMulti() ) )
		_self.echo( "Failed to create video encoder" );

	local img = CSqImage();
	if ( !img.Create( w, h ) )
	{	_self.echo( "Failed to create image buffer" ); return; }

	local pix = CSqBinary();
	if ( !img.refPixels( pix ) )
	{	_self.echo( "Failed to obtain image buffer pointer" ); return; }

	// *** AUDIO ***
	local aenc = CFfAudioEncoder();
	if ( !aenc.Create( avi.getAudioCodecId(), afmt, 1, sps.tointeger(), 0, CSqMulti() ) )
	{	_self.echo( "Failed to create audio encoder" ); return; }

	// Sine wave buffer
	local abuf_size = ( sps / fps ).tointeger();
	local div = sps / hz, pi2 = 3.14159 * 2;
	local sw = CSqBinary();
	if ( !sw.AllocateFLOAT( abuf_size ) )
	{   _self.echo( "!!! Failed to allocate memory for audio waveform" );
		return 0;
	} // end if

	
	_self.echo( "abuf_size = " + abuf_size );
	sw.setUsed( abuf_size * sw.sizeFLOAT() );

/*
	local max_audio_buffer = 1000000;
	local pa = CPaOutput();
	pa.setGlitchDetection( 0 );
	local fsize = 2;
	if ( !pa.Open( 0, pa.getDefaultOutputDevice(), 1, afmt, 0.2, sps, fsize ) )
	{   ::_self.echo( "!!! Failed to open output stream : " + pa.getLastError() );
		return;
	} // end if
	else if ( !pa.Start() )
	{   ::_self.echo( "!!! Failed to start output stream : " + pa.getLastError() );
		return;
	} // end if
*/
	local dts = 0, pts = 0; // _self.gmt_time();
	local num_frames = secs * fps;
	local frame = CSqBinary(), inf = CSqMulti();
	for ( local i = 0; i < num_frames; i++ )
	{
		// Blank the image
		pix.Zero();

		// Draw text
		local hrs = 0;
		local min = 0 + ( i / fps / 60 ).tointeger();
		local sec = ( ( i / fps ) % 60 ).tointeger();
		local f = ( i % fps ).tointeger();
		local str = format( "%0.2d:%0.2d:%0.2d.%0.2d", hrs, min, sec, f );

		::_self.print( str + "\r" );

		font.setColor( 128, 0, 0 );
		font.DrawImg( str, CSqPos( 9, 59 ), img, 1 );
		font.setColor( 255, 0, 0 );
		font.DrawImg( str, CSqPos( 10, 60 ), img, 1 );

		// Encode image
		venc.EncodeImage( img, frame, inf );

		// Write the frame to the file
		if ( frame.getUsed() )
			if ( !avi.WriteVideoFrame( frame, pts, dts inf ) )
			{	_self.echo( "WriteFrame() failed" ); return 0; }

		// Increase audio tone frequency on the second
		if ( !( i % fps ) )
			div = sps / ( hz + ( i * 20 / fps ) );

		// Create audio segment
		for( local s = 0; s < abuf_size; s++ )
			sw.setFLOAT( s, sin( ( i * abuf_size + s ) / div * pi2 ) );
/*
		if ( pa )
			if ( pa.getBufferedBytes() > max_audio_buffer )
				::_self.echo( "dropping audio : " + pa.getBufferedBytes() );

			else if ( !pa.Write( sw, abuf_size ) )
				::_self.echo( "Failed to write audio data" );
*/

		_self.echo( "\ni: " + sw.getUsed() );
		aenc.BufferData( sw );
		while ( aenc.Encode( CSqBinary(), frame, inf ) )
		{
			_self.echo( "o: " + frame.getUsed() );

			// Write audio buffer
			if ( frame.getUsed() )
				avi.WriteAudioFrame( frame, pts, dts, CSqMulti() );

		} // end while

		dts++;
		pts++;

	} // end for

}

// File maybe
local file = _self.get( "/", "cmdline.1" );
if ( !file.len() )
{	_self.echo( "Please specify a filename" );
	return;
} // end if

// Initialize free type library
local ft = CFtLibrary();
if ( ft.getLastError() )
{	_self.echo( "freetype2 failed to initialize : " + ft.getLastErrorStr() );
	return 0;
} // end if

local fontname = "cour.ttf";
local font = create_font( fontname, ft );
if ( !font )
{	_self.echo( "failed to create font initialize : " + fontname );
	return 0;
} // end if


// Create clip
create_clip( file, "MP42", 320, 240, 15, 440, 44100., 60, font );

// Release font correctly
font.Destroy();
ft.Destroy();
