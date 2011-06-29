// stdafx.cpp

#include "stdafx.h"

// CLvRtspClient
SQBIND_REGISTER_CLASS_BEGIN( CLvRtspClient, CLvRtspClient )

	SQBIND_MEMBER_FUNCTION( CLvRtspClient, Destroy )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, Open )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, Play )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, waitInit )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, LockVideo )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, UnlockVideo )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, LockAudio )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, UnlockAudio )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, isOpen )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getWidth )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getHeight )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getFps )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getFrameCount )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getVideoCodecName )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getAudioCodecName )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, isVideo )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, isAudio )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getExtraVideoData )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getExtraAudioData )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, setVideoHeader )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getSDP )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getSDPValue )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, setStreamOverTCP )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getStreamOverTCP )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, setTunnelOverHTTPPort )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getTunnelOverHTTPPort )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getNumAudioChannels )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getAudioSampleRate )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getAudioBps )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getVideoPts )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getVideoPtsSec )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getVideoPtsUSec )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getVideoDts )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getAudioPts )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getAudioPtsSec )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getAudioPtsUSec )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getAudioDts )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, setBlindLogin )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getBlindLogin )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getError )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getLastError )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getUrl )
//	SQBIND_MEMBER_FUNCTION( CLvRtspClient,  )
//	SQBIND_MEMBER_FUNCTION( CLvRtspClient,  )
//	SQBIND_MEMBER_FUNCTION( CLvRtspClient,  )
//	SQBIND_MEMBER_FUNCTION( CLvRtspClient,  )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CLvRtspClient );

// CLvRtspServer
SQBIND_REGISTER_CLASS_BEGIN( CLvRtspServer, CLvRtspServer )

	SQBIND_MEMBER_FUNCTION( CLvRtspServer, Destroy )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, StartServer )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, isThread )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, getUrl )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, needFrame )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, Msg )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CLvRtspServer );

// Export classes
static void SQBIND_Export_live555( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CLvRtspClient );
    SQBIND_EXPORT( x_vm, CLvRtspServer );
}

#if defined( SQBIND_STATIC )
	#include "lv_rtsp_client.cpp"
	#include "lv_rtsp_server.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_live555( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
