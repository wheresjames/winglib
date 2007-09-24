/*------------------------------------------------------------------
// auto_socket.h
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

#pragma once

//==================================================================
// CAutoSocket
//
/// Provides asynchronous socket communications.
/**

*/
//==================================================================
class CAutoSocket
{
public:

	enum
	{
		/// Socket is not connected
		eStatusDisconnected = 0,

		/// Socket is connecting
		eStatusConnecting,

		/// Socket is connected
		eStatusConnected
	};

public:

	/// Default constructor
	CAutoSocket();

	/// Destructor
	virtual ~CAutoSocket();

    /// Releases resources
    virtual void Destroy();


	//==============================================================
	// Connect()
	//==============================================================
	/// Connects the socket to the specified address
	/**
		\param [in] x_pAddress	    -	Remote address
		\param [in] x_uPort 		-	Connection port
		
		\return Non-zero if command was queued
	
		\see Shutdown()
	*/
    oexBOOL Connect( oexCSTR x_pAddress, oexUINT x_uPort );

	//==============================================================
	// Shutdown()
	//==============================================================
    /// Shuts down the socket
	/**	
		\return Non-zero if command was queued
	
		\see Connect()
	*/
    oexBOOL Shutdown();

	//==============================================================
	// Bind()
	//==============================================================
	/// Binds the socket to the specified port
	/**		
        \param [in] x_uPort -   The port number to bind to.

		\return Non-zero if command was queued
	*/
    oexBOOL Bind( oexUINT x_uPort );

	//==============================================================
	// Listen()
	//==============================================================
    /// Listens for connections ( starts a server on the bound port )
	/**		
        \param [in] x_uMax -   Maximum number of connections to
                                accept.

		\return Non-zero if command was queued
	*/
    oexBOOL Listen( oexUINT x_uMax = 0 );


	//==============================================================
	// Accept()
	//==============================================================
	/// Accepts an incomming connection on the specified socket
    /*
        \param [in] x_is    -   Socket that accepts the incomming
                                connection.
    */
    oexBOOL Accept( os::CIpSocket &x_is )
    {   return m_is.Accept( x_is ); }


protected:

	//==============================================================
	// OnSocketEvent()
	//==============================================================
	/// Processes socket events
	/**
		
		\return Non-zero if success
	
		\see 
	*/
	oexINT OnSocketEvent();


	//==============================================================
	// GetSocket()
	//==============================================================
	/// Returns a pointer to the encapsulated CWinSocket class
	os::CIpSocket* GetSocket() { return &m_is; }


	//==============================================================
	// OnRead()
	//==============================================================
	/// Called when data has been received from connected socket
	/**
		\param [in] nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnRead( oexINT x_nErr ) { return oexFALSE; }

	//==============================================================
	// OnWrite()
	//==============================================================
	/// Called when previously full transmit buffer is now empty.
	/**
		\param [in] nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnWrite( oexINT x_nErr ) { return oexFALSE; }
	
	//==============================================================
	// OnOOB()
	//==============================================================
	/// Called when Out-Of-Band data has been received from connected socket
	/**
		\param [in] nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnOOB( oexINT x_nErr ) { return oexFALSE; }
	
	//==============================================================
	// OnAccept()
	//==============================================================
	/// Called when an incomming connection request is recieved.
	/**
		\param [in] nErr	-	Zero if no error, otherwise socket error value.

		You should call Accept in response to this callback.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnAccept( oexINT x_nErr ) { return oexFALSE; }
	
	//==============================================================
	// OnConnect()
	//==============================================================
	/// Called when the socket has connection has completed, or failed.
	/**
		\param [in] nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnConnect( oexINT x_nErr ) { return oexFALSE; }
	
	//==============================================================
	// OnClose()
	//==============================================================
	/// Called when socket connection has been closed or aborted.
	/**
		\param [in] nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnClose( oexINT x_nErr ) { return oexFALSE; }
	
	//==============================================================
	// OnQOS()
	//==============================================================
	/// Called when Quality-Of-Service parameters has changed
	/**
		\param [in] nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnQOS( oexINT x_nErr ) { return oexFALSE; }
	
	//==============================================================
	// OnGroupQOS()
	//==============================================================
	/// Called when group Quality-Of-Service parameters has changed
	/**
		\param [in] nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnGroupQOS( oexINT x_nErr ) { return oexFALSE; }
	
	//==============================================================
	// OnRoutingInterfaceChange()
	//==============================================================
	/// Called when the routing interface parameters have changed.
	/**
		\param [in] nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnRoutingInterfaceChange( oexINT x_nErr ) { return oexFALSE; }
	
	//==============================================================
	// OnAddressListChange()
	//==============================================================
	/// Called when the multi-cast address list has changed
	/**
		\param [in] nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnAddressListChange( oexINT x_nErr ) { return oexFALSE; }
	
	//==============================================================
	// OnError()
	//==============================================================
	/// Called on socket error.
	/**
		\param [in] wEvent	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnError( oexUSHORT uwEvent ) { return oexFALSE; }

	//==============================================================
	// GetDataReadyHandle()
	//==============================================================
	/// Return a valid handle if you want to receive data tx messages
	/**
		
		\return Waitable event handle
	
		\see 
	*/
    virtual os::CSys::t_WAITABLE GetDataReadyHandle() { return oexNULL; }

	//==============================================================
	// OnDataReady()
	//==============================================================
	/// Called when the event handle returned by GetDataReadyHandle() is set
	/**
		
		\return Non-zero if success
	
		\see 
	*/
	virtual oexBOOL OnDataReady() { return oexFALSE; }

	//==============================================================
	// GetStatus()
	//==============================================================
	/// Gets the socket connection status
	oexUINT GetStatus() { return m_uStatus; }

	//==============================================================
	// IsConnected()
	//==============================================================
	/// Returns non-zero if the socket is connected
	oexBOOL IsConnected() { return eStatusConnected == m_uStatus; }

	//==============================================================
	// IsConnecting()
	//==============================================================
	/// Returns non-zero if the socket is connecting
	oexBOOL IsConnecting() { return eStatusConnecting == m_uStatus; }

public:

	//==============================================================
	// GetEventHandle()
	//==============================================================
	/// Retuns the current event handle
    os::CIpSocket::t_SOCKETEVENT GetEventHandle() 
    { return m_is.GetEventHandle(); }

	//==============================================================
	// GetIpSocket()
	//==============================================================
    /// Returns the CIpSocket class
    os::CIpSocket& GetIpSocket()
    {   return m_is; }

	//==============================================================
	// operator os::CIpSocket&
	//==============================================================
    /// CIpSocket reference operator
    operator os::CIpSocket&() 
    {   return m_is; }


private:

	/// Connection status
	oexUINT						m_uStatus;

	/// Windows socket class
	os::CIpSocket			    m_is;

};


// +++ Change the name here to just CProtocol, we will use it as
//     a base for serial port communication as well.

//==================================================================
// CSocketProtocol
//
/// Base class for CAutoSocket protocol handlers.
/**
	Derive from this class to implement a protocol handler for 
	TAutoSocket.

	Supports both TCP and UDP protocols.

	\see TAutoSocket, CAutoSocket, CIpSocket
*/
//==================================================================
class CSocketProtocol
{
public:

	/// Default constructor
	CSocketProtocol() { m_pAutoSocket = oexNULL; m_pSocket = oexNULL; }

	/// Destructor
	virtual ~CSocketProtocol() { m_pAutoSocket = oexNULL; m_pSocket = oexNULL; }

	//==============================================================
	// OnWrite()
	//==============================================================
	/// Called when there is data to be transmitted
	/**
		\param [in] x_nErr	-	Error code
		
		\return Non-zero if success
	
		\see 
	*/
	virtual oexBOOL OnWrite( oexINT x_nErr ) = 0;	

	//==============================================================
	// OnRead()
	//==============================================================
	/// Called when new data arrives
	/**
		\param [in] x_nErr	-	Error code
		
		\return Non-zero if success
	
		\see 
	*/
	virtual oexBOOL OnRead( oexINT x_nErr ) = 0;

	//==============================================================
	// OnConnect()
	//==============================================================
	/// Called when a connection is established
	/**
		\param [in] x_nErr	-	Error code
		
		\return Non-zero if success
	
		\see 
	*/
    virtual oexBOOL OnConnect( oexINT x_nErr ) { return oexFALSE; }

	//==============================================================
	// OnClose()
	//==============================================================
	/// Called when a connection is terminated
	/**
		\param [in] x_nErr	-	Error code
		
		\return Non-zero if success
	
		\see 
	*/
    virtual oexBOOL OnClose( oexINT x_nErr ) { return oexFALSE; }

	//==============================================================
	// DelayClose()
	//==============================================================
    /// Called in response to processing a close command
	/**
        You can over-ride this and return the number of milliseconds
        you want to wait before closing the connection.  This is
        usefull if your pending some command or waiting for data
        to go out.

		\return Return non-zero if handled
	*/
    virtual oexUINT DelayClose() { return 0; }

	//==============================================================
	// GetDataReadyHandle()
	//==============================================================
	/// Return a valid handle if you want to receive data tx messages
	/**
		\return Waitable event handle
	
		\see 
	*/
	virtual os::CSys::t_WAITABLE GetDataReadyHandle() = 0;

	//==============================================================
	// OnDataReady()
	//==============================================================
	/// Called when the event handle returned by GetDataReadyHandle() is set
	/**
		\return Non-zero if success
	
		\see 
	*/
	virtual oexBOOL OnDataReady() = 0;

	//==============================================================
	// GetSocket()
	//==============================================================
	/// Returns a pointer the attached socket
	os::CIpSocket* GetSocket() { return m_pSocket; }

	//==============================================================
	// SetSocket()
	//==============================================================
	/// Sets the attached socket
	/**
		\param [in] x_pSocket		-	Pointer to CWinSocket object
	*/
	void SetSocket( os::CIpSocket *x_pSocket ) { m_pSocket = x_pSocket; }

	//==============================================================
	// GetAutoSocket()
	//==============================================================
	/// Returns a pointer the attached socket
	CAutoSocket* GetAutoSocket() { return m_pAutoSocket; }

	//==============================================================
	// SetAutoSocket()
	//==============================================================
	/// Sets the attached socket
	/**
		\param [in] x_pSocket		-	Pointer to CAutoSocket object
	*/
	void SetAutoSocket( CAutoSocket *x_pSocket ) 
    {   m_pAutoSocket = x_pSocket; 
        RegisterCallbacks();
    }

    // For registering named event callbacks
//    oexBOOL RegisterEventCallback( CStr sName, CProxyController::t_CallbackDelegate fn, oexINT x_nTtl )
//    {   if ( !m_pAutoSocket ) return oexFALSE;
//        return m_pAutoSocket->RegisterEventCallback( sName, fn, x_nTtl ); 
//    }

    // Registering event handle callbacks
//    oexBOOL RegisterEventHandleCallback( os::CSys::t_WAITABLE hEvent, CProxyController::t_CallbackDelegate fn, oexINT x_nTtl )
//    {   if ( !m_pAutoSocket ) return oexFALSE;
//        return m_pAutoSocket->RegisterEventHandleCallback( hEvent, fn, x_nTtl ); 
//    }

    virtual void RegisterCallbacks() {}

private:

	/// Pointer to the attached socket
	os::CIpSocket			*m_pSocket;

	/// Pointer to the attached socket
	CAutoSocket		        *m_pAutoSocket;

};


//==================================================================
// TAutoSocket
//
/// Provides asynchronous socket communications.
/**
	This class derives from CAutoSocket and provides an interface
	to CSocketProtocol derived classes.  See CIpSocket for
	synchronous socket.

	Example:

  \code

	// Create stream socket
	TAutoSocket< CWspStream > wasStream;

	wasStream.Connect( "google.com", 80 );

	Sleep( 1000 );

	wasStream.GetProtocol()->Write( "GET / HTTP/1.0\r\n\r\n" );

	Sleep( 1000 );

	UINT uRead = 0;
	char szBuf[ 8 * 1024 ];
	wasStream.GetProtocol()->Read( szBuf, sizeof( szBuf ), &uRead );

	printf( szBuf );

	\see CAutoSocket, CSocketProtocol, CIpSocket

  \endcode

*/
//==================================================================
template < class T > class TAutoSocket : public CAutoSocket
{
public:

	/// Default constructor
	TAutoSocket() 
	{	m_apWsp->SetSocket( GetSocket() ); 
		m_apWsp->SetAutoSocket( this ); 
	}

	/// Destructor
	virtual ~TAutoSocket() { }

	//==============================================================
	// Protocol()
	//==============================================================
	/// Returns a pointer to the protocol interface
	T* Protocol() { return m_apWsp; }

	//==============================================================
	// GetDataReadyHandle()
	//==============================================================
	/// Return a valid handle if you want to receive data tx messages
	/**
		
		\return Waitable event handle
	
		\see 
	*/
	virtual os::CSys::t_WAITABLE GetDataReadyHandle() 
	{	return m_apWsp->GetDataReadyHandle();
	}

	//==============================================================
	// OnDataReady()
	//==============================================================
	/// Called when the event handle returned by GetDataReadyHandle() is set
	/**
		
		\return Non-zero if success
	
		\see 
	*/
	virtual oexBOOL OnDataReady() 
	{	return m_apWsp->OnDataReady();
	}

	//==============================================================
	// OnRead()
	//==============================================================
	/// Called when data has been received from connected socket
	/**
		\param [in] x_nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnRead( oexINT x_nErr )
	{	return m_apWsp->OnRead( x_nErr );
	}

	//==============================================================
	// OnWrite()
	//==============================================================
	/// Called when previously full transmit buffer is now empty.
	/**
		\param [in] x_nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnWrite( oexINT x_nErr )
	{	return m_apWsp->OnWrite( x_nErr );
	}

	//==============================================================
	// OnConnect()
	//==============================================================
	/// Called when previously full transmit buffer is now empty.
	/**
		\param [in] x_nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnConnect( oexINT x_nErr )
	{	return m_apWsp->OnConnect( x_nErr );
	}

	//==============================================================
	// OnClose()
	//==============================================================
	/// Called when previously full transmit buffer is now empty.
	/**
		\param [in] x_nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnClose( oexINT x_nErr )
	{	return m_apWsp->OnClose( x_nErr ); }

	//==============================================================
	// DelayClose()
	//==============================================================
    /// Called in response to processing a close command
	/**
        You can over-ride this and return the number of milliseconds
        you want to wait before closing the connection.  This is
        usefull if your pending some command or waiting for data
        to go out.

		\return Return non-zero if handled
	*/
    virtual oexUINT DelayClose()
	{	return m_apWsp->DelayClose(); }

private:

	/// Pointer to attached protocol object
	TMem< T >		m_apWsp;
};

