// stdafx.cpp

#include "stdafx.h"

// CLvRtspClient
SQBIND_REGISTER_CLASS_BEGIN( CLvRtspClient, CLvRtspClient )

	SQBIND_MEMBER_FUNCTION( CLvRtspClient, Destroy )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, Open )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, waitInit )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, LockVideo )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, UnlockVideo )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, LockAudio )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, UnlockAudio )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, isOpen )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getWidth )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getHeight )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getFrameCount )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getVideoCodecName )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getAudioCodecName )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, isVideo )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, isAudio )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CLvRtspClient );

// CLvRtspServer
SQBIND_REGISTER_CLASS_BEGIN( CLvRtspServer, CLvRtspServer )

	SQBIND_MEMBER_FUNCTION( CLvRtspServer, Destroy )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, StartServer )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, isThread )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, setServerCallback )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, setNextFrame )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, deliverFrame )
	SQBIND_MEMBER_FUNCTION( CLvRtspServer, getUrl )

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
