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

#if !defined( OEX_NOUNAME )
#	include <sys/utsname.h>
#endif

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

//////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////

#if defined( OEX_ANDROID )
#	if defined( __get_h_errno )
#		undef __get_h_errno
#	endif
//static int g_oex_errno = 0;
//int* __get_h_errno(void) { return (int*)&g_oex_errno; }
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

oexBOOL CIpAddress::SetRawAddress( oexINT64 x_llIp, oexINT32 x_uPort, oexINT32 x_uType )
{
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
	m_uCrc = ( (oexUINT16)ucHash[ 0 ] << 8 ) | (oexUINT16)ucHash[ 1 ];

    return oexTRUE;
}

oexBOOL CIpAddress::ValidateAddress()
{
    // Save the crc
    oexUINT16 uCrc = m_uCrc;

	// Zero crc for the check
    m_uCrc = 0;

    // Create hash
    oexUCHAR ucHash[ CCrcHash::eHashSize ];
    CCrcHash::Hash( &ucHash, &m_guid, sizeof( m_guid ) );

    // Verify the hash value
	oexUINT16 uCalc = ( (oexUINT16)ucHash[ 0 ] << 8 ) | (oexUINT16)ucHash[ 1 ];
    if ( uCrc != uCalc )
    {	oexASSERT( 0 );
    	Destroy();
    	return oexFALSE;
    }

    // Restore the crc
    m_uCrc = uCrc;

    return oexTRUE;
}

CStr CIpAddress::GetHostName()
{_STT();

	// Look up the host name
	char szHostName[ oexSTRSIZE ] = { 0 };
	if( ::gethostname( szHostName, sizeof( szHostName ) ) )
		return CStr();

	return CStr( oexMbToStr( szHostName ) );
}

CStr CIpAddress::GetFullHostName()
{_STT();
	CStr sDomain = GetDomainName();
	if ( sDomain.Length() )
		return GetHostName() << oexT( "." ) << sDomain;
	return GetHostName();
}


CStr CIpAddress::GetDomainName( oexCSTR x_pServer )
{_STT();
#if defined( OEX_NOUNAME )
	return CStr();
#else
	// Look up the domain name
	struct utsname un; oexZero( un );
	if( 0 > ::uname( &un ) || !un.domainname )
		return CStr();

	return CStr( oexMbToStr( un.domainname ) );
#endif
}

// +++ Make IPv6 safe
oexBOOL CIpAddress::SetDotAddress( oexCSTR x_pDotAddress, oexINT32 x_uPort, oexINT32 x_uType )
{
    if ( !oexVERIFY( x_pDotAddress ) )
        return oexFALSE;

    // Convert the dot address
    u_long ip = ntohl( inet_addr( oexStrToStr8Ptr( x_pDotAddress ) ) );
    if ( INADDR_NONE == ip )
        return oexFALSE;

    SetRawAddress( ip, x_uPort, x_uType );

    return oexTRUE;
}

// +++ Make IPv6 safe
CStr CIpAddress::GetDotAddress()
{
    in_addr ia;

    // Put the address in the structure
    ia.s_addr = htonl( (u_long)GetIpv4() );

	// Create dot address if needed
	return oexStr8ToStr( inet_ntop( AF_INET, &ia, CStr8().OexAllocate( 256 ), 256 ) );
}

oexCONST oexGUID* CIpAddress::GetId( oexGUID *x_pGuid )
{
    os::CSys::MemCpy( x_pGuid, &m_guid, sizeof( m_guid ) );
    return x_pGuid;
}

CIpAddress& CIpAddress::SetId( oexCONST oexGUID *x_pGuid )
{
    os::CSys::MemCpy( &m_guid, x_pGuid, sizeof( m_guid ) );
    return *this;
}

CStr CIpAddress::BuildUrl( CPropertyBag &x_pbUi )
{
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

	// Read in the scheme
	pb[ oexT( "scheme" ) ].ToString() = str.Parse( oexT( ":" ) );

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
		pb[ oexT( "host" ) ].ToString() = str;

	// Then whatever is left is the path
	else
		pb[ oexT( "path" ) ].ToString() = str;

	return pb;
}

oexBOOL CIpAddress::LookupUrl( oexCSTR x_pUrl, oexINT32 x_uPort, oexINT32 x_uType )
{
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
{
    // Lose old info
    Destroy();

    // Ensure we have a valid pointer
    if ( !oexCHECK_PTR( x_pServer ) )
        return oexFALSE;

// +++ Get this working eventually
// #if !defined( OEX_USE_GETHOSTBYNAME )
#if 0

	in_addr ip4;
	if ( inet_pton( PF_INET, oexStrToMbPtr( x_pServer ), &ip4 ) )
	{	SetRawAddress( ntohl( *(oexULONG*)&ip4.s_addr ), x_uPort, x_uType );
		return oexTRUE;
	} // end if

	struct addrinfo hints, *addr = 0;
	oexZeroMemory( &hints, sizeof( hints ) );
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;

	int rval = 0;
    if ( rval = getaddrinfo( x_pServer, 0, &hints, &addr ) )
    {  	oexERROR( rval, CStr().Fmt( oexT( "getaddrinfo( '%s' ) failed" ), oexStrToMbPtr( x_pServer ) ).Ptr() );
	} // end if

	int max = 128;
	while ( !oexCHECK_PTR( addr ) && max-- )
	{
		switch( addr->ai_family )
		{
			case AF_INET :
			{
				sockaddr_in *psai = (sockaddr_in*)addr->ai_addr;
				in_addr *pia = &psai->sin_addr;
			    SetRawAddress( ntohl( *(oexULONG*)&pia->s_addr ), x_uPort, x_uType );
			    return oexTRUE;

			} break;

			case AF_INET6 :
			{
				// +++ Add v6 support
				sockaddr_in6 *psai = (sockaddr_in6*)addr->ai_addr;
				sin6_addr *pia6 = &psai->sin6_addr;

			} break;

		} // end switch

		if ( addr == addr->ai_next )
			addr = 0;
		else
			addr = addr->ai_next;

	} // end while

	oexWARNING( 0, oexMks( oexT( "getaddrinfo( '" ), x_pServer, oexT( "' ) : lookup failed, max = " ), max ) );

	return oexFALSE;

#else

	// First try to interpret as dot address
	oexUINT uAddr = inet_addr( oexStrToStr8Ptr( x_pServer ) );
	if ( INADDR_NONE != uAddr )
	{   SetRawAddress( ntohl( uAddr ), x_uPort, x_uType );
		return oexTRUE;
	} // end if

    struct hostent *pHe;

	// Get host address
	oexDO( pHe = gethostbyname( oexStrToStr8Ptr( x_pServer ) ), !pHe && EINTR == h_errno, pHe );

    if ( !pHe || !pHe->h_addr_list )
    {  	oexERROR( h_errno, CStr().Fmt( oexT( "gethostbyname( '%s' ) failed" ), oexStrToMbPtr( x_pServer ) ).Ptr() );
        return oexFALSE;
	} // end if

    in_addr *pia = (in_addr*)*pHe->h_addr_list;
    if ( !oexCHECK_PTR( pia ) )
    {  	oexERROR( h_errno, CStr().Fmt( oexT( "gethostbyname( '%s' ) failed, h_addr_list is NULL" ), oexStrToMbPtr( x_pServer ) ).Ptr() );
        return oexFALSE;
	} // end if

    SetRawAddress( ntohl( *(oexUINT*)&pia->s_addr ), x_uPort, x_uType );

    return oexTRUE;

#endif

}

CPropertyBag CIpAddress::Lookup( oexCSTR x_pServer )
{_STT();

	CStr sServer( x_pServer );
	if ( !sServer.Length() )
		sServer = GetHostName();

    // Ensure we have a name
    if ( !sServer.Length() )
        return CPropertyBag();

	// First try to interpret as dot address
    struct hostent *pHe = gethostbyname( oexStrToStr8Ptr( x_pServer ) );

	if ( !pHe )
		return CPropertyBag();

	CPropertyBag pb;
	for ( int i = 0; pHe->h_addr_list[ i ]; i++ )
	{		
		in_addr *pia = (in_addr*)pHe->h_addr_list[ i ];
		if ( oexVERIFY_PTR( pia ) )
			pb[ oexMks( i ) ] = oexStr8ToStr( inet_ntoa( *pia ) );
		
    } // end for	

    return pb;
}


oexUINT CIpAddress::Arp( oexCSTR x_pDst, oexCSTR x_pSrc, oexBYTE *x_pAddr )
{
	return 0;
}

CPropertyBag CIpAddress::GetArpTable()
{
	return CPropertyBag();
}
