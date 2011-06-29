
_self.load_module( "live555", "" );
_self.load_module( "ffmpeg", "" );

local file = "testrtsp.avi";
local link = "rtsp://video3.multicasttech.com/AFTVComedy3GPP296.sdp";
local szDumpVideo = "rawvideo";
if ( szDumpVideo.len() ) CSqFile().mkdir( szDumpVideo );

local rtsp = CLvRtspClient();
if ( !rtsp.Open( link, 1, 0, CSqMulti() ) )
{	::_self.echo( "!!! Failed to initialize RTSP stream : " + link );
	return 0;
} // end if

rtsp.waitInit( 3000 );
if ( !rtsp.isOpen() )
{	::_self.echo( "!!! Failed to open RTSP stream : " + link );
	return 0;
} // end if

local fps = rtsp.getFps();
if ( 0 >= fps ) fps = 15;

local avi = CFfContainer();
// avi.setVideoExtraData( rtsp.getExtraVideoData() );
// avi.setAudioExtraData( rtsp.getExtraAudioData() );

if ( !avi.Create( file, "", CSqMulti() ) )
{	::_self.echo( "!!! Failed to create avi" );
	return 0;
} // end if

if ( rtsp.isVideo() && 0 > avi.AddVideoStream( CFfDecoder().LookupCodecId( rtsp.getVideoCodecName() ), 
											   rtsp.getWidth(), rtsp.getHeight(), rtsp.getFps() ) )
{	::_self.echo( "!!! Failed to add video stream" );
	return 0;
} // end if

if ( rtsp.isAudio() && 0 > avi.AddAudioStream( CFfAudioDecoder().LookupCodecId( rtsp.getAudioCodecName() ), 
											   rtsp.getNumAudioChannels(), rtsp.getAudioSampleRate(), 0 ) )
{	::_self.echo( "!!! Failed to add audio stream" );
	return 0;
} // end if

else if ( !avi.InitWrite() )
{	::_self.echo( "!!! Failed to initiailze avi" );
	return 0;
} // endif

::_self.echo( " ++++++++++++++++++ PLAY ++++++++++++++++++++" );

rtsp.Play();

local frame = CSqBinary();
local to = ::_self.gmt_time() + 8;
local vix = 0, aix = 0;
while ( to > ::_self.gmt_time() )
{
	while ( rtsp.LockAudio( frame, 0 ) )
	{	if ( frame.getUsed() )
			avi.WriteAudioFrame( frame, 0, 0, CSqMulti() );
		rtsp.UnlockAudio();
	} // end while

	while ( rtsp.LockVideo( frame, 0 ) )
	{
		if ( szDumpVideo.len() )
			CSqFile().put_contents_bin( ::_self.build_path( szDumpVideo, "v" + vix++ + ".raw" ), frame );
			
		if ( frame.getUsed() )
			avi.WriteVideoFrame( frame, 0, 0, CSqMulti() );			
			
		rtsp.UnlockVideo();			
		
	} // end while

	::_self.sleep( 15 );
	
} // end while

::_self.echo( " ++++++++++++++++++ DONE ++++++++++++++++++++" );
