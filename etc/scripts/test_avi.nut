
// Show a list of available devices
_self.echo( " --- AVI TEST ---" );

// Load modules
if ( _self.load_module( "freetype2", "" ) )
{	_self.echo( "!!! Freetype module failed to load" );
	return;
} // end if

if ( _self.load_module( "ffmpeg", "" ) )
{	_self.echo( "!!! FFMPEG module failed to load" );
	return;
} // end if

local vcodec = "MP42";
local w = 320, h = 240, fps = 15;
local fname = "cour.ttf";

// *** Create Font ***

// Initialize free type library
local flib = CFtLibrary();
if ( flib.getLastError() )
{	_self.echo( "!!! freetype2 failed to initialize : " + flib.getLastErrorStr() );
	return 0;
} // end if

local font = CFtFace();
if ( !flib.LoadFont( font, fname, 0 ) )
{	_self.echo( "!!! freetype2 failed to load font '" + fname + "' : " + ft.getLastError() );
	return 0;
} // end if

// Set font size
font.setCharSize( 25 * 64, 15 * 64, 100, 0 );

// *** A/V CONTAINER ***

// Create avi file
local avi = CSqAviFile();
if ( !avi.Create( "test.avi" ) )
{	_self.echo( "!!! Failed to create avi" );
	return;
} // end if

// Lookup video codec
local vcid = avi.fourCC( vcodec );
_self.echo( "Codec ID : " + vcodec + " -> " + vcid );

// Add stream
local vs = avi.AddVideoStream( vcid, w, h, fps.tofloat() );
_self.echo( "Video Stream : " + vs );
if ( 0 > vs )
{	_self.echo( "!!! Failed to add video stream" );
	return;
} // end if

// *** VIDEO ENCODER ***

local q = 10, p = CSqMulti();
p[ "encoder_params" ][ "quality" ] <- q.tostring();
p[ "encoder_params" ][ "profile" ] <- "baseline";    // "baseline";
p[ "encoder_params" ][ "preset"  ] <- "ultrafast";   // "ultrafast";
p[ "encoder_params" ][ "tune"    ] <- "zerolatency"; // "zerolatency";

local venc = CFfEncoder();
if ( !venc.Create( CFfDecoder().LookupCodecId( vcodec ), CFfConvert().PIX_FMT_YUV420P,
				  w, h, fps, 0, p ) )
{	_self.echo( "!!! Failed to create video encoder" );
	return;
} // end if

local img = CSqImage();
if ( !img.Create( w, h ) )
{	_self.echo( "!!! Failed to create image buffer" );
	return;
} // end if

local pix = CSqBinary();
if ( !img.refPixels( pix ) )
{	_self.echo( "!!! Failed to obtain image buffer pointer" );
	return;
} // end if

_self.echo( "Writing frames..." ); _self.flush();

local dts = 0, pts = 0;
local num_frames = 10 * fps;
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
	_self.print( str + "\r" ); _self.flush();

	font.setColor( 128, 0, 0 );
	font.DrawImg( str, CSqPos( 9, 59 ), img, 1 );
	font.setColor( 255, 0, 0 );
	font.DrawImg( str, CSqPos( 10, 60 ), img, 1 );

	inf[ "pts" ] <- pts.tostring();
	
	// Encode image
	venc.EncodeImage( img, frame, inf );

	// Write the frame to the file
	if ( frame.getUsed() )
		if ( !avi.AddVideoFrame( 0, 0, frame ) )
		{	_self.echo( "WriteFrame() failed" ); return 0; }

	dts++;
	pts++;

} // end for


_self.echo( "\nSUCCESS" );

// Have to be destroyed in this order
font.Destroy();
flib.Destroy();
