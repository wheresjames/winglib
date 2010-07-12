
_self.load_module( "ffmpeg", "" );
_self.load_module( "portaudio", "" );

class CGlobal
{
	blocking = 0;

	f = CFfContainer();
	pa = CPaOutput();

	dec = CSqBinary();
	frame = CSqBinary();
};

local _g = CGlobal();

function _init() : ( _g )
{
//	local file = _self.path( "../media/440hz.ogg" );
	local file = _self.path( "../../../../vhe.avi" );
//	local file = _self.path( "../../../../vhe.mp3" );
//	local file = _self.path( "../../../../vhe.mpg" );
//	local file = _self.path( "../../../../vhe.ogg" );
//	local file = _self.path( "../../../../vhe.wav" );

	_self.echo( "=====================================================" );
	_self.echo( " Playing  : " + file );
	_self.echo( " Blocking : " + _g.blocking );
	_self.echo( "=====================================================" );

	if ( !_g.f.Open( file, CSqMulti() ) )
	{	_self.echo( "failed to open file" );
		return;
	} // end if

	if ( 0 > _g.f.getAudioStream() )
	{	_self.echo( "File contains no audio" );
		return;
	} // end if

	if ( 0 > _g.f.isAudioCodec() )
	{	_self.echo( "Failed to create audio codec" );
		return;
	} // end if

	_self.echo( "Codec ID       : " + _g.f.getAudioCodecID() );
	_self.echo( "Codec Type     : " + _g.f.getAudioType() );
	_self.echo( "Codec Name     : " + _g.f.getAudioName() );
	_self.echo( "Channels       : " + _g.f.getAudioChannels() );
	_self.echo( "Sample Rate    : " + _g.f.getAudioSampleRate() );
	_self.echo( "Bit Rate       : " + _g.f.getAudioBitRate() );
	_self.echo( "Frame Size     : " + _g.f.getAudioFrameSize() );
	_self.echo( "=====================================================" );

	local fmt;
	switch( _g.f.getAudioBitsPerSample() )
	{	case 8 : fmt = CPaOutput().paUInt8; break;
		default : fmt = CPaOutput().paInt16; break;
	} // end switch

	local fsize = _g.f.getAudioFrameSize();
	if ( !_g.pa.Open( _g.blocking, _g.pa.getDefaultOutputDevice(),
					  _g.f.getAudioChannels(), fmt, 0.2,
				      _g.f.getAudioSampleRate().tofloat(), fsize ) )
	{   _self.echo( "!!! Failed to open output stream : " + _g.pa.getLastError() );
		return 0;
	} // end if

	if ( !_g.pa.Start() )
	{   _self.echo( "!!! Failed to start output stream : " + pa.getLastError() );
		return 0;
	} // end if
}

function _idle() : ( _g )
{
	if ( !_g.f.isOpen() )
	{
		// Wait on the whole thing to play
		if ( _g.pa.getBufferedBytes() )
			return 0;

		_self.echo( "---- SUCCESS ----" );

		return 1;

	} // end if

	local frame_info = CSqMulti();
	local min_buf = 64 * 1024;

	if ( _g.pa.getBufferedBytes() < min_buf )
	{
		_self.echo( "Buffer at : " + _g.pa.getBufferedBytes() );

		while ( _g.pa.getBufferedBytes() < min_buf )
		{
			// Read frame
			if ( 0 > ( stream = _g.f.ReadFrame( _g.frame, frame_info ) ) )
			{	_self.echo( "End of stream!" );
				_g.f.Destroy();
				return 0;
			} // end if

			// Is it audio?
			if ( _g.f.getAudioStream() == stream )
			{
				_self.echo( "Read : " + _g.frame.getUsed() );

				// Decode freme
				if ( !_g.f.DecodeAudioFrameBin( _g.frame, _g.dec, frame_info ) )
				{	_self.echo( "Failed to decode audio frame" );
					return -1;
				} // end if

				// Add to buffer
				if ( _g.dec.getUsed() )
					if ( !_g.pa.Write( _g.dec, _g.dec.getUsed() / _g.pa.getFrameBytes() ) )
					{	_self.echo( "Failed to write audio data" );
						return -1;
					} // end if

			} // end if

		} // end while

	} // end while

	return 0;
}

