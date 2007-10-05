/*------------------------------------------------------------------
// net_port.h
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


/// Basic port class
template < typename T_PORT, typename T_BUFFER = CCircBuf >
    class TBufferedPort : public T_PORT
{
public:

    /// Default constructor
    TBufferedPort()
    {
        m_bTxBlocking = oexFALSE;
    }

    /// Destructor
    virtual ~TBufferedPort()
    {

    }   

    // When data is read
	virtual oexBOOL OnRead( oexINT x_nErr )
    {
        // Process the incomming data
        if ( !T_PORT::OnRead( x_nErr ) )
	        return oexFALSE;

        // Buffer the data
        return Rx().Write( Recv() );
    }

    // When data is written
    virtual oexBOOL OnWrite( oexINT x_nErr )
    {
	    // Lock the transmit buffer
	    CTlLocalLock ll( Tx() );
	    if ( !ll.IsLocked() ) 
		    return oexFALSE;

	    // Not blocking now
	    m_bTxBlocking = oexFALSE;

	    // Send more data
	    OnTx( 0 );

	    return oexTRUE;
    }

    // When data is written
    virtual oexBOOL OnTx( oexINT x_nErr )
    {
	    // Lock the transmit buffer
	    CTlLocalLock ll( Tx() );
	    if ( !ll.IsLocked() ) 
		    return oexFALSE;

	    // Punt if we're blocking
	    if ( m_bTxBlocking ) 
            return oexTRUE;

	    oexUCHAR buf[ 1024 ];
	    oexUINT uReady = 0;

	    // Read blocks of data
	    while ( Tx().Peek( buf, sizeof( buf ), &uReady ) )
	    {
            // Send data
            oexINT res = Send( buf, uReady );

            // Returns zero if tx'er is full
            if ( !res )
            {
                m_bTxBlocking = oexTRUE;

                return oexFALSE;

            } // end if

		    // Remove the number of bytes sent from the buffer
		    else 
                Tx().AdvanceReadPtr( res );

	    } // end while

        return oexTRUE;
    }

public:

	//==============================================================
	// Write()
	//==============================================================
	/// Writes data to buffer
	/**
		\param [in] x_pData		-	Buffer containing write data
		\param [in] x_uSize		-	Size of the buffer in pData
		
		\return Non-zero if data was written to buffer
	
		\see 
	*/
	oexBOOL Write( oexCONST oexPVOID x_pData, oexUINT x_uSize )
    {
        // Buffer the data
        if ( !m_tx.Write( x_pData, x_uSize ) )
            return oexFALSE;

        // Start the tx'er
        OnTx( 0 );

        return oexTRUE;
    }

	//==============================================================
	// Write()
	//==============================================================
	/// Writes a NULL terminated string to buffer
	/**
		\param [in] x_pStr		-	Pointer to NULL terminated string
		
		\return Non-zero if data was written to buffer
	
		\see 
	*/
	oexBOOL Write( oexCSTR8 x_pStr )
    {	return Write( (oexPVOID)x_pStr, zstr::Length( x_pStr ) ); }


	//==============================================================
	// Write()
	//==============================================================
	/// Writes a string to buffer
	/**
		\param [in] x_sStr		-	String to be sent
		
		\return Non-zero if data was written to buffer
	
		\see 
	*/
	oexBOOL Write( CStr8 &x_sStr )
    {	return Write( (oexPVOID)x_sStr.Ptr(), x_sStr.Length() ); }

public:

    /// Returns a reference to the rx buffer
    T_BUFFER& Rx() 
    {   return m_rx; }

    /// Returns a reference to the tx buffer
    T_BUFFER& Tx() 
    {   return m_tx; }    

private:

    /// Non-zero if tx'er is blocking
    oexBOOL                     m_bTxBlocking;

    /// Rx buffer
    T_BUFFER                    m_rx;

    /// Tx buffer
    T_BUFFER                    m_tx;

};

