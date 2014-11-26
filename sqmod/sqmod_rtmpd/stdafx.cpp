// stdafx.cpp

#include "stdafx.h"
#include "log.h"

#if defined( _DEBUG )
	extern "C" FILE *netstackdump = 0;
	extern "C" FILE *netstackdump_read = 0;
#endif


oexLock _g_rtmpd_lock;
oex::CStr8 _g_rtmpd_sErrors;

extern "C" void RTMP_TLS_Init();

static void _rtmp_log(int level, const char *format, va_list vl)
{
	if ( level > 1 ) 
		return;

	oexAutoLock ll( _g_rtmpd_lock );

	oex::CStr8 s, msg;
	s.Fmt( "RTMP MSG %d: %s\n", level, msg.vFmt( format, vl ).c_str() );
	_g_rtmpd_sErrors += s;

}

// Export classes
static void SQBIND_Export_rtmpd( sqbind::VM x_vm )
{_STT();

	if ( !oexCHECK_PTR( x_vm ) )
		return;

	oexAutoLock ll( _g_rtmpd_lock );

	// This isn't thread safe, so do it now
	RTMP_TLS_Init();

	// Register logging function
	RTMP_LogSetCallback( _rtmp_log );
	
	// Register class instances
	CRtmpdSession::Register( x_vm );
}

#if defined( SQBIND_STATIC )
	#include "rtmpd_session.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_rtmpd( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
