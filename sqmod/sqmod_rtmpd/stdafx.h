
#pragma once

#include <string>
#include <list>
#include <map>
#include <vector>

#include "oexlib.h"
#include "sqbind.h"

#include "rtmp.h"

#include "rtmpd_session.h"

// RTMPD is not quit thread safe
extern "C" oexLock _g_rtmpd_lock;
