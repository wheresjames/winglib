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

#include "../../../oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

// Ensure size
oexSTATIC_ASSERT( sizeof( CFMap::t_HFILEMAP ) == sizeof( FILE* ) );

oexCONST CFMap::t_HFILEMAP CFMap::c_Failed = oexNULL;

CFMap::t_HFILEMAP CFMap::osCreateFileMapping( CFMap::t_HFILEMAP x_hFile, oexPVOID *x_pMem, oexINT64 x_llSize, oexINT64 *x_pllSize, etAccess x_eAccess, oexBOOL *x_pbAlreadyExists )
{
    // Sanity checks
    if ( !oexCHECK_PTR( x_pMem ) || CFMap::c_Failed == x_hFile )
        return CFMap::c_Failed;

	FILE *fd = (FILE*)x_hFile;
	oexPVOID pMem = mmap( oexNULL, x_llSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, (int)fd, 0 );
	if ( !pMem || MAP_FAILED == pMem )
	{	fclose( fd );
		return oexNULL;
	} // end if

	if ( x_pMem )
		*x_pMem = pMem;

	if ( x_pllSize )
		*x_pllSize = x_llSize;

	return (t_HFILEMAP)fd;
}

CFMap::t_HFILEMAP CFMap::osCreateFileMapping( oexCSTR x_pFile, oexPVOID *x_pMem, oexINT64 x_llSize, oexINT64 *x_pllSize, oexCSTR x_pName, etAccess x_eAccess, oexBOOL *x_pbAlreadyExists )
{
    // Sanity checks
    if ( !oexCHECK_PTR( x_pMem ) || !oexCHECK_PTR_NULL_OK( x_pName ) )
        return CFMap::c_Failed;

	// Create path
	CStr8 sPath;

	if ( x_pFile )
		sPath = oexStrToMb( x_pFile );

	else if ( x_pName )
	{
		// Convert name to multi-byte
		CStr8 sName = oexStrToMb( x_pName );
		if ( !sName.Length() )
			return CFMap::c_Failed;

		// Attempt to allocate space
		oexSTR8 pPath = sPath.OexAllocate( sName.Length() + PATH_MAX * 2 );
		if ( !pPath )
			return CFMap::c_Failed;

		// Attempt to create a temporary file
//		zstr::Copy( pPath, oexT( "/tmp/oex.tmp.XXXXXXXX" ) );

		// Make a temporary file
//		mkstemp( sPath._Ptr() );

		// Attempt to build a file name
		zstr::Copy( pPath, "/tmp/oex.shared.016dc44e-1208-4a38-9a48-9f97df77250b." );
		zstr::Append( pPath, sName.Ptr() );

	} // end if

	else
		return CFMap::c_Failed;

//	FILE *fd = fopen( sPath.Ptr(), "w+b" );
	FILE *fd = (FILE*)open( sPath.Ptr(), O_RDWR );
	if ( !fd )
		return CFMap::c_Failed;

	// Set the file size
	char buf[ 1024 ];
	oexMemSet( buf, 0, sizeof( buf ) );

	oexINT nSize = x_llSize;
	while ( nSize )
	{
		oexINT nWrite = nSize;
		if ( nWrite > sizeof( buf ) )
			nWrite = sizeof( buf );

		write( (int)fd, buf, nWrite );
		nSize -= nWrite;

	} // end while


	// Map memory
	oexPVOID pMem = mmap( oexNULL, x_llSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, (int)fd, 0 );
	if ( !pMem || MAP_FAILED == pMem )
	{	fclose( fd );
		return CFMap::c_Failed;
	} // end if

	if ( x_pMem )
		*x_pMem = pMem;

	if ( x_pllSize )
		*x_pllSize = x_llSize;

	return (t_HFILEMAP)fd;

/*
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

	HANDLE hFileHandle = oexNULL;
	if ( x_pFile )
	{	hFileHandle = CreateFile( x_pFile, GENERIC_READ | GENERIC_WRITE,
									 FILE_SHARE_READ | FILE_SHARE_WRITE,
									 NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( !oexVERIFY( INVALID_HANDLE_VALUE != hFileHandle ) )
			return c_Failed;

		if ( !x_llSize )
		{	DWORD dwHi, dwLo = GetFileSize( hFileHandle, &dwHi );
			x_llSize = (oexINT64)dwLo | ( (oexINT64)dwHi << 32 );
		} // end if

	} // end if
	else hFileHandle = (HANDLE)0xffffffff;

	// Create the file mapping
	HANDLE hFile = CreateFileMapping( hFileHandle, NULL, dwAccess,
			    					  (DWORD)( x_llSize >> 32 ), (DWORD)( x_llSize & 0xffffffff ), x_pName );

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

	// Set memory pointer
	if ( x_pMem ) *x_pMem = pMem;

	// Save the final size
	if ( x_pllSize ) *x_pllSize = x_llSize;

	// Use pointer as handle
	return (t_HFILEMAP)hFile;
*/
}

oexBOOL CFMap::osReleaseFileMapping( CFMap::t_HFILEMAP x_hFileMap, oexPVOID x_pMem, oexINT64 x_llSize )
{
	return oexFALSE;

	if ( x_pMem && oexCHECK_PTR( x_pMem ) )
		munmap( x_pMem, x_llSize );

	if ( c_Failed != x_hFileMap && oexCHECK_PTR( x_hFileMap ) )
		fclose( (FILE*)x_hFileMap );

	return oexTRUE;


/*	if ( x_pMem && oexCHECK_PTR( x_pMem ) )
		UnmapViewOfFile( (LPCVOID)x_pMem );

	if ( c_Failed != x_hFileMap && oexCHECK_PTR( x_hFileMap ) )
		CloseHandle( (HANDLE)x_hFileMap );

	return oexTRUE;
*/
}
