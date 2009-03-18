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

// BinReloc library
#include "opc/breloc/binreloc.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

oexSTATIC_ASSERT( sizeof( CBaseFile::t_HFILE ) >= sizeof( FILE* ) );
#if !defined( OEX_NODIRENT )
oexSTATIC_ASSERT( sizeof( CBaseFile::t_HFIND ) >= sizeof( DIR* ) );
#endif
oexSTATIC_ASSERT( sizeof( CBaseFile::t_HFILE ) >= sizeof( int ) );

const CBaseFile::t_HFILE CBaseFile::c_Invalid = (void*)-1;
const CBaseFile::t_HFIND CBaseFile::c_InvalidFindHandle = NULL;

CBaseFile::t_HFILE CBaseFile::Create( oexCSTR x_pFile, oexUINT x_eDisposition, oexUINT x_eAccess, oexUINT x_eShare, oexUINT x_uFlags, oexINT *x_pnError )
{
	// Select mode string
/*	oexCSTR pMode = oexT( "" );
    switch( x_eDisposition )
    {   case eDisCreateNew : pMode = oexT( "w+b" ); break;
        case eDisCreateAlways : pMode = oexT( "w+b" ); break;
        case eDisOpenExisting : pMode = oexT( "rb" ); break;
        case eDisOpenAlways : pMode = oexT( "a+b" ); break;
    } // end switch
*/
	// Ensure proper file pointer
	if ( !oexCHECK_PTR( x_pFile ) )
		return CBaseFile::c_InvalidFindHandle;

	// +++ Fix this stuff
	oexINT nMode = O_RDWR; // O_WRONLY O_RDONLY
	switch( x_eDisposition )
    {
    	case eDisCreateNew :
    		nMode |= O_CREAT | O_EXCL;
    		break;

        case eDisCreateAlways :
        	nMode |= O_CREAT;
        	break;

        case eDisOpenExisting :
        	break;

        case eDisOpenAlways :
        	nMode |= O_CREAT;
        	break;

	} // end switch

	// +++ Fix file permissions

	// Attempt to open the file
//	t_HFILE hFile =	fopen( oexStrToMbPtr( x_pFile ), oexStrToMbPtr( pMode ) );
	t_HFILE hFile =	(t_HFILE)open( oexStrToMbPtr( x_pFile ), nMode, 0777 );

	if ( x_pnError )
	{
		if ( CBaseFile::c_Invalid == hFile )
			*x_pnError = errno;
		else
			*x_pnError = 0;
	} // end if

	if ( CBaseFile::c_Invalid == hFile )
	{	oexERROR( errno, oexMks( oexT( "open( '" ), x_pFile, oexT( "', " ), nMode , oexT( " ) failed" ) ) );
		return hFile;
	} // end if

	// No data should be left in the file
	if ( ( eDisCreateAlways == x_eDisposition || eDisCreateNew == x_eDisposition )
		 && CBaseFile::c_Invalid != hFile )
		if ( -1 == ftruncate( oexPtrToInt( hFile ), 0 ) )
			oexWARNING( errno, oexMks( oexT( "ftruncate( " ), oexPtrToInt( hFile ), oexT( ", '" ), x_pFile, oexT( "' ) failed" ) ) );

	return hFile;
}

oexBOOL CBaseFile::Close( CBaseFile::t_HFILE x_hFile, oexINT *x_pnErr )
{
    if ( CBaseFile::c_Invalid == x_hFile )
        return oexFALSE;

    oexBOOL bRet = close( oexPtrToInt( x_hFile ) ) ? oexFALSE : oexTRUE;

    if ( x_pnErr )
    {
    	if ( bRet )
    		*x_pnErr = 0;
        else
        	*x_pnErr = errno;

    } // end if

    return bRet;
}

oexBOOL CBaseFile::Write( CBaseFile::t_HFILE x_hFile, oexCPVOID x_pData, oexINT64 x_llSize, oexINT64 *x_pllWritten, oexINT *x_pnErr )
{
	if ( c_Invalid == x_hFile )
		return oexFALSE;

//    oexUINT uWritten = write( x_pData, 1, x_uSize, (FILE*)hFile );
	oexINT64 llWritten = write( oexPtrToInt( x_hFile ), x_pData, x_llSize );

    if ( x_pllWritten )
        *x_pllWritten = llWritten;

    if ( x_pnErr )
    {
    	if ( x_llSize == llWritten )
    		*x_pnErr = 0;
        else
        	*x_pnErr = errno;

    } // end if

    return ( x_llSize == llWritten );
}

oexBOOL CBaseFile::Read( CBaseFile::t_HFILE x_hFile, oexPVOID x_pData, oexINT64 x_llSize, oexINT64 *x_pllRead, oexINT *x_pnErr )
{
	if ( c_Invalid == x_hFile )
		return oexFALSE;

//	oexUINT uRead = fread( x_pData, 1, x_uSize, (FILE*)hFile );
	oexINT64 llRead = read( oexPtrToInt( x_hFile ), x_pData, x_llSize );

	if ( 0 > llRead )
	{
	    if ( x_pnErr )
    		*x_pnErr = errno;

		if ( x_pllRead )
			*x_pllRead = 0;

	} // end if

    else if ( x_pllRead )
        *x_pllRead = (oexUINT)llRead;

    return ( 0 <= llRead );
}

oexBOOL CBaseFile::Flush( t_HFILE x_hFile )
{
	if ( c_Invalid == x_hFile )
		return oexFALSE;

	return !fsync( oexPtrToInt( x_hFile ) );
}


oexINT64 CBaseFile::Size( t_HFILE x_hFile )
{
	if ( c_Invalid == x_hFile )
		return 0;

#if defined( OEX_NOSTAT64 )

	struct stat s;
	if ( fstat( oexPtrToInt( x_hFile ), &s ) )
		return 0;

	return s.st_size;

#else

	struct stat64 s64;

	// +++ Ensure this works correctly
	if ( fstat64( oexPtrToInt( x_hFile ), &s64 ) )
		return 0;

	return s64.st_size;

#endif
}

oexINT64 CBaseFile::SetPointer( t_HFILE x_hFile, oexINT64 llMove, oexINT nMethod )
{
	if ( c_Invalid == x_hFile )
		return oexFALSE;

    // Get method
    oexINT nOrigin = 0;
    if ( nMethod == eFileOffsetBegin ) nOrigin = SEEK_SET;
    else if ( nMethod == eFileOffsetCurrent ) nOrigin = SEEK_CUR;
    else if ( nMethod == eFileOffsetEnd ) nOrigin = SEEK_END;
    else return -1;

    // Set new file position
	oexINT64 llPos = lseek( oexPtrToInt( x_hFile ), llMove, nOrigin );

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
#if defined( OEX_NODIRENT )
	return oexNULL;
#else

    // Sanity checks
    if ( !oexVERIFY_PTR( x_pPath ) || !oexVERIFY_PTR( x_pMask ) || !oexVERIFY_PTR( x_pFd ) )
        return oexNULL;

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
#endif
}

oexBOOL CBaseFile::FindNext( t_HFIND x_hFind, CBaseFile::SFindData *x_pFd )
{
#if defined( OEX_NODIRENT )
	return oexFALSE;
#else
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
#endif
}

oexBOOL CBaseFile::FindClose( t_HFIND x_hFind )
{
#if defined( OEX_NODIRENT )
	return oexFALSE;
#else
	return closedir( (DIR*)x_hFind ) ? oexFALSE : oexTRUE;
#endif
}


oexBOOL CBaseFile::DoesExist( oexCSTR x_pPath )
{
#if defined( OEX_NOSTAT64 )
	struct stat s;
	return stat( oexStrToMbPtr( x_pPath ), &s ) ? oexFALSE : oexTRUE;
#else
	struct stat64 s64;
	return	stat64( oexStrToMbPtr( x_pPath ), &s64 ) ? oexFALSE : oexTRUE;
#endif
}

oexBOOL CBaseFile::CreateFolder( oexCSTR x_pPath )
{   oexBOOL bRet = !mkdir( oexStrToMbPtr( x_pPath ), 0 ) ? oexFALSE : oexTRUE;
	chmod( oexStrToMbPtr( x_pPath ), 0755 );
	return bRet;
}

CStr CBaseFile::GetModPath( oexCSTR x_pPath )
{
	BrInitError error;
	if ( br_init( &error ) )
	{
		char *pPath = br_find_exe( oexNULL );

		if ( pPath )
		{
			if ( x_pPath && *x_pPath )
				return oexBuildPath( oexStr8ToStr( CStr8( pPath ) ).GetPath(), x_pPath );

			else
				return oexStr8ToStr( CStr8( pPath ) ).GetPath();

		} // end if

	} // end if

	// Don't really know if this is the best thing to do or not...
	if ( x_pPath && *x_pPath )
		return x_pPath;

	return CStr();
}

CStr CBaseFile::GetModFileName()
{
	BrInitError error;
	if ( br_init( &error ) )
	{
		char *pPath = br_find_exe( oexNULL );

		if ( pPath )
			return oexStr8ToStr( pPath );

	} // end if

	return CStr();
}


