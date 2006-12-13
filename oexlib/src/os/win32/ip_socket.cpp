/*------------------------------------------------------------------
// ip_socket.cpp
// Copyright (c) 2006 
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

#include "../../stdafx.h"
#include "stdwin.h"

OEXLIB_USING_NAMESPACE

// Socket version we will use
const WORD c_MinSocketVersion = MAKEWORD( 1, 1 );


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

oexLONG CIpSocket::m_lInit = -1;

const oexSOCKET CIpSocket::c_InvalidSocket = (oexSOCKET)INVALID_SOCKET;
const oexSOCKETEVENT CIpSocket::c_InvalidEvent = (oexSOCKETEVENT)WSA_INVALID_EVENT;

CIpSocket::CIpSocket()
{
	m_hSocket = c_InvalidSocket;

	m_hSocketEvent = c_InvalidEvent;

	m_uLastError = 0;

	m_uPort = 0;
}

CIpSocket::~CIpSocket()
{	
	/// Lose the current socket
	Destroy();
}

oexBOOL CIpSocket::InitSockets()
{
	// Quit if already initialized
	if ( m_lInit == 0 ) return 
		TRUE;

	WSADATA wd;

	// Attempt to initialize the Socket library
	m_lInit = WSAStartup( c_MinSocketVersion, &wd );

	return IsInitialized();
}

void CIpSocket::UninitSockets()
{
	// Punt if not initialized
	if ( !IsInitialized() ) 
		return;

	// Not initialized
	m_lInit	= -1;

	// Clean up socket lib
	WSACleanup();
}

void CIpSocket::Destroy()
{
	// Punt if not initialized
	if ( !IsInitialized() ) 
		return;

	// Ditch the event handle
//	CloseEventHandle();

	// Save socket pointer to temp
	SOCKET hSocket = (SOCKET)m_hSocket;

	// Ditch member variable
	m_hSocket = c_InvalidSocket;

	// Invalidate member variables
	m_uPort = 0;
	m_sDotAddress = oexT( "" );

	// Ensure valid socket handle
	if ( INVALID_SOCKET == hSocket ) 
		return;

	// Close the socket
	closesocket( hSocket );

	// Save the last error code
	m_uLastError = WSAGetLastError();
}

oexBOOL CIpSocket::Create( oexINT x_af, oexINT x_type, oexINT x_protocol )
{
	// Punt if not initialized
	if ( !IsInitialized() ) 
		return FALSE;

	// Close any open socket
	Destroy();

	// Create a scocket
	m_hSocket = (oexSOCKET)socket( (int)x_af, (int)x_type, (int)x_protocol );
	
	// Save the last error code
	m_uLastError = WSAGetLastError();

	// Create the event handle
//	CreateEventHandle();

	// Capture all events
//	EventSelect();

	return IsSocket();
}

oexBOOL CIpSocket::Bind( oexUINT x_uPort )
{
	// Punt if not initialized
	if ( !IsInitialized() ) 
		return FALSE;

	// Create socket if there is none
	if ( !IsSocket() && !Create() ) 
	{	Destroy(); return FALSE; }

	sockaddr_in sai;
	ZeroMemory( &sai, sizeof( sai ) );

	sai.sin_family = PF_INET;
	sai.sin_port = htons( (WORD)x_uPort );

	// Attempt to bind the socket
	int nRet = bind( (SOCKET)m_hSocket, (sockaddr*)&sai, sizeof( sockaddr_in ) );

	// Save the last error code
	m_uLastError = WSAGetLastError();

	return !nRet;
}

oexBOOL CIpSocket::Listen( oexUINT x_uMaxConnections )
{
	// Punt if not initialized
	if ( !IsInitialized() )
		return FALSE;

	// Must have socket
	if ( !IsSocket() ) return FALSE;

	// Valid number of connections?
	if ( x_uMaxConnections == 0 ) return FALSE;

	// Start the socket listening
	int nRet = listen( (SOCKET)m_hSocket, (int)x_uMaxConnections );

	// Save the last error code
	m_uLastError = WSAGetLastError();

	return nRet;
}

//==============================================================
// CIpSocket_Connect()
//==============================================================
/// Connects to the specified address
/**
	\param [in] pSa		-	The network address of the target computer.
	\param [in] uSize	-	Size of the structure in psai.

	If psai is NULL, the member variable m_sai is used as the target address.

	\return Returns non-zero if success, otherwise zero.
*/
static BOOL CIpSocket_Connect(SOCKET hSocket, sockaddr_in *pSa, UINT uSize)
{
	// Punt if not initialized
	if ( !CIpSocket::IsInitialized() )
		return FALSE;

	// Sanity check
	if ( !pSa || !uSize ) 
		return oexFALSE;
	
	// Attempt a connection
	int nRet = connect( hSocket, (PSOCKADDR)pSa, uSize );

	// Return the error code
	return ( !nRet || WSAEWOULDBLOCK == WSAGetLastError() );
}

//==============================================================
// CIpSocket_GetHostByName()
//==============================================================
/// Gets host address information from DNS server
/**
	\param [in] pHost	-	Address to lookup.
	\param [out] pHe	-	Receives the address information.
	\param [out] ppaddr -	Recieves the IP address
	\param [out] puPort	-	Receives the TCP port specified in pHost

	\return Returns non-zero if success.
*/
// +++ One day I will re-write this function to save my sanity...
static BOOL CIpSocket_GetHostByName(LPCTSTR pHost, LPHOSTENT *pHe, LPIN_ADDR *ppaddr, UINT *puPort)
{
	// Sanity check
	if ( pHost == NULL || *pHost == 0x0 ) return FALSE;

	// Does caller want the port?  
	// Assume it is the number following ':' character
	if ( puPort )
	{	UINT i = 0; while ( pHost[ i ] && ':' != pHost[ i ] ) i++;
		*puPort = pHost[ i ] ? st::COsString::strtoul( &pHost[ i + 1 ] ) : 0;
	} // end if

	// Does the caller want the address?
	if ( pHe )
	{
		// First try to interpret as dot address
		ULONG uAddr = inet_addr( pHost );
		if ( INADDR_NONE != uAddr )
		{
			// Initialize HOSTENT structure
			ZeroMemory( *pHe, sizeof( HOSTENT ) );

			// Set address information
			(*ppaddr)[ 0 ].S_un.S_addr = uAddr;

			// Save address
			(*pHe)->h_addr_list = (char**)&ppaddr;

			// Quit without calling any functions
			return TRUE;

		} // end if

		// Attempt to lookup the name
		else *pHe = gethostbyname( pHost );	 

	} // end if

	// How did it go?
	return ( !pHe || *pHe );	
}

BOOL CIpSocket::Connect(LPCTSTR pAddress, UINT uPort)
{
	if ( !pAddress ) return oexFALSE;

	// Punt if not initialized
	if ( !IsInitialized() )
		return FALSE;

	UINT		port = 0;
	HOSTENT		he;
	LPHOSTENT	pHe = &he;
	IN_ADDR		paddr[ 1 ];
	LPIN_ADDR	ppaddr = paddr;
	ZeroMemory( &paddr, sizeof( paddr ) );

	// Interpret the host address
	if ( !CIpSocket_GetHostByName( pAddress, &pHe, &ppaddr, uPort ? NULL : &port ) ) 
	{	m_uLastError = WSAGetLastError(); return FALSE; }

	// Ensure we get the correct port
	if ( !uPort ) uPort = port;

	// Save the address
	if ( pAddress ) m_sAddress = pAddress;

	// Save address information
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons( (WORD)uPort );
	sa.sin_addr = *( (LPIN_ADDR)*pHe->h_addr_list );

	// Create socket if there is none
	if ( !IsSocket() && !Create() ) 
	{	Destroy(); return FALSE; }

	// Save the port
	m_uPort = sa.sin_port;

	// Create dot address if needed
	m_sDotAddress = inet_ntoa( sa.sin_addr );

	// Use dot address as address if not specified
	if ( !m_sAddress.length() ) m_sAddress = m_sDotAddress;

	// Attempt to connect
	oexBOOL bRet = CIpSocket_Connect( (SOCKET)m_hSocket, &sa, sizeof( sa ) );

	// Save the last error code
	m_uLastError = WSAGetLastError();

	return bRet;
}

oexBOOL CIpSocket::EventSelect( oexLONG lEvents )
{	
	if ( !IsSocket() ) 
		return FALSE;

	return ( WSAEventSelect( (SOCKET)m_hSocket, (WSAEVENT)m_hSocketEvent, lEvents ) == 0 ); 
}


