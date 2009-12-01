/*------------------------------------------------------------------
// sqmod_extern.hpp
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

// Include engine requirements
#include "sqeng_extern.hpp"

#if defined ( OEX_WINDOWS )
#include <windows.h>
BOOL WINAPI DllMain( HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved ) { return true; }
#endif

extern "C" oexDECLARE_SRV_FUNCTION( SRV_GetModuleInfo );
extern "C" oex::oexRESULT SRV_GetModuleInfo( oex::os::service::SSrvInfo *pDi )
{
	if ( !oexCHECK_PTR( pDi ) )
	{	oexERROR( 0, oexT( "Invalid function argument" ) );
		return 0;
	} // end if

	// Clear structure
	oexZeroMemory( pDi, sizeof( oex::os::service::SSrvInfo ) );

	// Module name
	strncpy( pDi->szName, "Squirrel Engine", sizeof( pDi->szName ) - 1 );

	// Module description
	strncpy( pDi->szDesc, "Exports functionality to Squirrel-Script", sizeof( pDi->szDesc ) - 1 );

	// Set the Squirrel engine type
	pDi->guidType = sqbind::SQBIND_MODULE_IID;

	// pDi->guidId = ;

	// Create random guid
	oexUniqueGuid( &pDi->guidInstance );

	// Set version
	pDi->lVer = oexVERSION( 1, 0 );

	return 0;
}

extern "C" oexDECLARE_SRV_FUNCTION( SRV_Start );
extern "C" oex::oexRESULT SRV_Start( oex::SRawAllocator x_sRawAllocator, oex::oexCSTR x_pPath, oex::oexCSTR x_pCommandLine, oex::oexINT x_nCommandLine, oex::oexCPVOID x_pData )
{
	// Set our allocator
	oex::CMem::SetRawAllocator( x_sRawAllocator );

	return 0;
}

extern "C" oexDECLARE_SRV_FUNCTION( SRV_Stop );
extern "C" oex::oexRESULT SRV_Stop()
{
	// Switch back to default allocator
	oex::CMem::SetDefaultRawAllocator();

	return 0;
}

extern "C" oex::oexRESULT SQBIND_Export_Symbols( sqbind::VM x_vm, sqbind::SSqAllocator *x_pAllocator )
{
	// Set the memory allocator
	sqbind::SQBIND_SetAllocator( x_pAllocator );

	/* !!! NOTICE !!!

		If you get the error that SQBIND_Export() is not defined,
		it's your fault.  You need to provide SQBIND_Export() in
		your application before including this file.

		In the project examples this is done in stdafx.cpp.
		I don't care how you do it in your project.

	*/

    // Call user export function
    SQBIND_Export( x_vm );

	return 0;
}

