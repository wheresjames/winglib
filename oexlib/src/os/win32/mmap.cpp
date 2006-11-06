/*------------------------------------------------------------------
// mmap.cpp
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
// http://www.opengroup.org/onlinepubs/000095399/functions/mmap.html

#include "../../stdafx.h"
#include "stdwin.h"
#include "io.h"

OEXLIB_USING_NAMESPACE

const oexHMEMMAP CMMap::c_MapFailed = (void*)-1;

oexHMEMMAP CMMap::osMMap( oexCPVOID x_addr, oexINT64 x_len, oexINT x_prot, oexINT x_flags, oexINT x_fildes, oexINT x_off )
{
	// Validation
	oexVALIDATE_PTR_NULL_OK( x_addr );

	// Clear last error
	SetLastError( 0 );

	// Sanity check
	if ( eProtNone == x_prot ) 
	{	SetLastError( ERROR_INVALID_PARAMETER );
		return c_MapFailed;
	} // end if

	// Assume memory
	HANDLE hFile = INVALID_HANDLE_VALUE;

	// Did we get a file descriptor? 
	if ( x_fildes != -1 )
	{
		// Test file descriptor
		hFile = (HANDLE)_get_osfhandle( x_fildes );		
		if( INVALID_HANDLE_VALUE == hFile ) 
		{	SetLastError( ERROR_INVALID_HANDLE );
			return c_MapFailed;
		} // end if

	} // end if

	// Are we mapping a specified memory area?
	if ( 0 != ( x_flags & eMapFixed ) && !x_addr )
	{	SetLastError( ERROR_INVALID_PARAMETER );
		return c_MapFailed;
	} // end if

	// Security attributes
	SECURITY_ATTRIBUTES sa;
	ZeroMemory( &sa, sizeof( sa ) );
	sa.nLength = sizeof( sa );

	// Access attributes
	DWORD dwDesiredAccess = FILE_MAP_READ;
	DWORD flProtect = PAGE_READONLY;

	// Protection level
	if ( 0 != ( eProtWrite & x_prot ) )
	{
		// Shared access
		if ( 0 != ( eMapShared & x_flags ) )
		{
			sa.bInheritHandle = TRUE;
			dwDesiredAccess = FILE_MAP_ALL_ACCESS;

		} // end if

		// Private access
		else if ( 0 != ( eMapPrivate & x_flags ) )
		{	dwDesiredAccess = FILE_MAP_COPY;
			flProtect = PAGE_WRITECOPY;
		} // end else if

	} // end if

	// Create the file mapping
	HANDLE hMap = CreateFileMapping( hFile, &sa, flProtect, 
									 (DWORD)( (LONGLONG)x_len >> 32 ), 
									 (DWORD)( x_len & 0xffffffff ), 0 );
	if ( INVALID_HANDLE_VALUE == hMap )
		return c_MapFailed;

	// Map the file to a memory address
	void *pPtr = MapViewOfFileEx(	hMap, dwDesiredAccess, (DWORD)( (LONGLONG)x_off >> 32 ),
									(DWORD)( x_off & 0xffffffff ), (DWORD)( x_len & 0xffffffff ), 
									(LPVOID)x_addr );
	DWORD dwLastError = GetLastError();

	// Close the file handle
	CloseHandle( hMap );

	// Did we get a valid pointer?
	if ( !pPtr ) 
	{	SetLastError( dwLastError );
		return c_MapFailed;
	} // end if

	// Return the pointer
	return pPtr;
}


oexHMEMMAP CMMap::osMUnMap( oexHMEMMAP x_addr, size_t x_len )
{
	oexUNUSED( x_len );

	// Validation
	oexVALIDATE_PTR_NULL_OK( x_addr );

	// Don't try to unmap the NULL pointer
	if ( !x_addr ) return c_MapFailed;

	// Unmap the view
	return UnmapViewOfFile( x_addr ) ? 0 : c_MapFailed;
}
