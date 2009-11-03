
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
	#include "libswscale/swscale.h"
};

#define FFSQ_VIDEO2
#undef printf

#include "ff_decoder.h"
#include "ff_encoder.h"
#include "ff_convert.h"
#include "ff_container.h"
#include "ff_transcode.h"
