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

oexSTATIC_ASSERT( sizeof( CBaseFile::t_HFILE ) == sizeof( FILE* ) );
oexSTATIC_ASSERT( sizeof( CBaseFile::t_HFIND ) == sizeof( DIR* ) );

const CBaseFile::t_HFILE CBaseFile::c_Invalid = NULL;
const CBaseFile::t_HFIND CBaseFile::c_InvalidFindHandle = NULL;

CBaseFile::t_HFILE CBaseFile::Create( oexCSTR x_pFile, oexUINT x_eDisposition, oexUINT x_eAccess, oexUINT x_eShare, oexUINT x_uFlags, oexUINT *x_puError )
{
	// Select mode string
	oexCSTR pMode = oexT( "" );	
    switch( x_eDisposition )
    {   case eDisCreateNew : pMode = oexT( "w+b" ); break;
        case eDisCreateAlways : pMode = oexT( "w+b" ); break;
        case eDisOpenExisting : pMode = oexT( "rb" ); break;
        case eDisOpenAlways : pMode = oexT( "a+b" ); break;
    } // end switch

	// Attempt to open the file
	t_HFILE hFile =	fopen( oexStrToMbPtr( x_pFile ), oexStrToMbPtr( pMode ) );

	if ( x_puError )
	{
		if ( !hFile )
			*x_puError = errno;		
		else
			hFile = 0;
	} // end if

	return hFile;	
}

oexBOOL CBaseFile::Close( CBaseFile::t_HFILE x_hFile, oexUINT *x_puErr )
{
    if ( CBaseFile::c_InvalidFindHandle == x_hFile )
        return oexFALSE;

    oexBOOL bRet = fclose( (FILE*)x_hFile ) ? oexFALSE : oexTRUE;

    if ( x_puErr )
    {   if ( bRet ) *x_puErr = 0;
        else *x_puErr = errno;
    } // end if

    return bRet;
}

oexBOOL CBaseFile::Write( CBaseFile::t_HFILE hFile, oexCPVOID x_pData, oexUINT x_uSize, oexUINT *x_puWritten, oexUINT *x_puErr )
{
    oexUINT uWritten = fwrite( x_pData, 1, x_uSize, (FILE*)hFile );
    
    if ( x_puWritten )
        *x_puWritten = uWritten;

    if ( x_puErr )
    {   if ( x_uSize == uWritten ) *x_puErr = 0;
        else *x_puErr = errno;
    } // end if

    return ( x_uSize == uWritten );
}

oexBOOL CBaseFile::Read( CBaseFile::t_HFILE hFile, oexPVOID x_pData, oexUINT x_uSize, oexUINT *x_puRead, oexUINT *x_puErr )
{
	oexUINT uRead = fread( x_pData, 1, x_uSize, (FILE*)hFile );
    
    if ( x_puRead )
        *x_puRead = uRead;

	oexINT nErr = ferror( (FILE*)hFile );

    if ( x_puErr )
    	*x_puErr = nErr;

    return !nErr;
}

oexINT64 CBaseFile::Size( t_HFILE hFile )
{
	struct stat64 s64;

	// +++ Ensure this works correctly	
	if ( fstat64( (int)hFile, &s64 ) )
		return 0;
		
	return s64.st_size;	
}

oexINT64 CBaseFile::SetPointer( t_HFILE hFile, oexINT64 llMove, oexINT nMethod )
{
    // Get method
    oexINT nOrigin = 0;
    if ( nMethod == eFileOffsetBegin ) nOrigin = SEEK_SET;
    else if ( nMethod == eFileOffsetCurrent ) nOrigin = SEEK_CUR;
    else if ( nMethod == eFileOffsetEnd ) nOrigin = SEEK_END;
    else return -1;
    
    // Set new file position
    int ret = fseeko64( (FILE*)hFile, llMove, nOrigin );

	// Grab the current file  position so we can report it
	oexINT64 llPos = ftello64( (FILE*)hFile );
	
	if ( 0 > llPos )
		return 0;

    // Return the result
    return llPos;   
}

oexBOOL CBaseFile::Delete( oexCSTR x_pFile )
{
	if ( !oexCHECK_PTR( x_pFile ) )
		return oexFALSE;

	chmod( oexStrToMbPtr( x_pFile ), 0xffff );
	return unlink( oexStrToMbPtr( x_pFile ) ) ? oexFALSE : oexTRUE;
}

oexBOOL CBaseFile::RemoveFolder( oexCSTR x_pDir )
{	
	if ( !oexCHECK_PTR( x_pDir ) )
		return oexFALSE;

	chmod( oexStrToMbPtr( x_pDir ), 0xffff );
	return rmdir( oexStrToMbPtr( x_pDir ) ) ? oexFALSE : oexTRUE;
}

/*
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

*/

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

	DIR *hDir = opendir( oexStrToMbPtr( CStr::BuildPath( x_pPath, x_pMask ).Ptr() ) );
	if ( CBaseFile::c_InvalidFindHandle == hDir )
		return CBaseFile::c_InvalidFindHandle;

	errno = 0;		
	struct dirent *pD = readdir( hDir );
	if ( !pD || errno )
	{	closedir( hDir );
		return CBaseFile::c_InvalidFindHandle;
	} // end if

	CBaseFile_InitFindData( x_pFd );
	
	x_pFd->sName = oexMbToStr( pD->d_name );
	
    return (CBaseFile::t_HFIND)hDir;
}

oexBOOL CBaseFile::FindNext( t_HFIND x_hFind, CBaseFile::SFindData *x_pFd )
{
	DIR *hDir = (DIR*)x_hFind;

	errno = 0;		
	struct dirent *pD = readdir( hDir );
	if ( !pD || errno )
	{	closedir( hDir );
		return oexFALSE;
	} // end if

	CBaseFile_InitFindData( x_pFd );	
	x_pFd->sName = oexMbToStr( pD->d_name );
	
    return oexTRUE;
}

oexBOOL CBaseFile::FindClose( t_HFIND x_hFind )
{	return closedir( (DIR*)x_hFind ) ? oexFALSE : oexTRUE;
}


oexBOOL CBaseFile::DoesExist( oexCSTR x_pPath )
{	struct stat64 s64;
	return	stat64( oexStrToMbPtr( x_pPath ), &s64 ) ? oexFALSE : oexTRUE;
}

oexBOOL CBaseFile::CreateFolder( oexCSTR x_pPath )
{   return mkdir( oexStrToMbPtr( x_pPath ), 0 ) ? oexFALSE : oexTRUE; 
}


