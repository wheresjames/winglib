/*------------------------------------------------------------------
// oex_config.h
//
// Copyright (c) 1997
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

// Compile messages
#define OEX_VERBOSE_COMPILE

// The namespace to use
#define OEX_NAMESPACE		oex

// Do we want class testing enabled?
#define OEX_ENABLE_TESTS
//#undef OEX_ENABLE_TESTS

// Enables buffer over-run/under-run checking in release mode
//#define OEX_ENABLE_RELEASE_MODE_MEM_CHECK
 #undef OEX_ENABLE_RELEASE_MODE_MEM_CHECK

// Are we using windows?
#ifdef WIN32
#	define OEX_WIN32
#endif

// By default, the thread locks time out and usually the function
// will give up and treat it as an error, sixty seconds should be 
// plenty, but you may have some reason, like a reeeeaaaallllyyy
// slow system and you need longer waits.  I really don't recommend
// you set this lower.  If you're timing out that indicates a serious
// error (deadlocks, race conditions and such) and you should fix it.
#define oexDEFAULT_TIMEOUT           60000

// Enable CRT leak detection
#define OEX_CRT_LEAK_DETECTION

// Enable zip file support
#define OEX_ENABLE_ZIP

