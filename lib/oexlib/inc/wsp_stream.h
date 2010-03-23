/*------------------------------------------------------------------
// wsp_stream.h
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
// CWspStream
//
/// Implements a raw read write protocol for TAutoSocket
/**
	Implements a raw read write protocol for TAutoSocket	

  \code

	// Create stream socket
	TAutoSocket< CWspStream > wasStream;

	wasStream.Connect( "google.com", 80 );

    os::CSys::Sleep( 8000 );

	wasStream.Protocol()->Write( "GET / HTTP/1.0\r\n\r\n" );

	os::CSys::Sleep( 8000 );

    oexTRACE( "%s\n", wasStream.Protocol()->Read().Ptr() );

  \endcode

*/
//==================================================================
class CWspStream : public CSocketProtocol
{
public:

	/// Maximum amount of data transferred to the tx'er and rx'er
	enum { eMaxTransferBlockSize = 1024 };

public:	

	/// Default constructor
	CWspStream();

	/// Destructor
	virtual ~CWspStream();

    /// Register async callbacks
    virtual void RegisterCallbacks()
    {   
        GetAutoSocket()->OexRpcRegister( CWspStream, AsyncRead );
        GetAutoSocket()->OexRpcRegister( CWspStream, AsyncWrite );
        GetAutoSocket()->OexRpcRegister( CWspStream, WaitData );
    }

    //==============================================================
	// Write()
	//==============================================================
	/// Writes a string to the socket
	/**
		\param [in] x_pStr	-	String to write
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL Write( oexCSTR8 x_pStr )
	{	return m_tx.Write( oexStrToBin( x_pStr ) ); 
    }
	
	//==============================================================
	// Read()
	//==============================================================
	/// Reads data from the socket
	/**
		\param [in] x_pData	-	Buffer that receives the data
		\param [in] x_uSize	-	Size of buffer in pData
		\param [out] puRead	-	Number of bytes read
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL Read( oexPVOID x_pData, oexUINT x_uSize, oexUINT *x_puRead = oexNULL )
	{	return m_rx.Read( x_pData, x_uSize, x_puRead ); }
	
    CStr8 Read( oexUINT x_uSize = 0 )
    {   return m_rx.Read( x_uSize ); }

    CStr8 AsyncRead( oexUINT x_uSize )
    {   return m_rx.Read( x_uSize ); }

    oexBOOL WaitData( oexUINT x_uTimeout )
    {
//        if ( GetAutoSocket() )
//            GetAutoSocket()->SetReturnWaitHandle( m_rx.GetDataReadyHandle() );

        return oexTRUE;
    }

	//==============================================================
	// Write()
	//==============================================================
	/// Writes data to the socket
	/**
		\param [in] x_pData	-	Data to write
		\param [in] x_uSize	-	Number of bytes in pData
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL Write( oexCPVOID x_pData, oexUINT x_uSize )
	{	return m_tx.Write( x_pData, x_uSize ); }

    //==============================================================
	// Write()
	//==============================================================
	/// Writes a string to the socket
	/**
		\param [in] x_sStr	-	String to write
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL Write( CStr8 x_sStr )
	{	return m_tx.Write( x_sStr.Ptr(), x_sStr.Length() ); }
	
    oexBOOL AsyncWrite( CStr8 x_sStr )
    {   return m_tx.Write( x_sStr.Ptr(), x_sStr.Length() ); }


	//==============================================================
	// OnWrite()
	//==============================================================
	/// Called when there is data to be transmitted
	/**
		\param [in] x_nErr	-	Error code
		
		\return Non-zero if success
	
		\see 
	*/
	virtual oexBOOL OnWrite( oexINT x_nErr );	

	//==============================================================
	// OnRead()
	//==============================================================
	/// Called when new data arrives
	/**
		\param [in] x_nErr	-	Error code
		
		\return Non-zero if success
	
		\see 
	*/
	virtual oexBOOL OnRead( oexINT x_nErr );

	//==============================================================
	// OnTx()
	//==============================================================
	/// Call when tx'er is empty and new transmit data is available
	/**
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL OnTx();

	//==============================================================
	// GetDataReadyHandle()
	//==============================================================
	/// Return a valid handle if you want to receive data tx messages
	/**
		
		\return Waitable event handle
	
		\see 
	*/
    virtual os::CSys::t_WAITABLE GetDataReadyHandle()
	{	return m_tx.GetDataReadyHandle(); }

	//==============================================================
	// OnDataReady()
	//==============================================================
	/// Called when the event handle returned by GetDataReadyHandle() is set
	/**
		
		\return Non-zero if success
	
		\see 
	*/
	virtual oexBOOL OnDataReady() 
	{	return OnTx(); }

	//==============================================================
	// Rx()
	//==============================================================
	/// Returns a pointer to the receiver messenger
	CCircBuf& Rx() { return m_rx; }

	//==============================================================
	// Tx()
	//==============================================================
	/// Returns a pointer to the transmitter messenger
	CCircBuf& Tx() { return m_tx; }

	//==============================================================
	// DelayClose()
	//==============================================================
    /// Called in response to processing a close command
    virtual oexUINT DelayClose()
    {   return m_tx.WaitData( 0 ) ? 100 : 0; }

private:

	/// Transmit buffer queue
	CCircBuf					m_tx;

	/// Receive buffer queue
	CCircBuf					m_rx;

	/// Non-zero if the tx'er is busy
	oexBOOL						m_bTxBusy;

	/// Non-zero if the rx'er is busy
	oexBOOL						m_bRxBusy;

};


//==================================================================
// CWspEcho
//
/// This class will echo whatever it receives
/**
	Implements a raw read write protocol for TAutoSocket	

  \code


  \endcode

*/
//==================================================================
class CWspEcho : public CWspStream
{
public:

	//==============================================================
	// OnRead()
	//==============================================================
	/// Called when new data arrives
	/**
		\param [in] x_nErr	-	Error code
		
		\return Non-zero if success
	
		\see 
	*/
    virtual oexBOOL OnRead( oexINT x_nErr )
    {
	    // Process the incomming data
	    if ( !CWspStream::OnRead( x_nErr ) )
		    return oexFALSE;

        // Just write the data back out
        Write( Rx().Read() );

	    return oexTRUE;
    }
};
