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

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CFfDecoder, CFfDecoder )

	SQBIND_MEMBER_FUNCTION( CFfDecoder, Destroy )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, Create )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, isValid )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, FindStreamInfo )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, Decode )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, DecodeImage )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, getWidth )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, getHeight )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, getFps )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, getBitRate )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, setExtraData )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, getExtraData )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, BufferData )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, UnBufferData )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, getBufferSize )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, getH264FrameType )

	SQBIND_STATIC_FUNCTION( CFfDecoder, LookupCodecId )
	SQBIND_STATIC_FUNCTION( CFfDecoder, LookupCodecName )


SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CFfDecoder );

SQBIND_REGISTER_CLASS_BEGIN( CFfEncoder, CFfEncoder )

	SQBIND_MEMBER_FUNCTION( CFfEncoder, Destroy )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, Create )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, isValid )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, Encode )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, EncodeImage )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, getWidth )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, getHeight )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, getFps )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, getBitRate )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CFfEncoder );

SQBIND_REGISTER_CLASS_BEGIN( CFfConvert, CFfConvert )

	SQBIND_STATIC_FUNCTION( CFfConvert, CalcImageSize )
	SQBIND_STATIC_FUNCTION( CFfConvert, ConvertColorBB )
	SQBIND_STATIC_FUNCTION( CFfConvert, ConvertColorBB2 )
	SQBIND_STATIC_FUNCTION( CFfConvert, ConvertColorIB )
	SQBIND_STATIC_FUNCTION( CFfConvert, ConvertColorBI )
	SQBIND_STATIC_FUNCTION( CFfConvert, Rotate )
	SQBIND_STATIC_FUNCTION( CFfConvert, FlipVert )
	SQBIND_STATIC_FUNCTION( CFfConvert, FmtEquiv )

	// Format tyeps
	SQBIND_GLOBALCONST( PIX_FMT_RGB24 )
	SQBIND_GLOBALCONST( PIX_FMT_BGR24 )
	SQBIND_GLOBALCONST( PIX_FMT_RGB32 )
	SQBIND_GLOBALCONST( PIX_FMT_BGR32 )

	SQBIND_GLOBALCONST( PIX_FMT_MONOWHITE )
	SQBIND_GLOBALCONST( PIX_FMT_MONOBLACK )
	SQBIND_GLOBALCONST( PIX_FMT_GRAY8 )
	SQBIND_GLOBALCONST( PIX_FMT_GRAY16LE )
	SQBIND_GLOBALCONST( PIX_FMT_GRAY16BE )

	SQBIND_GLOBALCONST( PIX_FMT_YUV410P )
	SQBIND_GLOBALCONST( PIX_FMT_YUV411P )
	SQBIND_GLOBALCONST( PIX_FMT_YUV420P )
	SQBIND_GLOBALCONST( PIX_FMT_YUV422P )
	SQBIND_GLOBALCONST( PIX_FMT_YUV440P )
	SQBIND_GLOBALCONST( PIX_FMT_YUV444P )

	SQBIND_GLOBALCONST( PIX_FMT_YUYV422 )

	SQBIND_GLOBALCONST( PIX_FMT_YUVJ420P )
	SQBIND_GLOBALCONST( PIX_FMT_YUVJ422P )
	SQBIND_GLOBALCONST( PIX_FMT_YUVJ444P )
	SQBIND_GLOBALCONST( PIX_FMT_YUVJ440P )
	SQBIND_GLOBALCONST( PIX_FMT_YUVA420P )

	SQBIND_GLOBALCONST( PIX_FMT_UYVY422 )
	SQBIND_GLOBALCONST( PIX_FMT_UYYVYY411 )

	// Processing types
	SQBIND_GLOBALCONST( SWS_FAST_BILINEAR )
	SQBIND_GLOBALCONST( SWS_BILINEAR )
	SQBIND_GLOBALCONST( SWS_BICUBIC )
	SQBIND_GLOBALCONST( SWS_X )
	SQBIND_GLOBALCONST( SWS_POINT )
	SQBIND_GLOBALCONST( SWS_AREA )
	SQBIND_GLOBALCONST( SWS_BICUBLIN )
	SQBIND_GLOBALCONST( SWS_GAUSS )
	SQBIND_GLOBALCONST( SWS_SINC )
	SQBIND_GLOBALCONST( SWS_LANCZOS )
	SQBIND_GLOBALCONST( SWS_SPLINE )
	SQBIND_GLOBALCONST( SWS_SRC_V_CHR_DROP_MASK )
	SQBIND_GLOBALCONST( SWS_SRC_V_CHR_DROP_SHIFT )
	SQBIND_GLOBALCONST( SWS_PARAM_DEFAULT )
	SQBIND_GLOBALCONST( SWS_PRINT_INFO )
	SQBIND_GLOBALCONST( SWS_FULL_CHR_H_INT )
	SQBIND_GLOBALCONST( SWS_FULL_CHR_H_INP )
	SQBIND_GLOBALCONST( SWS_DIRECT_BGR )
	SQBIND_GLOBALCONST( SWS_ACCURATE_RND )
	SQBIND_GLOBALCONST( SWS_CPU_CAPS_MMX )
	SQBIND_GLOBALCONST( SWS_CPU_CAPS_MMX2 )
	SQBIND_GLOBALCONST( SWS_CPU_CAPS_3DNOW )
	SQBIND_GLOBALCONST( SWS_CPU_CAPS_ALTIVEC )
	SQBIND_GLOBALCONST( SWS_CPU_CAPS_BFIN )
	SQBIND_GLOBALCONST( SWS_MAX_REDUCE_CUTOFF )
	SQBIND_GLOBALCONST( SWS_CS_ITU709 )
	SQBIND_GLOBALCONST( SWS_CS_FCC )
	SQBIND_GLOBALCONST( SWS_CS_ITU601 )
	SQBIND_GLOBALCONST( SWS_CS_ITU624 )
	SQBIND_GLOBALCONST( SWS_CS_SMPTE170M )
	SQBIND_GLOBALCONST( SWS_CS_SMPTE240M )
	SQBIND_GLOBALCONST( SWS_CS_DEFAULT )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CFfConvert );

#define FFF_KEY_FRAME	AV_PKT_FLAG_KEY

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CFfAudioDecoder, CFfAudioDecoder )

	SQBIND_MEMBER_FUNCTION( CFfAudioDecoder, Destroy )
	SQBIND_MEMBER_FUNCTION( CFfAudioDecoder, Create )
	SQBIND_MEMBER_FUNCTION( CFfAudioDecoder, FindStreamInfo )
	SQBIND_MEMBER_FUNCTION( CFfAudioDecoder, Decode )
	SQBIND_MEMBER_FUNCTION( CFfAudioDecoder, getBufferSize )
	SQBIND_MEMBER_FUNCTION( CFfAudioDecoder, setExtraData )
	SQBIND_MEMBER_FUNCTION( CFfAudioDecoder, getExtraData )
	SQBIND_MEMBER_FUNCTION( CFfAudioDecoder, setSync )
	SQBIND_MEMBER_FUNCTION( CFfAudioDecoder, getSync )
	SQBIND_MEMBER_FUNCTION( CFfAudioDecoder, getChannels )
	SQBIND_MEMBER_FUNCTION( CFfAudioDecoder, getSampleRate )
	SQBIND_MEMBER_FUNCTION( CFfAudioDecoder, getBps )
//	SQBIND_MEMBER_FUNCTION( CFfAudioDecoder,  )

	SQBIND_STATIC_FUNCTION( CFfAudioDecoder, LookupCodecId )
	SQBIND_STATIC_FUNCTION( CFfAudioDecoder, LookupCodecName )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CFfAudioDecoder );

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CFfContainer, CFfContainer )

	SQBIND_MEMBER_FUNCTION( CFfContainer, Open )
	SQBIND_MEMBER_FUNCTION( CFfContainer, isOpen )
	SQBIND_MEMBER_FUNCTION( CFfContainer, ReadFrame )
	SQBIND_MEMBER_FUNCTION( CFfContainer, DecodeFrame )
	SQBIND_MEMBER_FUNCTION( CFfContainer, DecodeFrameBin )
	SQBIND_MEMBER_FUNCTION( CFfContainer, DecodeAudioFrameBin )

	SQBIND_MEMBER_FUNCTION( CFfContainer, Create )
	SQBIND_MEMBER_FUNCTION( CFfContainer, InitWrite )
	SQBIND_MEMBER_FUNCTION( CFfContainer, AddVideoStream )
	SQBIND_MEMBER_FUNCTION( CFfContainer, WriteFrame )
	SQBIND_MEMBER_FUNCTION( CFfContainer, WriteVideoFrame )
	SQBIND_MEMBER_FUNCTION( CFfContainer, AddAudioStream )
	SQBIND_MEMBER_FUNCTION( CFfContainer, WriteAudioFrame )
	SQBIND_MEMBER_FUNCTION( CFfContainer, Seek )

	SQBIND_MEMBER_FUNCTION( CFfContainer, Destroy )
	SQBIND_MEMBER_FUNCTION( CFfContainer, CloseStream )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getFps )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getWidth )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getHeight )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getVideoFormat )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getVideoCodecId )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getVideoStream )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioStream )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getFrameCount )
	SQBIND_MEMBER_FUNCTION( CFfContainer, isAudioCodec )
	SQBIND_MEMBER_FUNCTION( CFfContainer, isVideoCodec )

	SQBIND_MEMBER_FUNCTION( CFfContainer, setAudioExtraData )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioExtraData )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioSampleRate )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioBitRate )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioBitsPerSample )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioChannels )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioFrameSize )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioCodecID )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioType )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioName )
	SQBIND_MEMBER_FUNCTION( CFfContainer, isVideoCodec )
	SQBIND_MEMBER_FUNCTION( CFfContainer, isVideoCodec )

	SQBIND_GLOBALCONST( FFF_KEY_FRAME )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CFfContainer );

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CFfTranscode, CFfTranscode )

	SQBIND_MEMBER_FUNCTION( CFfTranscode, Init )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, Destroy )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, Transcode )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, GetRaw )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, GetImage )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, getDecoderCodecId )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, getEncoderCodecId )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, getWidth )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, getHeight )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, getFps )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, getBitRate )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, isValid )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CFfTranscode );

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CFfCapture, CFfCapture )

	SQBIND_MEMBER_FUNCTION( CFfCapture, Open )
	SQBIND_MEMBER_FUNCTION( CFfCapture, isOpen )
	SQBIND_MEMBER_FUNCTION( CFfCapture, ReadFrame )

	SQBIND_MEMBER_FUNCTION( CFfCapture, Destroy )
	SQBIND_MEMBER_FUNCTION( CFfCapture, getWidth )
	SQBIND_MEMBER_FUNCTION( CFfCapture, getHeight )
	SQBIND_MEMBER_FUNCTION( CFfCapture, getVideoFormat )
	SQBIND_MEMBER_FUNCTION( CFfCapture, getVideoCodecId )
	SQBIND_MEMBER_FUNCTION( CFfCapture, getFrameCount )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CFfCapture );


// Export classes
static void SQBIND_Export_ffmpeg( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

#if defined( OEX_DEBUG )
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

    SQBIND_EXPORT( x_vm, CFfDecoder );
    SQBIND_EXPORT( x_vm, CFfEncoder );
    SQBIND_EXPORT( x_vm, CFfAudioDecoder );
    SQBIND_EXPORT( x_vm, CFfConvert );
    SQBIND_EXPORT( x_vm, CFfContainer );
    SQBIND_EXPORT( x_vm, CFfTranscode );
    SQBIND_EXPORT( x_vm, CFfCapture );
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
