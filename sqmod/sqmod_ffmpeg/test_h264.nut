
_self.load_module( "ffmpeg", "" );

local cam = CVidDecoder();

if ( !cam.Create( CVidDecoder().CODEC_ID_H264 ) )
	_self.echo( "failed to create codec" );

else
	_self.echo( "codec created" );




