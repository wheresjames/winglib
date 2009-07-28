
_self.load_module( "ffmpeg", "" );

function _init()
{
	local enc = CFfEncoder();

	_self.echo( "Creating encoder..." );

	if ( !enc.Create( CFfEncoder().CODEC_ID_MPEG4 ) )
		_self.echo( "failed to create codec" );

	else
		_self.echo( "codec created" );

}


