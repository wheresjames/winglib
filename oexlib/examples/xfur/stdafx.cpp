/*------------------------------------------------------------------
// stdafx.cpp
// Copyright (c) 2006 
// Robert Umbehant
// winglib@wheresjames.com
// http://www.wheresjames.com
//
// Redistribution and use in source and binary forms, with or 
// without modification, are permitted for commercial and 
// non-commercial purposes, provided that the following 
// conditions are met:
//
// * Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// * The names of the developers or contributors may not be used to 
//   endorse or promote products derived from this software without 
//   specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
//   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/

#include "stdafx.h"


// Required by WxWidgets
#pragma comment( lib, "ComCtl32.lib" )
#pragma comment( lib, "Rpcrt4.lib" )


// MBSC
#ifndef _UNICODE
#	ifdef _DEBUG
#		pragma comment( lib, "wxbase26d.lib" )
#		pragma comment( lib, "wxmsw26d_core.lib" )
//#		pragma comment( lib, "wxbase26d_net.lib" )
//#		pragma comment( lib, "wxmsw26d_adv.lib" )
//#		pragma comment( lib, "wxexpatd.lib" )
//#		pragma comment( lib, "wxmsw26d_dbgrid.lib" )
//#		pragma comment( lib, "wxmsw26d_gl.lib" )
//#		pragma comment( lib, "wxmsw26d_html.lib" )
//#		pragma comment( lib, "wxmsw26d_media.lib" )
//#		pragma comment( lib, "wxmsw26d_qa.lib" )
//#		pragma comment( lib, "wxjpegd.lib" )
//#		pragma comment( lib, "wxpngd.lib" )
//#		pragma comment( lib, "wxregexd.lib" )
//#		pragma comment( lib, "wxtiffd.lib" )
//#		pragma comment( lib, "wxregexd.lib" )
//#		pragma comment( lib, "wxzlibd.lib" )
//#		pragma comment( lib, "wxbase26d_xml.lib" )
//#		pragma comment( lib, "wxmsw26d_xrc.lib" )
#	else
#		pragma comment( lib, "wxbase26.lib" )
#		pragma comment( lib, "wxmsw26_core.lib" )
#	endif

// Unicode
#else
#	ifdef _DEBUG
#		pragma comment( lib, "wxbase26ud.lib" )
#		pragma comment( lib, "wxmsw26ud_core.lib" )
#	else
#		pragma comment( lib, "wxbase26u.lib" )
#		pragma comment( lib, "wxmsw26u_core.lib" )
#	endif

#endif // _UNICODE
