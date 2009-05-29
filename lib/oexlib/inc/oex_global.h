/*------------------------------------------------------------------
// oex_global.h
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

#if !defined(_NEW)
#	ifndef __PLACEMENT_NEW_INLINE
#		define __PLACEMENT_NEW_INLINE

		/// Placement new
		inline void * oexCDECL operator new( oexSIZE_T, void *p ) { return p; }

		/// Placement delete
#		if !defined( _MSC_VER ) || _MSC_VER >= 1200
			inline void oexCDECL operator delete(void *, void *) {}
#		endif

#	endif
#endif

// For detecting memory leaks
#if defined( OEX_CRT_LEAK_DETECTION ) && defined( oexDEBUG ) && defined( OEX_WIN32 ) && !defined( __MINGW32__ )
#   define oexNEW                   new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#   define oexNEWAT( l, f )         new( _NORMAL_BLOCK, f, l )
#else
#   define oexNEW			        new
#   define oexNEWAT( l, f )         new
#endif

#define oexDELETE					delete
#define oexDELETE_ARR			    delete[]
#define oexPLACEMENT_NEW            new
