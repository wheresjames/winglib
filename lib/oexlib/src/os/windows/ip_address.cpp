/*------------------------------------------------------------------
// ip_address.cpp
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

#if !defined( OEX_NOIPHLPAPI )
#	include <iphlpapi.h>
#endif

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

oexBOOL CIpAddress::SetRawAddress( oexINT64 x_llIp, oexINT32 x_uPort, oexINT32 x_uType )
{_STT();
    // Set the ip address
    if ( x_uType == eAddrTypeIpv4 )
        m_uIpv4 = (oexUINT32)x_llIp, m_uIpv4Extra = 0;
    else m_llIpv6 = x_llIp;

    // Port number
    m_uPort = x_uPort;

    // Type information
    m_uType = x_uType;

    // Set the crc
    m_uCrc = 0;
    oexUCHAR ucHash[ CCrcHash::eHashSize ];
    CCrcHash::Hash( &ucHash, &m_guid, sizeof( m_guid ) );
    m_uCrc = *(oexUINT16*)&ucHash;

    return oexTRUE;
}

oexBOOL CIpAddress::ValidateAddress()
{_STT();
    // Save the crc
    oexUINT16 uCrc = m_uCrc;

	// Clear the crc while we hash
	m_uCrc = 0;

    // Create hash
    oexUCHAR ucHash[ CCrcHash::eHashSize ];
    CCrcHash::Hash( &ucHash, &m_guid, sizeof( m_guid ) );

    // Verify the hash value
    if ( uCrc != *(oexUINT16*)&ucHash )
    {//   oexASSERT( 0 ); 
		Destroy(); 
		return oexFALSE; 
	} // end if

    // Restore the crc
    m_uCrc = uCrc;

    return oexTRUE;
}

CStr CIpAddress::GetHostName()
{_STT();
	
	// Look up the host name
	char szHostName[ MAX_PATH ] = { 0 };
	if( ::gethostname( szHostName, sizeof( szHostName ) ) )
		return CStr();
	
	return CStr( oexMbToStr( szHostName ) );
}

CStr CIpAddress::GetFullHostName()
{_STT();
	CStr sDomain = GetDomainName();
	if ( sDomain.Length() )
		return sDomain << oexT( "\\" ) << GetHostName();
//		return GetHostName() << oexT( "." ) << sDomain;
	return GetHostName();
}

#define d_NET_API_FUNCTION __stdcall
typedef WCHAR* LMSTR;
typedef DWORD NET_API_STATUS;
typedef struct _WKSTA_INFO_100 
{	DWORD wki100_platform_id;
	LMSTR wki100_computername;
	LMSTR wki100_langroup;
	DWORD wki100_ver_major;
	DWORD wki100_ver_minor;
} WKSTA_INFO_100, *PWKSTA_INFO_100, *LPWKSTA_INFO_100;

typedef NET_API_STATUS (d_NET_API_FUNCTION *pfn_NetWkstaGetInfo)( LPWSTR servername, DWORD level, LPBYTE *bufptr );
typedef NET_API_STATUS (d_NET_API_FUNCTION *pfn_NetApiBufferFree)( LPVOID Buffer );

CStr CIpAddress::GetDomainName( oexCSTR x_pServer )
{_STT();

	CStr sRet;

	// Load netapi32.dll
	HMODULE hLib = LoadLibrary( oexT( "netapi32.dll" ) );
	if ( !hLib )
		return sRet;

	// Get function pointers
	pfn_NetApiBufferFree pNetApiBufferFree = (pfn_NetApiBufferFree)GetProcAddress( hLib, "NetApiBufferFree" );
	pfn_NetWkstaGetInfo pNetWkstaGetInfo = (pfn_NetWkstaGetInfo)GetProcAddress( hLib, "NetWkstaGetInfo" );

	// Attempt to read the domain name
	WKSTA_INFO_100 *pwi100 = oexNULL;
	if ( pNetWkstaGetInfo 
		 && !pNetWkstaGetInfo( ( x_pServer && *x_pServer ) ? (LPWSTR)oexStrToStrWPtr( x_pServer ) : oexNULL, 100, (LPBYTE*)&pwi100 ) )
		if ( pwi100 && pwi100->wki100_langroup )
			sRet = oexStrWToStr( pwi100->wki100_langroup );

	// Free buffer
	if ( pNetApiBufferFree && pwi100 )
		pNetApiBufferFree( pwi100 );

	// Free library
	FreeLibrary( hLib );

	// Send the domain name along
	return sRet;
}

oexBOOL CIpAddress::SetDotAddress( oexCSTR x_pDotAddress, oexINT32 x_uPort, oexINT32 x_uType )
{_STT();
#if defined( OEX_NOSOCKET2 )
	return oexFALSE;
#else
    if ( !oexVERIFY( x_pDotAddress ) )
        return oexFALSE;

    // Convert the dot address
    u_long ip = ntohl( inet_addr( oexStrToStr8Ptr( x_pDotAddress ) ) );
    if ( INADDR_NONE == ip )
        return oexFALSE;

    SetRawAddress( ip, x_uPort, x_uType );

    return oexTRUE;
#endif
}

CStr CIpAddress::GetDotAddress()
{_STT();
#if defined( OEX_NOSOCKET2 )
	return oexFALSE;
#else
    in_addr ia;

    // Put the address in the structure
    ia.S_un.S_addr = htonl( (u_long)GetIpv4() );

	// Create dot address if needed
	return oexStr8ToStr( inet_ntoa( ia ) );
#endif
}

oexCONST oexGUID* CIpAddress::GetId( oexGUID *x_pGuid )
{_STT();
    os::CSys::MemCpy( x_pGuid, &m_guid, sizeof( m_guid ) );
    return x_pGuid;
}

CIpAddress& CIpAddress::SetId( oexCONST oexGUID *x_pGuid )
{_STT();
    os::CSys::MemCpy( &m_guid, x_pGuid, sizeof( m_guid ) );
    return *this;
}

CStr CIpAddress::BuildUrl( CPropertyBag &x_pbUi )
{_STT();
	CStr str;

	// Scheme
	if ( x_pbUi[ oexT( "scheme" ) ].ToString().Length() )
		str << x_pbUi[ oexT( "scheme" ) ].ToString() << oexT( "://" );

	// Username and password?
	if ( x_pbUi[ oexT( "username" ) ].ToString().Length() )
	{
		if ( x_pbUi[ oexT( "password" ) ].ToString().Length() )
			str << x_pbUi[ oexT( "username" ) ].ToString()
			    << oexT( ":" )
			    << x_pbUi[ oexT( "password" ) ].ToString()
			    << oexT( "@" );
		else
			str << x_pbUi[ oexT( "username" ) ].ToString() << oexT( "@" );

	} // end if

	// Username and password?
	if ( x_pbUi[ oexT( "host" ) ].ToString().Length() )
	{
		if ( x_pbUi[ oexT( "host" ) ].ToString().Length() )
			str << x_pbUi[ oexT( "host" ) ].ToString()
			    << oexT( ":" )
			    << x_pbUi[ oexT( "port" ) ].ToString();
		else
			str << x_pbUi[ oexT( "host" ) ].ToString();

	} // end if

	// Ensure separator
	if ( oexT( '/' ) != *x_pbUi[ oexT( "path" ) ].ToString().Ptr()
	     && oexT( '\\' ) != *x_pbUi[ oexT( "path" ) ].ToString().Ptr() )
		str << oexT( '/' );

	// Append the path
	str << x_pbUi[ oexT( "path" ) ].ToString();

	// Adding separator
	if ( x_pbUi[ oexT( "extra" ) ].ToString().Length() )
		str << oexT( "?" ) << x_pbUi[ oexT( "extra" ) ].ToString();

	return str;
}


// Assuming formating like...
//  http://user:password@www.somesite.com/directory/somefile.php?param=1&param=2
CPropertyBag CIpAddress::ParseUrl( oexCSTR pUrl, oexUINT uMaxBufferSize )
{
	// Ensure a valid pointer
    if ( !pUrl )
        return CPropertyBag();

	// NULL terminated string? tsk, tsk...
	if ( 0 >= uMaxBufferSize )
		uMaxBufferSize = zstr::Length( pUrl );

	// Anything to parse?
	if ( !uMaxBufferSize )
		return CPropertyBag();

	// Propety bag object
    CPropertyBag pb;

	// Read into a string object
    CStr str( pUrl, uMaxBufferSize );

//	pb[ oexT( "scheme" ) ].ToString() = str.Parse( oexT( "://" ) );

	// Read in the scheme
	oexINT nScheme = str.FindSubStr( oexT( "://" ), 3 );
	if ( 0 <= nScheme && nScheme == str.FindSubStr( oexT( ":" ), 1 ) )
		pb[ oexT( "scheme" ) ].ToString() = str.SubStr( 0, nScheme );

	// Trim off leading forward slashes
	str.LTrim( oexT( ":" ) );
	str.LTrim( oexT( "/" ) );

	// Is there a username / password?
	CStr tmp = str.Parse( oexT( "@" ) );
	if ( tmp.Length() )
	{
		// Skip the @
		str++;

		// Divide username and password
		CStr s = tmp.Parse( oexT( ":" ) );
		if ( s.Length () )
		{	pb[ oexT( "username" ) ].ToString() = s;
			tmp++; pb[ oexT( "password" ) ].ToString() = tmp;
		} // end if
		else
			pb[ oexT( "username" ) ].ToString() = tmp;

	} // end if

	// Is there a username / password?
	tmp = str.Parse( oexT( "/" ) );
	if ( tmp.Length() )
	{
		CStr s = tmp.Parse( oexT( ":" ) );
		if ( s.Length () )
		{	pb[ oexT( "host" ) ].ToString() = s;
			tmp++; pb[ oexT( "port" ) ].ToString() = tmp;
		} // end if
		else
			pb[ oexT( "host" ) ].ToString() = tmp;

	} // end if

	// Grab the next token
	tmp = str.Parse( oexT( "?" ) );
	if ( tmp.Length() )
	{
		// Host or path?
		if ( !pb.IsKey( oexT( "host" ) ) )
			pb[ oexT( "host" ) ].ToString() = tmp;
		else
			pb[ oexT( "path" ) ].ToString() = tmp;

		// Parse extra part
		if ( str.Length() )
		{
			// Trim separator if any
			if ( oexT( '?' ) == *str.Ptr() )
			{
				str.LTrim( 1 );

				// Anything left over?
				if ( str.Length() )				
					pb[ oexT( "extra" ) ].ToString() = str.Parse( oexT( "#" ) );

				// Check for fragment
				if ( oexT( '#' ) == *str.Ptr() )
				{
					// Strip off sep
					str.LTrim( 1 );

					// Check for fragment
					if ( str.Length() )
						pb[ oexT( "fragment" ) ].ToString() = str;

				} // end if

				// Then it's all the extra
				else
					pb[ oexT( "extra" ) ].ToString() += str;

			} // end if

		} // end if
		
	} // end if

	// Whatever is left is the host if not yet parsed
	else if ( !pb.IsKey( oexT( "host" ) ) )
	{	CStr s = str.Parse( oexT( ":" ) );
		if ( s.Length () )
		{	pb[ oexT( "host" ) ].ToString() = s;
			str++; pb[ oexT( "port" ) ].ToString() = str;
		} // end if
		else
			pb[ oexT( "host" ) ].ToString() = str;
	} // end else if

	// Then whatever is left is the path
	else
		pb[ oexT( "path" ) ].ToString() = str;

	return pb;
}

/*

	// Dropping this MS stuff because of problems in WINCE

CStr CIpAddress::BuildUrl( CPropertyBag &x_pbUi )
{_STT();
    URL_COMPONENTS uc;
    os::CSys::Zero( &uc, sizeof( uc ) );
    uc.dwStructSize = sizeof( URL_COMPONENTS );

    uc.dwSchemeLength = x_pbUi[ oexT( "scheme" ) ].ToString().Length();
    uc.lpszScheme = x_pbUi[ oexT( "scheme" ) ].ToString()._Ptr();

    uc.dwHostNameLength = x_pbUi[ oexT( "host" ) ].ToString().Length();
    uc.lpszHostName = x_pbUi[ oexT( "host" ) ].ToString()._Ptr();

    uc.dwUserNameLength = x_pbUi[ oexT( "username" ) ].ToString().Length();
    uc.lpszUserName = x_pbUi[ oexT( "username" ) ].ToString()._Ptr();

    uc.dwPasswordLength = x_pbUi[ oexT( "password" ) ].ToString().Length();
    uc.lpszPassword = x_pbUi[ oexT( "password" ) ].ToString()._Ptr();

    uc.dwUrlPathLength = x_pbUi[ oexT( "path" ) ].ToString().Length();
    uc.lpszUrlPath = x_pbUi[ oexT( "path" ) ].ToString()._Ptr();

    // !!! Right or wrong, I'm going to hide this detail.
    CStr extra;
    if ( x_pbUi[ oexT( "extra" ) ].ToString().Length() )
    {   extra << oexT( "?" ) << x_pbUi[ oexT( "extra" ) ].ToString();
        uc.dwExtraInfoLength = extra.Length();
        uc.lpszExtraInfo = extra._Ptr();
    } // end if

    else
    {   uc.dwExtraInfoLength = x_pbUi[ oexT( "extra" ) ].ToString().Length();
        uc.lpszExtraInfo = x_pbUi[ oexT( "extra" ) ].ToString()._Ptr();
    } // end else

    uc.nScheme = (INTERNET_SCHEME)x_pbUi[ oexT( "scheme_id" ) ].ToLong();

    uc.nPort = (INTERNET_PORT)x_pbUi[ oexT( "port" ) ].ToLong();

    CStr str;
    DWORD dwLen = oexSTRSIZE;
    if ( !str.OexAllocate( oexSTRSIZE ) )
        return CStr();

    // Attempt to create url
    if ( !InternetCreateUrl( &uc, ICU_ESCAPE, str._Ptr(), &dwLen ) )
    {
        // Was the buffer too small?
        if ( ERROR_INSUFFICIENT_BUFFER != GetLastError() )
            return CStr();

        dwLen += 2;
        if ( !str.Allocate( dwLen ) )
            return CStr();

        // Try again
        if ( !InternetCreateUrl( &uc, ICU_ESCAPE, str._Ptr(), &dwLen ) )
            return CStr();

    } // end if

    return str;
}

CPropertyBag CIpAddress::ParseUrl( oexCSTR pUrl, oexUINT uMaxBufferSize )
{
    CPropertyBag pb;

    if ( !oexVERIFY( pUrl ) )
        return CPropertyBag();

    oexUINT uLen = zstr::Length( pUrl );

    // I assume the components can't be longer than the url
    if ( !uMaxBufferSize )
        uMaxBufferSize = uLen + 2;

    URL_COMPONENTS uc;
    os::CSys::Zero( &uc, sizeof( uc ) );
    uc.dwStructSize = sizeof( URL_COMPONENTS );

    uc.dwSchemeLength = uMaxBufferSize;
    uc.lpszScheme = pb[ oexT( "scheme" ) ].ToString().OexAllocate( uMaxBufferSize );

    uc.dwHostNameLength = uMaxBufferSize;
    uc.lpszHostName = pb[ oexT( "host" ) ].ToString().OexAllocate( uMaxBufferSize );

    uc.dwUserNameLength = uMaxBufferSize;
    uc.lpszUserName = pb[ oexT( "username" ) ].ToString().OexAllocate( uMaxBufferSize );

    uc.dwPasswordLength = uMaxBufferSize;
    uc.lpszPassword = pb[ oexT( "password" ) ].ToString().OexAllocate( uMaxBufferSize );

    uc.dwUrlPathLength = uMaxBufferSize;
    uc.lpszUrlPath = pb[ oexT( "path" ) ].ToString().OexAllocate( uMaxBufferSize );

    uc.dwExtraInfoLength = uMaxBufferSize;
    uc.lpszExtraInfo = pb[ oexT( "extra" ) ].ToString().OexAllocate( uMaxBufferSize );

    // Attempt to crack the url
    if ( !InternetCrackUrl( pUrl, uLen, ICU_DECODE | ICU_ESCAPE, &uc ) )
        return CPropertyBag();

    // Grab the port and scheme id
    pb[ oexT( "scheme_id" ) ] = uc.nScheme;
    pb[ oexT( "port" ) ] = uc.nPort;

    // !!! What a pain... See the above hack to hide this detail
    if ( oexT( '?' ) == *pb[ oexT( "extra" ) ].ToString().Ptr() )
        pb[ oexT( "extra" ) ].ToString().LTrim( 1 );

    return pb;
}
*/

oexBOOL CIpAddress::LookupUrl( oexCSTR x_pUrl, oexINT32 x_uPort, oexINT32 x_uType )
{_STT();
    // Lose old info
    Destroy();

    // Ensure we have a valid pointer
    if ( !oexVERIFY_PTR( x_pUrl ) )
        return oexFALSE;

    // Crack the url
    CPropertyBag pbUrl = ParseUrl( x_pUrl );
    if ( !pbUrl.Size() )
        return oexFALSE;

    // Did we get a host name?
    if ( !pbUrl[ oexT( "host" ) ].ToString().Length() )
        return oexFALSE;

    // Get the port
    if ( !x_uPort )
        x_uPort = pbUrl[ oexT( "port" ) ].ToLong();

    // Save the type
    m_uType = x_uType;

    return LookupHost( pbUrl[ oexT( "host" ) ].ToString().Ptr(), x_uPort );
}

oexBOOL CIpAddress::LookupHost( oexCSTR x_pServer, oexINT32 x_uPort, oexINT32 x_uType )
{_STT();
#if defined( OEX_NOSOCKET2 )
	return oexFALSE;
#else
    // Lose old info
    Destroy();

    // Ensure we have a valid pointer
    if ( !oexVERIFY_PTR( x_pServer ) )
        return oexFALSE;

	// First try to interpret as dot address
	ULONG uAddr = inet_addr( oexStrToStr8Ptr( x_pServer ) );
	if ( INADDR_NONE == uAddr )
    {
        LPHOSTENT pHe = gethostbyname( oexStrToStr8Ptr( x_pServer ) );

        if ( !pHe )
            return oexFALSE;

        LPIN_ADDR pia = (LPIN_ADDR)*pHe->h_addr_list;
        if ( !oexVERIFY_PTR( pia ) )
            return oexFALSE;

        // Grab the address
        uAddr = *(DWORD*)&pia->S_un.S_addr;

    } // end if

    SetRawAddress( ntohl( uAddr ), x_uPort, x_uType );

    return oexTRUE;
#endif
}

CPropertyBag CIpAddress::Lookup( oexCSTR x_pServer )
{_STT();
#if defined( OEX_NOSOCKET2 )
	return CSqMulti();
#else

	CStr sServer( x_pServer );
	if ( !sServer.Length() )
		sServer = GetHostName();

    // Ensure we have a name
    if ( !sServer.Length() )
        return CPropertyBag();

	// First try to interpret as dot address
    LPHOSTENT pHe = gethostbyname( oexStrToStr8Ptr( x_pServer ) );

	if ( !pHe )
		return CPropertyBag();

	CPropertyBag pb;
	for ( int i = 0; pHe->h_addr_list[ i ]; i++ )
	{		
		LPIN_ADDR pia = (LPIN_ADDR)pHe->h_addr_list[ i ];
		if ( oexVERIFY_PTR( pia ) )
			pb[ oexMks( i ) ] = oexStr8ToStr( inet_ntoa( *pia ) );
		
    } // end for	

    return pb;
	
#endif
}

typedef DWORD (WINAPI *pfn_SendARP)( oexUINT DestIP, oexUINT SrcIP, PULONG pMacAddr, PULONG PhyAddrLen );

oexUINT CIpAddress::Arp( oexCSTR x_pDst, oexCSTR x_pSrc, oexBYTE *x_pAddr )
{_STT();
	if ( !x_pDst || !x_pAddr )
		return 0;
		
	// Load netapi32.dll
	HMODULE hLib = LoadLibrary( oexT( "iphlpapi.dll" ) );
	if ( !hLib )
		return 0;

	// Get function pointers
	ULONG res = 0, uLen = 6;
	pfn_SendARP pSendARP = (pfn_SendARP)GetProcAddress( hLib, "SendARP" );
	if ( pSendARP )
		res = pSendARP( inet_addr( oexStrToMbPtr( x_pDst ) ), 
						x_pSrc ? inet_addr( oexStrToMbPtr( x_pSrc ) ) : 0, 
						(PULONG)x_pAddr, &uLen );
		
	FreeLibrary( hLib );
	
oexEcho( oexMks( res, " - ", x_pDst, " - ", (ULONG)inet_addr( oexStrToMbPtr( x_pDst ) ) ).Ptr() );

	return ( NO_ERROR == res ) ? uLen : 0;
}

typedef struct _s_MIB_IPNETROW 
{	DWORD	dwIndex;
	DWORD	dwPhysAddrLen;
	BYTE	bPhysAddr[ 8 ];
	DWORD	dwAddr;
	DWORD	dwType;
} s_MIB_IPNETROW;

typedef struct _s_MIB_IPNETTABLE
{	DWORD			dwNumEntries;
	s_MIB_IPNETROW	table[ 1 ];
} s_MIB_IPNETTABLE;

typedef DWORD (WINAPI *pfn_GetIpNetTable)( s_MIB_IPNETTABLE *pIpNetTable, PULONG pdwSize, BOOL bOrder );

CPropertyBag CIpAddress::GetArpTable()
{_STT();

	// Load netapi32.dll
	HMODULE hLib = LoadLibrary( oexT( "iphlpapi.dll" ) );
	if ( !hLib )
		return CPropertyBag();
		
	pfn_GetIpNetTable pGetIpNetTable = (pfn_GetIpNetTable)GetProcAddress( hLib, "GetIpNetTable" );
	if ( !pGetIpNetTable )
		return CPropertyBag();
		
	BYTE buf[ 64 * 1024 ];
	DWORD dwSize = sizeof( buf );
	if ( NO_ERROR != pGetIpNetTable( (s_MIB_IPNETTABLE*)buf, &dwSize, FALSE ) )
		return CPropertyBag();
		
	CPropertyBag pb;
	s_MIB_IPNETTABLE *pMint = (s_MIB_IPNETTABLE*)buf;
	for ( DWORD i = 0; i < pMint->dwNumEntries; i++ )
	{
		// Valid physical address?
		BYTE *pAddr = pMint->table[ i ].bPhysAddr;
		if ( 6 == pMint->table[ i ].dwPhysAddrLen 
			 && pAddr[ 0 ] && pAddr[ 1 ] && pAddr[ 2 ] 
			 && pAddr[ 3 ] && pAddr[ 4 ] && pAddr[ 5 ] )
		{
			CStr ipv4 = oexMbToStr( inet_ntoa( *(in_addr*)&pMint->table[ i ].dwAddr ) );

//			CPropertyBag &r = pb[ CStr( pMint->table[ i ].dwIndex ) ][ ipv4 ];
			CPropertyBag &r = pb[ ipv4 ];
			r[ oexT( "ipv4" ) ] = ipv4;
			
			DWORD mki = 0;
			while ( r.IsKey( CStr( mki ) ) )
				mki++;
			CStr mk( mki );
			
			r[ mk ][ oexT( "adapter" ) ] = CStr( pMint->table[ i ].dwIndex );
			r[ mk ][ oexT( "mac" ) ] = oex::CStr().Fmt( oexT( "%02X:%02X:%02X:%02X:%02X:%02X" ),
														pAddr[ 0 ], pAddr[ 1 ], pAddr[ 2 ], 
														pAddr[ 3 ],	pAddr[ 4 ], pAddr[ 5 ] );

			switch( pMint->table[ i ].dwType )
			{
				case 1 :
					r[ mk ][ oexT( "type" ) ] = oexT( "other" );
					break;
				
				case 2 :
					r[ mk ][ oexT( "type" ) ] = oexT( "invalid" );
					break;
				
				case 3 :
					r[ mk ][ oexT( "type" ) ] = oexT( "dynamic" );
					break;
				
				case 4 :
					r[ mk ][ oexT( "type" ) ] = oexT( "static" );
					break;

				default :
					r[ mk ][ oexT( "type" ) ] = oexT( "unknown" );
					break;
				
			} // end switch
			
		} // end if
		
	} // end for
	
	return pb;
}