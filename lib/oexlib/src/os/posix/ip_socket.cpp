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

/*
I've got a little black book with my poems in
I've got a bag with a toothbrush and a comb
When I'm a good dog they sometimes throw me a bone
I got elastic bands keeping my shoes on
Got those swollen hand blues.
Got thirteen channels of shit on the TV to choose from
I've got electric light
And I've got second sight
I've got amazing powers of observation

And that is how I know
When I try to get through
On the telephone to you
There'll be nobody home

I've got the obligatory Hendrix perm
And I've got the inevitable pinhole burns
All down the front of my favorite satin shirt
I've got nicotine stains on my fingers
I've got a silver spoon on a chain
I've got a grand piano to prop up my mortal remains
I've got wild staring eyes
I've got a strong urge to fly
But I've got nowhere to fly to
Ooooh Babe when I pick up the phone
There's still nobody home

I've got a pair of Gohills boots
And I've got fading roots.
*/

#include "../../../oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

// A few verifications
oexSTATIC_ASSERT( sizeof( CIpSocket::t_SOCKET ) >= sizeof( int ) );

#ifndef EPOLLRDHUP
#	define EPOLLRDHUP	0x2000
#endif

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
static oexBOOL CIpSocket_GetAddressInfo( CIpAddress *x_pIa, sockaddr_in *x_pSai )
{
    // Verify pointers
    if ( !oexVERIFY_PTR( x_pSai ) || !oexVERIFY_PTR( x_pIa ) )
        return oexFALSE;

    // Sets the raw address value
    x_pIa->SetRawAddress( ntohl( (oexULONG)x_pSai->sin_addr.s_addr ), ntohs( x_pSai->sin_port ) );

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
static oexBOOL CIpSocket_GetAddressInfo( CIpAddress *x_pIa, sockaddr *x_pSa )
{
   return CIpSocket_GetAddressInfo( x_pIa, (sockaddr_in*)x_pSa );
}

//==============================================================
// CIpSocket_SetAddressInfo()
//==============================================================
/// Copies the specified address
/**
	\param [in] x_pIa	-	Address information
	\param [in] x_pSai 	-	Structure to be filled in
*/
static oexBOOL CIpSocket_SetAddressInfo( CIpAddress *x_pIa, sockaddr_in *x_pSai )
{
    // Verify pointers
    if ( !oexVERIFY_PTR( x_pSai ) || !oexVERIFY_PTR( x_pIa ) )
        return oexFALSE;

    // Set the ip address
    x_pSai->sin_addr.s_addr = htonl( (oexULONG)x_pIa->GetIpv4() );

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
/*
static oexBOOL CIpSocket_SetAddressInfo( CIpAddress *x_pIa, sockaddr *x_pSa )
{
   return CIpSocket_GetAddressInfo( x_pIa, (sockaddr_in*)x_pSa );
}
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

oexLONG CIpSocket::m_lInit = -1;

static oexLONG g_CIpSocket_lInitCount = 0;

oexCONST CIpSocket::t_SOCKET CIpSocket::c_InvalidSocket = (CIpSocket::t_SOCKET)-1;
oexCONST CIpSocket::t_SOCKETEVENT CIpSocket::c_InvalidEvent = (CIpSocket::t_SOCKETEVENT)-1;
oexCONST CIpSocket::t_SOCKET CIpSocket::c_SocketError = (CIpSocket::t_SOCKET)-1;

oexINT CIpSocket::FlagWinToNix( oexINT x_nFlag )
{
#if defined( OEX_NOEPOLL )
	return 0;
#else

	oexINT nRet = 0;

	if ( eReadEvent & x_nFlag )
		nRet |= EPOLLIN | EPOLLPRI;

	if ( eWriteEvent & x_nFlag )
		nRet |= EPOLLOUT;

	if ( eConnectEvent & x_nFlag )
		nRet |= EPOLLIN | EPOLLPRI | EPOLLOUT;

	if ( eCloseEvent & x_nFlag )
		nRet |= EPOLLHUP | EPOLLRDHUP;

//	if ( eCreateEvent & x_nFlag )
//		nRet |= ;

	// +++ I think the accept flag is the same as the read or write ???
	if ( eAcceptEvent & x_nFlag )
		nRet |= EPOLLIN | EPOLLPRI | EPOLLOUT;

	return nRet;
#endif
}

oexINT CIpSocket::FlagNixToWin( oexINT x_nFlag )
{
#if defined( OEX_NOEPOLL )
	return 0;
#else

	oexINT nRet = 0;

	if ( ( EPOLLIN | EPOLLPRI ) & x_nFlag )
		nRet |= eReadEvent;

	if ( EPOLLOUT & x_nFlag )
		nRet |= eWriteEvent;

	if ( ( EPOLLIN | EPOLLPRI | EPOLLOUT ) & x_nFlag )
		nRet |= eConnectEvent;

	if ( ( EPOLLHUP | EPOLLRDHUP ) & x_nFlag )
		nRet |= eCloseEvent;

//	if(  & x_nFlag )
//		nRet |= eCreateEvent;

	// +++ I think the accept flag is the same as the read ???
	if ( ( EPOLLIN | EPOLLPRI | EPOLLOUT ) & x_nFlag )
		nRet |= eAcceptEvent;

	return nRet;
#endif
}

CIpSocket::CIpSocket()
{
	m_hSocket = c_InvalidSocket;

	m_hSocketEvent = c_InvalidEvent;

	m_uLastError = 0;

    m_uConnectState = 0;

    m_uEventState = 0;
    os::CSys::Zero( &m_uEventStatus, sizeof( m_uEventStatus ) );
    m_pEventObject = oexNULL;

    m_uSocketFamily = 0;
    m_uSocketType = 0;
    m_uSocketProtocol = 0;

	m_uReads = 0;
	m_uWrites = 0;
	m_uAccepts = 0;

}

CIpSocket::~CIpSocket()
{
	/// Lose the current socket
	Destroy();
}

oexLONG CIpSocket::GetInitCount()
{	return g_CIpSocket_lInitCount;
}

oexBOOL CIpSocket::InitSockets()
{
	// Add ref
	if ( 1 == oexInterlockedIncrement( &g_CIpSocket_lInitCount ) )
	{
		// Quit if already initialized
		if ( m_lInit == 0 )
			return oexTRUE;

		/// +++ Don't need init in linux?
		m_lInit = 0;

	} // end if

	// Don't cause SIGPIPE
	signal( SIGPIPE, SIG_IGN );

	return IsInitialized();
}

void CIpSocket::UninitSockets()
{
	// Deref
	if ( oexInterlockedDecrement( &g_CIpSocket_lInitCount ) )
		return;

	// Punt if not initialized
	if ( !IsInitialized() )
		return;

	// Not initialized
	m_lInit	= -1;
}

void CIpSocket::Destroy()
{
	// Ditch the event handle
	CloseEventHandle();

	// Save socket pointer to socket
	t_SOCKET hSocket = m_hSocket;

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

    m_uConnectState = 0;

	m_uReads = 0;
	m_uWrites = 0;
	m_uAccepts = 0;

	// Ensure valid socket handle
	if ( c_InvalidSocket == hSocket )
		return;

	if ( IsInitialized() )
	{
		// Shutdown the socket
		if ( -1 == shutdown( oexPtrToInt( hSocket ), SHUT_RDWR ) )
		{	m_uLastError = errno;
			if ( ENOTCONN != errno )
				oexERROR( errno, oexT( "shutdown() failed" ) );
		} // end if

		// Close the socket
		if ( -1 == close( oexPtrToInt( hSocket ) ) )
		{	m_uLastError = errno;
			oexERROR( errno, oexT( "close() failed" ) );
		} // end if

	} // end if

}

oexBOOL CIpSocket::Shutdown()
{
	if ( c_InvalidSocket == m_hSocket )
        return oexFALSE;

    // Shut down the socket
    if ( -1 == shutdown( oexPtrToInt( m_hSocket ), SHUT_RDWR ) )
    {	m_uLastError = errno;
		oexERROR( errno, oexT( "shutdown() failed" ) );
	} // end if

    return oexTRUE;
}


oexBOOL CIpSocket::Create( oexINT x_af, oexINT x_type, oexINT x_protocol )
{
	// Punt if not initialized
	if ( !IsInitialized() )
		return oexFALSE;

	// Close any open socket
	Destroy();

	// Create a scocket
	m_hSocket = (t_SOCKET)socket( (int)x_af, (int)x_type, (int)x_protocol );

	if ( c_InvalidSocket == m_hSocket )
    {	m_uLastError = errno;
		oexERROR( errno, oexT( "socket() failed" ) );
		return oexFALSE;
	} // end if

	m_uLastError = 0;

    // Save socket connect params
    m_uSocketFamily = x_af;
    m_uSocketType = x_type;
    m_uSocketProtocol = x_protocol;

	int set = 1;
	if ( -1 == setsockopt( oexPtrToInt( m_hSocket ), SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set) ) )
		oexWARNING( errno, oexT( "socket interface does not support SO_REUSEADDR" ) );

	// Capture all events
	EventSelect();

	return IsSocket();
}

oexBOOL CIpSocket::Bind( oexUINT x_uPort )
{
	// Punt if not initialized
	if ( !IsInitialized() )
		return oexFALSE;

	// Create socket if there is none
	if ( !IsSocket() && !Create() )
	{	Destroy(); return oexFALSE; }

	sockaddr_in sai;
	oexZeroMemory( &sai, sizeof( sai ) );
	sai.sin_family = PF_INET;
	sai.sin_port = htons( (oexUSHORT)x_uPort );

	// Attempt to bind the socket
	int nRet = bind( oexPtrToInt( m_hSocket ), (sockaddr*)&sai, sizeof( sockaddr_in ) );

	if ( -1 == nRet )
    {	m_uLastError = errno;
		oexERROR( errno, oexT( "bind() failed" ) );
	} // end if
	else
	{
		m_uLastError = 0;

	    // Grab the address
    	CIpSocket_GetAddressInfo( &m_addrLocal, &sai );

	} // end if

	return !nRet;
}

oexBOOL CIpSocket::Listen( oexUINT x_uMaxConnections )
{
	// Punt if not initialized
	if ( !IsInitialized() )
		return oexFALSE;

	// Must have socket
	if ( !IsSocket() )
        return oexFALSE;

	// Valid number of connections?
	if ( x_uMaxConnections == 0 )
        x_uMaxConnections = SOMAXCONN;

	// Start the socket listening
	int nRet = listen( oexPtrToInt( m_hSocket ), (int)x_uMaxConnections );

	if ( -1 == nRet )
    {	m_uLastError = errno;
		oexERROR( errno, oexT( "listen() failed" ) );
		return oexFALSE;
	} // end if

	m_uLastError = 0;

	// We're trying to connect
	m_uConnectState |= eCsConnecting;

    // Return the result
	return !nRet;
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
	{	Destroy(); return oexFALSE; }

    // Were we passed a URL?
    if ( !x_uPort && !m_addrPeer.LookupUrl( x_pAddress ) )
        return oexFALSE;

    // Lookup the host address
    else if ( !m_addrPeer.LookupHost( x_pAddress, x_uPort ) )
        return oexFALSE;

    sockaddr_in si;
    oexZeroMemory( &si, sizeof( si ) );
    si.sin_family = m_uSocketFamily;
    CIpSocket_SetAddressInfo( &m_addrPeer, &si );

    // Attempt to connect
    int nRet = connect( oexPtrToInt( m_hSocket ), (sockaddr*)&si, sizeof( si ) );

	if ( -1 == nRet )
    {	m_uLastError = errno;
		oexERROR( errno, oexT( "connect() failed" ) );
	} // end if

	else
	{
		m_uLastError = 0;
		
		// We're trying to connect
		m_uConnectState |= eCsConnecting;

	} // end else if

    if ( nRet && EINPROGRESS != m_uLastError )
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
		return oexFALSE;

    // Lose any current connection there might be
    x_is.Destroy();

	// Accept the connection
	sockaddr saAddr;
	socklen_t iAddr = sizeof( saAddr );

	// Accept and encapsulate the socket
	oexBOOL bSuccess = x_is.Attach( (t_SOCKET)accept( oexPtrToInt( m_hSocket ), &saAddr, &iAddr ) );

	if ( !bSuccess )
    {	m_uLastError = errno;
		oexERROR( errno, oexT( "accept() failed" ) );
	} // end if

	else
		m_uLastError = 0;

    // Grab the address
    CIpSocket_GetAddressInfo( &m_addrPeer, &saAddr );

	// Capture all events
	x_is.EventSelect();

	// Child is connecting
	x_is.m_uConnectState |= eCsConnecting;

	// Accepted
	m_uAccept++;
	
    return oexTRUE;
}

oexBOOL CIpSocket::CreateEventHandle()
{
#if defined( OEX_NOEPOLL )
	return oexFALSE;
#else

    // Check for event handle
    if ( IsEventHandle() )
        return oexTRUE;

	// Create an event handle
	m_hSocketEvent = (oexPVOID)epoll_create( eMaxEvents );

	if ( -1 == oexPtrToInt( m_hSocketEvent ) )
    {	m_uLastError = errno;
		oexERROR( errno, oexT( "epoll_create() failed" ) );
	} // end if

	else
		m_uLastError = 0;

	// Create event object array and initialize
	m_pEventObject = OexAllocNew< epoll_event >( eMaxEvents );
	oexZeroMemory( m_pEventObject, sizeof( epoll_event ) * eMaxEvents );

    return oexTRUE;
#endif
}

void CIpSocket::CloseEventHandle()
{
#if defined( OEX_NOEPOLL )
	return;
#else

	if ( IsInitialized() )
		if ( c_InvalidEvent != m_hSocketEvent )
		{
			// Close event handle
			if ( -1 == close( oexPtrToInt( m_hSocketEvent ) ) )
			{	m_uLastError = errno;
				oexERROR( errno, oexT( "close() failed" ) );
			} // end if

			else
				m_uLastError = 0;

		} // end if

	// Give up on socket event handle
	m_hSocketEvent = c_InvalidEvent;

	// Release event object
	if ( oexCHECK_PTR( m_pEventObject ) )
		OexAllocDelete( (epoll_event*)m_pEventObject );

	m_pEventObject = oexNULL;

#endif
}

oexBOOL CIpSocket::EventSelect( oexLONG x_lEvents )
{
#if defined( OEX_NOEPOLL )
	return oexFALSE;
#else
    // Punt if no socket
	if ( !IsSocket() )
		return oexFALSE;

    // Must have event handle
    if ( !IsEventHandle() || !m_pEventObject )
        CreateEventHandle();

	// Clear events
	m_uEventState = 0;

	epoll_event ev;
	oexZeroMemory( &ev, sizeof( ev ) );
	ev.data.fd = oexPtrToInt( m_hSocket );
	ev.events = EPOLLERR | FlagWinToNix( x_lEvents );

	// Set the event masks
	int nRes = epoll_ctl( oexPtrToInt( m_hSocketEvent ), EPOLL_CTL_ADD, oexPtrToInt( m_hSocket ), &ev );

	if ( -1 == nRes )
    {	m_uLastError = errno;
		oexERROR( errno, oexT( "epoll_ctl() failed" ) );
		return oexFALSE;
	} // end if

	return oexTRUE;
#endif
}

oexUINT CIpSocket::WaitEvent( oexLONG x_lEventId, oexUINT x_uTimeout )
{
#if defined( OEX_NOEPOLL )
	return 0;
#else
	// Must have a socket handle
	if ( !IsSocket() )
        return 0;

    // Must have event handle
    if ( !IsEventHandle() || !m_pEventObject )
    {
    	if ( !CreateEventHandle() )
    	{	oexERROR( m_uLastError, oexT( "Failed to create socket event" ) );
    		return 0;
		} // end if

    	if ( !EventSelect() )
    	{	oexERROR( m_uLastError, oexT( "Failed to enable socket events" ) );
            return 0;
		} // end if

    } // end if

	// +++ Ensure our event is being waited on?

	// Grab pointer to event object
	epoll_event *pev = (epoll_event*)m_pEventObject;

	// Save start time
	oexUINT uEnd = CHqTimer::GetBootCount() + x_uTimeout;
	for( ; ; ) // forever
	{
        // What's the event state
        if ( 0 == ( m_uEventState & x_lEventId ) )
        {
			// Wait for events
			oexINT nRes;
			do { nRes = epoll_wait( oexPtrToInt( m_hSocketEvent ), pev, eMaxEvents, x_uTimeout );
			} while ( -1 == nRes && EINTR == errno );

			if ( -1 == nRes )
			{
				// Log error
				m_uLastError = errno;
				oexERROR( errno, oexMks( oexT( "epoll_wait() failed : m_hSocketEvent = " ), oexPtrToInt( m_hSocketEvent ) ) );

				// Disconnected?
				m_uConnectState |= eCsError;

				// Just ditch if they aren't waiting for the close event
				m_uEventState |= eCloseEvent;
				if ( !( x_lEventId & eCloseEvent ) )
					return 0;

			} // end if

			// Check for closed socket
//			if ( !nRes )
//				m_uEventState |= eCloseEvent;

			// Process all events
			if ( 0 < nRes )
				for ( oexINT i = 0; i < nRes; i++ )
				{
					// Convert to windows flags
					oexUINT uFlags = FlagNixToWin( pev[ i ].events );

					// Save the status of all events
					if ( pev[ i ].data.fd == oexPtrToInt( m_hSocket ) )
					{	pev[ i ].events = 0;
						for ( oexUINT uMask = 1; uMask < eAllEvents; uMask <<= 1 )
							if ( uFlags & uMask )
							{
								// Get bit offset
								oexUINT uOffset = GetEventBit( uMask );

								// Save the event info
								m_uEventState |= uMask;
								m_uEventStatus[ uOffset ] = 0;

								// Signal activity
								m_uConnectState |= eCsActivity;

								// Turn off error flag
//								m_uConnectState &= ~eCsError;

								// +++ Signal when we get a connect message
//								if ( 0 != ( ( EPOLLIN | EPOLLOUT ) & uMask ) )
//									m_uConnectState |= 2;

							} // end if

					} // end if

				} // end for

            // !!!  Kludge around missing connect message
            if ( !( m_uConnectState & eCsConnected ) && ( m_uConnectState & eCsActivity ) )
            {   m_uConnectState |= eCsConnected;
//				m_uConnectState &= ~eCsError;
                m_uEventState |= eConnectEvent;
                m_uEventStatus[ eConnectBit ] = 0;
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
		oexUINT uTick = CHqTimer::GetBootCount();
		if ( uEnd <= uTick )
            return 0;

		// Adjust timeout
		x_uTimeout = uEnd - uTick;

	} // end if

	// Can't get here...
	return 0;
#endif
}

oexUINT CIpSocket::GetEventBit( oexLONG x_lEventMask )
{
    // !!!  Events will be returned by WaitEvent() in the order
    //      they appear below.

	if ( 0 != ( eConnectEvent & x_lEventMask ) )
        return eConnectBit;

	if ( 0 != ( eAcceptEvent & x_lEventMask ) )
        return eAcceptBit;

	if ( 0 != ( eWriteEvent & x_lEventMask ) )
        return eWriteBit;

	if ( 0 != ( eReadEvent & x_lEventMask ) )
        return eReadBit;

	if ( 0 != ( eOobEvent & x_lEventMask ) )
        return eOobBit;

	if ( 0 != ( eQosEvent & x_lEventMask ) )
        return eQosBit;

	if ( 0 != ( eGroupQosEvent & x_lEventMask ) )
        return eGroupQosBit;

	if ( 0 != ( eRoutingInterfaceChangeEvent & x_lEventMask ) )
        return eRoutingInterfaceChangeBit;

	if ( 0 != ( eCloseEvent & x_lEventMask ) )
        return eCloseBit;

	return 0;
}

oexCSTR CIpSocket::GetErrorMsg( oexUINT x_uErr )
{
	return oexT( "Unknown" );

/*	oexCSTR ptr = oexT( "Unknown Winsock error" );

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
*/
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

    sockaddr_in si;
    socklen_t nSize = sizeof( si );
    os::CSys::Zero( &si, sizeof( si ) );
    si.sin_family = m_uSocketFamily;

	// Receive data from socket
	x_uFlags |= MSG_NOSIGNAL;
	int nRes = recvfrom( oexPtrToInt( m_hSocket ), x_pData, (int)x_uSize,
                         (int)x_uFlags, (sockaddr*)&si, &nSize );

	m_uReads++;

	if ( -1 == nRes )
    {	m_uLastError = errno;
		m_uEventState |= eCloseEvent;
		m_uConnectState |= eCsError;
		oexWARNING( errno, oexT( "recvfrom() failed" ) );
		if ( x_puRead )
            *x_puRead = 0;
		return oexFALSE;
	} // end if

	m_uLastError = 0;

    // Save the address
    CIpSocket_GetAddressInfo( &m_addrPeer, &si );

	// Check for closed socket
	if ( !nRes )
        return 0;

	// Check for socket error
	if ( -1 == nRes || x_uSize < (oexUINT)nRes  || 0 > nRes )
	{
		// Nothing read
		if ( x_puRead )
            *x_puRead = 0;

		return 0;

	} // end if

	// Save the number of bytes read
	if ( x_puRead )
        *x_puRead = nRes;

	return nRes;
}

CStr8 CIpSocket::RecvFrom( oexUINT x_uMax, oexUINT x_uFlags )
{
    // Do we have a size limit?
    if ( x_uMax )
    {
        // Allocate buffer
        CStr8 sBuf;
        sBuf.Allocate( x_uMax );

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
            && (oexINT)uRead >= oexSTRSIZE )
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
	x_uFlags |= MSG_NOSIGNAL;
	int nRes = recv( oexPtrToInt( m_hSocket ), x_pData, (int)x_uSize, (int)x_uFlags );

	m_uReads++;

	if ( -1 == nRes )
    {	m_uLastError = errno;
		m_uEventState |= eCloseEvent;
		m_uConnectState |= eCsError;
		oexWARNING( errno, oexT( "recv() failed" ) );
		if ( x_puRead )
            *x_puRead = 0;
		return oexFALSE;
	} // end if

	m_uLastError = 0;

	// Check for closed socket
	if ( !nRes )
	{	m_uEventState |= eCloseEvent;
        return 0;
	} // end if

	// Check for socket error
	if ( x_uSize < (oexUINT)nRes  || 0 > nRes )
	{
		// Nothing read
		if ( x_puRead )
            *x_puRead = 0;

		return 0;

	} // end if

	// Save the number of bytes read
	if ( x_puRead )
        *x_puRead = (oexUINT)nRes;

	return nRes;
}

CStr8 CIpSocket::Recv( oexUINT x_uMax, oexUINT x_uFlags )
{
    // Do we have a size limit?
    if ( x_uMax )
    {
        // Allocate buffer
        CStr8 sBuf;
        sBuf.Allocate( x_uMax );

        // Attempt to read data
        oexUINT uRead = Recv( sBuf._Ptr(), x_uMax, oexNULL, x_uFlags );

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
    while ( 0 < ( uRead = Recv( sBuf._Ptr( uOffset ), oexSTRSIZE, oexNULL, x_uFlags ) )
            && (oexINT)uRead >= oexSTRSIZE )
    {
		if ( (oexINT)uRead > oexSTRSIZE )
			uRead = oexSTRSIZE;

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
    sockaddr_in si;
    os::CSys::Zero( &si, sizeof( si ) );
    si.sin_family = m_uSocketFamily;
    CIpSocket_SetAddressInfo( &m_addrPeer, &si );

    // Send the data
	x_uFlags |= MSG_NOSIGNAL;
    int nRes = sendto( oexPtrToInt( m_hSocket ), x_pData, (int)x_uSize,
                       (int)x_uFlags, (sockaddr*)&si, sizeof( si ) );

	m_uWrites++;

	// Check for error
	if ( -1 == nRes )
	{
		// Get the last error code
		m_uLastError = errno;

		m_uEventState |= eCloseEvent;

		m_uConnectState |= eCsError;

		oexWARNING( errno, oexT( "sendto() failed" ) );

		// Number of bytes sent
		if ( x_puSent )
            *x_puSent = 0;

		// Error
		return 0;

	} // end if

	m_uLastError = 0;

	// Save the number of bytes sent
	if ( x_puSent )
        *x_puSent = nRes;

	return nRes;
}

oexUINT CIpSocket::Send( oexCPVOID x_pData, oexUINT x_uSize, oexUINT *x_puSent, oexUINT x_uFlags )
{
	// Initialize bytes sent
	if ( x_puSent )
        *x_puSent = 0;

	// Must have a socket handle
	if ( !IsSocket() )
        return 0;

	// Attempt to send the data
	x_uFlags |= MSG_NOSIGNAL;
	int nRes = send( oexPtrToInt( m_hSocket ), x_pData, (int)x_uSize, (int)x_uFlags );

	m_uWrites++;

	// Check for error
	if ( -1 == nRes )
	{
		// Get the last error code
		m_uLastError = errno;

		m_uEventState |= eCloseEvent;

		m_uConnectState |= eCsError;

		oexWARNING( errno, oexT( "send() failed" ) );

		// Number of bytes sent
		if ( x_puSent )
            *x_puSent = 0;

		// Error
		return 0;

	} // end if

	m_uLastError = 0;

	// Save the number of bytes sent
	if ( x_puSent )
        *x_puSent = nRes;

	return nRes;
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
	sockaddr_in sai;
    os::CSys::Zero( &sai, sizeof( sai ) );
	socklen_t len = sizeof( sai );

	// Get the socket info
	if ( -1 == getpeername( oexPtrToInt( x_hSocket ), (sockaddr*)&sai, &len ) )
		if ( ENOTCONN != errno )
			oexERROR( errno, oexT( "getpeername() failed" ) );

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
	sockaddr_in sai;
    os::CSys::Zero( &sai, sizeof( sai ) );
	socklen_t len = sizeof( sai );

	// Get the socket info
	if ( -1 == getsockname( oexPtrToInt( x_hSocket ), (sockaddr*)&sai, &len ) )
		oexERROR( errno, oexT( "getsockname() failed" ) );

    // Format the info
    return CIpSocket_GetAddressInfo( x_pIa, &sai );
}

