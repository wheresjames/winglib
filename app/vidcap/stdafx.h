#pragma once

#ifdef WIN32

	// Microsoft trying to chain us to their stuff
	#define _CRT_SECURE_NO_WARNINGS
	//#define WIN32_LEAN_AND_MEAN

	#include <windows.h>

#endif

//#define WITHOUT_UNICODE
//#define wxUSE_UNICODE 0

// WxWidgets
#include "wx/wx.h"

//#include "wx/utils.h"
//#include "wx/gdicmn.h"
//#include "wx/frame.h"
//#include "wx/sysopt.h"
//#include "wx/image.h"


// oexlib
#include "../../lib/oexlib/oexlib.h"



