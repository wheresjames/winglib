// stdafx.cpp

#include "stdafx.h"

oexLock _g_ffmpeg_lock;

#ifndef assert
extern "C"  void assert(int expression)
{
}
#endif

extern "C"
{

#if defined( _MSC_VER ) && _MSC_VER >= 1600
	int _imp____lc_codepage = 0; // CP_ACP 1252;
#endif

#if defined( OEX_WINDOWS )
	extern "C" int usleep( int usec ) { oexMicroSleep( usec ); return usec; }
#endif

//const uint64_t ff_pw_20 = 0x0014001400140014ULL;
//const uint64_t ff_pw_20 __attribute__ ((aligned (8))) = 0x0014001400140014ULL;
//const unsigned long long ff_pw_20 = 0x0014001400140014ULL;
//DECLARE_ALIGNED_8 (const uint64_t, ff_pw_20 ) = 0x0014001400140014ULL;

//int snd_strerror( const char *p )
//{_STT();
//	return 0;
//}

}

// Export classes
static void SQBIND_Export_ffmpeg( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

#if defined( oexDEBUG )
	av_log_set_level( AV_LOG_INFO );
//	av_log_set_level( AV_LOG_DEBUG );
#else
//	av_log_set_level( AV_LOG_WARNING );
	av_log_set_level( AV_LOG_ERROR );
//	av_log_set_level( AV_LOG_FATAL );
#endif

	// Register codecs
	av_register_all();
#if !defined( OEX_WINDOWS )
//	avdevice_register_all();
#endif

	CFfDecoder::Register( x_vm );
	CFfEncoder::Register( x_vm );
	CFfAudioDecoder::Register( x_vm );
	CFfConvert::Register( x_vm );
	CFfTranscode::Register( x_vm );
	CFfCapture::Register( x_vm );
	CFfContainer::Register( x_vm );
}

#if defined( SQBIND_STATIC )
	#include "ff_decoder.cpp"
	#include "ff_encoder.cpp"
	#include "ff_audio_decoder.cpp"
	#include "ff_convert.cpp"
	#include "ff_container.cpp"
	#include "ff_transcode.cpp"
	#include "ff_captuer.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_ffmpeg( x_vm ); }

	// +++ Leaking to avoid crash on ctrl+c, please fix one day!
//	#define SQBIND_Exit _g_ffmpeg_lock.Detach();

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
