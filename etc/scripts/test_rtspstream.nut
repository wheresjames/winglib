
_self.load_module( "irrlicht", "" );
_self.load_module( "live555", "" );
_self.load_module( "ffmpeg", "" );
_self.load_module( "portaudio", "" );

class CRtspStream
{
	bDecodeVideo = 1;
	bDecodeAudio = 0;
	bPlayAudio = 0;

	rtsp = 0;

	dec = 0;
	frame = 0;
	vfail = 0;

	adec = 0;
	araw = 0;
	aframe = 0;	
	afail = 0;
	
	pa = 0;

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

		if ( adec )
			adec.Destroy(), dec = 0;

		araw = 0;			
		aframe = 0;
		afail = 0;
		
		pa = 0;
	}

	function Play( link )
	{
	
		Close();

		::_self.echo( "\nConnecting to : " + link + "\n" );

		rtsp = CLvRtspClient();
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
			if ( "H264" == rtsp.getVideoCodecName() )
				rtsp.setVideoHeader( "\x00\x00\x01" );
				
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
			::_self.echo( "iii Creating audio decoder for " + rtsp.getAudioCodecName() );
			
			adec = CFfAudioDecoder();
			// if ( !adec.Create( CFfAudioDecoder().LookupCodecId( rtsp.getAudioCodecName() ) ) )
			if ( !adec.Create( CFfAudioDecoder().LookupCodecId( "LATM" ) ) )
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
					switch( 8 )
					{	case 8 : fmt = CPaOutput().paUInt8; break;
						default : fmt = CPaOutput().paInt16; break;
					} // end switch
					local fsize = 2;
				
					if ( !pa.Open( 0, pa.getDefaultOutputDevice(), 1, fmt, 0.2, 8000., fsize ) )
					{   _self.echo( "!!! Failed to open output stream : " + pa.getLastError() );
						pa = 0;
					} // end if
				
					else if ( !pa.Start() )
					{   _self.echo( "!!! Failed to start output stream : " + pa.getLastError() );
						pa = 0;
					} // end if
					
				} // end if
								
			} // end else
			
		} // end else

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
			
		if ( aframe.getUsed() )
		{
			::_self.echo( "asz = " + aframe.getUsed() );
//			if ( !pa.Write( aframe, aframe.getUsed() / pa.getFrameBytes() ) );
			
//			::_self.echo( aframe.AsciiHexStr( 16, 16 ) );
			
			if ( 0 < adec.Decode( aframe, araw, CSqMulti() ) )
			{
				::_self.echo( " ++++++++++++++++++ AUDIO PACKET : " + araw.getUsed() );		
				
				if ( pa )
					if ( !pa.Write( araw, araw.getUsed() / pa.getFrameBytes() ) )
					{	_self.echo( "Failed to write audio data" );
						return -1;
					} // end if
				
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

		if ( rtsp.LockVideo( frame, 0 ) )
		{	dec.BufferData( frame, CSqMulti() );
			rtsp.UnlockVideo();
			::_self.echo( "lv" );
		} // end if

		if ( 0 >= dec.getBufferSize() )
			return 0;

		::_self.echo( "sz = " + dec.getBufferSize() );

		return dec.Decode( CSqBinary(), CFfConvert().PIX_FMT_RGB32, buffer, CSqMulti(), 0 );
	}

}

class CGlobal
{
	rtsp_sources =
	{
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
	_g.irr.Init( "Irr Test", 640, 480, 1 );

	_self.echo( "...setting ambient light...\n" );
	_g.irr.SetAmbientLight( CSqirrColorf( 0.5, 0.5, 0.5 ) );

	_self.echo( "...adding lights...\n" );
	_g.irr.AddLight( CSqirrVector3d( 0., 100., 0. ), CSqirrColorf( 1., 1., 1. ), 100. );
	_g.irr.AddLight( CSqirrVector3d( 0., 100., 50. ), CSqirrColorf( 1., 1., 1. ), 100. );

//	_self.echo( "adding sky...\n" );
//	_g.irr.AddSkyDome( _self.path( "../imgs/sky.png" ), 16, 16, 100., 100. );

	_self.echo( "...adding camera...\n" );
	local cam = _g.irr.AddCamera( CSqirrVector3d( 0, 10, -15 ), CSqirrVector3d( 0, 0, 0 ) );
//	cam.SetLens( 1., 2.4, 3.2 );

	_self.echo( "...adding cube...\n" );
	_g.cube = _g.irr.AddCube( 10. );

	_self.echo( "...adding animator...\n" );
	local ani = _g.irr.AddRotateAnimator( CSqirrVector3d( 0, 0.4, 0 ) );
	_g.cube.AddAnimator( ani );

	// Set draw function
	_self.set_timer( ".", 30, "OnDraw" )

	// Start the video stream
	_g.stream = CRtspStream();
	_g.stream.Play( _g.rtsp_sources[ "adventure" ][ 1 ] );

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
			if ( _g.tex ) _g.cube.SetTexture( 0, _g.tex );
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

