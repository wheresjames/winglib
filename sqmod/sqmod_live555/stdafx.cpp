// stdafx.cpp

#include "stdafx.h"

// Export classes
static void SQBIND_Export_live555( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

	// Increase output buffer size
	OutPacketBuffer::maxSize = 1000000;

	// Register class instances
	CLvRtspClient::Register( x_vm );
	CLvRtspServer::Register( x_vm );
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
