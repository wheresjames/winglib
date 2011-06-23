/*------------------------------------------------------------------
// sys_util.cpp
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

// Disable microsoft security warnings
// Wouldn't have to do this but StringCchVPrintfW() has issues.
#if defined( _MSC_VER )
#	pragma warning( disable : 4995 4996 )
#elif defined( __INTEL_COMPILER )
#	pragma warning( disable : 1786 )
#endif

static HKEY RootKeyFromName( const CStr &sKey )
{_STT();
	if ( sKey.ICmp( oexT( "HKLM" ) ) )
		return HKEY_LOCAL_MACHINE;
	else if ( sKey.ICmp( oexT( "HKCU" ) ) )
		return HKEY_CURRENT_USER;
	else if ( sKey.ICmp( oexT( "HKCC" ) ) )
		return HKEY_CURRENT_CONFIG;
	else if ( sKey.ICmp( oexT( "HKCR" ) ) )
		return HKEY_CLASSES_ROOT;
	else if ( sKey.ICmp( oexT( "HKPD" ) ) )
		return HKEY_PERFORMANCE_DATA;
	else if ( sKey.ICmp( oexT( "HKU" ) ) )
		return HKEY_USERS;

	return oexNULL;
}

static CStr RegValueToString( DWORD dwType, LPVOID pBuf, DWORD dwSize )
{_STT();
	if ( !pBuf || !dwSize )
		return CStr();

	switch( dwType )
	{
		case REG_DWORD :
			if ( sizeof( oexUINT32 ) != dwSize ) return CStr();
			return CStr( *( (oexUINT32*)pBuf ) );

		case REG_QWORD :
			if ( sizeof( oexUINT64 ) != dwSize ) return CStr();
			return CStr( *( (oexUINT64*)pBuf ) );

		case REG_BINARY :
			return oexMbToStr( CStr8( (oexCSTR8)pBuf, dwSize ) );

		case REG_SZ :
		case REG_MULTI_SZ :
		case REG_EXPAND_SZ :

			// Size includes NULL string
			if ( dwSize )
				dwSize--;

			return CStr( (oexCSTR)pBuf, dwSize );
			
		default :
			oexWARNING( 0, oexMks( oexT( "Unknown Registry key Type : " ), dwType ) );
			break;

	} // end switch

	return CStr();
}

oexBOOL CSysUtil::IsRegKey( const CStr &x_sKey, const CStr &x_sPath )
{_STT();
	HKEY hRoot = RootKeyFromName( x_sKey );
	if ( !hRoot )
		return oexFALSE;

	HKEY hKey = NULL;
	LONG lRes = RegCreateKeyEx( hRoot, x_sPath.Ptr(),
								0, NULL, 0, KEY_READ, NULL, &hKey, NULL );
	if ( ERROR_SUCCESS != lRes || !hKey )
		return oexFALSE;
	RegCloseKey( hKey );

	return oexTRUE;
}

oexBOOL CSysUtil::IsRegValue( const CStr &x_sKey, const CStr &x_sPath, const CStr &x_sName )
{_STT();
	HKEY hRoot = RootKeyFromName( x_sKey );
	if ( !hRoot )
		return oexFALSE;

	HKEY hKey = NULL;
	LONG lRes = RegCreateKeyEx( hRoot, x_sPath.Ptr(),
								0, NULL, 0, KEY_READ, NULL, &hKey, NULL );
	if ( ERROR_SUCCESS != lRes || !hKey )
		return oexFALSE;

	DWORD dwType = 0, dwSize = 0;
	lRes = RegQueryValueEx( hKey, x_sName.Ptr(), 0, &dwType, 0, &dwSize );

	RegCloseKey( hKey );

	return ERROR_SUCCESS == lRes;
}

CStr CSysUtil::GetRegString( const CStr &x_sKey, const CStr &x_sPath, const CStr &x_sName )
{_STT();
	HKEY hRoot = RootKeyFromName( x_sKey );
	if ( !hRoot )
		return CStr();

	HKEY hKey = NULL;
	LONG lRes = RegCreateKeyEx( hRoot, x_sPath.Ptr(),
								0, NULL, 0, KEY_READ | KEY_QUERY_VALUE, NULL, &hKey, NULL );
	if ( ERROR_SUCCESS != lRes || !hKey )
	{	// iii This is expected to fail if key doesn't exist
//		oexNOTICE( lRes, oexMks( oexT( "RegCreateKeyEx() failed : " ), 
//								x_sKey.Ptr(), oexT( " : " ), 
//								x_sPath.Ptr() ) );
		return CStr();
	} // end if

	// RegDisableReflectionKey( hKey );

	DWORD dwType = 0, dwSize = 0;
	lRes = RegQueryValueEx( hKey, x_sName.Ptr(), 0, &dwType, 0, &dwSize );
	if ( ERROR_SUCCESS != lRes )
	{	// iii This is expected to fail if value doesn't exist
//		oexNOTICE( lRes, oexMks( oexT( "RegQueryValueEx() failed : " ), 
//								x_sKey.Ptr(), oexT( " : " ), 
//								x_sPath.Ptr(), oexT( " : " ),
//								x_sName.Ptr() ) );
		RegCloseKey( hKey );
		return CStr();
	} // end if

	// Allocate memory for value
	CBin buf;
	if ( !buf.Allocate( dwSize ) )
		return CStr();

	lRes = RegQueryValueEx( hKey, x_sName.Ptr(), 0, &dwType, (LPBYTE)buf._Ptr(), &dwSize );
	if ( ERROR_SUCCESS != lRes )
	{	oexERROR( lRes, oexMks( oexT( "RegQueryValueEx() failed : " ), 
								x_sKey.Ptr(), oexT( " : " ), 
								x_sPath.Ptr(), oexT( " : " ),
								x_sName.Ptr() ) );
		RegCloseKey( hKey );
		return CStr();
	} // end if

	// Return the value as a string
	return RegValueToString( dwType, buf._Ptr(), dwSize );
}

oexBOOL CSysUtil::SetRegString( const CStr &x_sKey, const CStr &x_sPath, const CStr &x_sName, const CStr &x_sValue )
{_STT();
	HKEY hRoot = RootKeyFromName( x_sKey );
	if ( !hRoot )
		return oexFALSE;

	HKEY hKey = NULL;
	LONG lRes = RegCreateKeyEx( hRoot, x_sPath.Ptr(),
								0, NULL, 0, KEY_WRITE | KEY_SET_VALUE, NULL, &hKey, NULL );
	if ( ERROR_SUCCESS != lRes || !hKey )
	{	oexERROR( lRes, oexMks( oexT( "RegCreateKeyEx() failed : " ), 
								x_sKey.Ptr(), oexT( " : " ), 
								x_sPath.Ptr() ) );
		return oexFALSE;
	} // end if

	// +++ Should we do mb/unicode here?

	// Set the value
	CStr8 val = oexStrToMb( x_sValue );
	lRes = RegSetValueEx( hKey, x_sName.Ptr(), 0, REG_SZ, (const BYTE *)val.Ptr(), val.Length() );
	if ( ERROR_SUCCESS != lRes )
		oexERROR( lRes, oexMks( oexT( "RegSetValueEx() failed : " ), 
								x_sKey.Ptr(), oexT( " : " ), 
								x_sPath.Ptr(), oexT( " : " ),
								x_sName.Ptr() ) );

	RegCloseKey( hKey );

	return oexFALSE;
}

oexBOOL CSysUtil::DeleteRegKey( const CStr &x_sKey, const CStr &x_sPath, oexBOOL x_bSubKeys )
{_STT();

	HKEY hRoot = RootKeyFromName( x_sKey );
	if ( !hRoot )
		return oexFALSE;

	// Delete sub keys if needed
	if ( x_bSubKeys ) 
		DeleteRegSubKeys( x_sKey, x_sPath );

	// Attempt to delete the key
	return ( RegDeleteKey( hRoot, x_sPath.Ptr() ) == ERROR_SUCCESS ) ? oexTRUE : oexFALSE;
}

oexBOOL _DeleteRegSubKeys( HKEY hRoot, const CStr &x_sPath )
{_STT();

	HKEY		hKey = NULL;
	char		szKey[ oexSTRSIZE ];
	DWORD		dwSize = oexSTRSIZE - 1;

	// Open The Key
	if( RegOpenKeyEx( hRoot, x_sPath.Ptr(), 0, KEY_ALL_ACCESS, &hKey ) != ERROR_SUCCESS )
		return oexFALSE;

	// For each sub key
	while ( RegEnumKeyEx(	hKey, 0, szKey, &dwSize, 
							NULL, NULL, NULL, NULL ) == ERROR_SUCCESS )
	{
		// A little recursion
		_DeleteRegSubKeys( hKey, szKey );

		// Attempt to delete the key
		RegDeleteKey( hKey, szKey );

		// Reset size
		dwSize = oexSTRSIZE - 1;

	} // end while

	// Close the key
	RegCloseKey( hKey );

	return oexTRUE;
}

oexBOOL CSysUtil::DeleteRegSubKeys( const CStr &x_sKey, const CStr &x_sPath )
{_STT();
	return _DeleteRegSubKeys( RootKeyFromName( x_sKey ), x_sPath );
}

oexLONG _GetRegKeys( CPropertyBag &pb, HKEY hRoot, const CStr &x_sPath, oexBOOL x_bValues, oexBOOL x_bSubKeys )
{_STT();

	if ( !hRoot )
		return 0;

	HKEY		hKey = NULL;
	char		szKey[ oexSTRSIZE ];
	DWORD		dwSize = oexSTRSIZE - 1;

	// Open The Key
	if( RegOpenKeyEx( hRoot, x_sPath.Ptr(), 0, KEY_ALL_ACCESS, &hKey ) != ERROR_SUCCESS )
		return 0;

	// For each sub key
	oexLONG lCount = 0;
	while ( RegEnumKeyEx(	hKey, 0, szKey, &dwSize, 
							NULL, NULL, NULL, NULL ) == ERROR_SUCCESS )
	{
		lCount++;

		if ( !x_bValues )
			pb[ szKey ] = oexT( "" );

		else
		{
			CBin buf;
			DWORD dwType = 0, dwSize = 0;
			if ( ERROR_SUCCESS == RegQueryValueEx( hKey, szKey, 0, &dwType, 0, &dwSize ) )
				if ( buf.Allocate( dwSize ) )
					if ( ERROR_SUCCESS == RegQueryValueEx( hKey, szKey, 0, &dwType, (LPBYTE)buf._Ptr(), &dwSize ) )
						pb[ szKey ] = RegValueToString( dwType, buf._Ptr(), dwSize );

		} // end if

		// Add sub keys
		if ( x_bSubKeys )
			lCount += _GetRegKeys( pb[ szKey ], hKey, szKey, x_bValues, x_bSubKeys );

		// Reset size
		dwSize = oexSTRSIZE - 1;

	} // end while

	// Close the key
	RegCloseKey( hKey );

	return lCount;
}

CPropertyBag CSysUtil::GetRegKeys( const CStr &x_sKey, const CStr &x_sPath, oexBOOL x_bValues, oexBOOL x_bSubKeys )
{_STT();
	CPropertyBag pb;
	_GetRegKeys( pb, RootKeyFromName( x_sKey ), x_sPath, x_bValues, x_bSubKeys );
	return pb;
}

CPropertyBag CSysUtil::GetDisksInfo( oexBOOL bInfo )
{_STT();

	CPropertyBag pb;
	oexTCHAR szDrive[ 16 ] = oexT( "A:" );
	DWORD dwDrives = GetLogicalDrives(), dw = 1;

	// Build a list of drives
	while ( dwDrives && dw && szDrive[ 0 ] <= oexT( 'Z' ) )
	{
		// Get info on this drive if it exists
		if ( dwDrives & dw )
		{
			if ( bInfo )
				pb[ szDrive ] = GetDiskInfo( szDrive );
			else
				pb[ szDrive ] = GetDriveTypeStr( szDrive );
		} // end if

		// Next drive position
		dwDrives &= ~dw; 
		dw <<= 1;
		szDrive[ 0 ]++;

	} // end while
	
	return pb;
}

CStr CSysUtil::GetDriveTypeStr(const CStr &x_sDrive)
{_STT();

	switch( GetDriveType( x_sDrive.Ptr() ) )
	{	case DRIVE_NO_ROOT_DIR : 	return oexT( "noroot" ); break;
		case DRIVE_REMOVABLE : 		return oexT( "removable" ); break;
		case DRIVE_FIXED : 			return oexT( "fixed" ); break;
		case DRIVE_REMOTE :			return oexT( "remote" ); break;
		case DRIVE_CDROM :			return oexT( "cdrom" ); break;
		case DRIVE_RAMDISK :		return oexT( "ramdisk" ); break;
		default : break;	
	} // end switch

	return oexT( "unknown" );
}

CPropertyBag CSysUtil::GetDiskInfo(const CStr &x_sDrive)
{_STT();

	// Sanity check
	if ( !x_sDrive.Length() ) 
		return CPropertyBag();

	CPropertyBag pb;
	pb[ "drive" ] = x_sDrive;
	pb[ oexT( "drive_type" ) ] = GetDriveTypeStr( x_sDrive.Ptr() );
	
	// Get volume information
	DWORD dwSn = 0, dwMax = 0, dwFlags = 0;
	char szVolume[ 1024 * 8 ] = { 0 }, szFileSystem[ 1024 * 8 ] = { 0 };
	if ( GetVolumeInformation(	x_sDrive.Ptr(), szVolume, sizeof( szVolume ),
								&dwSn, &dwMax, &dwFlags,
								szFileSystem, sizeof( szFileSystem ) ) )
	{	pb[ oexT( "volume" ) ] = oexMbToStrPtr( szVolume );
		pb[ oexT( "serial" ) ] = dwSn;
		pb[ oexT( "max_filename" ) ] = dwMax;
		pb[ oexT( "flags" ) ] = dwFlags;
		pb[ oexT( "file_system" ) ] = oexMbToStrPtr( szFileSystem );
	} // end if

	// More disk info
	DWORD dwSectorsPerCluster = 0, dwBytesPerSector = 0, dwFreeClusters = 0, dwClusters = 0;
	if ( GetDiskFreeSpace( x_sDrive.Ptr(), &dwSectorsPerCluster, &dwBytesPerSector, &dwFreeClusters, &dwClusters ) )
	{	pb[ oexT( "sectors_per_cluster" ) ] = dwSectorsPerCluster;
		pb[ oexT( "bytes_per_sector" ) ] = dwBytesPerSector;
		pb[ oexT( "clusters_free" ) ] = dwFreeClusters;
		pb[ oexT( "clusters" ) ] = dwClusters;
	} // end if
	
	// Get disk space
	ULARGE_INTEGER liFreeBytesAvailable, liTotalNumberOfBytes, liTotalNumberOfBytesFree;
	if ( GetDiskFreeSpaceEx( x_sDrive.Ptr(), &liFreeBytesAvailable, &liTotalNumberOfBytes, &liTotalNumberOfBytesFree ) )
	{	pb[ oexT( "bytes" ) ] = liTotalNumberOfBytes.QuadPart;
		pb[ oexT( "bytes_free" ) ] = liTotalNumberOfBytesFree.QuadPart;
		pb[ oexT( "bytes_used" ) ] = liTotalNumberOfBytes.QuadPart - liTotalNumberOfBytesFree.QuadPart;
		pb[ oexT( "bytes_available" ) ] = liFreeBytesAvailable.QuadPart;
		pb[ oexT( "bytes_unavailable" ) ] = liTotalNumberOfBytes.QuadPart - liFreeBytesAvailable.QuadPart;
	} // end if
	
	// Get the dos name
	TCHAR buf[ MAX_PATH ] = { 0 };
	DWORD dw = QueryDosDevice( x_sDrive.Ptr(), buf, sizeof( buf ) );
	if ( dw && dw < sizeof( buf ) )
		buf[ dw ] = 0, pb[ oexT( "dos_name" ) ] = buf;
	
	return pb;
}
