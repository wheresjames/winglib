
#pragma once

#include <string>
#include <list>
#include <map>
#include <vector>

#include "oexlib.h"
#include "sqbind.h"

#ifdef OEX_WINDOWS
#	include "winsock2.h"
#	include "mysql.h"
#else
#	include "mysql/mysql.h"
#endif

#include "sq_mysql.h"
