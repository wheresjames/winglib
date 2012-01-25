
_self.load_module( "irrlicht", "" );
_self.load_module( "live555", "" );
_self.load_module( "ffmpeg", "" );
_self.load_module( "portaudio", "" );

class CRtspStream
{
	bDecodeVideo = 1;
	bDecodeAudio = 1;
	bPlayAudio = 1;

//	szDumpVideo = "";
	szDumpVideo = "rawvideo";

	file = "";
//	file = "rtsptest.avi";
	rec_avi = 0;

	rtsp = 0;

	dec = 0;
	frame = 0;
	vfail = 0;

	adec = 0;
	araw = 0;
	aframe = 0;	
	afail = 0;

	pa = 0;
	vb = 0;
	
	frame_count = 0;
	last_frame = 0;

	aframe_count = 0;
	last_aframe = 0;
	
	max_audio_buffer = 1000000;
	max_video_buffer = 2000000;
	video_offset = 300000;
	
	vix = 0;

	function getWidth() 
	{
		return rtsp ? rtsp.getWidth() : 0;
	}

	function getHeight() 
	{
		return rtsp ? rtsp.getHeight() : 0;
	}

	function isReady()
	{
		return ( rtsp && rtsp.waitInit( 0 ) && rtsp.isOpen() ) ? 1 : 0;
	}

	function Close()
	{
		if ( rtsp )
			rtsp.Destroy(), rtsp = 0;

		if ( dec )
			dec.Destroy(), dec = 0;

		frame = 0;
		vfail = 0;

		if ( rec_avi )
			rec_avi.Destroy(), rec_avi = 0;

		if ( adec )
			adec.Destroy(), dec = 0;

		araw = 0;
		aframe = 0;
		afail = 0;

		pa = 0;

		vix = 0;
	}

	function Play( link, user = "", pass = "" )
	{
	
		Close();

		::_self.echo( "\nConnecting to : " + link + "\n" );

		rtsp = CLvRtspClient();
//		rtsp.setStreamOverTCP( 0 );
//		rtsp.setTunnelOverHTTPPort( 0 );

		if ( szDumpVideo.len() )
			CSqFile().mkdir( szDumpVideo );

		local params = CSqMulti();

		// Username / Password
		if ( user.len() )
			params[ "username" ] <- user,
			params[ "password" ] <- pass;

		rtsp.setBlindLogin( 1 );

		if ( !rtsp.Open( link, bDecodeVideo, bDecodeAudio, params ) )
		{	::_self.echo( "Failed to open RTSP stream : " + link );
			return 0;
		} // end if

		return 1;
	}

	function CreateDecoder()
	{
		if ( !rtsp.isVideo() )
			::_self.echo( "iii no video" );

		else
		{
			// Insert H264 Header if needed
			if ( "H264" == rtsp.getVideoCodecName() )
				rtsp.setVideoHeader( CSqBinary( "\x00\x00\x01" ) );

//			local w = rtsp.getWidth(), h = rtsp.getHeight(), fps = rtsp.getFps();
			local w = 1922, h = 1080, fps = rtsp.getFps();
			::_self.echo( "iii Creating video decoder for " + rtsp.getVideoCodecName() 
						   + " / " + w + " x " + h + " x " + fps );

			dec = CFfDecoder();
			dec.setExtraData( rtsp.getExtraVideoData() );
			if ( !dec.Create( CFfDecoder().LookupCodecId( rtsp.getVideoCodecName() ), 
							  CFfConvert().PIX_FMT_YUV420P,
							  w, h, fps,
							  2000000, CSqMulti( "cmp=-2" ) ) )
			{	::_self.echo( "!!! Failed to create decoder for " + rtsp.getVideoCodecName() );
				dec = 0;
				vfail = 1;
//				return 0;
			} // end if

			else
				frame = CSqBinary();

		} // end else

		if ( !rtsp.isAudio() )
			::_self.echo( "iii no audio" );

		else
		{
			::_self.echo( "iii Creating audio decoder for " + rtsp.getAudioCodecName() 
						  + " : channels = " + rtsp.getNumAudioChannels()
						  + " : rate = " + rtsp.getAudioSampleRate()
						  + " : bps = " + rtsp.getAudioBps() );

			adec = CFfAudioDecoder();
			//::_self.echo( rtsp.getExtraAudioData().AsciiHexStr( 16, 16 ) );
			adec.setExtraData( rtsp.getExtraAudioData() );
			if ( !adec.Create( CFfAudioDecoder().LookupCodecId( rtsp.getAudioCodecName() ),
							   0, 0, 0 ) )
			{	::_self.echo( "!!! Failed to create decoder for " + rtsp.getAudioCodecName() );
				adec = 0;
				afail = 1;
//				return 0;
			} // end if

			else
			{
				araw = CSqBinary(), aframe = CSqBinary();

				if ( bPlayAudio )
				{
					pa = CPaOutput();
					pa.setGlitchDetection( 0 );
					local fmt;
					switch( 16 )
					{	case 8 : fmt = CPaOutput().paUInt8; break;
						default : fmt = CPaOutput().paInt16; break;
					} // end switch
					local fsize = 2;

					if ( !pa.Open( 0, pa.getDefaultOutputDevice(), 
									  rtsp.getNumAudioChannels(), 
									  fmt, 0.2, rtsp.getAudioSampleRate().tofloat(), fsize ) )
					{   ::_self.echo( "!!! Failed to open output stream : " + pa.getLastError() );
						pa = 0;
					} // end if

					else if ( !pa.Start() )
					{   ::_self.echo( "!!! Failed to start output stream : " + pa.getLastError() );
						pa = 0;
					} // end if
					
					else
						::_self.echo( "Started audio device" );

				} // end if

			} // end else

		} // end else

		// Do we want to record the stream?
		if ( file.len() && !rec_avi )
			RecordToFile( file, 
						  rtsp.getVideoCodecName(), rtsp.getWidth(), rtsp.getHeight(), rtsp.getFps(),
						  rtsp.getAudioCodecName(), rtsp.getNumAudioChannels(), rtsp.getAudioSampleRate(), 0 );

		::_self.echo( " !!! STARTING RTSP STREAM !!!" );

		rtsp.Play();

		return 1;
	}

	function PlayAudio()
	{
		if ( afail || !isReady() || !rtsp.isAudio() )
			return 0;

		if ( !adec ) 
		{	CreateDecoder()
			if ( afail )
				return 0;
		} // end if

		local frames_rx = 0;
		while ( rtsp.LockAudio( aframe, 0 ) )
		{
			frames_rx++;

			if ( aframe.getUsed() )
			{
				// Debugging
				aframe_count++;
				if ( last_aframe > rtsp.getAudioPts() )
				{	::_self.echo( "\r\n\r\n!!!!!!!!!!!!!!!! AUDIO BACKWARD AT frame " + aframe_count );
					::_self.echo( "at = " + last_aframe + " -> " + rtsp.getAudioPtsSec() + "." + rtsp.getAudioPtsUSec() + " = " + rtsp.getAudioPts() + "\r\n" );
				} // end if
				last_aframe = rtsp.getAudioPts();

				// Are we recording?
				if ( rec_avi )
					if ( !rec_avi.WriteAudioFrame( aframe, rtsp.getAudioPts(), rtsp.getAudioDts(), CSqMulti() ) )
						::_self.echo( "!!! Failed to write audio frame" );

				while ( 0 < adec.Decode( aframe, araw, CSqMulti() ) )
					if ( pa && araw.getUsed() )
					{
						if ( pa.getBufferedBytes() > max_audio_buffer )
							::_self.echo( "dropping audio : " + pa.getBufferedBytes() );

						else if ( !pa.WriteTs( araw, araw.getUsed() / pa.getFrameBytes(), 
											   rtsp.getAudioPts() ) )
							::_self.echo( "Failed to write audio data" );

						aframe.setUsed( 0 );

					} // end if

			} // end if

			rtsp.UnlockAudio();

		} // end if

		return frames_rx;
	}

	function Draw( buffer )
	{
		if ( vfail || !isReady() || !rtsp.isVideo() )
			return 0;

		if ( !dec ) 
		{	CreateDecoder()
			if ( vfail )
				return 0;
		} // end if

		// Create video buffer if needed
		if ( pa && !vb )
		{	vb = CSqFifoShare();
			local fps = rtsp.getFps(); if ( 0 >= fps ) fps = 30;
			if ( !vb.Create( "", "", max_video_buffer, 60 * fps, "" ) ) 
			{	::_self.echo( "!!! " + vb.getLastErrorStr() ); vb = 0; }
		} // end if

		// Buffer frames
		// Note, we're buffering the compressed frames to save memory ;)
		while ( rtsp.LockVideo( frame, 0 ) )
		{
//			::_self.echo( frame.AsciiHexStr( 16, 4 ) );

			frame_count++;
			if ( last_frame > rtsp.getVideoPts() )
			{	::_self.echo( "\r\n\r\n!!!!!!!!!!!!!!!! VIDEO BACKWARD AT frame " + frame_count );
				::_self.echo( " vt = " + last_frame + " -> " + rtsp.getVideoPtsSec() + "." + rtsp.getVideoPtsUSec() + " = " + rtsp.getVideoPts() + "\r\n" );
			} // end if
			last_frame = rtsp.getVideoPts();

			// Dump raw frames
			if ( szDumpVideo.len() )
				CSqFile().put_contents_bin( ::_self.build_path( szDumpVideo, "v" + vix++ + ".raw" ), frame );

			// Are we recording?
			if ( rec_avi )
				rec_avi.WriteVideoFrame( frame, rtsp.getVideoPts(), rtsp.getVideoDts(), CSqMulti() );

			// Buffer for later if syncing to audio
			if ( vb ) vb.Write( frame, "", 0, rtsp.getVideoPts() );

			// Otherwise, just send it straight to the decoder
			else dec.BufferData( frame, CSqMulti() );

			rtsp.UnlockVideo();			

		} // end if

		// Just draw it if we're not syncing to audio
		if ( !vb )
		{	if ( 0 >= dec.getBufferSize() ) return 0;
			return dec.Decode( CSqBinary(), CFfConvert().PIX_FMT_RGB32, buffer, CSqMulti(), 0 );
		} // end if

		// Show error if more than a second off
		local diff = vb.ReadTs() - pa.getTs();
		if ( 1000000 < diff || -1000000 > diff )
		{	::_self.echo( "\r\n\r\n!!!!!!!!!!!!!!!! SYNC ERROR" );
			::_self.echo( "D = " + diff + ", V = " + vb.ReadTs() + ", A = " + pa.getTs() + "\r\n" );
		} // end if

		// Decode up to the audio position
		// +++ using 'while' here causes instant recovery from a glitch, 
		//     but 'if' seems to actually be smoother in some real world conditions
//		if
		while
		 ( vb.isRead() && vb.ReadTsMin() < ( pa.getTs() - video_offset ) ) 
			dec.Decode( vb.ReadData(), CFfConvert().PIX_FMT_RGB32, buffer, CSqMulti(), 0 ), vb.incReadPtr();

		// Catchup and show, haha, can't do this, must decode every frame, duh
//		while ( vb.isRead() && vb.ReadTs() < ( pa.getTs() - video_offset ) ) vb.incReadPtr()
//		if ( vb.isRead() ) dec.Decode( vb.ReadData(), CFfConvert().PIX_FMT_RGB32, buffer, CSqMulti(), 0 );

		return 1;
	}

	function RecordToFile( file, vfmt, w, h, fps, afmt, ch, sr, bps )
	{
		rec_avi = CFfContainer();

		if ( 0 >= fps )
			fps = 15;

		if ( afmt.len() )
			rec_avi.setAudioExtraData( rtsp.getExtraAudioData() );

		if ( !rec_avi.Create( file, "", CSqMulti() ) )
			::_self.echo( "Failed to create avi" );
		else if ( vfmt.len() && 0 > rec_avi.AddVideoStream( CFfDecoder().LookupCodecId( vfmt ), w, h, fps ) )
			::_self.echo( "Failed to add video stream" );
		else if ( afmt.len() && 0 > rec_avi.AddAudioStream( CFfAudioDecoder().LookupCodecId( afmt ), ch, sr, bps ) )
			::_self.echo( "Failed to add audio stream" );
		else if ( !rec_avi.InitWrite() )
			::_self.echo( "Failed to initiailze avi" );
		else
			::_self.echo( "iii Saving to file : " + file );	
		
	}	

}

class CGlobal
{
	rtsp_sources =
	{
		yt1			= [ "yt1",	"rtsp://v8.cache1.c.youtube.com/CjgLENy73wIaLwlnoDu0pt7zDRMYESARFEIJbXYtZ29vZ2xlSARSB3Jlc3VsdHNgnLTe56Djt-FNDA==/0/0/0/video.3gp" ],
		yt2			= [ "yt2",	"rtsp://v4.cache8.c.youtube.com/CjgLENy73wIaLwkU67OEyLSkyBMYESARFEIJbXYtZ29vZ2xlSARSB3Jlc3VsdHNgzoOa_IDtxOFNDA==/0/0/0/video.3gp" ],
		yt3			= [ "yt3",	"rtsp://v1.cache7.c.youtube.com/CjgLENy73wIaLwmY52udh9o1TRMYESARFEIJbXYtZ29vZ2xlSARSB3Jlc3VsdHNghbzo27OzxJ1ODA==/0/0/0/video.3gp" ],
	};

	quit = 0;

	irr = 0;
	cube = 0;
	tex = 0;
	stream = 0;
};

local _g = CGlobal();

function _init() : ( _g )
{
	_g.irr = CSqIrrlicht();

//	_g.irr.SetStereo( 1 );
	_self.echo( "...Initializing...\n" );
	_g.irr.Init( "RTSP Test", 640, 480, 1 );

	_self.echo( "...setting ambient light...\n" );
	_g.irr.SetAmbientLight( CSqirrColorf( 0.5, 0.5, 0.5 ) );

	_self.echo( "...adding lights...\n" );
	_g.irr.AddLight( CSqirrVector3d( 0., 100., 0. ), CSqirrColorf( 1., 1., 1. ), 100. );
	_g.irr.AddLight( CSqirrVector3d( 0., 100., 50. ), CSqirrColorf( 1., 1., 1. ), 100. );

//	_self.echo( "adding sky...\n" );
//	_g.irr.AddSkyDome( _self.path( "../imgs/sky.png" ), 16, 16, 100., 100. );

	_self.echo( "...adding camera...\n" );
//	local cam = _g.irr.AddCamera( CSqirrVector3d( 0, 10, 15 ), CSqirrVector3d( 0, 0, 0 ) );
	local cam = _g.irr.AddCamera( CSqirrVector3d( 0, 0, 15 ), CSqirrVector3d( 0, 0, 0 ) );
//	cam.SetLens( 1., 2.4, 3.2 );

    _g.cube = _g.irr.AddGrid( 50., 50., 1, 1, 0., 2, CSqirrColor( 255, 255, 255 ), 2 );
	_g.cube.SetPosition( CSqirrVector3d( -25, -25, -25 ) );

	_self.echo( "...adding cube...\n" );
//	_g.cube = _g.irr.AddCube( 10. );

	_self.echo( "...adding animator...\n" );
//	local ani = _g.irr.AddRotateAnimator( CSqirrVector3d( 0, 0.4, 0 ) );
//	_g.cube.AddAnimator( ani );

	// Set draw function
	_self.set_timer( ".", 30, "OnDraw" )

	// Which stream to use
	local link = ::_self.get( "/", "cmdline.1" );
	if ( !link.len() )
	{	local stream = ::_self.get( "/", "cmdline.s" );
		if ( !stream.len() || !( stream in _g.rtsp_sources ) ) stream = "yt3";
		link = _g.rtsp_sources[ stream ][ 1 ];
	} // end if

	// Start the video stream
	_g.stream = CRtspStream();
	_g.stream.Play( link, _self.get( "/", "cmdline.u" ), _self.get( "/", "cmdline.p" ) );

	return 0;
}

function _idle() : ( _g )
{
	return _g.quit;
}

function OnDraw() : ( _g )
{
//	::_self.echo( "." );

	// Keep audio playing
	_g.stream.PlayAudio();

	if ( !_g.tex )
	{
		// Null draw to get size
		if ( 0 >= _g.stream.getWidth() || 0 >= _g.stream.getHeight() )
			_g.stream.Draw( CSqBinary() );
	
		// Decode a frame to get the width / height
		else if ( 0 < _g.stream.getWidth() && 0 < _g.stream.getHeight() )
		{	_g.tex = _g.irr.CreateTexture( _g.stream.getWidth(), _g.stream.getHeight(), 0 );
			if ( _g.tex )
			{	_g.cube.SetTexture( 0, _g.tex );
				_self.echo ( "iii Created texture " + _g.stream.getWidth() + " x " + _g.stream.getHeight() );
			} // end if
			else _self.echo( "!!! Error creating texture " + _g.stream.getWidth() + " x " + _g.stream.getHeight() );
		} // end if

	} // end if

	else
	{
		local buffer = _g.tex.Lock();
		if ( buffer.getUsed() )
			_g.stream.Draw( buffer ),
			_g.tex.Unlock();

	} // end else

	_g.quit = _g.irr.Draw( CSqirrColor( 100, 100, 100 ) );

	return 0;
}

