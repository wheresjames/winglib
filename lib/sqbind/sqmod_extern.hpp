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
	strncpy( pDi->szName, oexT( "Squirrel Engine" ), sizeof( pDi->szName ) - 1 );

	// Module description
	strncpy( pDi->szDesc, oexT( "Exports functionality to Squirrel-Script" ), sizeof( pDi->szDesc ) - 1 );

	// Set the Squirrel engine type
	pDi->guidType = sqbind::SQBIND_MODULE_IID;

	// pDi->guidId = ;
		
	// Create random guid
	oexUniqueGuid( &pDi->guidInstance );
	
	// Set version
	pDi->lVer = oexVERSION( 1, 0 );

	return 0;
}

#if defined( _MSC_VER )
#include <Windows.h>
static oex::oexCPVOID GetModuleBaseAddress( oex::oexCPVOID x_pPtr )
{   MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery( x_pPtr, &mbi, sizeof( mbi ) );
    return (HMODULE)mbi.AllocationBase;
}
#endif

extern "C" oex::oexRESULT SQBIND_Export( sqbind::VM *x_vm, oex::oexCPVOID x_pHeapCheck )
{
	// !!!Heaps must match 
	/**
		If this fails, you probably didn't use shared linking.

		The squirren engine must be able to create and destroy objects, since the engine
		is linking across module boundry, each module must use the same heap.  With the
		current design of the squirrel engine, this is a limitation of this modular 
		architecture.	
	*/

#if defined( _MSC_VER )
	if ( !oexVERIFY( GetModuleBaseAddress( &malloc ) == GetModuleBaseAddress( x_pHeapCheck ) ) )
		return -1;
#else
	if ( !oexVERIFY( x_pHeapCheck != (oex::oexCPVOID)&malloc ) )
		return -1;
#endif

    // Call user export function
    SQMOD_Export( x_vm );

	return 0;
}

