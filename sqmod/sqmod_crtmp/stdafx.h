
#pragma once

#include <string>
#include <list>
#include <map>
#include <vector>

#include "oexlib.h"
#include "sqbind.h"

#include "netio/netio.h"
#include "protocols/protocolmanager.h"
#include "protocols/protocolfactorymanager.h"
#include "protocols/defaultprotocolfactory.h"
#include "protocols/rtmp/basertmpappprotocolhandler.h"
#include "application/baseclientapplication.h"

#include "crtmp_server.h"

extern "C" oexLock _g_crtmp_lock;
