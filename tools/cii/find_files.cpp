// find_files.cpp

#if defined( _WIN32 )
#	include "Windows.h"
#endif
#include <string>

#include "str.h"
#include "find_files.h"

namespace ff
{

HFIND FindFirst( const char *x_pPath, const char *x_pMask, SFindData *x_pFd )
{
	// Sanity checks
	if ( !x_pPath || !x_pMask || !x_pFd )
		return c_invalid_hfind;

	WIN32_FIND_DATA wfd;
	ZeroMemory( &wfd, sizeof( wfd ) );

	// Find first file
	HANDLE hFind = ::FindFirstFile( BuildPath< char >( std::basic_string< char >( x_pPath ), std::basic_string< char >( x_pMask ) ).c_str(), &wfd );
	if ( INVALID_HANDLE_VALUE == hFind )
		return c_invalid_hfind;

	// Copy over data
	x_pFd->uFileAttributes = wfd.dwFileAttributes;
	x_pFd->ftCreated = (tcnv::tc_int64)wfd.ftCreationTime.dwLowDateTime | ( (tcnv::tc_int64)wfd.ftCreationTime.dwHighDateTime << 32 );
	x_pFd->ftLastAccess = (tcnv::tc_int64)wfd.ftLastAccessTime.dwLowDateTime | ( (tcnv::tc_int64)wfd.ftLastAccessTime.dwHighDateTime << 32 );
	x_pFd->ftLastModified = (tcnv::tc_int64)wfd.ftLastWriteTime.dwLowDateTime | ( (tcnv::tc_int64)wfd.ftLastWriteTime.dwHighDateTime << 32 );
	x_pFd->llSize = (tcnv::tc_int64)wfd.nFileSizeLow | ( (tcnv::tc_int64)wfd.nFileSizeHigh << 32 );
	zstr::Copy( (char*)x_pFd->szName, sizeof( x_pFd->szName ), (const char*)wfd.cFileName );

	// Return the file handle
	return (HFIND)hFind;
}

bool FindNext( HFIND x_hFind, SFindData *x_pFd )
{
	// Sanity checks
	if ( c_invalid_hfind != x_hFind || !x_pFd )
		return false;

	WIN32_FIND_DATA wfd;
	ZeroMemory( &wfd, sizeof( wfd ) );

	// Attempt to find the next file
	if ( !::FindNextFile( x_hFind, &wfd ) )
		return false;

	// Copy over data
	x_pFd->uFileAttributes = wfd.dwFileAttributes;
	x_pFd->ftCreated = (tcnv::tc_int64)wfd.ftCreationTime.dwLowDateTime | ( (tcnv::tc_int64)wfd.ftCreationTime.dwHighDateTime << 32 );
	x_pFd->ftLastAccess = (tcnv::tc_int64)wfd.ftLastAccessTime.dwLowDateTime | ( (tcnv::tc_int64)wfd.ftLastAccessTime.dwHighDateTime << 32 );
	x_pFd->ftLastModified = (tcnv::tc_int64)wfd.ftLastWriteTime.dwLowDateTime | ( (tcnv::tc_int64)wfd.ftLastWriteTime.dwHighDateTime << 32 );
	x_pFd->llSize = (tcnv::tc_int64)wfd.nFileSizeLow | ( (tcnv::tc_int64)wfd.nFileSizeHigh << 32 );
	zstr::Copy( (char*)x_pFd->szName, sizeof( x_pFd->szName ), (const char*)wfd.cFileName );

	return true;
}

bool FindClose( HFIND x_hFind )
{
	if ( c_invalid_hfind == x_hFind )
		return false;
	return ::FindClose( x_hFind ) ? true : false;
}

}; // namespace ff