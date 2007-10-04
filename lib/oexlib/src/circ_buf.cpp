/*------------------------------------------------------------------
// circ_buf.cpp
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

#include "../oexlib.h"

OEX_USING_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCircBuf::CCircBuf( oexBOOL x_bSync, oexUINT x_uSize, oexBOOL x_bAutoGrow )
{
	m_bEmpty = oexTRUE;
	m_uSize = 0;
    m_pBuf = oexNULL;

	m_bAutoGrow = x_bAutoGrow;
	m_uMaxSize = eDefaultMaxBufferSize;

    m_pBi = oexNULL;

    m_uPokePtr = 0;

	if ( x_bSync ) 
    {   m_hDataReady = os::CEvent::osCreateEvent( oexNULL, oexTRUE );
        m_hEmpty = os::CEvent::osCreateEvent( oexNULL, oexTRUE );
    } // end if
	else 
    {   m_hDataReady = oexNULL;
        m_hEmpty = oexNULL;
    } // end else

	if ( x_uSize ) 
        Allocate( x_uSize );
}


CCircBuf::~CCircBuf()
{
	Destroy();

	// Lose the sync
	if ( m_hDataReady )
	{	os::CMutex::osReleaseMutex( m_hDataReady );
		m_hDataReady = oexNULL;
	} // end if
}

oexBOOL CCircBuf::NameEvents( oexCSTR x_pBaseName )
{
    // Unnamed?
    if ( !x_pBaseName || !*x_pBaseName )
    {
        m_lock.Create();
    	if ( m_hDataReady )
	        os::CMutex::osReleaseMutex( m_hDataReady );
        m_hDataReady = os::CEvent::osCreateEvent( oexNULL, oexTRUE );

    } // end else

    else
    {
        m_lock.Create( CStr( x_pBaseName ).Append( oexT( "_lock" ) ).Ptr() );

    	if ( m_hDataReady )
	        os::CMutex::osReleaseMutex( m_hDataReady );
        m_hDataReady = os::CEvent::osCreateEvent( CStr( x_pBaseName ).Append( oexT( "_data" ) ).Ptr(), oexTRUE );

    } // end else

    return oexTRUE;
}

oexBOOL CCircBuf::Allocate( oexUINT x_uSize )
{
	// Lock the buffer
	CTlLocalLock ll( &m_lock );
	if ( !ll.IsLocked() ) 
        return oexFALSE;

	// Lose the old buffer
	Destroy(); 
	
	// Create a new buffer
	if ( !m_memBuffer.OexNew( x_uSize + sizeof( SBufferInfo ), oexFALSE, oexTRUE ).Ptr() ) 
		return oexFALSE; 
	
    // Get the buffer info pointer
    m_pBi = (SBufferInfo*)m_memBuffer.Ptr();

    // The actual buffer
    m_pBuf = m_memBuffer.Ptr( sizeof( SBufferInfo ) );

    // Initialize the buffer information
    m_pBi->uReadPtr = 0;
    m_pBi->uWritePtr = 0;

	// Remember the size of the new buffer
	m_uSize = m_memBuffer.Size() - sizeof( SBufferInfo ); 
	
	return oexTRUE; 
}

oexUINT CCircBuf::Resize(oexUINT x_uNewSize)
{
	// Lock the buffer
	CTlLocalLock ll( &m_lock );
	if ( !ll.IsLocked() ) 
        return oexFALSE;
	
    // Ensure we actually have a buffer
    if ( !m_pBi )
        return oexFALSE;

    // Can't resize a memory mapping
    if ( m_memBuffer.IsMapped() )
        return oexFALSE;

	// Ensure we don't go over the maximum
	if ( x_uNewSize > m_uMaxSize ) 
        return oexFALSE;

	// Punt if we're already there
	if ( x_uNewSize == m_uSize ) 
        return oexFALSE;

	// Create new buffer
	TMem< oexUCHAR > temp;
	if ( !temp.OexNew( x_uNewSize, oexFALSE, oexTRUE ).Ptr() ) 
		return oexFALSE;

    // How much memory did we actually get
    x_uNewSize = temp.Size() - sizeof( SBufferInfo );

	oexUINT uWritten = GetMaxRead( m_pBi->uReadPtr, m_pBi->uWritePtr, m_uSize );
	oexUINT uPoked = GetMaxRead( m_pBi->uReadPtr, m_uPokePtr, m_uSize );
    oexUINT uUsed = cmn::Max( uPoked, uWritten );

	// How many will we copy over?
	if ( uUsed > m_uSize ) 
        uUsed = m_uSize;
	if ( uUsed > x_uNewSize ) 
        uUsed = x_uNewSize;

	if ( uUsed )
	{
		oexUCHAR *pView;
		oexUINT i = 0, uRead = 0, uView;

		// Read all the blocks into the buffer
		while ( GetView( i++, m_pBi->uReadPtr, uUsed, m_pBuf, m_uSize, &pView, &uView ) )
            os::CSys::MemCpy( &( (oexUCHAR*)temp.Ptr( sizeof( SBufferInfo ) ) )[ uRead ], pView, uView ), uRead += uView;

	} // end if

    // Copy the buffer info data
    os::CSys::MemCpy( temp.Ptr(), m_memBuffer.Ptr(), sizeof( SBufferInfo ) );

	// Swap over the buffer pointer
	m_memBuffer.Assume( temp );

    // Get the buffer info pointer
    m_pBi = (SBufferInfo*)m_memBuffer.Ptr();

    // The actual buffer
    m_pBuf = m_memBuffer.Ptr( sizeof( SBufferInfo ) );

	// Save the new buffer size
	m_uSize = x_uNewSize;

	// Adjust write pointers
    m_pBi->uReadPtr = 0;
	m_pBi->uWritePtr = uWritten;
	m_uPokePtr = uPoked;

	// Return the new buffer size
	return x_uNewSize;		
}

oexUINT CCircBuf::EnsureWriteSpace( oexUINT x_uSize, oexUINT x_uReadPtr, oexUINT x_uWritePtr, oexUINT x_uMax )
{
	// We need one more byte as a marker
	x_uSize++;

	// Do we have enough space?
	oexUINT uAvailable = GetMaxWrite( x_uReadPtr, x_uWritePtr, x_uMax );
	if ( uAvailable >= x_uSize ) 
        return oexTRUE;

	// Double the amount of space
	uAvailable = 2; 
	while ( uAvailable && uAvailable < ( m_uSize + x_uSize ) ) 
        uAvailable <<= 1;

	// Do we have a valid size?
	if ( !uAvailable ) 
        return oexFALSE;

	// Create a buffer large enough for the data
	Resize( uAvailable );

	return oexTRUE;
}

void CCircBuf::Destroy()
{
	// Lock the buffer
	CTlLocalLock ll( &m_lock );
	if ( !ll.IsLocked() ) 
        return;
	
	// Empty the buffer
	Empty();

    // Lose the buffer
	m_uSize = 0;
    m_pBi = oexNULL;
    m_pBuf = oexNULL;
	m_memBuffer.Delete();

    m_uPokePtr = 0;

}

oexBOOL CCircBuf::Read( CStr8 &x_sStr, oexUINT x_uMax )
{
	// Lock the buffer
	CTlLocalLock ll( &m_lock );
	if ( !ll.IsLocked() ) 
        return oexFALSE;

    // How much data is available?
    oexUINT uAvailable = GetMaxRead();
    if ( !uAvailable )
        return oexFALSE;

    // Zero means all of it
    if ( !x_uMax || x_uMax > uAvailable ) 
        x_uMax = uAvailable;

    // Anything to read?
    if ( !x_uMax )
        return oexFALSE;

    // Allocate memory
    if ( !x_sStr.OexAllocate( x_uMax ) )
        return oexFALSE;

	oexUINT uRead = 0;

	// Read the string from the buffer
	Read( x_sStr._Ptr(), x_uMax, &uRead );

    // Ensure didn't read more than we should have
    oexASSERT( x_uMax >= uRead );

    // Don't assume it's NULL terminated
    x_sStr.SetLength( uRead );

    return oexTRUE;
}


oexBOOL CCircBuf::Read( oexPVOID x_pvBuf, oexUINT x_uMax, oexUINT *x_puRead, oexUINT *x_puPtr, oexUINT x_uEncode )
{
	// Lock the buffer
	CTlLocalLock ll( &m_lock );
	if ( !ll.IsLocked() ) 
        return oexFALSE;

    if ( !m_pBi )
        return oexFALSE;

	// Where to start?
	oexUINT uPtr = NormalizePtr( x_puPtr ? *x_puPtr : 0, m_uSize );

	// Anything to read?
	oexUINT uSize = GetMaxRead( uPtr, m_pBi->uWritePtr, m_uSize );
	if ( !uSize ) 
        return oexFALSE;

	// Do they just want to know the size?
	if ( x_pvBuf == oexNULL || x_uMax == 0 )
	{	if ( x_puRead ) 
            *x_puRead = uSize;
		return oexTRUE;
	} // end if

	// Are we short a few bytes?
	if ( uSize < x_uMax ) 
	{
		// Assume we MUST read dwMax if we can't tell the user otherwise
		if ( x_puRead == oexNULL ) 
            return oexFALSE;

	} // end if

	// Do we have more than enough?
	else if ( uSize > x_uMax )
	{		
		// Truncate to user buffer
		uSize = x_uMax; 

	} // end else

	oexUCHAR *pView = oexNULL;
	oexUINT i = 0, uRead = 0, uView;

	// Read all the blocks into the buffer
	while ( GetView( i, uPtr, uSize, m_pBuf, m_uSize, &pView, &uView ) )
	{
		// Copy the data
		os::CSys::MemCpy( &( (oexUCHAR*)x_pvBuf )[ uRead ], pView, uView );

		// Decode if needed
		if ( x_uEncode ) OnDecode( x_uEncode, i, &( (oexUCHAR*)x_pvBuf )[ uRead ], uView );

		// For inspecting the actual read data
		OnInspectRead( i, pView, uView );

		// Keep count of the bytes read
		uRead += uView;

		// Next block
		i++;

	} // end while

	// Update the pointer if required
	if ( x_puPtr ) 
        *x_puPtr = NormalizePtr( AdvancePtr( uPtr, uSize, m_uSize ), m_uSize );

	// How many bytes were read?
	if ( x_puRead ) 
        *x_puRead = uRead;

	return oexTRUE;
}

oexBOOL CCircBuf::Write( oexCPVOID x_pvBuf, oexUINT x_uSize, oexUINT *x_puPtr, oexUINT x_uEncode )
{
	// Lock the buffer
	CTlLocalLock ll( &m_lock );
	if ( !ll.IsLocked() ) 
        return oexFALSE;

	// Just ignore a NULL write
	if ( x_uSize == 0 ) 
        return oexTRUE;

    // Attempt to allocate space if none
    if ( !m_pBi )
    {
        if ( !Allocate( x_uSize ) )
            return oexFALSE;

        if ( !m_pBi )
            return oexFALSE;

    } // end if

	// Where to start?
	oexUINT uPtr = NormalizePtr( x_puPtr ? *x_puPtr : 0, m_uSize );

	// Ensure buffer space
	if ( x_uSize > GetMaxWrite( m_pBi->uReadPtr, uPtr, m_uSize ) ) 
        return oexFALSE;

	oexUCHAR *pView;
	oexUINT i = 0, uWritten = 0, uView;

	// Read all the blocks into the buffer
	while ( GetView( i, uPtr, x_uSize, m_pBuf, m_uSize, &pView, &uView ) )
	{	
		// Copy the data
		os::CSys::MemCpy( pView, &( (oexUCHAR*)x_pvBuf )[ uWritten ], uView );

		// Encode if needed
		if ( x_uEncode ) 
            OnEncode( x_uEncode, i, pView, uView );

		// For inspecting the actual Write data
		OnInspectWrite( i, pView, uView );

		// Track the number of bytes written
		uWritten += uView;

		// Next block
		i++;

	} // end while

	// Update the pointer if required
	if ( x_puPtr ) 
        *x_puPtr = AdvancePtr( uPtr, x_uSize, m_uSize );

	return oexTRUE;
}


oexUINT CCircBuf::GetMaxRead( oexUINT x_uReadPtr, oexUINT x_uWritePtr, oexUINT x_uMax )
{
	// Do we have a buffer?
	if ( x_uMax == 0 ) 
        return 0;

	// Is the buffer empty?
	if ( x_uReadPtr == x_uWritePtr ) 
        return 0;

	// Check for inside use
	if ( x_uWritePtr > x_uReadPtr ) 
		return x_uWritePtr - x_uReadPtr;

	// Outside use
	return x_uMax - ( x_uReadPtr - x_uWritePtr );
}

oexUINT CCircBuf::GetMaxWrite( oexUINT x_uReadPtr, oexUINT x_uWritePtr, oexUINT x_uMax )
{	// Do we have a buffer?
	if ( x_uMax == 0 ) 
        return 0;

	// Must normalize the write pointer
	oexUINT uNWritePtr = x_uWritePtr;
	if ( uNWritePtr >= x_uMax ) 
        uNWritePtr %= x_uMax;

	// Is the buffer empty?
	if ( x_uReadPtr == uNWritePtr ) 
	{	if ( x_uReadPtr != x_uWritePtr ) 
			return 0;
		return x_uMax;
	} // end if

	// Check for inside use
	if ( uNWritePtr > x_uReadPtr ) 
		return x_uMax - ( uNWritePtr - x_uReadPtr );

	// Check for unusable space	( write pointer can't advance onto read pointer )
	if ( ( uNWritePtr + 1 ) == x_uReadPtr )
		return 0;

	// Outside use
	return x_uReadPtr - uNWritePtr;
}

oexBOOL CCircBuf::Write(oexCSTR8 x_pStr)
{
	// Write the string into the buffer
    return Write( x_pStr, zstr::Length( x_pStr ) );
}

oexBOOL CCircBuf::Read(oexSTR8 x_pStr, oexUINT x_uMax)
{
	oexUINT uRead = 0;

	// Read the string from the buffer
	Read( x_pStr, x_uMax, &uRead );

	// NULL terminate string
	if ( uRead >= x_uMax ) 
        uRead = x_uMax - 1;
	x_pStr[ uRead ] = 0;

	return oexTRUE;
}

oexBOOL CCircBuf::Read( oexPVOID x_pvBuf, oexUINT x_uMax, oexUINT *x_puRead, oexUINT x_uEncode )
{
    if ( !m_pBi )
        return oexFALSE;

	if ( !Read( x_pvBuf, x_uMax, x_puRead, &m_pBi->uReadPtr, x_uEncode ) ) 
        return oexFALSE;

	Defrag(); 

    return oexTRUE;
}

oexBOOL CCircBuf::Peek( oexPVOID x_pvBuf, oexUINT x_uMax, oexUINT *x_puRead, oexLONG x_lOffset, oexUINT x_uEncode )
{
	if ( !m_uSize || !m_pBi ) 
        return oexFALSE;

	// Where to peek
	oexUINT uReadPtr = m_pBi->uReadPtr;	

	// Offset into the buffer
	uReadPtr = AdvancePtr( uReadPtr, x_lOffset, m_uSize );

	// Read from here
	return Read( x_pvBuf, x_uMax, x_puRead, &uReadPtr, x_uEncode );
}

oexBOOL CCircBuf::Peek( CStr8 &x_sStr, oexUINT x_uMax )
{
	// Lock the buffer
	CTlLocalLock ll( &m_lock );
	if ( !ll.IsLocked() ) 
        return oexFALSE;

    // Zero means all of it
    if ( !x_uMax ) 
        x_uMax = GetMaxRead();

    // Anything to read?
    if ( !x_uMax )
        return oexFALSE;

    // Allocate memory
    if ( !x_sStr.OexAllocate( x_uMax ) )
        return oexFALSE;

	oexUINT uRead = 0;

	// Read the string from the buffer
	Peek( x_sStr._Ptr(), x_uMax, &uRead );

    // Ensure didn't read more than we should have
    oexASSERT( x_uMax >= uRead );

    // Don't assume it's NULL terminated
    x_sStr.SetLength( uRead );

    return oexTRUE;
}

void CCircBuf::Defrag()
{
	// Lock the buffer
	CTlLocalLock ll( &m_lock );
	if ( !ll.IsLocked() ) 
        return;

    if ( !m_pBi )
        return;

	// Set the pointers back to zero if the buffer is empty
	if ( m_pBi->uReadPtr == NormalizePtr( m_pBi->uWritePtr, m_uSize ) ) 
        Empty();
}

oexUINT CCircBuf::AdvancePtr( oexUINT x_uPtr, oexLONG x_lStep, oexUINT x_uMax )
{
	// Ensure valid max
	if ( !x_uMax ) 
        return x_uPtr;

	// Offset pointer
	x_uPtr += x_lStep;

	// Wrap the pointer
	if ( x_uPtr > x_uMax ) 
        x_uPtr %= x_uMax;

	return x_uPtr;
}

oexUINT CCircBuf::NormalizePtr( oexUINT x_uPtr, oexUINT x_uMax )
{
	if ( x_uMax && x_uPtr >= x_uMax ) 
        return x_uPtr % x_uMax;
	return x_uPtr;
}

oexBOOL CCircBuf::GetView( oexUINT x_uView, oexUINT x_uPtr, oexUINT x_uSize, oexUCHAR *x_pucRing, oexUINT x_uMax, oexUCHAR **x_pucBuf, oexUINT *x_puSize )
{
	// Verify buffers
	if ( x_pucRing == oexNULL || !x_uSize || !x_uMax ) 
        return oexFALSE;
	if ( x_pucBuf == oexNULL || x_puSize == oexNULL ) 
        return oexFALSE;
	if ( x_uPtr >= x_uMax ) 
        return oexFALSE;

	// How many bytes left till the end of the buffer?
	oexUINT uLeft = x_uMax - x_uPtr;

	if ( x_uView == 0 )
	{
		// Does it wrap?
		if ( x_uSize > uLeft ) x_uSize = uLeft;

		// Save buffer metrics on the first part of the buffer
		*x_pucBuf = &x_pucRing[ x_uPtr ];
		*x_puSize = x_uSize;

		return oexTRUE;

	} // end if

	else if ( x_uView == 1 )
	{
		// Is there a second view?
		if ( x_uSize <= uLeft ) 
            return oexFALSE;

		// The second part of the buffer
		*x_pucBuf = x_pucRing;
		*x_puSize = x_uSize - uLeft;

		return oexTRUE;

	} // end else if

	return oexFALSE;
}


oexBOOL CCircBuf::WaitData(oexUINT x_uTimeout)
{
    // No data if no buffer
    if ( !m_pBi )
        return oexFALSE;

	// Wait for data
	if ( os::CSys::waitSuccess != os::CSys::WaitForSingleObject( m_hDataReady, x_uTimeout ) )
		return oexFALSE;

	return oexTRUE;
}

oexBOOL CCircBuf::WaitEmpty(oexUINT x_uTimeout)
{
    // Empty if no buffer
    if ( !m_pBi )
        return oexTRUE;

	// Wait for data
	if ( os::CSys::waitSuccess != os::CSys::WaitForSingleObject( m_hEmpty, x_uTimeout ) )
		return oexFALSE;

	return oexTRUE;
}

