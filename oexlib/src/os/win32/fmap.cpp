/*------------------------------------------------------------------
// fmap.cpp
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

#include "../../stdafx.h"
#include "stdwin.h"

OEXLIB_USING_NAMESPACE

const oexHFILEMAP CFMap::c_Failed = NULL;

oexHFILEMAP CFMap::osCreateFileMapping( oexPVOID *x_pMem, oexINT64 x_llSize, oexCSTR x_pName, etAccess x_eAccess, oexBOOL *x_pbAlreadyExists )
{
	// Validation
	oexVALIDATE_PTR( x_pMem );
	oexVALIDATE_PTR_NULL_OK( x_pName );

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

	// Create the file mapping
	HANDLE hFile = CreateFileMapping( (HANDLE)0xffffffff, NULL, dwAccess, 
			    					  (DWORD)( x_llSize >> 32 ), (DWORD)( x_llSize & 0xffffffff ), x_pName );

	// Do they want to know if it was pre-existing?
	if ( x_pbAlreadyExists )
		*x_pbAlreadyExists = ( ERROR_ALREADY_EXISTS == GetLastError() );

	// Did we get the handle
	if ( NULL == hFile ) return c_Failed;
	
	// Get a pointer to the file data
	oexPVOID pMem = (oexPVOID)MapViewOfFile( hFile, dwMemAccess, 0, 0, 0 );

	// Close the file handle / View remains unmapped until a call to UnmapViewOfFile()
	CloseHandle( hFile );

	// Set memory pointer
	if ( x_pMem ) *x_pMem = pMem;

	// Use pointer as handle
	return (oexHFILEMAP)pMem;
}

oexBOOL CFMap::osReleaseFileMapping( oexHFILEMAP x_hFileMap )
{
	// Ensure valid pointer
	oexVALIDATE_PTR_NULL_OK( x_hFileMap );
	if ( c_Failed == x_hFileMap ) return oexFALSE;

	// Unmap the view of the file
	return UnmapViewOfFile( (LPCVOID)x_hFileMap ) ? oexTRUE : oexFALSE;
}