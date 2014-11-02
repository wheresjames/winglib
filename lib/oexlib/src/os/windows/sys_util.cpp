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

#if !defined( OEX_NO_CPUID ) && !defined( __MINGW32__ )
#	include <intrin.h>
#endif

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
	LONG lRes = RegOpenKeyEx( hRoot, x_sPath.Ptr(), 0, KEY_READ | KEY_QUERY_VALUE, &hKey );
	if ( ERROR_SUCCESS != lRes || !hKey )
	{	// iii This is expected to fail if key doesn't exist
		if ( ERROR_FILE_NOT_FOUND != lRes && ERROR_PATH_NOT_FOUND != lRes )
			oexNOTICE( lRes, oexMks( oexT( "RegCreateKeyEx() failed : " ),
									x_sKey.Ptr(), oexT( " : " ),
									x_sPath.Ptr() ) );
		return CStr();
	} // end if

	// RegDisableReflectionKey( hKey );

	DWORD dwType = 0, dwSize = 0;
	lRes = RegQueryValueEx( hKey, x_sName.Ptr(), 0, &dwType, 0, &dwSize );
	if ( ERROR_SUCCESS != lRes )
	{	// iii This is expected to fail if value doesn't exist
		if ( ERROR_FILE_NOT_FOUND != lRes && ERROR_PATH_NOT_FOUND != lRes )
			oexNOTICE( lRes, oexMks( oexT( "RegQueryValueEx() failed : " ),
									x_sKey.Ptr(), oexT( " : " ),
									x_sPath.Ptr(), oexT( " : " ),
									x_sName.Ptr() ) );
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

	// Close the registry key
	RegCloseKey( hKey );

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
	TCHAR		szKey[ oexSTRSIZE ];
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
		if ( RegDeleteKey( hKey, szKey ) != ERROR_SUCCESS )
			return oexFALSE;

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

	// Open The Key
	HKEY hKey = NULL;
	if( RegOpenKeyEx( hRoot, x_sPath.Ptr(), 0, KEY_ALL_ACCESS, &hKey ) != ERROR_SUCCESS )
		return 0;

	// Total item count
	oexLONG lCount = 0;

	// Add values if needed
	if ( x_bValues )
	{
		TCHAR szKey[ oexSTRSIZE ] = { 0 };
		DWORD dwLen = oexSTRSIZE - 1, dwIndex = 0, dwType = 0, dwSize = 0;
		while ( RegEnumValue( hKey, dwIndex++, szKey, &dwLen, 0, &dwType, 0, &dwSize ) == ERROR_SUCCESS )
		{
			lCount++;

			// Get value data
			CBin buf;
			if ( buf.Allocate( dwSize ) )
				if ( ERROR_SUCCESS == RegQueryValueEx( hKey, szKey, 0, &dwType, (LPBYTE)buf._Ptr(), &dwSize ) )
					pb[ szKey ] = RegValueToString( dwType, buf._Ptr(), dwSize );

			// Reset key buffer length
			dwLen = oexSTRSIZE - 1;

		} // end while
	} // end if

	// Add subkeys if needed
	if ( x_bSubKeys )
	{
		TCHAR szKey[ oexSTRSIZE ] = { 0 };
		DWORD dwLen = oexSTRSIZE - 1, dwIndex = 0;
		while ( RegEnumKeyEx(	hKey, dwIndex++, szKey, &dwLen,
								NULL, NULL, NULL, NULL ) == ERROR_SUCCESS )
		{
			lCount++;

			// Add sub keys
			lCount += _GetRegKeys( pb[ szKey ], hRoot, oexBuildPath( x_sPath, szKey, oexT( '\\' ) ), x_bValues, x_bSubKeys );

			// Reset key buffer length
			dwLen = oexSTRSIZE - 1;

		} // end while

	} // end if

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

#if !defined( FSCTL_IS_VOLUME_MOUNTED )
#define FSCTL_IS_VOLUME_MOUNTED 0x00090028
#endif

oexBOOL CSysUtil::IsMounted( const CStr &x_sDrive )
{
	CStr sDrive( oexT( "\\\\.\\" ) );
	sDrive += x_sDrive;

	// Attempt to open the drive
	HANDLE hDrive = CreateFile( sDrive.Ptr(), GENERIC_READ,
								FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
								0, OPEN_EXISTING, 0, 0 );

	if ( INVALID_HANDLE_VALUE == hDrive )
		return ERROR_FILE_NOT_FOUND != GetLastError();

	DWORD dwBytes;
	BOOL bRet = DeviceIoControl( hDrive, FSCTL_IS_VOLUME_MOUNTED, 0, 0, 0, 0, &dwBytes, 0 );

	CloseHandle( hDrive );

	return bRet;
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
	pb[ oexT( "drive" ) ] = x_sDrive;
	pb[ oexT( "drive_type" ) ] = GetDriveTypeStr( x_sDrive.Ptr() );

	// See if a disk is loaded
	oexBOOL bMounted = IsMounted( x_sDrive );
	pb[ oexT( "drive_mounted" ) ] = bMounted ? 1 : 0;

	// Only do this if the disk is mounted
	if ( bMounted )
	{
		// Get volume information
		DWORD dwSn = 0, dwMax = 0, dwFlags = 0;
		TCHAR szVolume[ 1024 * 8 ] = { 0 }, szFileSystem[ 1024 * 8 ] = { 0 };
		if ( GetVolumeInformation(	x_sDrive.Ptr(), szVolume, sizeof( szVolume ),
									&dwSn, &dwMax, &dwFlags,
									szFileSystem, sizeof( szFileSystem ) ) )
		{	pb[ oexT( "volume" ) ] = szVolume;
			pb[ oexT( "serial" ) ] = dwSn;
			pb[ oexT( "max_filename" ) ] = dwMax;
			pb[ oexT( "flags" ) ] = dwFlags;
			pb[ oexT( "file_system" ) ] = szFileSystem;
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
		{
			// Drive usage
			pb[ oexT( "bytes" ) ] = liTotalNumberOfBytes.QuadPart;
			pb[ oexT( "bytes_free" ) ] = liTotalNumberOfBytesFree.QuadPart;
			pb[ oexT( "bytes_used" ) ] = liTotalNumberOfBytes.QuadPart - liTotalNumberOfBytesFree.QuadPart;
			pb[ oexT( "bytes_available" ) ] = liFreeBytesAvailable.QuadPart;
			pb[ oexT( "bytes_unavailable" ) ] = liTotalNumberOfBytes.QuadPart - liFreeBytesAvailable.QuadPart;

			// If there is storage, calculate percentages
			if( liTotalNumberOfBytes.QuadPart )
			{
				// Available percentages
				pb[ oexT( "percent_available" ) ]
					= CStr().Fmt( oexT( "%.2f" ), (double)liFreeBytesAvailable.QuadPart / (double)liTotalNumberOfBytes.QuadPart * (double)100 );
				if ( liTotalNumberOfBytes.QuadPart > liFreeBytesAvailable.QuadPart )
					pb[ oexT( "percent_unavailable" ) ]
						= CStr().Fmt( oexT( "%.2f" ), (double)( liTotalNumberOfBytes.QuadPart - liFreeBytesAvailable.QuadPart )
												/ (double)liTotalNumberOfBytes.QuadPart * (double)100 );

				// Used percentages
				pb[ oexT( "percent_free" ) ]
					= CStr().Fmt( oexT( "%.2f" ), (double)liTotalNumberOfBytesFree.QuadPart / (double)liTotalNumberOfBytes.QuadPart * (double)100 );
				if ( liTotalNumberOfBytes.QuadPart > liTotalNumberOfBytesFree.QuadPart )
					pb[ oexT( "percent_used" ) ]
						= CStr().Fmt( oexT( "%.2f" ), (double)( liTotalNumberOfBytes.QuadPart - liTotalNumberOfBytesFree.QuadPart )
												/ (double)liTotalNumberOfBytes.QuadPart * (double)100 );
			} // en dif

		} // end if

	} // end if

	// Get the dos name
	TCHAR buf[ MAX_PATH ] = { 0 };
	DWORD dw = QueryDosDevice( x_sDrive.Ptr(), buf, sizeof( buf ) );
	if ( dw && dw < sizeof( buf ) )
		buf[ dw ] = 0, pb[ oexT( "dos_name" ) ] = buf;

	return pb;
}

BOOL CALLBACK EnumDesktopProc( LPTSTR lpszDesktop,  LPARAM lParam )
{
//	oexSHOW( lpszDesktop );
//	oexEcho( lpszDesktop );

	return TRUE;
}


class CSysUtil_CSwitchDesktop
{
public:
	CSysUtil_CSwitchDesktop( bool bSwitchCurrent )
	{	m_hOld = NULL;
		m_hDesk = NULL;

		EnumDesktops( NULL, &EnumDesktopProc, 0 );

		if ( bSwitchCurrent )
			SwitchCurrent();
	}
	~CSysUtil_CSwitchDesktop()
	{	RestoreDesktop();
	}
	void RestoreDesktop()
	{	if ( m_hOld )
			::SetThreadDesktop( m_hOld ), m_hOld = NULL;
		if ( m_hDesk )
			::CloseDesktop( m_hDesk ), m_hDesk = NULL;
	}
	bool SwitchCurrent()
	{	m_hOld = ::GetThreadDesktop( GetCurrentThreadId() );
//		m_hDesk = ::OpenInputDesktop( 0, FALSE, READ_CONTROL );
		m_hDesk = ::OpenDesktop( oexT( "WinSta0" ), 0, FALSE, READ_CONTROL );
		if ( !m_hDesk || !::SetThreadDesktop( m_hDesk ) )
			RestoreDesktop();
		else
			oexEcho( "hi" );
		return m_hDesk ? true : false;
	}

private:
	HDESK m_hDesk;
	HDESK m_hOld;
};

struct CSysUtil_SScreenInfo
{	oexINT 	nScreen;
	LPRECT	pRect;
};

static BOOL CALLBACK CSysUtil_ScreenInfo( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData )
{_STT();

	// Verify pointer
	CSysUtil_SScreenInfo *pdi = (CSysUtil_SScreenInfo*)dwData;
	if ( !pdi )
		return FALSE;

	// Just in case ;)
	if ( 0 > pdi->nScreen )
		return FALSE;

	// Wait for our screen
	if ( pdi->nScreen-- )
		return TRUE;

	// Ensure valid rect
	if ( !lprcMonitor
		 || ( 0 >= ( lprcMonitor->right - lprcMonitor->left ) )
		 || ( 0 >= ( lprcMonitor->bottom - lprcMonitor->top ) ) )
		pdi->nScreen = -2;

	// Copy the rect
	else
		::CopyRect( pdi->pRect, lprcMonitor );

	return FALSE;
}

struct _SSysUtilCapInfo
{
	BITMAPINFO	bmi;
	HDC			hDC;
	VOID 		*pImg;
	HBITMAP 	hBmp;
	HBITMAP 	hOldBmp;
	oexLONG		lFmt;
	oexLONG 	lScreenWidth;
	oexLONG		lScreenHeight;
	DWORD		dwCpyFlags;
};

oexINT CSysUtil::ReleaseScreenCapture( CBin *x_pInf )
{_STT();

	// Ensure valid structure
	if ( !x_pInf || sizeof( _SSysUtilCapInfo ) != x_pInf->getUsed() )
		return 0;

	_SSysUtilCapInfo *p = (_SSysUtilCapInfo*)x_pInf->Ptr();
	if ( !p )
		return 0;

	if ( p->hDC && p->hOldBmp )
		::SelectObject( p->hDC, p->hOldBmp );

	if ( p->hDC )
		::DeleteDC( p->hDC );

	if ( p->hBmp )
		::DeleteObject( p->hBmp );

	x_pInf->Destroy();

	return 1;
}

oexINT CSysUtil::InitScreenCapture( CBin *x_pInf, oexLONG x_nScreen, oexLONG fmt, oexLONG w, oexLONG h )
{_STT();

	// Attempt to initialize the image structure
	if ( !x_pInf || !x_pInf->Allocate( sizeof( _SSysUtilCapInfo ) ) )
		return 0;

	// Initialize and get a pointer to the new structure
	x_pInf->Zero(); x_pInf->setUsed( sizeof( _SSysUtilCapInfo ) );
	_SSysUtilCapInfo *p = (_SSysUtilCapInfo*)x_pInf->Ptr();
	if ( !p )
		return 0;

	// Switch to current desktop
//	CSysUtil_CSwitchDesktop sd( true );

	// Copy flags
	p->dwCpyFlags = SRCCOPY;

	// +++ CAPTUREBLT Makes the mouse flicker?
//	p->dwCpyFlags = CAPTUREBLT | SRCCOPY;

	// Save the capture format
	p->lFmt = fmt;
	p->lScreenWidth = 0;
	p->lScreenHeight = 0;

	// Get the desktop window
	HWND hDt = GetDesktopWindow();
	if ( !hDt )
		hDt = GetShellWindow();

	// Was a screen specified?
	if ( 0 <= x_nScreen )
	{
		// Find the monitor region
		RECT rc = { 0, 0, 0, 0 };
		CSysUtil_SScreenInfo si = { x_nScreen, &rc };
		EnumDisplayMonitors( NULL, NULL, &CSysUtil_ScreenInfo, (LPARAM)&si );
		if ( -1 == si.nScreen )
			p->lScreenWidth = rc.right - rc.left,
			p->lScreenHeight = rc.bottom - rc.top;
	} // end if

	// Valid width / height?
	if ( 0 >= p->lScreenWidth || 0 >= p->lScreenHeight )
	{
		// Do we have a valid window handle?
		if ( hDt && ::IsWindow( hDt ) )
		{	RECT rc;
			::GetClientRect( hDt, &rc );
			p->lScreenWidth = rc.right - rc.left;
			p->lScreenHeight = rc.bottom - rc.top;
		} // end if

		else
		{	p->lScreenWidth = GetSystemMetrics( SM_CXFULLSCREEN );
			p->lScreenHeight = GetSystemMetrics( SM_CYFULLSCREEN );
		} // end else

	} // end if

	// What is the capture size?
	if ( 0 >= w )
		w = p->lScreenWidth;
	if ( 0 >= h )
		h = p->lScreenHeight;

	// Grab default dc
	HDC hDC = ::GetDC( hDt );
	if ( !hDC )
		return 0;

	p->hDC = ::CreateCompatibleDC( NULL );
	if ( !p->hDC )
	{	::ReleaseDC( hDt, hDC );
		return 0;
	} // end if

	oexLONG bpp = 24;
	p->bmi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	p->bmi.bmiHeader.biWidth = w;
	p->bmi.bmiHeader.biHeight = h;
	p->bmi.bmiHeader.biPlanes = 1;
	p->bmi.bmiHeader.biBitCount = bpp;
	p->bmi.bmiHeader.biSizeImage = cmn::Align4( cmn::FitTo< oexLONG >( bpp, 8 ) * w ) * h;
	p->bmi.bmiHeader.biCompression = BI_RGB;

	// Create a bitmap compatible with the default dc
	p->hBmp = CreateDIBSection( hDC, &p->bmi, DIB_RGB_COLORS, &p->pImg, 0, 0 );
	if ( !p->hBmp || !p->pImg )
	{	ReleaseScreenCapture( x_pInf );
		::ReleaseDC( hDt, hDC );
		return 0;
	} // end if

	// Select the new bitmap
	p->hOldBmp = (HBITMAP)::SelectObject( p->hDC, p->hBmp );

	// Pick a decent stretch mode
	SetStretchBltMode( p->hDC, HALFTONE );

	// Release default dc
	::ReleaseDC( hDt, hDC );

	return 1;
}

oexINT CSysUtil::GetScreenCaptureInfo( CBin *x_pInf, CPropertyBag *pb )
{_STT();

	// Ensure valid structure
	if ( !x_pInf || sizeof( _SSysUtilCapInfo ) != x_pInf->getUsed() || !pb )
		return 0;

	_SSysUtilCapInfo *p = (_SSysUtilCapInfo*)x_pInf->Ptr();
	if ( !p || !p->hDC )
		return 0;

	// Save important image information
	(*pb)[ oexT( "w" ) ] = p->bmi.bmiHeader.biWidth;
	(*pb)[ oexT( "h" ) ] = p->bmi.bmiHeader.biHeight;
	(*pb)[ oexT( "bpp" ) ] = p->bmi.bmiHeader.biBitCount;
	(*pb)[ oexT( "sz" ) ] = p->bmi.bmiHeader.biSizeImage;
	(*pb)[ oexT( "cmp" ) ] = p->bmi.bmiHeader.biCompression;

	(*pb)[ oexT( "fmt" ) ] = p->lFmt;
	(*pb)[ oexT( "sw" ) ] = p->lScreenWidth;
	(*pb)[ oexT( "sh" ) ] = p->lScreenHeight;

	return pb->Size();
}

static oexINT CSysUtil_CopyDC( HDC hDst, LPRECT pDst, HDC hSrc, LPRECT pSrc, DWORD dwFlags )
{_STT();
	if ( !pSrc || !pDst )
		return 0;

	// Work out the sizes
	oexLONG dw = pDst->right - pDst->left;
	oexLONG dh = pDst->bottom - pDst->top;
	oexLONG sw = pSrc->right - pSrc->left;
	oexLONG sh = pSrc->bottom - pSrc->top;

	// Same size?
	if ( dw == sw && dh == sh )
		return ::BitBlt( hDst, pDst->left, pDst->top, dw, dh,
						 hSrc, pSrc->left, pSrc->top, dwFlags );

	// Stretch
	return ::StretchBlt( hDst, pDst->left, pDst->top, dw, dh,
						 hSrc, pSrc->left, pSrc->top, sw, sh, dwFlags );
}

oexINT CSysUtil::GetNumScreens()
{_STT();
	return GetSystemMetrics( SM_CMONITORS );
}

static BOOL CALLBACK CSysUtil_GetScreenInfo( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData )
{_STT();

	// Verify pointer
	CPropertyBag *pb = (CPropertyBag*)dwData;
	if ( !pb )
		return FALSE;

	// Add an entry for this screen
	oexINT nIdx = pb->Size();
	CPropertyBag &r = (*pb)[ nIdx ];

	if ( lprcMonitor )
	{	r[ oexT( "draw" ) ][ oexT( "x" ) ] = lprcMonitor->left;
		r[ oexT( "draw" ) ][ oexT( "y" ) ] = lprcMonitor->top;
		r[ oexT( "draw" ) ][ oexT( "w" ) ] = lprcMonitor->right - lprcMonitor->left;
		r[ oexT( "draw" ) ][ oexT( "h" ) ] = lprcMonitor->bottom - lprcMonitor->top;
	} // end if

	// Save information
	MONITORINFOEX mi; oexZero( mi );
	mi.cbSize = sizeof( mi );
	if ( GetMonitorInfo( hMonitor, &mi ) )
	{
		// Total size
		r[ oexT( "monitor" ) ][ oexT( "x" ) ] = mi.rcMonitor.left;
		r[ oexT( "monitor" ) ][ oexT( "y" ) ] = mi.rcMonitor.top;
		r[ oexT( "monitor" ) ][ oexT( "w" ) ] = mi.rcMonitor.right - mi.rcMonitor.left;
		r[ oexT( "monitor" ) ][ oexT( "h" ) ] = mi.rcMonitor.bottom - mi.rcMonitor.top;

		// Working size
		r[ oexT( "work" ) ][ oexT( "x" ) ] = mi.rcWork.left;
		r[ oexT( "work" ) ][ oexT( "y" ) ] = mi.rcWork.top;
		r[ oexT( "work" ) ][ oexT( "w" ) ] = mi.rcWork.right - mi.rcWork.left;
		r[ oexT( "work" ) ][ oexT( "h" ) ] = mi.rcWork.bottom - mi.rcWork.top;

		r[ oexT( "flags" ) ] = mi.dwFlags;
		r[ oexT( "name" ) ] = mi.szDevice;

	} // end if

	return TRUE;
}

oexINT CSysUtil::GetScreenInfo( CPropertyBag *pb )
{_STT();

	if ( !pb )
		return 0;

	// Out with the old
	pb->Destroy();

	// Switch to current desktop
//	CSysUtil_CSwitchDesktop sd( true );

	// Enum displays
	EnumDisplayMonitors( NULL, NULL, &CSysUtil_GetScreenInfo, (LPARAM)pb );

	// How many did we get?
	return pb->Size();
}

oexINT CSysUtil::LockScreen( CBin *x_pInf, CBin *x_pImg, oexINT x_nScreen )
{_STT();

	// Ensure valid structure
	if ( !x_pInf || sizeof( _SSysUtilCapInfo ) != x_pInf->getUsed() )
		return 0;

	_SSysUtilCapInfo *p = (_SSysUtilCapInfo*)x_pInf->Ptr();
	if ( !p )
		return 0;

	// Switch to current desktop
//	CSysUtil_CSwitchDesktop sd( true );

	// Get the desktop window
	HWND hDt = GetDesktopWindow();
	if ( !hDt )
		hDt = GetShellWindow();
	if ( hDt && ::IsWindow( hDt ) )
	{	RECT rc;
		::GetClientRect( hDt, &rc );
		p->lScreenWidth = rc.right - rc.left;
		p->lScreenHeight = rc.bottom - rc.top;
	} // end if
	else
	{	p->lScreenWidth = GetSystemMetrics( SM_CXFULLSCREEN );
		p->lScreenHeight = GetSystemMetrics( SM_CYFULLSCREEN );
	} // end else

	// Grab default dc
	BOOL bDelete = FALSE;
	HDC hDC = ::GetDC( hDt );

	// Fall back
	if ( !hDC )
		hDt = NULL, bDelete = TRUE,
		hDC = ::CreateDC( oexT( "DISPLAY" ), NULL, NULL, NULL );

	if ( !hDC )
		return 0;

	BOOL bRet = oexFALSE;
	RECT rcDst, rcSrc;
	SetRect( &rcDst, 0, 0, p->bmi.bmiHeader.biWidth, p->bmi.bmiHeader.biHeight );

	// Whole desktop?
	if ( 0 > x_nScreen )
		SetRect( &rcSrc, 0, 0, p->lScreenWidth, p->lScreenHeight ),
		bRet = CSysUtil_CopyDC( p->hDC, &rcDst, hDC, &rcSrc, p->dwCpyFlags );

	else
	{
		// Find the monitor region
		CSysUtil_SScreenInfo si = { x_nScreen, &rcSrc };
		EnumDisplayMonitors( NULL, NULL, &CSysUtil_ScreenInfo, (LPARAM)&si );
		if ( -1 == si.nScreen )
			bRet = CSysUtil_CopyDC( p->hDC, &rcDst, hDC, &rcSrc, p->dwCpyFlags );

	} // end else

	// Release default dc
	if ( bDelete )
		::DeleteDC( hDC );
	else
		::ReleaseDC( hDt, hDC );

	// Set the image buffer pointer
	if ( bRet )
		x_pImg->setBuffer( p->pImg, p->bmi.bmiHeader.biSizeImage, 0, oexFALSE );
	else
		x_pImg->Destroy();

	return x_pImg->getUsed();
}

oexINT CSysUtil::UnlockScreen( CBin *x_pInf, CBin *x_pImg )
{_STT();

	return 1;
}

oexINT CSysUtil::GetMouseInfo( CPropertyBag *pb )
{_STT();

	if ( !pb )
		return 0;

	// Switch to current desktop
//	CSysUtil_CSwitchDesktop sd( true );

	POINT pos;
	if ( ::GetCursorPos( &pos ) )
		(*pb)[ oexT( "x" ) ] = pos.x, (*pb)[ oexT( "y" ) ] = pos.y;

	return pb->Size();
}

oexLONG CSysUtil::GetMousePos()
{_STT();

	// Switch to current desktop
//	CSysUtil_CSwitchDesktop sd( true );

	POINT pos;
	if ( !::GetCursorPos( &pos ) )
		return 0;

	return ( short( pos.y ) << 16 ) | short( pos.x );
}

oexINT CSysUtil::SetMousePos( oexLONG x, oexLONG y )
{_STT();
	// Switch to current desktop
//	CSysUtil_CSwitchDesktop sd( true );

	return ::SetCursorPos( x, y );
}

oexINT CSysUtil::QueueInput( CPropertyBag *pb )
{_STT();

	if ( !pb )
		return 0;

	// Switch to current desktop
//	CSysUtil_CSwitchDesktop sd( true );

	INPUT in;

	// For each input item
	for ( CPropertyBag::iterator it; pb->List().Next( it ); )
		if ( pb->IsKey( oexT( "type" ) ) )
		{
			oexZero( in );

			// Mouse input?
			if ( (*pb)[ oexT( "type" ) ].ToString() == oexT( "mouse" ) )
			{	in.type = INPUT_MOUSE;
				in.mi.dx = (*pb)[ oexT( "x" ) ].ToLong();
				in.mi.dy = (*pb)[ oexT( "y" ) ].ToLong();
				in.mi.mouseData = (*pb)[ oexT( "data" ) ].ToLong();
				in.mi.dwFlags = (*pb)[ oexT( "flags" ) ].ToLong();
				in.mi.time = (*pb)[ oexT( "time" ) ].ToLong();
				// in.mi.dwExtraInfo = (*pb)[ oexT( "" ) ].ToLong();
			} // end if

			// Keyboard input?
			else if ( (*pb)[ oexT( "type" ) ].ToString() == oexT( "keyboard" ) )
			{	in.type = INPUT_KEYBOARD;
				in.ki.wVk = (*pb)[ oexT( "vk" ) ].ToLong();
				in.ki.wScan = (*pb)[ oexT( "scan" ) ].ToLong();
				in.ki.dwFlags = (*pb)[ oexT( "flags" ) ].ToLong();
				in.ki.time = (*pb)[ oexT( "time" ) ].ToLong();
				// in.ki.dwExtraInfo = (*pb)[ oexT( "" ) ].ToLong();
			} // end else

			// Hardware input?
			else if ( (*pb)[ oexT( "type" ) ].ToString() == oexT( "hardware" ) )
			{	in.type = INPUT_HARDWARE;
				in.hi.uMsg = (*pb)[ oexT( "msg" ) ].ToLong();
				in.hi.wParamL = (*pb)[ oexT( "low" ) ].ToLong();
				in.hi.wParamH = (*pb)[ oexT( "hi" ) ].ToLong();
			} // end else

			// ???
			else
				continue;

			// Send the input
			SendInput( 1, &in, sizeof( in ) );

		} // end if

	return 1;
}

oexINT CSysUtil::GetMemoryStatus( CPropertyBag *pb )
{
	if ( !pb )
		return 0;

	// Get memory info
	MEMORYSTATUSEX msex; oexZero( msex );
	msex.dwLength = sizeof( msex );
	if ( !GlobalMemoryStatusEx( &msex ) )
		return 0;

	// Copy into property bag
	(*pb)[ oexT( "memory_load" ) ] = msex.dwMemoryLoad;
	(*pb)[ oexT( "total_physical" ) ] = msex.ullTotalPhys;
	(*pb)[ oexT( "avail_physical" ) ] = msex.ullAvailPhys;
	(*pb)[ oexT( "total_page_file" ) ] = msex.ullTotalPageFile;
	(*pb)[ oexT( "avail_page_file" ) ] = msex.ullAvailPageFile;
	(*pb)[ oexT( "total_virtual" ) ] = msex.ullTotalVirtual;
	(*pb)[ oexT( "avail_virtual" ) ] = msex.ullAvailVirtual;
	(*pb)[ oexT( "avail_extended_virtual" ) ] = msex.ullAvailExtendedVirtual;

	return 1;
}

oexINT CSysUtil::i_cpuid( int *reg, oexINT i )
{
#if defined( OEX_NO_CPUID )
	return 0;
#else
#if defined( __MINGW32__ )
	__asm__ __volatile__
		(
			"cpuid": "=a" (reg[0]), "=b" (reg[1]), "=c" (reg[2]), "=d" (reg[3]) : "a" (i)
		);
	return 1;
#else
	__cpuid( reg, i );
	return 1;
#endif
#endif
}

typedef struct tag_t_PROCESSENTRY32
{	DWORD		dwSize;
	DWORD		cntUsage;
	DWORD		th32ProcessID;
	ULONG_PTR	th32DefaultHeapID;
	DWORD		th32ModuleID;
	DWORD		cntThreads;
	DWORD		th32ParentProcessID;
	LONG		pcPriClassBase;
	DWORD		dwFlags;
	TCHAR		szExeFile[ MAX_PATH ];
} t_PROCESSENTRY32, *t_PPROCESSENTRY32;

typedef struct t_tagPROCESS_MEMORY_COUNTERS 
{	DWORD		cb;
	DWORD		PageFaultCount;
	SIZE_T		PeakWorkingSetSize;
	SIZE_T		WorkingSetSize;
	SIZE_T		QuotaPeakPagedPoolUsage;
	SIZE_T		QuotaPagedPoolUsage;
	SIZE_T		QuotaPeakNonPagedPoolUsage;
	SIZE_T		QuotaNonPagedPoolUsage;
	SIZE_T		PagefileUsage;
	SIZE_T		PeakPagefileUsage;
} t_PROCESS_MEMORY_COUNTERS, *t_PPROCESS_MEMORY_COUNTERS;

typedef struct t_tagLANGANDCODEPAGE 
{	WORD 		wLanguage;
	WORD 		wCodePage;
} t_tagLANGANDCODEPAGE;

typedef void* t_PPS_POST_PROCESS_INIT_ROUTINE;

typedef struct t_tagUNICODE_STRING 
{	USHORT 		Length;
	USHORT 		MaximumLength;
	PWSTR  		Buffer;
} t_UNICODE_STRING;

typedef struct t_tagPEB_LDR_DATA 
{	BYTE       		Reserved1[ 8 ];
	PVOID      		Reserved2[ 3 ];
	LIST_ENTRY 		InMemoryOrderModuleList;
} t_PEB_LDR_DATA;

typedef struct t_tagRTL_USER_PROCESS_PARAMETERS 
{	BYTE           		Reserved1[ 16 ];
	PVOID          		Reserved2[ 10 ];
	t_UNICODE_STRING 	ImagePathName;
	t_UNICODE_STRING 	CommandLine;
} t_RTL_USER_PROCESS_PARAMETERS;

typedef struct t_tagPEB 
{	BYTE                          	Reserved1[ 2 ];
	BYTE                          	BeingDebugged;
	BYTE                          	Reserved2[ 1 ];
	PVOID                         	Reserved3[ 2 ];
	t_PEB_LDR_DATA                	*Ldr;
	t_RTL_USER_PROCESS_PARAMETERS 	*ProcessParameters;
	BYTE                          	Reserved4[ 104 ];
	PVOID                         	Reserved5[ 52 ];
	t_PPS_POST_PROCESS_INIT_ROUTINE	*PostProcessInitRoutine;
	BYTE                          	Reserved6[ 128 ];
	PVOID                         	Reserved7[ 1 ];
	ULONG                         	SessionId;
} t_PEB;

typedef struct t_tagPROCESS_BASIC_INFORMATION 
{   PVOID 		Reserved1;
    t_PEB		*PebBaseAddress;
    PVOID 		Reserved2[ 2 ];
    ULONG_PTR	UniqueProcessId;
    PVOID 		Reserved3;
} t_PROCESS_BASIC_INFORMATION;

typedef enum et_PROCESSINFOCLASS 
{    et_ProcessBasicInformation = 0
} et_PROCESSINFOCLASS;

#define def_TH32CS_SNAPPROCESS	0x00000002
#define def_TH32CS_SNAPNOHEAPS	0x40000000

typedef HANDLE (WINAPI *pfn_CreateToolhelp32Snapshot)( DWORD dwFlags, DWORD th32ProcessID );
typedef BOOL (WINAPI *pfn_Process32First)( HANDLE hSnapshot, t_PPROCESSENTRY32 lppe );
typedef BOOL (WINAPI *pfn_Process32Next)( HANDLE hSnapshot, t_PPROCESSENTRY32 lppe );
typedef DWORD (WINAPI *pfn_GetProcessImageFileName)( HANDLE hProcess, LPTSTR lpImageFileName, DWORD nSize );
typedef DWORD (WINAPI *pfn_GetModuleFileNameEx)( HANDLE hProcess, HMODULE hModule, LPTSTR lpFilename, DWORD nSize );
typedef BOOL (WINAPI *pfn_GetProcessMemoryInfo)( HANDLE Process, t_PPROCESS_MEMORY_COUNTERS ppsmemCounters, DWORD cb );
typedef BOOL (WINAPI *pfn_GetProcessTimes)( HANDLE hProcess, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime );
typedef DWORD (WINAPI *pfn_GetFileVersionInfoSize)( LPCTSTR lptstrFilename, LPDWORD lpdwHandle );
typedef BOOL (WINAPI *pfn_GetFileVersionInfo)( LPCTSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData );
typedef BOOL (WINAPI *pfn_VerQueryValue)( LPCVOID pBlock, LPCTSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen );
typedef LONG (WINAPI *pfn_NtQueryInformationProcess)( HANDLE ProcessHandle, et_PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength );

#define FT2LL(ft)	(((oexUINT64)ft.dwHighDateTime << 32)|(oexUINT64)ft.dwLowDateTime)

#if defined( oexUNICODE )
#	define fnGetProcessImageFileName	"GetProcessImageFileNameW"
#	define fnGetModuleFileNameEx		"GetModuleFileNameExW"
#	define fnK32GetModuleFileNameEx		"K32GetModuleFileNameExW"
#	define fnGetFileVersionInfoSize		"GetFileVersionInfoSizeW"
#	define fnGetFileVersionInfo			"GetFileVersionInfoW"
#	define fnVerQueryValue				"VerQueryValueW"
#	define fnNtQueryInformationProcess	"NtQueryInformationProcess"
#else
#	define fnGetProcessImageFileName	"GetProcessImageFileNameA"
#	define fnGetModuleFileNameEx		"GetModuleFileNameExA"
#	define fnK32GetModuleFileNameEx		"K32GetModuleFileNameExA"
#	define fnGetFileVersionInfoSize		"GetFileVersionInfoSizeA"
#	define fnGetFileVersionInfo			"GetFileVersionInfoA"
#	define fnVerQueryValue				"VerQueryValueA"
#	define fnNtQueryInformationProcess	"NtQueryInformationProcess"
#endif

oexINT CSysUtil::GetProcessList( CPropertyBag *pb, bool bProcessInfo )
{
	if ( !pb )
		return -1;

	// Functions
	pfn_CreateToolhelp32Snapshot pCreateToolhelp32Snapshot = 0;
	pfn_Process32First pProcess32First = 0;
	pfn_Process32Next pProcess32Next = 0;

	// Load kernel32.dll
	HMODULE hKernel32 = LoadLibrary( oexT( "kernel32.dll" ) ), hPsapi = 0;
	if ( !hKernel32 )
		return -1;

	// Load functions
	pCreateToolhelp32Snapshot = (pfn_CreateToolhelp32Snapshot)GetProcAddress( hKernel32, "CreateToolhelp32Snapshot" );
	pProcess32First = (pfn_Process32First)GetProcAddress( hKernel32, "Process32First" );
	pProcess32Next = (pfn_Process32Next)GetProcAddress( hKernel32, "Process32Next" );

	// Ensure we got these functions
	if ( !pCreateToolhelp32Snapshot || !pProcess32First || !pProcess32Next )
	{	FreeLibrary( hKernel32 );
		return -1;
	} // end if

	// Create snapshot
	t_PROCESSENTRY32 pe; oexZero( pe ); pe.dwSize = sizeof( t_PROCESSENTRY32 );
	HANDLE hSs = pCreateToolhelp32Snapshot( def_TH32CS_SNAPPROCESS, 0 );
	if ( INVALID_HANDLE_VALUE == hSs )
	{	FreeLibrary( hKernel32 );
		return -1;
	} // end if

	long i = 0;
	if( pProcess32First( hSs, &pe ) )
	{
		do
		{	// Did we get a valid process id?
//			if ( pe.th32ProcessID )
			{
				CPropertyBag &r = (*pb)[ i++ ];
				r[ oexT( "pid" ) ] = pe.th32ProcessID;
				r[ oexT( "threads" ) ] = pe.cntThreads;
				r[ oexT( "parent" ) ] = pe.th32ParentProcessID;
				r[ oexT( "base_priority" ) ] = pe.pcPriClassBase;
				if ( pe.szExeFile && *pe.szExeFile )
					r[ oexT( "file" ) ] = pe.szExeFile;

				if ( bProcessInfo )
					GetProcessInfo( pe.th32ProcessID, &r[ oexT( "info" ) ] );

				// Re init struct
				oexZero( pe ); pe.dwSize = sizeof( t_PROCESSENTRY32 );

			} // end if

		} while( pProcess32Next( hSs, &pe ) );

	} // end if

	// Free the library
	FreeLibrary( hKernel32 );

	return i;
}

oexINT CSysUtil::GetProcessInfo( oexLONG lPid, CPropertyBag *pb )
{
	if ( !pb )
		return -1;

	// Functions
	pfn_GetProcessImageFileName pGetProcessImageFileName = 0;
	pfn_GetModuleFileNameEx pGetModuleFileNameEx = 0;
	pfn_GetProcessMemoryInfo pGetProcessMemoryInfo = 0;
	pfn_GetProcessTimes pGetProcessTimes = 0;

	// Load kernel32.dll
	HMODULE hKernel32 = LoadLibrary( oexT( "kernel32.dll" ) ), hPsapi = 0;
	if ( !hKernel32 )
		return -2;

	// Load extra functions
	pGetProcessImageFileName = (pfn_GetProcessImageFileName)GetProcAddress( hKernel32, fnGetProcessImageFileName );
	pGetModuleFileNameEx = (pfn_GetModuleFileNameEx)GetProcAddress( hKernel32, fnK32GetModuleFileNameEx );
	pGetProcessMemoryInfo = (pfn_GetProcessMemoryInfo)GetProcAddress( hKernel32, "GetProcessMemoryInfo" );
	pGetProcessTimes = (pfn_GetProcessTimes)GetProcAddress( hKernel32, "GetProcessTimes" );

	// Maybe they moved?
	if ( !pGetProcessImageFileName || !pGetModuleFileNameEx || !pGetProcessMemoryInfo )
	{	hPsapi = LoadLibrary( oexT( "psapi.dll" ) );
		if ( hPsapi )
		{	if ( !pGetProcessImageFileName )
				pGetProcessImageFileName = (pfn_GetProcessImageFileName)GetProcAddress( hPsapi, fnGetProcessImageFileName );
			if ( !pGetModuleFileNameEx )
				pGetModuleFileNameEx = (pfn_GetModuleFileNameEx)GetProcAddress( hPsapi, fnGetModuleFileNameEx );
			if ( !pGetProcessMemoryInfo )
				pGetProcessMemoryInfo = (pfn_GetProcessMemoryInfo)GetProcAddress( hPsapi, "GetProcessMemoryInfo" );
		} // end if
	} // end if

	(*pb)[ oexT( "error" ) ] = 0;		
		
	// Get file name
	oexINT ret = 0;
	HANDLE hProc = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, lPid );
	if ( INVALID_HANDLE_VALUE == hProc )
	{	ret = -3;
		(*pb)[ oexT( "errpos" ) ] = ret;
		(*pb)[ oexT( "error" ) ] = GetLastError();		
	} // end if
	else
	{
/*		// Load ntdll.dll
		HMODULE hNtDll = LoadLibrary( oexT( "ntdll.dll" ) );
		if ( hNtDll )
		{		
			// Load functions
			pfn_NtQueryInformationProcess
				pNtQueryInformationProcess
					= (pfn_NtQueryInformationProcess)GetProcAddress( hNtDll, fnNtQueryInformationProcess );

//oexSHOW( (long long)pNtQueryInformationProcess );
//oexFlush_stdout();
			if ( pNtQueryInformationProcess )
			{
				ULONG lSize = 0;
				t_PROCESS_BASIC_INFORMATION pbi;
				if ( 0 == pNtQueryInformationProcess( hProc, et_ProcessBasicInformation, &pbi, sizeof( pbi ), &lSize ) )
				{
//oexSHOW( (long long)pbi.PebBaseAddress );			
//oexSHOW( (long long)pbi.PebBaseAddress->ProcessParameters );			
//oexSHOW( (long long)pbi.PebBaseAddress )				
// oexSHOW( pbi.PebBaseAddress->ProcessParameters->CommandLine.Length ); 
// CStrW s( pbi.PebBaseAddress->ProcessParameters->CommandLine.Buffer );
//oexFlush_stdout();

					
					
				} // end if
//oexM();
//oexFlush_stdout();
				
			} // end if
			
		} // end if
*/	
		DWORD dwExitCode = 0;
		if ( !GetExitCodeProcess( hProc, &dwExitCode ) || STILL_ACTIVE != dwExitCode )
		{
			ret = -4;
			(*pb)[ oexT( "errpos" ) ] = ret;
			(*pb)[ oexT( "exitcode" ) ] = dwExitCode;
			(*pb)[ oexT( "error" ) ] = GetLastError();		
		} // end if
		else
		{
			// Get process handle count
			DWORD dwHandles = 0;
			if ( GetProcessHandleCount( hProc, &dwHandles ) )
				(*pb)[ oexT( "handles" ) ] = dwHandles;

			// Get user and domain names
			HANDLE hToken = 0;
			if ( !OpenProcessToken( hProc, TOKEN_QUERY, &hToken ) )
			{
				ret = -5;
				(*pb)[ oexT( "errpos" ) ] = ret;
				(*pb)[ oexT( "error" ) ] = GetLastError();		
			} // end if
			else
			{
				char buf[ 1024 ];
				DWORD dwSize = sizeof( buf );
				if( GetTokenInformation( hToken, TokenUser, (void*)buf, dwSize, &dwSize )
					&& buf && dwSize >= sizeof( TOKEN_USER ) )
				{
					TOKEN_USER *ptu = (TOKEN_USER*)buf;
					PSID sid = ptu->User.Sid;
					TCHAR cbUser[ 256 ] = { 0 };
					DWORD dwUser = sizeof( cbUser ) / sizeof( TCHAR );
					TCHAR cbDomain[ 256 ] = { 0 };
					DWORD dwDomain = sizeof( cbUser ) / sizeof( TCHAR );
					SID_NAME_USE snu;

					// Get account information
					if ( LookupAccountSid( 0, sid, cbUser, &dwUser, cbDomain, &dwDomain, &snu) )
					{	(*pb)[ oexT( "user" ) ] = CStr( cbUser, dwUser );
						(*pb)[ oexT( "domain" ) ] = CStr( cbDomain, dwDomain );
					} // end if

				} // end if

				CloseHandle( hToken );

			} // end if

			oex::oexUINT64 ts = (*pb)[ oexT( "ts" ) ].ToInt64(),
						   now = oexGmtTime().GetUnixTimeUs() * 100;
			oex::oexUINT64 diff = ( 0 < ts && ts < now ) ? ( now - ts ) : 0;

			// Timestamp
			(*pb)[ oexT( "ts" ) ] = now;
			(*pb)[ oexT( "diff" ) ] = diff;

			if ( pGetProcessTimes )
			{	FILETIME tCreated, tExit, tKernel, tUser;
				if ( pGetProcessTimes( hProc, &tCreated, &tExit, &tKernel, &tUser ) )
				{	ret++;

					CPropertyBag &cpu = (*pb)[ oexT( "cpu" ) ];

					if ( 0 < diff )
					{
						// Calculate kernel percent
						oex::oexINT64 t = cpu[ oexT( "kernel" ) ].ToUInt64();
						if ( 0 < t )
							cpu[ oexT( "diff_kernel" ) ] = FT2LL(tKernel) - t,
							cpu[ oexT( "percent_kernel" ) ] = (double)( FT2LL(tKernel) - t ) * (double)100 / (double)diff;

						// Calculate user percent
						t = cpu[ oexT( "user" ) ].ToUInt64();
						if ( 0 < t )
							cpu[ oexT( "diff_user" ) ] = FT2LL(tUser) - t,
							cpu[ oexT( "percent_user" ) ] = (double)( FT2LL(tUser) - t ) * (double)100 / (double)diff;

						// Calculate user percent
						t = cpu[ oexT( "total" ) ].ToUInt64();
						if ( 0 < t )
							cpu[ oexT( "diff_total" ) ] = FT2LL(tKernel) + FT2LL(tUser) - t,
							cpu[ oexT( "percent_total" ) ] = (double)( FT2LL(tKernel) + FT2LL(tUser) - t ) * (double)100 / (double)diff;

					} // end if

					cpu[ oexT( "created" ) ] = FT2LL(tCreated);
					cpu[ oexT( "exit" ) ] = FT2LL(tExit);
					cpu[ oexT( "kernel" ) ] = FT2LL(tKernel);
					cpu[ oexT( "user" ) ] = FT2LL(tUser);
					cpu[ oexT( "total" ) ] = FT2LL(tKernel) + FT2LL(tUser);

				} // end if
			} // end if

			if ( pGetProcessImageFileName )
			{	ret++;
				TCHAR szFile[ MAX_PATH * 4 ];
				DWORD dwLen = pGetProcessImageFileName( hProc, szFile, sizeof( szFile ) );
				if ( 0 < dwLen && sizeof( szFile ) > dwLen )
				{	szFile[ dwLen ] = 0;
					(*pb)[ oexT( "dos" ) ] = szFile;
				} // end if
			} // end if

			if ( pGetModuleFileNameEx )
			{	ret++;
				TCHAR szFile[ MAX_PATH * 4 ];
				DWORD dwLen = pGetModuleFileNameEx( hProc, 0, szFile, sizeof( szFile ) );
				if ( 0 < dwLen && sizeof( szFile ) > dwLen )
				{	szFile[ dwLen ] = 0;
					(*pb)[ oexT( "path" ) ] = szFile;

					// Load version.dll
					HMODULE hVersion = LoadLibrary( oexT( "version.dll" ) );
					if ( hVersion )
					{
						// Load functions
						pfn_GetFileVersionInfoSize
							pGetFileVersionInfoSize
								= (pfn_GetFileVersionInfoSize)GetProcAddress( hVersion, fnGetFileVersionInfoSize );
						pfn_GetFileVersionInfo
							pGetFileVersionInfo
								= (pfn_GetFileVersionInfo)GetProcAddress( hVersion, fnGetFileVersionInfo );
						pfn_VerQueryValue
							pVerQueryValue
								= (pfn_VerQueryValue)GetProcAddress( hVersion, fnVerQueryValue );
								
						if ( pGetFileVersionInfoSize && pGetFileVersionInfo && pVerQueryValue )
						{	DWORD dwVisz = pGetFileVersionInfoSize( szFile, 0 );
							if ( 0 < dwVisz )
							{	char *buf = new char[ dwVisz * 2 ];
								if ( buf && pGetFileVersionInfo( szFile, 0, dwVisz, buf ) )
								{
									UINT uLen = 0;
									VS_FIXEDFILEINFO *pFfi = 0;
									pVerQueryValue( buf , oexT("\\") , (LPVOID *)&pFfi , &uLen );
									if ( pFfi )
									{
										CStr sVer;
										sVer = HIWORD( pFfi->dwFileVersionMS );
										sVer += oexT( "." );
										sVer += LOWORD( pFfi->dwFileVersionMS );
										sVer += oexT( "." );
										sVer += HIWORD( pFfi->dwFileVersionLS );
										sVer += oexT( "." );
										sVer += LOWORD( pFfi->dwFileVersionLS );

										(*pb)[ oexT( "version" ) ] = sVer;

									} // end if
									
									uLen = 0;
									t_tagLANGANDCODEPAGE *lacp;
									pVerQueryValue( buf , oexT("\\VarFileInfo\\Translation") , (LPVOID *)&lacp , &uLen );
									
									static oexTCHAR *pStrings[] = 
									{ 	oexT( "Comments" ),
										oexT( "CompanyName" ),
										oexT( "FileDescription" ),
										oexT( "FileVersion" ),
										oexT( "InternalName" ),
										oexT( "LegalCopyright" ),
										oexT( "LegalTrademarks" ),
										oexT( "OriginalFilename" ),
										oexT( "PrivateBuild" ),
										oexT( "ProductName" ),
										oexT( "ProductVersion" ),
										oexT( "SpecialBuild" ),
										0
									};
									
									for ( long i = 0; i < ( uLen / sizeof( t_tagLANGANDCODEPAGE ) ); i++ )
									{
										CPropertyBag &r = (*pb)[ oexT( "Strings" ) ][ oexFmt( oexT( "%04x" ), lacp[ i ].wLanguage ) ];
										for ( long s = 0; pStrings[ s ]; s++ )
										{
											CStr sSub = oexFmt( oexT( "\\StringFileInfo\\%04x%04x\\%s" ),
																lacp[ i ].wLanguage, lacp[ i ].wCodePage, pStrings[ s ] );

											oexTCHAR *pStr = 0;
											if ( pVerQueryValue( buf, sSub.c_str(), (void**)&pStr, &uLen ) && 1 < uLen && pStr )
												r[ pStrings[ s ] ] = CStr( pStr, uLen - 1 );
	
										} // end if
									
									} // end for
									
								} // end if
								
							} // end if
						} // end if
						
					} // end if

				} // end if
			} // end if

			if ( pGetProcessMemoryInfo )
			{	ret++;
				t_PROCESS_MEMORY_COUNTERS pmc; oexZero( pmc ); pmc.cb = sizeof( pmc );
				if ( pGetProcessMemoryInfo( hProc, &pmc, sizeof( pmc ) ) )
				{	CPropertyBag &inf = (*pb)[ oexT( "mem" ) ];
					inf[ oexT( "page_faults" ) ] = pmc.PageFaultCount;
					inf[ oexT( "peak_working_set" ) ] = pmc.PeakWorkingSetSize;
					inf[ oexT( "working_set" ) ] = pmc.WorkingSetSize;
					inf[ oexT( "peak_paged" ) ] = pmc.QuotaPeakPagedPoolUsage;
					inf[ oexT( "paged" ) ] = pmc.QuotaPagedPoolUsage;
					inf[ oexT( "peak_nonpaged" ) ] = pmc.QuotaPeakNonPagedPoolUsage;
					inf[ oexT( "nonpaged" ) ] = pmc.QuotaNonPagedPoolUsage;
					inf[ oexT( "page_files" ) ] = pmc.PagefileUsage;
					inf[ oexT( "peak_page_files" ) ] = pmc.PeakPagefileUsage;
				} // end if
			} // end if

		} // end else

		CloseHandle( hProc );

	} // end else

	if ( hPsapi )
		FreeLibrary( hPsapi );

	// Free the library
	FreeLibrary( hKernel32 );

	return ret;
}
