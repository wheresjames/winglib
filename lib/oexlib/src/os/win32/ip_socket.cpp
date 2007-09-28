/*------------------------------------------------------------------
// ip_socket.cpp
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

#include <WinSock2.h>
#pragma comment( lib, "WS2_32.lib" )

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

// A few verifications
oexSTATIC_ASSERT( sizeof( CIpSocket::t_SOCKET ) == sizeof( SOCKET ) );
oexSTATIC_ASSERT( sizeof( CIpSocket::t_SOCKETEVENT ) == sizeof( WSAEVENT ) );

// Socket version we will use
oexCONST WORD c_MinSocketVersion = MAKEWORD( 1, 1 );

//////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////

//==============================================================
// CIpSocket_GetAddressInfo()
//==============================================================
/// Copies the specified address
/**
	\param [in] x_pIa	-	Address structure to be filled in.
	\param [in] x_pSai 	-	Address information
*/
static oexBOOL CIpSocket_GetAddressInfo( CIpAddress *x_pIa, SOCKADDR_IN *x_pSai )
{
    // Verify pointers
    if ( !oexVERIFY_PTR( x_pSai ) || !oexVERIFY_PTR( x_pIa ) )
        return oexFALSE;

    // Sets the raw address value
    x_pIa->SetRawAddress( ntohl( *(DWORD*)&x_pSai->sin_addr.S_un.S_addr ), ntohs( x_pSai->sin_port ) );

    return oexTRUE;
}

//==============================================================
// CIpSocket_GetAddressInfo()
//==============================================================
/// Copies the specified address
/**
	\param [in] x_pIa	-	Address structure to be filled in.
	\param [in] x_pSa 	-	Address information
*/
static oexBOOL CIpSocket_GetAddressInfo( CIpAddress *x_pIa, SOCKADDR *x_pSa )
{   return CIpSocket_GetAddressInfo( x_pIa, (SOCKADDR_IN*)x_pSa ); }

//==============================================================
// CIpSocket_SetAddressInfo()
//==============================================================
/// Copies the specified address
/**
	\param [in] x_pIa	-	Address information
	\param [in] x_pSai 	-	Structure to be filled in
*/
static oexBOOL CIpSocket_SetAddressInfo( CIpAddress *x_pIa, SOCKADDR_IN *x_pSai )
{
    // Verify pointers
    if ( !oexVERIFY_PTR( x_pSai ) || !oexVERIFY_PTR( x_pIa ) )
        return oexFALSE;

    // Set the ip address
    *(DWORD*)&x_pSai->sin_addr.S_un.S_addr = htonl( (DWORD)x_pIa->GetIpv4() );

    // Set the port
    x_pSai->sin_port = htons( (short)x_pIa->GetPort() );

    return oexTRUE;
}

//==============================================================
// CIpSocket_SetAddressInfo()
//==============================================================
/// Copies the specified address
/**
	\param [in] x_pIa	-	Address information
	\param [in] x_pSai 	-	Structure to be filled in
*/
static oexBOOL CIpSocket_SetAddressInfo( CIpAddress *x_pIa, SOCKADDR *x_pSa )
{   return CIpSocket_GetAddressInfo( x_pIa, (SOCKADDR_IN*)x_pSa ); }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

oexLONG CIpSocket::m_lInit = -1;

oexCONST CIpSocket::t_SOCKET CIpSocket::c_InvalidSocket = (CIpSocket::t_SOCKET)INVALID_SOCKET;
oexCONST CIpSocket::t_SOCKETEVENT CIpSocket::c_InvalidEvent = (CIpSocket::t_SOCKETEVENT)WSA_INVALID_EVENT;
oexCONST CIpSocket::t_SOCKET CIpSocket::c_SocketError = (CIpSocket::t_SOCKET)SOCKET_ERROR;


CIpSocket::CIpSocket()
{
	m_hSocket = c_InvalidSocket;

	m_hSocketEvent = c_InvalidEvent;

	m_uLastError = 0;

    m_uConnectState = 0;

    m_uEventState = 0;
    os::CSys::Zero( &m_uEventStatus, sizeof( m_uEventStatus ) );

    m_uSocketFamily = 0;
    m_uSocketType = 0;
    m_uSocketProtocol = 0;
}

CIpSocket::~CIpSocket()
{	
	/// Lose the current socket
	Destroy();
}

oexBOOL CIpSocket::InitSockets()
{
	// Quit if already initialized
	if ( m_lInit == 0 ) 
        return oexTRUE;

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
	CloseEventHandle();

	// Save socket pointer to socket
	SOCKET hSocket = (SOCKET)m_hSocket;

	// Ditch member variable
	m_hSocket = c_InvalidSocket;

	// Invalidate member variables
    m_sConnect.Destroy();
    m_addrPeer.Destroy();
    m_addrLocal.Destroy();

    m_uConnectState = 0;
    m_uEventState = 0;
    os::CSys::Zero( &m_uEventStatus, sizeof( m_uEventStatus ) );

    m_uSocketFamily = 0;
    m_uSocketType = 0;
    m_uSocketProtocol = 0;

	// Ensure valid socket handle
	if ( INVALID_SOCKET == hSocket ) 
		return;

	// Close the socket
	closesocket( hSocket );

	// Save the last error code
	m_uLastError = WSAGetLastError();
}

oexBOOL CIpSocket::Shutdown()
{
	if ( c_InvalidSocket == m_hSocket )
        return oexFALSE;

    // Shut down the socket
    shutdown( (SOCKET)m_hSocket, SD_BOTH );

    return oexTRUE;
}


oexBOOL CIpSocket::Create( oexINT x_af, oexINT x_type, oexINT x_protocol )
{
	// Punt if not initialized
	if ( !IsInitialized() ) 
		return FALSE;

	// Close any open socket
	Destroy();

	// Create a scocket
	m_hSocket = (t_SOCKET)socket( (int)x_af, (int)x_type, (int)x_protocol );
	
	// Save the last error code
	m_uLastError = WSAGetLastError();

    // Was there an error?
    if ( c_InvalidSocket == m_hSocket )
        return oexFALSE;

    // Save socket connect params
    m_uSocketFamily = x_af;
    m_uSocketType = x_type;
    m_uSocketProtocol = x_protocol;

	// Capture all events
	EventSelect();

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

    // Grab the address
    CIpSocket_GetAddressInfo( &m_addrLocal, &sai );

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
	if ( !IsSocket() ) 
        return FALSE;

	// Valid number of connections?
	if ( x_uMaxConnections == 0 ) 
        x_uMaxConnections = SOMAXCONN;

	// Start the socket listening
	int nRet = listen( (SOCKET)m_hSocket, (int)x_uMaxConnections );

	// Save the last error code
	m_uLastError = WSAGetLastError();

    // Return the result
	return c_SocketError != (t_SOCKET)nRet;
}


oexBOOL CIpSocket::Connect( oexCSTR x_pAddress, oexUINT x_uPort)
{
	if ( !oexVERIFY_PTR( x_pAddress ) )
        return oexFALSE;

	// Punt if not initialized
	if ( !IsInitialized() )
		return oexFALSE;

	// Create socket if there is none
	if ( !IsSocket() && !Create() ) 
	{	Destroy(); return FALSE; }

    // Were we passed a URL?
    if ( !x_uPort && !m_addrPeer.LookupUrl( x_pAddress ) )
        return oexFALSE;

    // Lookup the host address
    else if ( !m_addrPeer.LookupHost( x_pAddress, x_uPort ) )
        return oexFALSE;

    SOCKADDR_IN si;
    os::CSys::Zero( &si, sizeof( si ) );
    si.sin_family = m_uSocketFamily;
    CIpSocket_SetAddressInfo( &m_addrPeer, &si );

    // Attempt to connect
    int nRet = connect( (SOCKET)m_hSocket, (PSOCKADDR)&si, sizeof( si ) );

	// Save the last error code
	m_uLastError = WSAGetLastError();

    if ( nRet && WSAEWOULDBLOCK != m_uLastError )
        return oexFALSE;

	return oexTRUE;
}

oexBOOL CIpSocket::Attach( t_SOCKET x_hSocket ) 
{	
    Destroy(); 
	
    m_hSocket = x_hSocket; 

    return IsSocket(); 
}

oexBOOL CIpSocket::Accept( CIpSocket &x_is )
{
    // Punt if no socket
	if ( !IsSocket() ) 
		return FALSE;

    // Lose any current connection there might be
    x_is.Destroy();

	// Accept the connection
	SOCKADDR saAddr; int iAddr = sizeof( saAddr );

	// Accept and encapsulate the socket
	BOOL bSuccess = x_is.Attach( (t_SOCKET)accept( (SOCKET)m_hSocket, &saAddr, &iAddr ) );

    // Check for error
    if ( !bSuccess )
  	{   m_uLastError = WSAGetLastError(); 
        return FALSE; 
    } // end if

    // Grab the address
    CIpSocket_GetAddressInfo( &m_addrPeer, &saAddr );

	// Capture all events
	x_is.EventSelect();

    return oexTRUE;
}

oexBOOL CIpSocket::CreateEventHandle()
{
    // Check for event handle
    if ( IsEventHandle() )
        return oexTRUE;

    // Create socket event
    m_hSocketEvent = WSACreateEvent();

    // How did it turn out?
    return c_InvalidEvent != m_hSocketEvent;
}

void CIpSocket::CloseEventHandle()
{
    // Do we have a valid handle?
    if ( c_InvalidEvent != m_hSocketEvent )
    {
        // Close the event handle
        WSACloseEvent( m_hSocketEvent );

        // Invalid handle value
        m_hSocketEvent = c_InvalidEvent;

    } // end if
}

oexBOOL CIpSocket::EventSelect( oexLONG x_lEvents )
{	
    // Punt if no socket
	if ( !IsSocket() ) 
		return FALSE;

    // Must have event handle
    if ( !IsEventHandle() )
        CreateEventHandle();

	return ( WSAEventSelect( (SOCKET)m_hSocket, (WSAEVENT)m_hSocketEvent, x_lEvents ) == 0 ); 
}

oexUINT CIpSocket::WaitEvent( oexLONG x_lEventId, oexUINT x_uTimeout )
{
	// Must have a socket handle
	if ( !IsSocket() ) 
        return 0;

    // Must have event handle
    if ( !IsEventHandle() )
    {   if ( !CreateEventHandle() || !EventSelect() )
            return 0;
    } // end if

	// Save start time
	UINT uEnd = GetTickCount() + x_uTimeout;
	for( ; ; )
	{
        // What's the event state
        if ( 0 == ( m_uEventState & x_lEventId ) )
        {
            if ( x_uTimeout )
            {
		        // Wait for event
		        UINT uRet = WaitForSingleObject( m_hSocketEvent, x_uTimeout );

		        // Check for timeout or error
		        if ( uRet != WAIT_OBJECT_0 ) 
                    return 0;

            } // end if

		    // Reset the network event
		    ResetEvent( m_hSocketEvent );

		    // Get network events
            WSANETWORKEVENTS wne; os::CSys::Zero( &wne, sizeof( wne ) );
		    if ( SOCKET_ERROR == WSAEnumNetworkEvents( (SOCKET)m_hSocket, m_hSocketEvent, &wne ) )
			    return 0;

            // Save the status of all events
            for ( oexUINT uMask = 1; uMask < eAllEvents; uMask <<= 1 )
                if ( wne.lNetworkEvents & uMask )
                {   
                    // Get bit offset
                    oexUINT uOffset = GetEventBit( uMask );
                    
                    // Save the event info
                    m_uEventState |= uMask;
                    m_uEventStatus[ uOffset ] = wne.iErrorCode[ uOffset ];
                    
                    // Signal activity
                    m_uConnectState |= 1;

                    // Signal when we get a connect message
                    if ( 0 != ( FD_CONNECT & wne.lNetworkEvents ) )
                        m_uConnectState |= 2;

                } // end if

            // !!!  Kludge around missing connect message
            //      If we're accepting a connection, we don't
            //      get a connect message.  ( At least on Windows )
            if ( !( m_uConnectState & 2 ) && ( m_uConnectState & 1 ) )
            {   m_uConnectState |= 2;
                wne.lNetworkEvents |= FD_CONNECT;
                m_uEventState |= FD_CONNECT;
                m_uEventStatus[ FD_CONNECT_BIT ] = 0;
            } // end if

        } // end if

		// Did our event go off?
		if ( 0 != ( m_uEventState & x_lEventId ) )
		{
            // Get the first event
            oexUINT uBit = GetEventBit( x_lEventId & m_uEventState );
            oexUINT uMask = 1 << uBit;

            // Acknowledge this event
            m_uEventState &= ~uMask;

			// Save the error code
			m_uLastError = m_uEventStatus[ uBit ];

			// We received the event
			return uMask;

		} // end if

		// Have we timed out?
		UINT uTick = GetTickCount();
		if ( uEnd <= uTick ) 
            return 0;

		// Adjust timeout
		x_uTimeout = uEnd - uTick;

	} // end if

	// Can't get here...
	return 0;
}

oexUINT CIpSocket::GetEventBit( oexLONG x_lEventMask )
{
    // !!!  Events will be returned by WaitEvent() in the order
    //      they appear below.

	if ( 0 != ( FD_CONNECT & x_lEventMask ) ) 
        return FD_CONNECT_BIT;

	if ( 0 != ( FD_ACCEPT & x_lEventMask ) ) 
        return FD_ACCEPT_BIT;	

	if ( 0 != ( FD_WRITE & x_lEventMask ) ) 
        return FD_WRITE_BIT;

	if ( 0 != ( FD_READ & x_lEventMask ) ) 
        return FD_READ_BIT;	

	if ( 0 != ( FD_OOB & x_lEventMask ) ) 
        return FD_OOB_BIT;	

	if ( 0 != ( FD_QOS & x_lEventMask ) ) 
        return FD_QOS_BIT;

	if ( 0 != ( FD_GROUP_QOS & x_lEventMask ) ) 
        return FD_GROUP_QOS_BIT;

	if ( 0 != ( FD_ROUTING_INTERFACE_CHANGE & x_lEventMask ) ) 
        return FD_ROUTING_INTERFACE_CHANGE_BIT;

	if ( 0 != ( FD_CLOSE & x_lEventMask ) ) 
        return FD_CLOSE_BIT;

	return 0;
}

oexCSTR CIpSocket::GetErrorMsg( oexUINT x_uErr )
{
	oexCSTR ptr = oexT( "Unknown Winsock error" );

	switch( x_uErr )
	{
		case WSAEACCES:
			ptr = oexT( "Access Denied" );
			break;
		case WSAEADDRINUSE:
			ptr = oexT( "Address already in use" );
			break;
		case WSAEADDRNOTAVAIL:
			ptr = oexT( "Cannot assign requested address" );
			break;
		case WSAEAFNOSUPPORT:
			ptr = oexT( "Address family not supported by protocol family" );
			break;
		case WSAEALREADY:
			ptr = oexT( "Operation already in progress" );
			break;
		case WSAECONNABORTED:
			ptr = oexT( "Software caused connection abort" );
			break;
		case WSAECONNREFUSED:
			ptr = oexT( "Connection refused" );
			break;
		case WSAECONNRESET:
			ptr =oexT( "Connection reset by peer" );
			break;
		case WSAEDESTADDRREQ:
			ptr =oexT( "Destination addres required" );
			break;
		case WSAEFAULT:
			ptr =oexT( "Bad Address" );
			break;
		case WSAEHOSTDOWN:
			ptr =oexT( "Host is down" );
			break;
		case WSAEHOSTUNREACH:
			ptr =oexT( "Host is unreachable" );
			break;
		case WSAEINPROGRESS:
			ptr =oexT( "Operation is now in progress" );
			break;
		case WSAEINTR:
			ptr =oexT( "Interrupted function call" );
			break;
		case WSAEINVAL:
			ptr =oexT( "Invalid argument" );
			break;
		case WSAEISCONN:
			ptr =oexT( "Socket is already connected" );
			break;
		case WSAEMFILE:
			ptr =oexT( "Too many open files" );
			break;
		case WSAEMSGSIZE:
			ptr =oexT( "Message is too long" );
			break;
		case WSAENETDOWN:
			ptr =oexT( "Network is down" );
			break;
		case WSAENETRESET:
			ptr =oexT( "Network dropped connection on reset" );
			break;
		case WSAENETUNREACH:
			ptr =oexT( "Network is unreachable" );
			break;
		case WSAENOBUFS:
			ptr =oexT( "Insufficient buffer space is available" );
			break;
		case WSAENOPROTOOPT:
			ptr =oexT( "Bad protocol option" );
			break;
		case WSAENOTCONN:
			ptr =oexT( "Socket is not connected" );
			break;
		case WSAENOTSOCK:
			ptr =oexT( "Socket operation on non-socket" );
			break;
		case WSAEOPNOTSUPP:
			ptr =oexT( "Operation not supported" );
			break;
		case WSAEPFNOSUPPORT:
			ptr =oexT( "Protocol family not supported" );
			break;
		case WSAEPROCLIM:
			ptr =oexT( "Too many processes" );
			break;
		case WSAEPROTONOSUPPORT:
			ptr =oexT( "Protocol not supported" );
			break;
		case WSAEPROTOTYPE:
			ptr =oexT( "Protocol wrong type for socket" );
			break;
		case WSAESHUTDOWN:
			ptr =oexT( "Cannot send after socket shutdown" );
			break;
		case WSAESOCKTNOSUPPORT:
			ptr =oexT( "Socket type not supported" );
			break;
		case WSAETIMEDOUT:
			ptr =oexT( "Connection timed out" );
			break;
		case WSATYPE_NOT_FOUND:
			ptr =oexT( "Class type not found" );
			break;
		case WSAEWOULDBLOCK:
			ptr =oexT( "Resource temporarily unavailable (Would block)" );
			break;
		case WSAHOST_NOT_FOUND:
			ptr =oexT( "Host not found" );
			break;
		case WSA_INVALID_HANDLE:
			ptr =oexT( "Specified event object handle is invalid" );
			break;
		case WSA_INVALID_PARAMETER:
			ptr =oexT( "One or mor parameters are invalid" );
			break;
//		case WSAINVALIDPROCTABLE;
//			ptr =oexT( "Invalid procedure table from service provider" );
//			break;
//		case WSAINVALIDPROVIDER:
//			ptr =oexT( "Invalid service provider version number" );
//			break;
		case WSA_IO_INCOMPLETE:
			ptr =oexT( "Overlapped I/O event object not in signaled state" );
			break;
		case WSA_IO_PENDING:
			ptr =oexT( "Overlapped I/O operations will complete later" );
			break;
		case WSA_NOT_ENOUGH_MEMORY:
			ptr =oexT( "Insufficient memory available" );
			break;
		case WSANOTINITIALISED: 
			ptr =oexT( "Successful WSAStartup not yet performed" );
			break;
		case WSANO_DATA:
			ptr =oexT( "Valid name, no data record of requested type" );
			break;
		case WSANO_RECOVERY:
			ptr =oexT( "Non-recoverable error has occured" );
			break;
//		case WSAPROVIDERFAILEDINIT:
//			ptr =oexT( "Unable to initialize a service provider" );
//			break;
		case WSASYSCALLFAILURE:
			ptr =oexT( "System call failure" );
			break;
		case WSASYSNOTREADY:
			ptr =oexT( "Network subsystem is unavailable" );
			break;
		case WSATRY_AGAIN:
			ptr =oexT( "Non-authoritative host not found" );
			break;
		case WSAVERNOTSUPPORTED:
			ptr =oexT( "WINSOCK.DLL version not supported" );
			break;
		case WSAEDISCON:
			ptr =oexT( "Graceful shutdown in progress" );
			break;
		case WSA_OPERATION_ABORTED:
			ptr =oexT( "Overlapped I/O operation has been aborted" );
			break;
//		case WSAE:
//			ptr =oexT( "" );
//			break;

	} // end switch

	return ptr;
}

//oexUINT CIpSocket::CheckReturn() {}

oexUINT CIpSocket::RecvFrom( oexPVOID x_pData, oexUINT x_uSize, oexUINT *x_puRead, oexUINT x_uFlags )
{
	// Initialize bytes read
	if ( x_puRead ) 
        *x_puRead = 0;

	// Must have a socket handle
	if ( !IsSocket() ) 
        return oexFALSE;

    SOCKADDR_IN si;
    int nSize = sizeof( si );
    os::CSys::Zero( &si, sizeof( si ) );
    si.sin_family = m_uSocketFamily;

	// Receive data from socket
	int nRet = recvfrom( (SOCKET)m_hSocket, (LPSTR)x_pData, (int)x_uSize, 
                         (int)x_uFlags, (LPSOCKADDR)&si, &nSize );

    // Grab the last error code
    m_uLastError = WSAGetLastError();

    // Save the address
    CIpSocket_GetAddressInfo( &m_addrPeer, &si );

	// Check for closed socket
	if ( !nRet ) 
        return 0;

	// Check for socket error
	if ( SOCKET_ERROR == nRet || x_uSize < (oexUINT)nRet  || 0 > nRet )
	{
		// Is the socket blocking?
		if ( WSAEWOULDBLOCK != m_uLastError )
			return 0;

		// Nothing read
		if ( x_puRead ) 
            *x_puRead = 0;

		return 0;

	} // end if

	// Save the number of bytes read
	if ( x_puRead ) 
        *x_puRead = nRet;

	return nRet;
}

CStr8 CIpSocket::RecvFrom( oexUINT x_uMax, oexUINT x_uFlags )
{   
    // Do we have a size limit?
    if ( x_uMax )
    {
        // Allocate buffer
        CStr8 sBuf; 
        oexCHAR *pBuf = sBuf.Allocate( x_uMax ); 

        // Attempt to read data
        oexUINT uRead = RecvFrom( sBuf._Ptr(), x_uMax, oexNULL, x_uFlags );

        // Accept as the length
        sBuf.SetLength( uRead ); 

        return sBuf;

    } // end if

    // Allocate buffer
    CStr8 sBuf; 
    oexUINT uRead = 0, uOffset = 0;

    // Allocate space
    if ( !sBuf.OexAllocate( oexSTRSIZE ) )
        return sBuf;

    // Read all available data
    while ( 0 < ( uRead = RecvFrom( sBuf._Ptr( uOffset ), oexSTRSIZE, oexNULL, x_uFlags ) ) 
            && uRead >= oexSTRSIZE )
    {
        // Allocate more space
        uOffset += uRead;        
        if ( !sBuf.Allocate( uOffset + oexSTRSIZE ) )
            return sBuf;
        
    } // end while

    // Set the length
    sBuf.SetLength( uOffset + uRead );

    return sBuf;
}


oexUINT CIpSocket::Recv( oexPVOID x_pData, oexUINT x_uSize, oexUINT *x_puRead, oexUINT x_uFlags )
{
	// Initialize bytes read
	if ( x_puRead ) 
        *x_puRead = 0;

	// Must have a socket handle
	if ( !IsSocket() ) 
        return oexFALSE;

	// Receive data from socket
	int nRet = recv( (SOCKET)m_hSocket, (LPSTR)x_pData, (int)x_uSize, (int)x_uFlags );

    // Grab the last error code
    m_uLastError = WSAGetLastError();

	// Check for closed socket
	if ( !nRet ) 
        return 0;

	// Check for socket error
	if ( SOCKET_ERROR == nRet || x_uSize < (oexUINT)nRet  || 0 > nRet )
	{
		// Is the socket blocking?
		if ( WSAEWOULDBLOCK != m_uLastError )
			return 0;

		// Nothing read
		if ( x_puRead ) 
            *x_puRead = 0;

		return 0;

	} // end if

	// Save the number of bytes read
	if ( x_puRead ) 
        *x_puRead = nRet;

	return nRet;
}

CStr8 CIpSocket::Recv( oexUINT x_uMax, oexUINT x_uFlags )
{   
    // Do we have a size limit?
    if ( x_uMax )
    {
        // Allocate buffer
        CStr8 sBuf; 
        oexCHAR *pBuf = sBuf.Allocate( x_uMax ); 

        // Attempt to read data
        oexUINT uRead = Recv( sBuf._Ptr(), x_uMax, oexNULL, x_uFlags );

        // Accept as the length
        sBuf.SetLength( uRead ); 

        return sBuf;

    } // end if

/*  // +++ Old method

    // Allocate buffer
    CStr sBuf; 
    oexUINT uRead;
    oexTCHAR ucBuf[ oexSTRSIZE ];

    // Read all available data
    while ( 0 < ( uRead = Recv( ucBuf, sizeof( ucBuf ), oexNULL, x_uFlags ) ) )
        sBuf.Append( ucBuf, uRead );

    return sBuf;
*/

    // Allocate buffer
    CStr8 sBuf; 
    oexUINT uRead = 0, uOffset = 0;

    // Allocate space
    if ( !sBuf.OexAllocate( oexSTRSIZE ) )
        return sBuf;

    // Read all available data
    while ( 0 < ( uRead = Recv( sBuf._Ptr( uOffset ), oexSTRSIZE, oexNULL, x_uFlags ) ) 
            && uRead >= oexSTRSIZE )
    {
        // Allocate more space
        uOffset += uRead;        
        if ( !sBuf.Allocate( uOffset + oexSTRSIZE ) )
            return sBuf;
        
    } // end while

    // Set the length
    sBuf.SetLength( uOffset + uRead );

    return sBuf;
}


oexUINT CIpSocket::SendTo( oexCONST oexPVOID x_pData, oexUINT x_uSize, oexUINT *x_puSent, oexUINT x_uFlags )
{
	// Initialize bytes sent
	if ( x_puSent ) 
        *x_puSent = 0;

	// Must have a socket handle
	if ( !IsSocket() ) 
        return 0;

    // Use the peer address
    SOCKADDR_IN si;
    os::CSys::Zero( &si, sizeof( si ) );
    si.sin_family = m_uSocketFamily;
    CIpSocket_SetAddressInfo( &m_addrPeer, &si );

    // Send the data
    int nRet = sendto( (SOCKET)m_hSocket, (LPCSTR)x_pData, (int)x_uSize, 
                       (int)x_uFlags, (LPSOCKADDR)&si, sizeof( si ) );

	// Get the last error code
	m_uLastError = WSAGetLastError();

	// Check for error
	if ( SOCKET_ERROR == nRet )
	{
		// Is the socket blocking?
		if ( WSAEWOULDBLOCK != m_uLastError )
			return 0;

		// Number of bytes sent
		if ( x_puSent ) 
            *x_puSent = 0;
			
		// Error
		return 0;

	} // end if

	// Save the number of bytes sent
	if ( x_puSent ) 
        *x_puSent = nRet;

	return nRet;
}


oexUINT CIpSocket::Send( oexCONST oexPVOID x_pData, oexUINT x_uSize, oexUINT *x_puSent, oexUINT x_uFlags )
{
	// Initialize bytes sent
	if ( x_puSent ) 
        *x_puSent = 0;

	// Must have a socket handle
	if ( !IsSocket() ) 
        return 0;

	// Attempt to send the data
	int nRet = send( (SOCKET)m_hSocket, (LPCSTR)x_pData, (int)x_uSize, (int)x_uFlags );

	// Get the last error code
	m_uLastError = WSAGetLastError();

	// Check for error
	if ( SOCKET_ERROR == nRet )
	{
		// Is the socket blocking?
		if ( WSAEWOULDBLOCK != m_uLastError )
			return 0;

		// Number of bytes sent
		if ( x_puSent ) 
            *x_puSent = 0;
			
		// Error
		return 0;

	} // end if

	// Save the number of bytes sent
	if ( x_puSent ) 
        *x_puSent = nRet;

	return nRet;
}

oexBOOL CIpSocket::GetPeerAddress( t_SOCKET x_hSocket, CIpAddress *x_pIa )
{
    if ( !oexVERIFY_PTR( x_pIa ) )
        return oexFALSE;

    // Reset address information
    x_pIa->Destroy();

    // Ensure socket
    if ( c_InvalidSocket == x_hSocket )
        return oexFALSE;

    // Init socket structure
	SOCKADDR_IN sai; 
    os::CSys::Zero( &sai, sizeof( sai ) );
	int len = sizeof( sai );

	// Get the socket info
	if ( getpeername( (SOCKET)x_hSocket, (sockaddr*)&sai, &len ) )
		return FALSE;

    // Format the info
    return CIpSocket_GetAddressInfo( x_pIa, &sai );
}

oexBOOL CIpSocket::GetLocalAddress( t_SOCKET x_hSocket, CIpAddress *x_pIa )
{
    if ( !oexVERIFY_PTR( x_pIa ) )
        return oexFALSE;

    // Reset address information
    x_pIa->Destroy();

    // Ensure socket
    if ( c_InvalidSocket == x_hSocket )
        return oexFALSE;

    // Init socket structure
	SOCKADDR_IN sai; 
    os::CSys::Zero( &sai, sizeof( sai ) );
	int len = sizeof( sai );

	// Get the socket info
	if ( getsockname( (SOCKET)x_hSocket, (sockaddr*)&sai, &len ) )
		return FALSE;

    // Format the info
    return CIpSocket_GetAddressInfo( x_pIa, &sai );
}
