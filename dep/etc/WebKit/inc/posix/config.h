
#include "autotoolsconfig.h"

#include <wtf/Platform.h>

#define JS_EXPORTDATA

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

