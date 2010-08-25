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

#include "../../../oexlib.h"

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
			return CStr();

		case REG_SZ :
		case REG_MULTI_SZ :
		case REG_EXPAND_SZ :

			// Size includes NULL string
			if ( dwSize )
				dwSize--;

			return CStr( (oexCSTR)pBuf, dwSize );

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
		return CStr();

	DWORD dwType = 0, dwSize = 0;
	lRes = RegQueryValueEx( hKey, x_sName.Ptr(), 0, &dwType, 0, &dwSize );
	if ( ERROR_SUCCESS != lRes )
	{	RegCloseKey( hKey );
		return CStr();
	} // end if

	// Allocate memory for value
	CBin buf;
	if ( !buf.Allocate( dwSize ) )
		return CStr();

	lRes = RegQueryValueEx( hKey, x_sName.Ptr(), 0, &dwType, (LPBYTE)buf._Ptr(), &dwSize );
	if ( ERROR_SUCCESS != lRes )
	{	RegCloseKey( hKey );
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
		return oexFALSE;

	RegCloseKey( hKey );

	return oexFALSE;
}

CPropertyBag CSysUtil::GetDiskInfo(const CStr &x_sDrive)
{_STT();
	// Sanity check
	if ( !x_sDrive.Length() ) 
		return CPropertyBag();

	DWORD 	dwSn = 0;
	DWORD	dwMax = 0;
	DWORD	dwFlags = 0;
	char	szVolume[ 1024 * 8 ] = { 0 };
	char	szFileSystem[ 1024 * 8 ] = { 0 };
	
	// Get volume information
	if ( !GetVolumeInformation(	x_sDrive.Ptr(), szVolume, sizeof( szVolume ),
								&dwSn, &dwMax, &dwFlags,
								szFileSystem, sizeof( szFileSystem ) ) )
		return CPropertyBag();

	CPropertyBag pb;
	pb[ "drive" ] = x_sDrive;
	pb[ "volume" ] = oexMbToStrPtr( szVolume );
	pb[ "serial" ] = dwSn;
	pb[ "max_filename" ] = dwMax;
	pb[ "flags" ] = dwFlags;
	pb[ "file_system" ] = oexMbToStrPtr( szFileSystem );

	return pb;
}
