// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CLvRtspClient, CLvRtspClient )

	SQBIND_MEMBER_FUNCTION( CLvRtspClient, Destroy )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, Open )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, LockFrame )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, UnlockFrame )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, isOpen )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getWidth )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getHeight )
	SQBIND_MEMBER_FUNCTION( CLvRtspClient, getFrameCount )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CLvRtspClient );


// Export classes
static void SQBIND_Export_ffmpeg( sqbind::VM x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;


    SQBIND_EXPORT( x_vm, CLvRtspClient );
}

#if defined( SQBIND_STATIC )
	#include "lv_rtsp_client.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{	SQBIND_Export_ffmpeg( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
