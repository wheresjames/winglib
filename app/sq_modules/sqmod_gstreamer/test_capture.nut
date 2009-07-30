
_self.echo( "Testing gstreamer..." );

_self.load_module( "gstreamer", "" );

local cam = CGsCapture();

if ( cam.Init() )
	_self.echo( "Init() failed" );






