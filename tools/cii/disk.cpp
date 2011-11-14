// disk.cpp

#if defined( _WIN32 )
#	include <Windows.h>
#	include <stdio.h>
#else
#endif

#include <string>
#include "str.h"
#include "disk.h"

namespace disk
{

#if defined( _WIN32 )
		typedef FILE* tHFILE;
#else
		typedef int tHFILE;
#endif

HFILE Open( const char *x_pFile, const char *x_pMode )
{
	return (HFILE)fopen( x_pFile, x_pMode );
}

t_size Write( const void *x_pData, t_size x_nSize, t_size x_nCount, HFILE x_hFile )
{
	return fwrite( x_pData, x_nSize, x_nCount, (tHFILE)x_hFile );
}

t_size Read( void *x_pData, t_size x_nSize, t_size x_nCount, HFILE x_hFile )
{
	return fread( x_pData, x_nSize, x_nCount, (tHFILE)x_hFile );
}

t_size Close( HFILE x_hFile )
{
	return fclose( (tHFILE)x_hFile );
}

#if defined( _WIN32 )

bool mkdir( const char *x_pPath )
{
	return ::CreateDirectory( x_pPath, NULL ) ? true : false;
}

bool exists( const char *x_pPath )
{
	return ( INVALID_FILE_ATTRIBUTES != ::GetFileAttributes( x_pPath ) ) ? true : false;
}

bool unlink( const char *x_pFile )
{
	::SetFileAttributes( x_pFile, FILE_ATTRIBUTE_NORMAL );
	return ::DeleteFile( x_pFile ) ? true : false;
}

bool rmdir( const char *x_pPath )
{
	::SetFileAttributes( x_pPath, FILE_ATTRIBUTE_NORMAL );
	return ::RemoveDirectory( x_pPath ) ? true : false;
}

str::tc_int64 Size( const char *x_pFile )
{
	if ( !x_pFile || !*x_pFile )
		return 0;

	// Get file info
    WIN32_FILE_ATTRIBUTE_DATA wfad; memset( &wfad, 0, sizeof( wfad ) );	
	if ( !GetFileAttributesEx( x_pFile, GetFileExInfoStandard, (LPVOID)&wfad ) )
		return 0;

	// Return the size
	return (str::tc_int64)wfad.nFileSizeLow | ( (str::tc_int64)wfad.nFileSizeHigh << 32 );
}

str::tc_int64 Size( HFILE hFile )
{
	// Ensure valid handle
	if ( c_invalid_hfile == hFile ) 
		return 0;

	// Save the current position and seek to the end of the file
	long pos = ftell( (tHFILE)hFile );
	fseek ( (tHFILE)hFile, 0, SEEK_END );

	// Read the file size
    long size = ftell( (tHFILE)hFile );

	// Restore position and return the size
	fseek( (tHFILE)hFile, pos, SEEK_SET );
	return size;
}


HFIND FindFirst( const char *x_pPath, const char *x_pMask, SFindData *x_pFd )
{
	// Sanity checks
	if ( !x_pPath || !x_pMask || !x_pFd )
		return c_invalid_hfind;

	WIN32_FIND_DATA wfd;
	ZeroMemory( &wfd, sizeof( wfd ) );

	// Where will we be looking?
	std::basic_string< char > sRoot = FilePath< char >( std::basic_string< char >( x_pPath ), std::basic_string< char >( x_pMask ) );

	// Find first file
	HANDLE hFind = ::FindFirstFile( sRoot.c_str(), &wfd );
	if ( INVALID_HANDLE_VALUE == hFind )
		return c_invalid_hfind;

	// Copy over data
	x_pFd->uFileAttributes = wfd.dwFileAttributes;
	x_pFd->ftCreated = (str::tc_int64)wfd.ftCreationTime.dwLowDateTime | ( (str::tc_int64)wfd.ftCreationTime.dwHighDateTime << 32 );
	x_pFd->ftLastAccess = (str::tc_int64)wfd.ftLastAccessTime.dwLowDateTime | ( (str::tc_int64)wfd.ftLastAccessTime.dwHighDateTime << 32 );
	x_pFd->ftLastModified = (str::tc_int64)wfd.ftLastWriteTime.dwLowDateTime | ( (str::tc_int64)wfd.ftLastWriteTime.dwHighDateTime << 32 );
	x_pFd->llSize = (str::tc_int64)wfd.nFileSizeLow | ( (str::tc_int64)wfd.nFileSizeHigh << 32 );
	zstr::Copy( x_pFd->szName, sizeof( x_pFd->szName ), wfd.cFileName );

	// Return the file handle
	return (HFIND)hFind;
}

bool FindNext( HFIND x_hFind, SFindData *x_pFd )
{
	// Sanity checks
	if ( c_invalid_hfind == x_hFind || !x_pFd )
		return false;

	WIN32_FIND_DATA wfd;
	ZeroMemory( &wfd, sizeof( wfd ) );

	// Attempt to find the next file
	if ( !::FindNextFile( x_hFind, &wfd ) )
		return false;

	// Copy over data
	x_pFd->uFileAttributes = wfd.dwFileAttributes;
	x_pFd->ftCreated = (str::tc_int64)wfd.ftCreationTime.dwLowDateTime | ( (str::tc_int64)wfd.ftCreationTime.dwHighDateTime << 32 );
	x_pFd->ftLastAccess = (str::tc_int64)wfd.ftLastAccessTime.dwLowDateTime | ( (str::tc_int64)wfd.ftLastAccessTime.dwHighDateTime << 32 );
	x_pFd->ftLastModified = (str::tc_int64)wfd.ftLastWriteTime.dwLowDateTime | ( (str::tc_int64)wfd.ftLastWriteTime.dwHighDateTime << 32 );
	x_pFd->llSize = (str::tc_int64)wfd.nFileSizeLow | ( (str::tc_int64)wfd.nFileSizeHigh << 32 );
	zstr::Copy( x_pFd->szName, sizeof( x_pFd->szName ), wfd.cFileName );

	return true;
}

bool FindClose( HFIND x_hFind )
{
	if ( c_invalid_hfind == x_hFind )
		return false;
	return ::FindClose( x_hFind ) ? true : false;
}

#else

#endif

}; // namespace ff