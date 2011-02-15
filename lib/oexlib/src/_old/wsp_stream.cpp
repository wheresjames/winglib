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

#include "oexlib.h"

OEX_USING_NAMESPACE


CWspStream::CWspStream()
{
	m_bTxBusy = oexFALSE;
	m_bRxBusy = oexFALSE;
}

CWspStream::~CWspStream()
{

}

oexBOOL CWspStream::OnWrite( oexINT x_nErr )
{
	// Lock the transmit buffer
	CTlLocalLock ll( *Tx() );
	if ( !ll.IsLocked() ) 
        return oexFALSE;		

	// Not blocking now
	m_bTxBusy = oexFALSE;

	// Send more data
	return OnTx();
}

oexBOOL CWspStream::OnRead( int nErr )
{
	// For some reason, active-x controls seem to re-enter here...
	if ( m_bRxBusy ) 
        return oexFALSE;
	m_bRxBusy = oexTRUE;

	// Receive the bytes
	oexUCHAR buf[ eMaxTransferBlockSize ];

	oexUINT uBytes;
	while ( ( uBytes = GetSocket()->Recv( buf, sizeof( buf ) ) ) > 0 && sizeof( buf ) >= uBytes )

		// Write into the rx'er
		m_rx.Write( buf, uBytes );

	// Not busy
	m_bRxBusy = oexFALSE;

	return oexTRUE;
}

oexBOOL CWspStream::OnTx()
{
	// Lock the transmit buffer
	CTlLocalLock ll( m_tx );
	if ( !ll.IsLocked() ) 
		return oexFALSE;

	// Punt if tx'er is full
	if ( m_bTxBusy ) 
        return oexTRUE;

	oexUINT uReady;
	oexUCHAR buf[ eMaxTransferBlockSize ];

	// Read blocks of data
	while ( m_tx.Peek( buf, sizeof( buf ), &uReady ) )
	{
		// Send the data on
		oexINT res = GetSocket()->Send( buf, uReady );

		// Check for socket error
		if ( res == 0 || (oexUINT)res > uReady ) 
		{
            // Tx'er is busy
			m_bTxBusy = oexTRUE;

			return oexTRUE;

		} // end if

		// Remove the number of bytes sent from the buffer
		m_tx.AdvanceReadPtr( res );

		// Punt if tx'er is full
		if ( (oexUINT)res != uReady )
		{	m_bTxBusy = oexTRUE;
			return oexTRUE;
		} // end if

	} // end while

	return oexTRUE;
}
