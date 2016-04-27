
_self.load_module( "ffmpeg", "" );

local avi = CFfContainer();
local link = "E:/nobck/temp/pano/20100813/R-213000-214500-27000.avi";

if ( !avi.Open( link, CSqMulti() ) )
{	_self.echo( "Failed to open : " + link );
	return 0;
} // end if

// Seek to a position
if ( !avi.Seek( avi.getVideoStream(), 4400, 4 ) )
//if ( !avi.Seek( -1, 2250, 4 ) )
{	_self.echo( "Seek failed : " + link );
//	return 0;
} // end if

// Find a frame
local i = 0, frame = CSqBinary(), frame_info = CSqMulti();
while ( 0 <= ( i = avi.ReadFrame( frame, frame_info ) ) 
		&& ( i != avi.getVideoStream() || !frame_info[ "flags" ].toint() ) )
		_self.echo( frame_info.print_r( 1 ).tostring() )
	;

if ( 0 > i )
{	_self.echo( "No video : " + link );
	return 0;

} // end if

_self.echo( frame_info.print_r( 1 ) );

// Decode frame
local decframe = CSqBinary();
if ( !avi.DecodeFrameBin( frame, CFfFmt().AV_PIX_FMT_BGR24, decframe, frame_info, 1 ) )
{	_self.echo( "Failed to decode frame : " + link );
	return 0;
} // end if

// Create image object
local img = CSqImage();
if ( !img.Create( avi.getWidth(), avi.getHeight() ) )
{	_self.echo( "Failed to create image : " + link );
	return 0;
} // end if

// Copy image data
img.setPixels( decframe );

// Save as jpeg
if ( !img.Save( _self.root( "seek_test.jpg" ), "" ) )
{	_self.echo( "Failed to create image : " + seek_test.jpg );
	return 0;
} // end if

_self.echo( "Created : seek_test.jpg" );

