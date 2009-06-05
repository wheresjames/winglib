
#include "autotoolsconfig.h"

#include <wtf/Platform.h>

#define JS_EXPORTDATA

#define max max
#define min min

#if !COMPILER(MSVC7) && !PLATFORM(WIN_CE)
#	ifndef _CRT_RAND_S
#		define _CRT_RAND_S
#	endif
#endif

#define HAVE_STDINT_H 1
#define HAVE_STRING_H 1
#define WTF_CHANGES 1

#ifdef __cplusplus
#undef new
#undef delete
#include <wtf/FastMalloc.h>
#endif

#if !PLATFORM(QT) && !PLATFORM(WX)
#include <wtf/DisallowCType.h>
#endif

