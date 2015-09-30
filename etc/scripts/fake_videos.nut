
_self.load_module( "ffmpeg", "" );
_self.load_module( "freetype2", "" );

function create_font( name, ft )
{
//	_self.echo( "Fonts folder : '" + _self.get_sys_folder( 0, "fonts" ) + "'" );

	local font = CFtFace();
	if ( !ft.LoadFont( font, name, 0 ) )
	{	_self.echo( "freetype2 failed to load font '" + name + "' : " + ft.getLastError() );
		return 0;
	} // end if

	// Set font size
	font.setCharSize( 25 * 64, 15 * 64, 100, 0 );

	return font;
}

function create_clip( root, name, devs, w, h, fps, fmt, tone, afmt, sec, font, hrs, mins )
{
//	local asr = 44100;
	local asr = 11025;
	local ach = 2;

	if ( devs.len() )
	{	_self.echo( "+++ Haven't implemented this yet :(" );
		return;
	} // end if

	::_self.echo( "Creating : " + name + " - " + fmt + " - " + w + "x" + h + "x" + fps + "x" + sec + " / " + afmt + ":" + tone + "Hz" );

	if ( root.len() )
		name = ::_self.build_path( root, name )

	local avi = CFfContainer();
	local enc = CFfEncoder();
	local frame = CSqBinary();

	// Audio?
	local aenc = 0, abuf, aframe = 0, aid = 0, aref = 0, aoff = 0;
	if ( 0 < tone && afmt.len() )
	{
		aenc = CFfAudioEncoder();
		aframe = CSqBinary();
		abuf = CSqBinary();

		// Create audio codec
		aid = CFfAudioDecoder().LookupCodecId( afmt );
		if ( !aid || !aenc.Create( aid, ::_self.tFloat, 2, asr, 0, CSqMulti( "cmp=-2") ) )
		{	_self.echo( "Failed to create audio codec : " + afmt );
			aenc = 0, aframe = 0; abuf = 0;
		} // end if

		// Create sine wave
		else
		{
			// Calculate a buffer size and allocate
			local bsize = asr;
			if ( !abuf.Size() && !abuf.AllocateFLOAT( bsize * ach ) )
				abuf = 0;

			if ( abuf )
			{
				// Create sine wave
				abuf.setUsed( bsize * ach * abuf.sizeFLOAT() );

				// Create sine wave
				local div = asr.tofloat() / tone.tofloat(), pi2 = 3.14159 * 2.;
				local div2 = asr.tofloat() / tone.tofloat() * 1.5;
				for( local i = 0; i < bsize; i++ )
					for ( local ch = 0; ch < ach; ch++ )
						abuf.setFLOAT( ( i * ach ) + ch, sin( aref / div * pi2 ) + sin( aref / div2 * pi2 ) ), aref++;

			} // end if

		} // end else

	} // end if

	if ( !avi.Create( name, "", CSqMulti() ) )
	{	_self.echo( "Failed to create avi : " + name ); return; }

	if ( 0 > avi.AddVideoStream( CFfDecoder().LookupCodecId( fmt ), w, h, fps ) )
	{	_self.echo( "Failed to add video stream" ); return; }

	if ( aid )
		if ( 0 > avi.AddAudioStream( aid, ::_self.tFloat, 2, asr, 0 ) )
			_self.echo( "Failed to add audio stream : " + afmt + " : " + aid );

	if ( !avi.InitWrite() )
	{	_self.echo( "Failed to initiailze avi" ); return; }

	if ( !enc.Create( avi.getVideoCodecId(), CFfConvert().AV_PIX_FMT_YUV420P,
					  w, h, fps, 1000000, CSqMulti() ) )
		_self.echo( "Failed to create encoder" );

	local img = CSqImage();
	if ( !img.Create( w, h ) )
	{	_self.echo( "Failed to create image buffer" ); return; }

	local pix = CSqBinary();
	if ( !img.refPixels( pix ) )
	{	_self.echo( "Failed to obtain image buffer pointer" ); return; }

	local num_frames = sec * fps;
	local frame = CSqBinary(), inf = CSqMulti();
	for ( local i = 0; i < num_frames; i++ )
	{
		// Blank the image
		pix.Zero();

		// Draw text
		local min = mins + ( i / fps / 60 ).tointeger();
		local sec = ( ( i / fps ) % 60 ).tointeger();
		local f = ( i % fps ).tointeger();
		local str = format( "%0.2d:%0.2d:%0.2d.%0.2d", hrs, min, sec, f );

		::_self.print( str + "\r" );

		font.setColor( 255, 0, 0 );
		font.DrawImg( str, CSqPos( 9, 59 ), img, 1 );
		font.setColor( 255, 255, 255 );
		font.DrawImg( str, CSqPos( 10, 60 ), img, 1 );

		// Encode image
		if ( !enc.EncodeImage( img, frame, inf ) )
		{	/*_self.echo( "Failed to encode frame" ); return; */ }

		// Write the frame to the file
		if ( !avi.WriteFrame( frame, inf ) )
		{	_self.echo( "Failed to write to avi file" ); return; }

		// Audio?
		if ( aid )
		{
			if ( abuf )
			{
				local bsize = abuf.getUsed() / fps * _self.type_size( _self.tFloat ) * ach;
				local sub = abuf.getSub( bsize * aoff, bsize );
				if ( fps <= ++aoff ) aoff = 0;

				// Buffer the audio frame
				aenc.BufferData( sub );

				// Encode audio data
				local inf = CSqMulti();
				while ( aenc.Encode( CSqBinary(), aframe, inf ) )
				{
//					_self.echo( "AUDIO = " + aframe.getUsed() + ", pts = " + aenc.getPts() );

					// Write audio data
					if ( aframe.getUsed() )
					{
						if ( !avi.WriteAudioFrame( aframe, 0, 0, CSqMulti() ) )
							_self.echo( "!!! Error writing audio frame to video stream" );
						aframe.setUsed( 0 );

					} // end if

				} // end while

			} // end else

		} // end if

	} // end for

}

function create_clips( root, t, inv, ts, te, devs, empty, w, h, fps, fmt, tone, afmt, sec, font )
{
	// Current time
	local gmt = _self.gmt_time();

	// Get the starting time stamp
	if ( 0 > t ) t = gmt + t;

	// Alias time to interval
	local t = t - ( t % inv );

	// Create clips
	local time = CSqTime();
	while ( t < gmt )
	{
		// Start time
		time.SetUnixTime( t );
//		local file = ::_self.build_path( root, time.FormatTime( ts ) );
		local file = time.FormatTime( ts );

		// End time
		time.SetUnixTime( t + inv );
		file += time.FormatTime( te );

		// Ensure folders exist
		local path = CSqFile().get_path( file );
		if ( devs.len() )
			foreach( d in devs )
				CSqFile().mkdir( ::_self.build_path( ::_self.build_path( root, d ), path ) );
		else
			CSqFile().mkdir( ::_self.build_path( root, path ) );

		// Empty files?
		if ( empty )
		{
			if ( devs.len() )
				foreach( d in devs )
					CSqFile().put_contents( ::_self.build_path( ::_self.build_path( root, d ), file ),
											d + " : " + file );

			else
				CSqFile().put_contents( ::_self.build_path( root, file ), file );

		} // end if

		// Create this clip
		else
			create_clip( root, file, devs, w, h, fps, fmt, tone, afmt, inv, font,
						 ( t % ( 24 * 60 * 60 ) / ( 60 * 60 ) ).tointeger(),
						 ( t % ( 60 * 60 ) / 60 ).tointeger() );

		// Next interval
		t += inv;

	} // end while

}

// File maybe
local file = _self.get( "/", "cmdline.f" );

// Output folder
local out = _self.get( "/", "cmdline.1" );
if ( !out.len() && !file.len() )
{	_self.echo( "Please specify a destination folder or filename" );
	return;
} // end if

// Non-zero for empty files
local empty = _self.isset( "/", "cmdline.e" ) ? 1 : 0

// Device list
local d = _self.get( "/", "cmdline.d" ), devs = [];
if ( d.len() ) devs = split( d, "," );

// Image width
local w = _self.tolong( _self.get( "/", "cmdline.w" ) );
if ( 0 >= w || 2000 < w ) w = 320;

// Image height
local h = _self.tolong( _self.get( "/", "cmdline.h" ) );
if ( 0 >= h || 2000 < h ) h = ( w * 3 / 4 ).tointeger();

// Frames per second
local fps = _self.tolong( _self.get( "/", "cmdline.fps" ) );
if ( 0 >= fps || 100 < fps ) fps = 15;

// Codec
local fmt = _self.get( "/", "cmdline.fmt" );
if ( !fmt.len() ) fmt = "MP42";

// Clip interval
local inv = _self.tolong( _self.get( "/", "cmdline.inv" ) );
if ( 0 >= inv ) inv = 15 * 60;

// Start timestamp filename template
local ts = _self.get( "/", "cmdline.ts" );
if ( !ts.len() ) ts = "%Y%c%d/R-%g%m%s";

// End timestamp filename template
local te = _self.get( "/", "cmdline.te" );
if ( !te.len() ) te = "-%g%m%s-" + ( fps * inv ) + ".avi";

// Time frame
local t = _self.tolong( _self.get( "/", "cmdline.t" ) );
if ( !t ) t = -3600;

// Audio tone?
local tone = _self.tolong( _self.get( "/", "cmdline.tone" ) );
local afmt = _self.get( "/", "cmdline.afmt" );
if ( !afmt.len() ) afmt = "AC3";

// Initialize free type library
local ft = CFtLibrary();
if ( ft.getLastError() )
{	_self.echo( "freetype2 failed to initialize : " + ft.getLastErrorStr() );
	return 0;
} // end if

// Create font
local fname = "cour.ttf";
local font = create_font( fname, ft );
if ( !font )
{	_self.echo( "Failed to create font : " + fname );
	return 0;
} // end if

// Single file
if ( file.len() )
	create_clip( "", file, w, h, fps, fmt, tone, afmt, inv, font,
				 _self.tolong( _self.get( "/", "cmdline.hrs" ) ),
				 _self.tolong( _self.get( "/", "cmdline.min" ) ) );

// Clip folder structure
else
	create_clips( out, t, inv, ts, te, devs, empty, w, h, fps, fmt, tone, afmt, 15 * 60, font );

