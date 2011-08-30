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

#include "oexlib.h"
#include "std_os.h"

#if !defined( OEX_NOPWD )
#	include "pwd.h"
#endif

// BinReloc library
#include "opc/breloc/binreloc.h"
#include "opc/breloc/basic.cpp"
//#include "opc/breloc/normal.cpp"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

oexSTATIC_ASSERT( sizeof( CBaseFile::t_HFILE ) >= sizeof( FILE* ) );
#if !defined( OEX_NODIRENT )
oexSTATIC_ASSERT( sizeof( CBaseFile::t_HFIND ) >= sizeof( DIR* ) );
#endif
oexSTATIC_ASSERT( sizeof( CBaseFile::t_HFILE ) >= sizeof( int ) );

const CBaseFile::t_HFILE CBaseFile::c_Invalid = (void*)-1;
const CBaseFile::t_HFIND CBaseFile::c_InvalidFindHandle = NULL;

static oexCSTR8 g_szModulePath = oexNULL;

oexBOOL CBaseFile::InitFileSystem()
{
	BrInitError error;
	g_szModulePath = _br_find_exe( &error );

	return oexTRUE;
}

oexBOOL CBaseFile::FreeFileSystem()
{
	if ( g_szModulePath )
	{	free( (void*)g_szModulePath );
		g_szModulePath = oexNULL;
	} // end if

	return oexTRUE;
}


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

	// +++ Fix sharing flags
	// O_SHARE_RDONLY O_SHARE_WRONLY O_SHARE_RDWR O_SHARE_NONE

	// Decide mode
	oexINT nMode = 0;
	oexUINT uWhat = ( os::CBaseFile::eAccessRead | os::CBaseFile::eAccessWrite );
	if ( ( uWhat & x_eAccess ) == uWhat )
		nMode = O_RDWR;
	else if ( 0 != ( os::CBaseFile::eAccessRead & x_eAccess ) )
		nMode = O_RDONLY;
	else if ( 0 != ( os::CBaseFile::eAccessWrite & x_eAccess ) )
		nMode = O_WRONLY;

	switch( x_eDisposition )
    {
    	case eDisCreateNew :
    		nMode |= O_CREAT | O_EXCL;
    		break;

        case eDisCreateAlways :
        	nMode |= O_CREAT; // | O_TRUNC;
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
	{	if ( 2 != errno ) // Don't log non-existant files
			oexERROR( errno, oexMks( oexT( "open( '" ), x_pFile, oexT( "', " ), nMode , oexT( " ) failed" ) ) );
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

oexBOOL CBaseFile::Write( CBaseFile::t_HFILE x_hFile, oexCPVOID x_pData, CBaseFile::t_size x_llSize, CBaseFile::t_size *x_pllWritten, oexINT *x_pnErr )
{
	if ( c_Invalid == x_hFile )
		return oexFALSE;

	CBaseFile::t_size llWritten = write( oexPtrToInt( x_hFile ), x_pData, x_llSize );

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

oexBOOL CBaseFile::Read( CBaseFile::t_HFILE x_hFile, oexPVOID x_pData, CBaseFile::t_size x_llSize, CBaseFile::t_size *x_pllRead, oexINT *x_pnErr )
{
	if ( c_Invalid == x_hFile )
		return oexFALSE;

	CBaseFile::t_size llRead = read( oexPtrToInt( x_hFile ), x_pData, x_llSize );

	if ( 0 > llRead )
	{
	    if ( x_pnErr )
    		*x_pnErr = errno;

		if ( x_pllRead )
			*x_pllRead = 0;

	} // end if

    else if ( x_pllRead )
        *x_pllRead = llRead;

    return ( 0 <= llRead );
}

oexBOOL CBaseFile::Flush( t_HFILE x_hFile )
{
	if ( c_Invalid == x_hFile )
		return oexFALSE;

	return !fsync( oexPtrToInt( x_hFile ) );
}

typedef struct { const char *pName; int dwFlag; } CBaseFile_ATTDESC;

CPropertyBag CBaseFile::GetFileInfo( oexCSTR x_pFile )
{_STT();

	CPropertyBag pb;
	if ( !x_pFile || !*x_pFile )
		return pb;

	// Save path
	pb[ "full" ] = x_pFile;

#if defined( OEX_NOSTAT64 )

	struct stat s;
	if ( stat( x_pFile, &s ) )
		return pb;

#else

	// +++ Ensure this works correctly
	struct stat64 s;
	if ( stat64( x_pFile, &s ) )
		return pb;

#endif

	// Save file info into structure
	pb[ "os_flags" ] = s.st_mode;

	static CBaseFile_ATTDESC ad[] = 
	{
		// Flags
		{ "socket",		S_IFSOCK },
		{ "link",		S_IFLNK },
		{ "normal",		S_IFREG },
		{ "blockdev",	S_IFBLK },
		{ "directory",	S_IFDIR },
		{ "chardev",	S_IFCHR },
		{ "fifo",		S_IFIFO },
		{ "is_uid",		S_ISUID },
		{ "is_gid",		S_ISGID },
		{ "is_vtx",		S_ISVTX },
		
		// Permissions
		{ "p_ur",		S_IRUSR },
		{ "p_uw",		S_IWUSR },
		{ "p_ux",		S_IXUSR },
		{ "p_gr",		S_IRGRP },
		{ "p_gw",		S_IWGRP },
		{ "p_gx",		S_IXGRP },
		{ "p_or",		S_IROTH },
		{ "p_ow",		S_IWOTH },
		{ "p_ox",		S_IXOTH },

		{ 0, 0 }
	};

	// Add flags
	for ( oexINT i = 0; ad[ i ].pName; i++ )
		if ( ( ad[ i ].dwFlag & s.st_mode ) == ad[ i ].dwFlag )
			pb[ ad[ i ].pName ] = oexT( "1" );

	// Linux info
	pb[ "device" ] = s.st_dev;
	pb[ "inode" ] = s.st_ino;
	pb[ "mode" ] = s.st_mode;
	pb[ "permissions" ] = ( S_IRWXU | S_IRWXG | S_IRWXO ) & s.st_mode;
	pb[ "nlink" ] = s.st_nlink;
	pb[ "uid" ] = s.st_uid;
	pb[ "gid" ] = s.st_gid;
	pb[ "rdev" ] = s.st_rdev;
	pb[ "blksize" ] = s.st_blksize;
	pb[ "blocks" ] = s.st_blocks;
	
	//pb[ "ft_created" ] = ;
	pb[ "ft_last_access" ] = s.st_atime;
	pb[ "ft_last_modified" ] = s.st_mtime;
	pb[ "ft_last_status_change" ] = s.st_ctime;
	pb[ "size" ] = s.st_size;

	return pb;
}

oexINT64 CBaseFile::GetFileSize( oexCSTR x_pFile )
{
	if ( !x_pFile || !*x_pFile )
		return 0;

#if defined( OEX_NOSTAT64 )

	struct stat s;
	if ( stat( x_pFile, &s ) )
		return 0;

	return s.st_size;

#else

	// +++ Ensure this works correctly
	struct stat64 s64;
	if ( stat64( x_pFile, &s64 ) )
		return 0;

	return s64.st_size;

#endif

}

CBaseFile::t_size CBaseFile::Size( t_HFILE x_hFile )
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

CBaseFile::t_size CBaseFile::SetPointer( t_HFILE x_hFile, CBaseFile::t_size llMove, oexINT nMethod )
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
	CBaseFile::t_size llPos = lseek( oexPtrToInt( x_hFile ), llMove, nOrigin );

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

void CBaseFile_SetFindData( CBaseFile::SFindData *x_pFd, const dirent *x_pD )
{
	if ( !oexCHECK_PTR( x_pFd ) || !oexCHECK_PTR( x_pD ) )
		return;

	// Init structure
	CBaseFile_InitFindData( x_pFd );

	// Save file name
	x_pFd->sName = oexMbToStr( x_pD->d_name );

	// Is it a directory?
	if ( DT_DIR & x_pD->d_type )
		x_pFd->uFileAttributes |= CBaseFile::eFileAttribDirectory;
}

CBaseFile::t_HFIND CBaseFile::FindFirst( oexCSTR x_pPath, oexCSTR x_pMask, CBaseFile::SFindData *x_pFd )
{
#if defined( OEX_NODIRENT )
	return CBaseFile::c_InvalidFindHandle;
#else

    // Sanity checks
    if ( !oexVERIFY_PTR( x_pPath ) || !oexVERIFY_PTR( x_pMask ) || !oexVERIFY_PTR( x_pFd ) )
        return CBaseFile::c_InvalidFindHandle;

	DIR *hDir = opendir( oexStrToMbPtr( x_pPath ) );
	if ( CBaseFile::c_InvalidFindHandle == hDir )
		return CBaseFile::c_InvalidFindHandle;

	errno = 0;
	struct dirent *pD = readdir( hDir );
	if ( !pD || errno )
	{	closedir( hDir );
		return CBaseFile::c_InvalidFindHandle;
	} // end if

	// Set file data
	CBaseFile_SetFindData( x_pFd, pD );

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
		return oexFALSE;

	CBaseFile_SetFindData( x_pFd, pD );

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

static oexBOOL _CreateFolder( oexCONST CStr &x_sPath )
{
	// Attempt to make the specified folder
	oexBOOL bRet = !mkdir( oexStrToMb( x_sPath ).Ptr(), 0755 ) ? oexTRUE : oexFALSE;
	if ( bRet )
	{	chmod( oexStrToMb( x_sPath ).Ptr(), 0755 );
		return oexTRUE;
	} // end if

	// Create sub path
	CStr sPath = x_sPath.GetPath();
	if ( sPath.Length() != x_sPath.Length() )
		return _CreateFolder( sPath );

	return oexFALSE;
}

oexBOOL CBaseFile::CreateFolder( oexCSTR x_pPath )
{
	if ( !x_pPath || !*x_pPath )
		return oexFALSE;

	return _CreateFolder( x_pPath );
}

CStr CBaseFile::GetModPath( oexCSTR x_pPath )
{
	// Ensure we have the module path
	if ( !g_szModulePath )
	{
		if ( oexCHECK_PTR( x_pPath ) )
			return CStr( x_pPath );
		else
			return CStr();

	} // end if

	if ( oexCHECK_PTR( x_pPath ) && *x_pPath )
		return oexBuildPath( oexStr8ToStr( CStr8( g_szModulePath ) ).GetPath(), x_pPath );

	return oexStr8ToStr( CStr8( g_szModulePath ) ).GetPath();

}

CStr CBaseFile::GetModFileName( oexCPVOID x_pInstance )
{
	if ( !g_szModulePath )
		return CStr();

	return oexStr8ToStr( g_szModulePath );
}

oexBOOL CBaseFile::Rename( oexCSTR x_pOld, oexCSTR x_pNew )
{
	if ( !x_pOld || !*x_pOld || !x_pNew || !*x_pNew )
		return oexFALSE;

	return rename( oexStrToMbPtr( x_pOld ), oexStrToMbPtr( x_pNew ) );
}

oexBOOL CBaseFile::Copy( oexCSTR x_pOld, oexCSTR x_pNew )
{
	if ( !x_pOld || !*x_pOld || !x_pNew || !*x_pNew )
		return oexFALSE;

	// Source file
	int fSrc = open( oexStrToMbPtr( x_pOld ), O_RDONLY, 0777 );
	if ( -1 == fSrc )
		return oexFALSE;

	int fDst = open( oexStrToMbPtr( x_pOld ), O_CREAT | O_WRONLY, 0777 );
	if ( -1 == fDst )
	{	close( fSrc );
		return oexFALSE;
	} // end if

	// Copy file data
	char buf[ 1024 ];	
	int nRead = 0, nTotal = 0;
	while ( 0 < ( nRead = read( fSrc, buf, sizeof( buf ) ) ) )
	{	int nErr =  write( fDst, buf, nRead );
		if ( 0 > nErr ) return oexFALSE;
		nTotal += nRead;
	} // end while

	// Close file handles
	close( fSrc );
	close( fDst );

	return nTotal;
}

static CStr CBaseFile_GetHome()
{
	// First attempt to user environment
	const char *pHome = getenv( "HOME" );
	if ( pHome && *pHome )
		return oexMbToStr( pHome );
		
#if !defined( OEX_NOPWD )
	// Attempt to read user home directory
	struct passwd *pw = getpwuid( getuid() );
	if ( pw && pw->pw_dir && *pw->pw_dir )
		return oexMbToStr( pw->pw_dir );
#endif

	return oexT( "~/" );
}

CStr CBaseFile::GetSysFolder( oexBOOL x_bShared, oexINT x_nFolderId, oexINT x_nMaxLength )
{
	// Get the folder
	switch( x_nFolderId )
	{
		case eFidNone :
			break;

		case eFidTemp :
		{
			const char *pTmp;
			if ( !( pTmp = getenv( "TMPDIR" ) ) 
				 && !( pTmp = getenv( "TEMP" ) )
				 && !( pTmp = getenv( "TMP" ) ) 
			   )
				return oexT( "/tmp" );

			return oexMbToStr( pTmp );

		} break;

		case eFidSystem :
			return CBaseFile_GetHome();

		case eFidUserOs :
			return oexT( "/sys" );

		case eFidCurrent :
		{
			// Ask for the current working directory
			CStr8 s;
			char *cwd = s.Allocate( oexSTRSIZE );
			if ( !cwd || !getcwd( cwd, oexSTRSIZE ) || !*cwd )
				s.Destroy();

			return s;

		} break;

		case eFidRoot :
		case eFidDefDrive :
			return oexT( "/" );

		case eFidFonts :
			return oexT( "/usr/share/fonts/truetype/msttcorefonts" );

		case eFidSettings :
			if ( x_bShared )
				return oexT( "/var/lib" );
			return oexBuildPath( CBaseFile_GetHome(), oexT( ".config" ) );

		case eFidDesktop :
			return oexBuildPath( CBaseFile_GetHome(), oexT( "Desktop" ) );

		case eFidDownloads :
			return oexBuildPath( CBaseFile_GetHome(), oexT( "Downloads" ) );

		case eFidTemplates :
			return oexBuildPath( CBaseFile_GetHome(), oexT( "Templates" ) );

		case eFidPublic :
			return oexBuildPath( CBaseFile_GetHome(), oexT( "Public" ) );

		case eFidDocuments :
			return oexBuildPath( CBaseFile_GetHome(), oexT( "Documents" ) );

		case eFidMusic :
			return oexBuildPath( CBaseFile_GetHome(), oexT( "Music" ) );

		case eFidPictures :
			return oexBuildPath( CBaseFile_GetHome(), oexT( "Pictures" ) );

		case eFidVideo :
			return oexBuildPath( CBaseFile_GetHome(), oexT( "Videos" ) );

		default :
			break;

	} // end switch

	return oexT( "" );
}
