/*------------------------------------------------------------------
// ip_socket.h
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

#pragma once

//==================================================================
// CIpSocket
//
/// Windows Socket API wrapper class
/**
	Provides synchronous socket support

	For most advanced applications, you will probably want the 
	asynchronous capability of TWinAsyncSocket.

	Example:
	\code

	CIpSocket ws;
	char szError[ 1024 ] = "";

	// Connect to web site
	if ( !ws.Connect( "google.com", 80 ) || !ws.WaitEvent( FD_CONNECT ) )

		printf( ws.GetLastErrorStr( szError ) );

	else
	{
		// Send the page request
		ws.Send( "GET / HTTP/1.0\r\n\r\n" );

		// Wait for data
		if ( m_ws.WaitEvent( FD_READ ) )
		{
			char szBuf[ 8 * 1024 ] = "";

			// Read in page data
			UINT uRead = ws.Recv( szBuf, sizeof( szBuf ) );

			printf( szBuf );

		} // end if

	} // end if

	\endcode

	\see CIpAsyncSocket, CIpSocketProtocol, CIpSocket
*/
//==================================================================
class CIpSocket
{
public:

	struct SAddress
	{
		/// Connection URL
		oexString			sUrl;

		/// Connection Server name
		oexString			sServer;

		/// Connection Port
		oexUINT				uPort;

		/// Connection IP Address
		oexUINT32			uAddr;

	};

	/// Address Family
	enum
	{
		/// Unspecified 
		eAfUnspec		= 0,

		/// Unix sockets
//		eAfUnix			= 1,

		// Internet IP sockets
		eAfInet			= 2
	};

	/// Socket Type
	enum
	{
		/// TCP / Stream
		eSockTcp		= 1,

		/// UDP / Datagram
		eSockUdp		= 2

	};

	/// Socket 2 values
	enum
	{
		eReadBit						= 0,
		eReadEvent						= ( 1 << eReadBit ),

		eWriteBit						= 1,
		eWriteEvent						= ( 1 << eWriteBit ),

		eOobBit							= 2,
		eOobEvent						= ( 1 << eOobBit ),

		eAcceptBit						= 3,
		eAcceptEvent						= ( 1 << eAcceptBit ),

		eConnectBit						= 4,
		eConnectEvent					= ( 1 << eConnectBit ),

		eCloseBit						= 5,
		eCloseEvent						= ( 1 << eCloseBit ),

		eQosBit							= 6,
		eQosEvent						= ( 1 << eQosBit ),

		eGroupQosBit					= 7,
		eGroupQosEvent					= ( 1 << eGroupQosBit ),

		eRoutingInterfaceChangeBit		= 8,
		eRoutingInterfaceChangeEvent	= ( 1 << eRoutingInterfaceChangeBit ),

		eAddressListChangeBit			= 9,
		eAddressListChangeEvent			= ( 1 << eAddressListChangeBit ),

		eNumEvents						= 10,
		eAllEvents						= ( ( 1 << eNumEvents ) - 1 )
	};

	/// Failure value
	static const oexSOCKET c_InvalidSocket;

	/// Invalid socket event value
	static const oexSOCKETEVENT c_InvalidEvent;


public:

	/// Default Constructor
	CIpSocket();

	/// Destructor
	virtual ~CIpSocket();

	//==============================================================
	// Destroy()
	//==============================================================
	/// Closes the socket and releases related resources
	void Destroy();

	//==============================================================
	// InitSockets()
	//==============================================================
	/// Initializes the Socket API
	/**
		\warning	You must call this function before any other 
					socket functions!  Best if called from your
					application startup code.  Call UninitSockets()
					before your application shuts down and after
					all instances of this class have been closed.

		\return Returns non-zero if success.
	*/
	static oexBOOL InitSockets();

	//==============================================================
	// UninitSockets()
	//==============================================================
	/// Uninitializes the Windows Socket API
	static void UninitSockets();	

	//==============================================================
	// IsInitialized()
	//==============================================================
	/// Returns non-zero if the Socket API was successfully initialized.
	static oexBOOL IsInitialized() { return ( m_lInit == 0 ); }

public:
	
	//==============================================================
	// Attach()
	//==============================================================
	/// Attaches to existing socket handle
	/**
		\param [in] x_hSocket		-	Existing socket handle

		\return Returns non-zero if success		
	*/
	oexBOOL Attach( oexSOCKET x_hSocket ) 
	{	Destroy(); 
		m_hSocket = x_hSocket; 
		return IsSocket(); 
	}

	//==============================================================
	// Detach()
	//==============================================================
	/// Detaches from existing socket handle without releasing it.
	void Detach() { m_hSocket = c_InvalidSocket; }
	
	//==============================================================
	// IsSocket()
	//==============================================================
	/// Returns non-zero if the class contains a valid socket handle
	oexBOOL IsSocket() { return ( m_hSocket != c_InvalidSocket ); }
					 
	//==============================================================
	// GetSocketHandle()
	//==============================================================
	/// Returns a handle to the socket
	oexSOCKET GetSocketHandle() { return m_hSocket; }

	//==============================================================
	// Create()
	//==============================================================
	/// Creates a new socket handle.
	/**
		\param [in] x_af		-	Address family specification.
		\param [in] x_type		-	The type specification.
		\param [in] x_protocol	-	The protocol to be used with the socket.

		\return Returns non-zero if success.

		\see Bind(), Listen(), Connect()
	*/
	virtual oexBOOL Create( oexINT x_af = eAfInet, oexINT x_type = eSockTcp, oexINT x_protocol = 0 );

	//==============================================================
	// GetLastError()
	//==============================================================
	/// Returns the most recent error code
	oexUINT GetLastError() { return m_uLastError; }

public:

	//==============================================================
	// Bind()
	//==============================================================
	/// Binds the open socket to the specified Port
	/**
		\param [in] x_uPort	-	Port to bind to.

		\return Returns non-zero if success.

		\see Create(), Listen(), Connect()
	*/
	oexBOOL Bind( oexUINT x_uPort );

	//==============================================================
	// Listen()
	//==============================================================
	/// Creates a socket listening on the bound port.
	/**
		\param [in] uMaxConnections	-	The maximum number of connections allowed.

		\return Returns non-zero if success, otherwise zero.

		\see Create(), Bind(), Connect()
	*/
	oexBOOL Listen( oexUINT x_uMaxConnections = 32 );

	//==============================================================
	// Connect()
	//==============================================================
	/// Address of remote peer.
	/**
		\param [in] x_pAddress	-	URL formed address of remote peer.
		\param [in] x_uPort		-	Port of remote peer.

		\return Returns non-zero if success.
	*/
	oexBOOL Connect( oexCSTR x_pAddress, oexUINT x_uPort );

public:

	//==============================================================
	// EventSelect()
	//==============================================================
	/// Selects which events will generate callbacks
	/**
		\param [in] lEvents	-	The events to hook.

		\return Returns non-zero if success.
	*/
	oexBOOL EventSelect( oexLONG lEvents =	eReadEvent | eWriteEvent 
											| eAcceptEvent | eConnectEvent 
											| eCloseEvent );

public:

	//==============================================================
	// Recv()
	//==============================================================
	/// Reads data from the socket
	/**
		\param [in] x_pData		-	Receives the socket data
		\param [in] x_uSize		-	Size of buffer in pData
		\param [in] x_puRead	-	Receives the number of bytes read
		\param [in] x_uFlags	-	Socket receive flags
		
		\return Number of bytes read or c_InvalidSocket if failure.
	
		\see 
	*/
	oexUINT Recv( oexPVOID x_pData, oexUINT x_uSize, oexUINT *x_puRead = NULL, oexUINT x_uFlags = 0 );

	//==============================================================
	// Send()
	//==============================================================
	/// Writes data to the socket
	/**
		\param [in] x_pData		-	Buffer containing write data
		\param [in] x_uSize		-	Size of the buffer in pData
		\param [in] x_puSent	-	Receives the number of bytes written
		\param [in] x_uFlags	-	Socket write flags
		
		\return Number of bytes sent or c_InvalidSocket if failure.
	
		\see 
	*/
	oexUINT Send( const oexPVOID x_pData, oexUINT x_uSize, oexUINT *x_puSent = NULL, oexUINT x_uFlags = 0 );

	//==============================================================
	// Send()
	//==============================================================
	/// Writes a NULL terminated string to the socket
	/**
		\param [in] x_pStr		-	Pointer to NULL terminated string
		\param [in] x_puSent	-	Number of bytes sent
		\param [in] x_uFlags	-	Socket write flags
		
		\return Number of bytes sent or c_InvalidSocket if failure.
	
		\see 
	*/
	oexUINT Send( oexCSTR x_pStr, oexUINT *x_puSent = NULL, oexUINT x_uFlags = 0 )
	{	return Send( (oexPVOID)x_pStr, strlen( x_pStr ), x_puSent, x_uFlags ); }

public:

	//==============================================================
	// GetPeerName()
	//==============================================================
	/// Gets the remote socket information
	/**
		\param [out] x_pName	-	Receives the remote address of the connected socket.
		\param [out] x_puPort	-	Receives the remote TCP port of the connected socket.

		\return Returns non-zero if success.
	*/
	oexBOOL GetPeerName( oexSTR x_pName, oexUINT *x_puPort = NULL ) { return 0; }

	//==============================================================
	// GetSocketName()
	//==============================================================
	/// Gets the local socket information
	/**
		\param [out] x_pName	-	Receives the local address of the connected socket.
		\param [out] x_pdwPort	-	Receives the local TCP port of the connected socket.

		\return Returns non-zero if success.
	*/
	oexBOOL GetSocketName( oexSTR x_pName, oexUINT *x_puPort = NULL ) { return 0; }


private:

	/// Socket API initialization return code
	static oexLONG			m_lInit;

	/// Socket handle
	oexSOCKET				m_hSocket;

	/// Last error code
	oexUINT					m_uLastError;

	/// Address
	oexString				m_sAddress;

	/// Dot address
	oexString				m_sDotAddress;

	/// Port
	oexUINT					m_uPort;

	/// Socket event handle
	oexSOCKETEVENT			m_hSocketEvent;
};

