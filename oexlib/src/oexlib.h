/*------------------------------------------------------------------
// oexlib.h
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

#pragma once

// STL
#include <string>
#include <map>

// Use params are in this file
#include "oexlib_config.h"

// Name space
#include "oexlib_namespace.h"

OEXLIB_START_NAMESPACE

// Library setup
#include "oexlib_typedefs.h"
#include "oexlib_macros.h"
#include "oexlib_debugging.h"
#include "oexlib_exceptions.h"

// Include proper OS specific stuff
#ifdef OEXLIB_WIN32
#	include "oexlib_os_win32.h"
#endif

// OS specific classes
#include "os/mmap.h"
#include "os/fmap.h"
#include "os/mutex.h" 

// Other People's Code
#include "opc/md5.h"

// Library Implementation
#include "thread_safe.h"
#include "file_mapping.h"
#include "Memory.h"
#include "RFun.h"
#include "circ_buf.h"
#include "data_packet.h"

OEXLIB_END_NAMESPACE

