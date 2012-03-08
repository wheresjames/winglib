
#pragma once

#include <string>
#include <list>
#include <map>
#include <vector>

#include "oexlib.h"
#include "sqbind.h"

extern "C"
{
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libavdevice/avdevice.h"
	#include "libswscale/swscale.h"
};

#define FFSQ_VIDEO2
#undef printf

#ifndef OEX_OLDFF
#	define OEX_NEWFF
#endif

// FFMPEG is not quite thread safe
extern "C" oexLock _g_ffmpeg_lock;

#ifdef OEX_NEWFF
	int memshare_open( AVIOContext **pAc, const char *filename, int flags );
#endif

/*
// Custom read write protocol
int memshare_open( URLContext *h, const char *filename, int flags );
int memshare_read( URLContext *h, unsigned char *buf, int size );
#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(52, 68, 0)
int memshare_write( URLContext *h, unsigned char *buf, int size );
#else
int memshare_write( URLContext *h, const unsigned char *buf, int size );
#endif
int64_t memshare_seek( URLContext *h, int64_t pos, int whence );
int memshare_close( URLContext *h );
*/

#include "ff_decoder.h"
#include "ff_encoder.h"
#include "ff_audio_decoder.h"
#include "ff_convert.h"
#include "ff_container.h"
#include "ff_transcode.h"
#include "ff_capture.h"
