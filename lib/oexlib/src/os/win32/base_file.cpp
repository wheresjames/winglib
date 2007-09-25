/*------------------------------------------------------------------
// base_file.cpp
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

oexSTATIC_ASSERT( sizeof( CBaseFile::t_HFILE ) == sizeof( HANDLE ) );
oexSTATIC_ASSERT( sizeof( CBaseFile::t_HFIND ) == sizeof( HANDLE ) );

const CBaseFile::t_HFILE CBaseFile::c_Invalid = INVALID_HANDLE_VALUE;
const CBaseFile::t_HFIND CBaseFile::c_InvalidFindHandle = INVALID_HANDLE_VALUE;

CBaseFile::t_HFILE CBaseFile::Create( oexCSTR x_pFile, oexUINT x_eDisposition, oexUINT x_eAccess, oexUINT x_eShare, oexUINT x_uFlags, oexUINT *x_puError )
{
    DWORD dwDisposition = OPEN_EXISTING;
    switch( x_eDisposition )
    {   case eDisCreateNew : dwDisposition = CREATE_NEW; break;
        case eDisCreateAlways : dwDisposition = CREATE_ALWAYS; break;
        case eDisOpenExisting : dwDisposition = OPEN_EXISTING; break;
        case eDisOpenAlways : dwDisposition = OPEN_ALWAYS; break;
    } // end switch

    DWORD dwAccess = 0;
    if ( 0 != ( x_eAccess & eAccessRead ) ) dwAccess |= GENERIC_READ;
    if ( 0 != ( x_eAccess & eAccessWrite ) ) dwAccess |= GENERIC_WRITE;
    if ( 0 != ( x_eAccess & eAccessExecute ) ) dwAccess |= GENERIC_EXECUTE;

    DWORD dwShare = 0;
    if ( 0 != ( x_eShare & eShareDelete ) ) dwShare |= FILE_SHARE_DELETE;
    if ( 0 != ( x_eShare & eShareRead ) ) dwShare |= FILE_SHARE_READ;
    if ( 0 != ( x_eShare & eShareWrite ) ) dwShare |= FILE_SHARE_WRITE;

    t_HFILE hFile = ::CreateFile( x_pFile, dwAccess, dwShare, NULL, dwDisposition, x_uFlags, NULL );

    if ( x_puError )
    {   if ( INVALID_HANDLE_VALUE == hFile )
            *x_puError = GetLastError();
        else *x_puError = 0;
    } // end if

    return hFile;        
}


oexBOOL CBaseFile::Close( CBaseFile::t_HFILE x_hFile, oexUINT *x_puErr )
{
    if ( INVALID_HANDLE_VALUE == x_hFile )
        return oexFALSE;

    oexBOOL bRet = ::CloseHandle( x_hFile ) ? oexTRUE : oexFALSE;

    if ( x_puErr )
    {   if ( bRet ) *x_puErr = 0;
        else *x_puErr = GetLastError();
    } // end if

    return bRet;
}


oexBOOL CBaseFile::Write( CBaseFile::t_HFILE hFile, oexCPVOID x_pData, oexUINT x_uSize, oexUINT *x_puWritten, oexUINT *x_puErr )
{
    DWORD dwWritten = 0;
    oexBOOL bRet = ::WriteFile( hFile, x_pData, x_uSize, &dwWritten, NULL ) ? oexTRUE : oexFALSE;
    
    if ( x_puWritten )
        *x_puWritten = dwWritten;

    if ( x_puErr )
    {   if ( bRet ) *x_puErr = 0;
        else *x_puErr = GetLastError();
    } // end if

    return bRet;
}

oexBOOL CBaseFile::Read( CBaseFile::t_HFILE hFile, oexPVOID x_pData, oexUINT x_uSize, oexUINT *x_puRead, oexUINT *x_puErr )
{
    DWORD dwRead = 0;
    oexBOOL bRet = ::ReadFile( hFile, x_pData, x_uSize, &dwRead, NULL ) ? oexTRUE : oexFALSE;
    
    if ( x_puRead )
        *x_puRead = dwRead;

    if ( x_puErr )
    {   if ( bRet ) *x_puErr = 0;
        else *x_puErr = GetLastError();
    } // end if

    return bRet;
}

oexINT64 CBaseFile::Size( t_HFILE hFile )
{   if ( c_Invalid == hFile ) return 0;
    DWORD dwHi = 0, dwLo = ::GetFileSize( hFile, &dwHi );
    return (oexINT64)dwLo | ( (oexINT64)dwHi << 32 );
}

oexINT64 CBaseFile::SetPointer( t_HFILE hFile, oexINT64 llMove, oexINT nMethod )
{
    // Get method
    DWORD dwMethod = 0;
    if ( nMethod == eFileOffsetBegin ) dwMethod = FILE_BEGIN;
    else if ( nMethod == eFileOffsetCurrent ) dwMethod = FILE_CURRENT;
    else if ( nMethod == eFileOffsetEnd ) dwMethod = FILE_END;
    else return -1;

    // Do the move
    SetLastError( NO_ERROR );
	LONG lHi = (DWORD)( ( llMove >> 32 ) & 0xffffffff );
    DWORD dwRet = ::SetFilePointer( hFile, (LONG)( llMove & 0xffffffff ), &lHi, dwMethod );

    // Check for error
    if ( dwRet == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR )
        return 0;

    // Return the result
    return (oexINT64)dwRet | ( (oexINT64)lHi << 32 );
}

oexBOOL CBaseFile::Delete( oexCSTR x_pFile )
{	::SetFileAttributes( x_pFile, FILE_ATTRIBUTE_NORMAL );
    return ::DeleteFile( x_pFile ) ? oexTRUE : oexFALSE;
}

oexBOOL CBaseFile::DeleteDirectory( oexCSTR x_pDir )
{	::SetFileAttributes( x_pDir, FILE_ATTRIBUTE_NORMAL );
    return ::RemoveDirectory( x_pDir ) ? oexTRUE : oexFALSE;
}


static oexUINT g_ConvBaseFileAttribToWinAttrib[] =
{
    CBaseFile::eFileAttribReadOnly,             FILE_ATTRIBUTE_READONLY,
    CBaseFile::eFileAttribHidden,               FILE_ATTRIBUTE_HIDDEN,
    CBaseFile::eFileAttribSystem,               FILE_ATTRIBUTE_SYSTEM,
    CBaseFile::eFileAttribDirectory,            FILE_ATTRIBUTE_DIRECTORY,
    CBaseFile::eFileAttribArchive,              FILE_ATTRIBUTE_ARCHIVE,
    CBaseFile::eFileAttribDevice,               FILE_ATTRIBUTE_DEVICE,
    CBaseFile::eFileAttribNormal,               FILE_ATTRIBUTE_NORMAL,
    CBaseFile::eFileAttribTemporary,            FILE_ATTRIBUTE_TEMPORARY,
    CBaseFile::eFileAttribSparseFile,           FILE_ATTRIBUTE_SPARSE_FILE,
    CBaseFile::eFileAttribReparsePoint,         FILE_ATTRIBUTE_REPARSE_POINT,
    CBaseFile::eFileAttribCompressed,           FILE_ATTRIBUTE_COMPRESSED,
    CBaseFile::eFileAttribOffline,              FILE_ATTRIBUTE_OFFLINE,
    CBaseFile::eFileAttribNotContentIndexed,    FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,
    CBaseFile::eFileAttribEncrypted,            FILE_ATTRIBUTE_ENCRYPTED,
    -1, -1
};

static DWORD CBaseFile_ToFileAttributes( oexUINT uAttrib )
{
    DWORD dwAttrib = 0;
    for( oexUINT i = 0; -1 != g_ConvBaseFileAttribToWinAttrib[ i ] 
                        && -1 != g_ConvBaseFileAttribToWinAttrib[ i + 1 ];
                        i += 2 )
    if ( 0 != ( uAttrib & g_ConvBaseFileAttribToWinAttrib[ i ] ) )
        dwAttrib |= g_ConvBaseFileAttribToWinAttrib[ i + 1 ];

    return dwAttrib;
}

static oexUINT CBaseFile_ToCBaseFileAttributes( DWORD dwAttrib )
{
    DWORD uAttrib = 0;
    for( oexUINT i = 0; -1 != g_ConvBaseFileAttribToWinAttrib[ i ] 
                        && -1 != g_ConvBaseFileAttribToWinAttrib[ i + 1 ];
                        i += 2 )
    if ( 0 != ( dwAttrib & g_ConvBaseFileAttribToWinAttrib[ i + 1 ] ) )
        uAttrib |= g_ConvBaseFileAttribToWinAttrib[ i ];

    return uAttrib;
}

oexUINT CBaseFile::GetFileAttrib( oexCSTR x_pFile )
{   return CBaseFile_ToCBaseFileAttributes( ::GetFileAttributes( x_pFile ) ); }

oexBOOL CBaseFile::SetFileAttrib( oexCSTR x_pFile, oexUINT x_uAttrib )
{   return ::SetFileAttributes( x_pFile, x_uAttrib ) ? oexTRUE : oexFALSE; }

void CBaseFile_InitFindData( CBaseFile::SFindData *x_pFd )
{   x_pFd->llSize = 0;
    x_pFd->sName.Destroy();
    x_pFd->uFileAttributes = 0;
    x_pFd->ftCreated = 0;
    x_pFd->ftLastAccess = 0;
    x_pFd->ftLastModified = 0;
}

CBaseFile::t_HFIND CBaseFile::FindFirst( oexCSTR x_pPath, oexCSTR x_pMask, CBaseFile::SFindData *x_pFd )
{
    // Sanity checks
    if ( !oexVERIFY_PTR( x_pPath ) || !oexVERIFY_PTR( x_pMask ) || !oexVERIFY_PTR( x_pFd ) )
        return oexFALSE;

    WIN32_FIND_DATA wfd;
    ZeroMemory( &wfd, sizeof( wfd ) );
    
    // Find first file
    HANDLE hFind = ::FindFirstFile( CStr::BuildPath( x_pPath, x_pMask ).Ptr(), &wfd );
    if ( INVALID_HANDLE_VALUE == hFind )
        return CBaseFile::c_InvalidFindHandle;

    // Copy over data
    x_pFd->uFileAttributes = wfd.dwFileAttributes;
    x_pFd->ftCreated = (oexINT64)wfd.ftCreationTime.dwLowDateTime | ( (oexINT64)wfd.ftCreationTime.dwHighDateTime << 32 );
    x_pFd->ftLastAccess = (oexINT64)wfd.ftLastAccessTime.dwLowDateTime | ( (oexINT64)wfd.ftLastAccessTime.dwHighDateTime << 32 );
    x_pFd->ftLastModified = (oexINT64)wfd.ftLastWriteTime.dwLowDateTime | ( (oexINT64)wfd.ftLastWriteTime.dwHighDateTime << 32 );
    x_pFd->llSize = (oexINT64)wfd.nFileSizeLow | ( (oexINT64)wfd.nFileSizeHigh << 32 );
    x_pFd->sName = wfd.cFileName;

    // Return the file handle
    return (CBaseFile::t_HFIND)hFind;
}

oexBOOL CBaseFile::FindNext( t_HFIND x_hFind, CBaseFile::SFindData *x_pFd )
{
    // Sanity checks
    if ( !oexVERIFY( vInvalidFindHandle() != x_hFind ) || !oexVERIFY_PTR( x_pFd ) )
        return oexFALSE;

    WIN32_FIND_DATA wfd;
    ZeroMemory( &wfd, sizeof( wfd ) );

    // Attempt to find the next file
    if ( !::FindNextFile( x_hFind, &wfd ) )
        return oexFALSE;

    // Copy over data
    x_pFd->uFileAttributes = wfd.dwFileAttributes;
    x_pFd->ftCreated = (oexINT64)wfd.ftCreationTime.dwLowDateTime | ( (oexINT64)wfd.ftCreationTime.dwHighDateTime << 32 );
    x_pFd->ftLastAccess = (oexINT64)wfd.ftLastAccessTime.dwLowDateTime | ( (oexINT64)wfd.ftLastAccessTime.dwHighDateTime << 32 );
    x_pFd->ftLastModified = (oexINT64)wfd.ftLastWriteTime.dwLowDateTime | ( (oexINT64)wfd.ftLastWriteTime.dwHighDateTime << 32 );
    x_pFd->llSize = (oexINT64)wfd.nFileSizeLow | ( (oexINT64)wfd.nFileSizeHigh << 32 );
    x_pFd->sName = wfd.cFileName;

    return oexTRUE;
}

oexBOOL CBaseFile::FindClose( t_HFIND x_hFind )
{   if ( vInvalidFindHandle() == x_hFind )
        return oexFALSE;
    return ::FindClose( x_hFind ) ? oexTRUE : oexFALSE;
}


oexBOOL CBaseFile::DoesExist( oexCSTR x_pPath )
{   return INVALID_FILE_ATTRIBUTES != ::GetFileAttributes( x_pPath );
}

oexBOOL CBaseFile::CreateFolder( oexCSTR x_pPath )
{   return ::CreateDirectory( x_pPath, NULL ) ? oexTRUE : oexFALSE; }