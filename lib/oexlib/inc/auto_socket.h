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
class CAutoSocket : public os::CIpSocket
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

public:

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
	// ProcessEvents()
	//==============================================================
	/// Processes any socket events
    oexBOOL ProcessEvents()
    {   return ( oexRES_OK == OnSocketEvent() );
    }

	//==============================================================
	// RegisterFunctions()
	//==============================================================
	/// Registers public functions
    void RegisterFunctions( CDispatch *x_pDispatch )
    {
        // Sanity check
        if ( !x_pDispatch )
            return;

        // Register user callable functions
        x_pDispatch->OexRpcRegister( os::CIpSocket, Connect );
        x_pDispatch->OexRpcRegister( os::CIpSocket, Shutdown );
        x_pDispatch->OexRpcRegister( os::CIpSocket, Bind );
        x_pDispatch->OexRpcRegister( os::CIpSocket, Listen );
    }

private:

	/// Connection status
	oexUINT						m_uStatus;

};


