/*------------------------------------------------------------------
// fmap.cpp
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

#include "oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

// Ensure size
oexSTATIC_ASSERT( sizeof( CFMap::t_HFILEMAP ) == sizeof( HANDLE ) );

oexCONST CFMap::t_HFILEMAP CFMap::c_Failed = NULL;

CFMap::t_HFILEMAP CFMap::Create( CFMap::t_HFILEMAP x_hFile, oexPVOID *x_pMem, oexINT64 x_llSize, oexINT64 *x_pllSize, etAccess x_eAccess, oexBOOL *x_pbAlreadyExists, oexINT64 x_llOffset )
{
	return oexNULL;
}

CFMap::t_HFILEMAP CFMap::Create( oexCSTR x_pFile, oexPVOID *x_pMem, oexINT64 x_llSize, oexINT64 *x_pllSize, oexCSTR x_pName, etAccess x_eAccess, oexBOOL *x_pbAlreadyExists, oexINT64 x_llOffset )
{
    // Sanity checks
    if ( !oexCHECK_PTR( x_pMem ) || !oexCHECK_PTR_NULL_OK( x_pName ) )
        return oexFALSE;

	// Initialize pointer
	if ( x_pMem ) *x_pMem = NULL;

	DWORD dwAccess = 0;
	DWORD dwMemAccess = 0;

	// Figure out the access levels
	if ( eAccessRead & x_eAccess )
	{	if ( eAccessWrite & x_eAccess )
			dwAccess = PAGE_READWRITE, dwMemAccess = FILE_MAP_ALL_ACCESS;
		else dwAccess = PAGE_READONLY, dwMemAccess = FILE_MAP_READ;
	} // end if
	else if ( eAccessWrite & x_eAccess )
		dwAccess = PAGE_READWRITE, dwMemAccess = FILE_MAP_WRITE;

	// Allow access
	SECURITY_ATTRIBUTES  sa, *pSa = NULL;
	sa.nLength = sizeof( SECURITY_ATTRIBUTES );
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = (SECURITY_DESCRIPTOR*)LocalAlloc(LPTR,SECURITY_DESCRIPTOR_MIN_LENGTH);
	if ( sa.lpSecurityDescriptor
		 && InitializeSecurityDescriptor( (SECURITY_DESCRIPTOR*)sa.lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION )
		 && SetSecurityDescriptorDacl( (SECURITY_DESCRIPTOR*)sa.lpSecurityDescriptor, TRUE, (PACL)NULL, FALSE ) )
		pSa = &sa;

	HANDLE hFileHandle = oexNULL;
	if ( x_pFile )
	{	hFileHandle = CreateFile( x_pFile, GENERIC_READ | GENERIC_WRITE,
									 FILE_SHARE_READ | FILE_SHARE_WRITE,
									 pSa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( !oexVERIFY( INVALID_HANDLE_VALUE != hFileHandle ) )
		{	if ( pSa )
				LocalFree( pSa->lpSecurityDescriptor );
			return c_Failed;
		} // end if

		if ( !x_llSize )
		{	DWORD dwHi, dwLo = GetFileSize( hFileHandle, &dwHi );
			x_llSize = (oexINT64)dwLo | ( (oexINT64)dwHi << 32 );
		} // end if

	} // end if
	else hFileHandle = (HANDLE)~0;

	// Create the file mapping
	HANDLE hFile = CreateFileMapping( hFileHandle, pSa, dwAccess,
			    					  (DWORD)( x_llSize >> 32 ), (DWORD)( x_llSize & 0xffffffff ), x_pName );

	if ( pSa )
		LocalFree( pSa->lpSecurityDescriptor );

	// Do they want to know if it was pre-existing?
	if ( x_pbAlreadyExists )
		*x_pbAlreadyExists = ( ERROR_ALREADY_EXISTS == GetLastError() );

	// Close the file handle
	if ( x_pFile )
		CloseHandle( hFileHandle );

	// Did we get the handle
	if ( NULL == hFile )
		return c_Failed;

	// Get a pointer to the file data
	oexPVOID pMem = (oexPVOID)MapViewOfFile( hFile, dwMemAccess, 0, 0, 0 );
	if ( !pMem )
	{	CloseHandle( hFile );
		return c_Failed;
	} // end if

	// Verify memory size
	MEMORY_BASIC_INFORMATION mbi;
	if ( VirtualQuery( pMem, &mbi, sizeof( mbi ) ) < sizeof( mbi )
		 || MEM_COMMIT != mbi.State
		 || mbi.BaseAddress != pMem
		 || mbi.RegionSize < (SIZE_T)x_llSize )
	{	UnmapViewOfFile( pMem );
		CloseHandle( hFile );
		return c_Failed;
	} // end if

	// Set memory pointer
	if ( x_pMem ) *x_pMem = pMem;

	// Save the final size
	if ( x_pllSize ) *x_pllSize = x_llSize;

	// Use pointer as handle
	return (t_HFILEMAP)hFile;
}

oexBOOL CFMap::Release( CFMap::t_HFILEMAP x_hFileMap, oexPVOID x_pMem, oexINT64 x_llSize )
{
	oexUNUSED( x_llSize );

	if ( x_pMem && oexCHECK_PTR( x_pMem ) )
		UnmapViewOfFile( (LPVOID)x_pMem );

	if ( c_Failed != x_hFileMap && oexCHECK_PTR( x_hFileMap ) )
		CloseHandle( (HANDLE)x_hFileMap );

	return oexTRUE;
}

