
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

//	file = "";
	file = "rtsptest.avi";
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

	function Play( link )
	{
	
		Close();

		::_self.echo( "\nConnecting to : " + link + "\n" );

		rtsp = CLvRtspClient();

//		rtsp.setStreamOverTCP( 0 );
//		rtsp.setTunnelOverHTTPPort( 0 );

		if ( szDumpVideo.len() )
			CSqFile().mkdir( szDumpVideo );

		if ( !rtsp.Open( link, bDecodeVideo, bDecodeAudio, CSqMulti() ) )
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
//			if ( "H264" == rtsp.getVideoCodecName() )
//				rtsp.setVideoHeader( CSqBinary( "\x00\x00\x01" ) );

			::_self.echo( "iii Creating video decoder for " + rtsp.getVideoCodecName() );

			dec = CFfDecoder();
			dec.setExtraData( rtsp.getExtraVideoData() );
			if ( !dec.Create( CFfDecoder().LookupCodecId( rtsp.getVideoCodecName() ), 
							  CFfConvert().PIX_FMT_YUV420P,
							  rtsp.getWidth(), rtsp.getHeight(), rtsp.getFps(),
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

		if ( !rtsp.LockAudio( aframe, 0 ) )
			return 0;
	
//		::_self.echo( "at = " + rtsp.getAudioPtsSec() + "." + rtsp.getAudioPtsUSec() + " = " + rtsp.getAudioPts() );

		if ( aframe.getUsed() )
		{
//			::_self.echo( "IN_ATS = " + rtsp.getAudioPts() );
		
			// Are we recording?
			if ( rec_avi )
//				if ( !rec_avi.WriteAudioFrame( aframe, rtsp.getAudioPts(), rtsp.getAudioDts(), CSqMulti() ) )
				if ( !rec_avi.WriteAudioFrame( aframe, 0, 0, CSqMulti() ) )
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

		return 0;
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

//		::_self.echo( " vt = " + rtsp.getVideoPtsSec() + "." + rtsp.getVideoPtsUSec() + " = " + rtsp.getVideoPts() );

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

			// Dump raw frames
			if ( szDumpVideo.len() )
				CSqFile().put_contents_bin( ::_self.build_path( szDumpVideo, "v" + vix++ + ".raw" ), frame );
		
			// Are we recording?
			if ( rec_avi )
				rec_avi.WriteVideoFrame( frame, 0, 0, CSqMulti() );
//				rec_avi.WriteVideoFrame( frame, rtsp.getVideoPts(), rtsp.getVideoDts(), CSqMulti() );
		
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

//		::_self.echo( "ATS = " + vb.ReadTs() );
		
		// Decode up to the audio position
		if ( vb.isRead() && vb.ReadTs() < ( pa.getTs() - video_offset ) )
			dec.Decode( vb.ReadData(), CFfConvert().PIX_FMT_RGB32, buffer, CSqMulti(), 0 ), vb.incReadPtr();

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
	// http://www.americafree.tv/VLC/
	rtsp_sources =
	{
		adventure	= [ "Adventure",	"rtsp://video3.americafree.tv/AFTVAdventureH2641000.sdp" ],
		cartoons	= [ "Cartoons",		"rtsp://video3.americafree.tv/AFTVCartoonsH2641000.sdp" ],
		classic 	= [ "Classic", 		"rtsp://video3.americafree.tv/AFTVClassicsH2641000.sdp" ],
		comedy 		= [ "Comedy", 		"rtsp://video3.americafree.tv/AFTVComedyH2641000.sdp" ],
	
/*	
		nasa 		= [ "NASA", 		"rtsp://a1352.l1857053128.c18570.g.lq.akamaistream.net/D/1352/18570/v0001/reflector:53128" ],
		adventure 	= [ "Adventure", 	"rtsp://video3.multicasttech.com/AFTVAdventure3GPP296.sdp" ],
		cartoons 	= [ "Cartoons", 	"rtsp://video2.multicasttech.com/AFTVCartoons3GPP296.sdp" ],
		classic 	= [ "Classic", 		"rtsp://video3.multicasttech.com/AFTVClassics3GPP296.sdp" ],
		comedy 		= [ "Comedy", 		"rtsp://video3.multicasttech.com/AFTVComedy3GPP296.sdp" ],
		crime 		= [ "Crime", 		"rtsp://video2.multicasttech.com/AFTVCrime3GPP296.sdp" ],
		halloween	= [ "Halloween", 	"rtsp://video3.multicasttech.com/AFTVHalloween3GPP296.sdp" ],
		horror		= [ "Horror", 		"rtsp://video2.multicasttech.com/AFTVHorror3GPP296.sdp" ],
		indy		= [ "Indy Movies", 	"rtsp://video2.multicasttech.com/AFTVIndyMovies3GPP296.sdp" ],
		mystery		= [ "Mystery", 		"rtsp://video2.multicasttech.com/AFTVMystery3GPP296.sdp" ],
		scifi		= [ "SciFi", 		"rtsp://video2.multicasttech.com/AFTVSciFi3GPP296.sdp" ],
		western		= [ "Westerns", 	"rtsp://video2.multicasttech.com/AFTVWesterns3GPP296.sdp" ],
		espana		= [ "Espana", 		"rtsp://video3.multicasttech.com/EspanaFree3GPP296.sdp" ],
*/
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

	// Start the video stream
	_g.stream = CRtspStream();
	_g.stream.Play( _g.rtsp_sources[ "yt3" ][ 1 ] );
//	_g.stream.Play( _g.rtsp_sources[ "adventure" ][ 1 ] );
//	_g.stream.Play( _g.rtsp_sources[ "comedy" ][ 1 ] );

	return 0;
}

function _idle() : ( _g )
{
	return _g.quit;
}

function OnDraw() : ( _g )
{
	// Keep audio playing
	_g.stream.PlayAudio();

	if ( !_g.tex )
	{
		// Decode a frame to get the width / height
		if ( 0 < _g.stream.getWidth() && 0 < _g.stream.getHeight() )
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

