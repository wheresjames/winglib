
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

// FFMPEG is not quite thread safe
extern "C" oexLock _g_ffmpeg_lock;

#include "ff_decoder.h"
#include "ff_encoder.h"
#include "ff_audio_decoder.h"
#include "ff_convert.h"
#include "ff_container.h"
#include "ff_transcode.h"
#include "ff_capture.h"
