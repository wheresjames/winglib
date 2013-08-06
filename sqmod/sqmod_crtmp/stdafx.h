
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
#include "protocols/liveflv/innetliveflvstream.h"
#include "protocols/liveflv/inboundliveflvprotocol.h"
#include "protocols/rtmp/basertmpappprotocolhandler.h"
#include "application/baseclientapplication.h"
#include "application/clientapplicationmanager.h"

#include "crtmp_server.h"

extern "C" oexLock _g_crtmp_lock;
