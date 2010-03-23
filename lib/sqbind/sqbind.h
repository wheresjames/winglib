/*------------------------------------------------------------------
// sqbind.h
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
// * Imporant note on GCC 4.x
//		http://squirrel-lang.org/forums/thread/1110.aspx
//----------------------------------------------------------------*/

#pragma once

// Kill Microsoft ads
#define _CRT_SECURE_NO_DEPRECATE 1

#if defined( OEX_WINDOWS ) && !defined( OEX_GCC )
#	pragma warning( disable : 4996 )
#endif

// Using SqPlus script binding for now
//#define SQBIND_SQBIND
#define SQBIND_SQPLUS
//#define SQBIND_JKBIND

#define WSQBIND_NOREPLY	( (int*)0 )
#define WSQBIND_NEW(__ptr,__type) {__ptr=(__type *)sq_vm_malloc(sizeof(__type));new (__ptr) __type;}
#define WSQBIND_DELETE(__ptr,__type) {__ptr->~__type();sq_vm_free(__ptr,sizeof(__type));}

// Include squirrel headers
#include <squirrel.h>
#include <sqstdio.h>
#include <sqstdstring.h>
#include <sqstdmath.h>
#include <sqstdaux.h>

// sqbind defines
#include "sq_defines.h"
namespace sqbind
{
	// There will be a class called CSqMsgQueue
	class CSqMsgQueue;
}
// stl classes we will import to squirrel
#include "import.h"

namespace sqbind
{
	/// Squirrel Engine ID {DBD585B7-9B61-475e-9DB5-0CC25C698814}
	static const oex::oexCSTR SQBIND_ENGINE_TYPEID = oexT( "DBD585B7-9B61-475e-9DB5-0CC25C698814" );
	static const oex::oexGUID SQBIND_ENGINE_IID = oexINITGUID( 0xdbd585b7, 0x9b61, 0x475e, 0x9d, 0xb5, 0xc, 0xc2, 0x5c, 0x69, 0x88, 0x14 );

	/// Squirrel Module ID {20856ECF-F93E-4f49-AA02-C5784A4146F5}
	static const oex::oexCSTR SQBIND_MODULE_TYPEID = oexT( "20856ECF-F93E-4f49-AA02-C5784A4146F5" );
	static const oex::oexGUID SQBIND_MODULE_IID = oexINITGUID( 0x20856ecf, 0xf93e, 0x4f49, 0xaa, 0x2, 0xc5, 0x78, 0x4a, 0x41, 0x46, 0xf5 );

	/// Allocator
	struct SSqAllocator
	{
		void* (*fnMalloc) ( size_t size );

		void* (*fnRealloc) ( void *ptr, size_t size );

		void (*fnFree) ( void *ptr );
	};

    /// Executes the specified script
	typedef oex::oexRESULT ( *SQBIND_Execute ) ( oex::oexCSTR x_sScript, oex::oexBOOL x_bFile );

	/// This function exports the Squirrel functions contained in the module
	/**
		\param [out] x_vm			- Pointer to the Squirrel Virtual Machine in which functions
									  will be exported.
		\param [in] x_pAllocator	- Pointer to the memory allocator to use, you *MUST* use this
									  if the heaps will be different!!!
	*/
    typedef oex::oexRESULT ( *PFN_SQBIND_Export_Symbols ) ( sqbind::VM x_vm, SSqAllocator *x_pAllocator );

    /// Bind native squirrel functions
    void SqBindAll( sqbind::VM x_vm );

	/// Sets the allocator for squirrel engine
	/**
		This is a must if different heaps are used.
	*/
	oex::oexRESULT SQBIND_SetAllocator( SSqAllocator *x_pAllocator );

	// Lot's of objects need pointers to this class
	class CSqEngine;

	#include "module_manager.h"
	#include "msg_queue.h"
	#include "sq_engine.h"
	#include "script_thread.h"
	#include "sq_tools.h"
};




