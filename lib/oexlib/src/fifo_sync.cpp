/*------------------------------------------------------------------
// fifo_sync.cpp
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

CFifoSync::CFifoSync( oexBOOL x_bSync, t_size x_uSize, oexINT x_nWriteMode, t_size x_uMaxBuffers ) :
	CCircBuf( x_bSync, 0, x_nWriteMode )
{
    // Must be power of two
    m_uMaxBuffers = cmn::NextPower2( (t_size)x_uMaxBuffers );

    m_pBi = oexNULL;
    m_pBuf = oexNULL;

	Allocate( x_uSize );
}

CFifoSync::~CFifoSync()
{   Destroy2();
}

void CFifoSync::Destroy2()
{
	// Lock the buffer
	oexAutoLock ll( *this );
	if ( !ll.IsLocked() )
        return;

    m_pBi = oexNULL;
    m_pBuf = oexNULL;
    m_auSize.Delete();

    Destroy();
}

void CFifoSync::Destroy()
{
    CCircBuf::Destroy();
}


CCircBuf::t_size CFifoSync::Write( oexCPVOID x_pBuf, t_size x_uSize, oexUINT x_uEncode )
{
	// Lock the buffer
	oexAutoLock ll( *this );
	if ( !ll.IsLocked() )
        return oexFALSE;

	// Prepare to write data
	if ( !InitFifoWrite( x_uSize ) )
		return oexFALSE;

	// Add data to the buffer
	if ( !AddFifo( x_pBuf, x_uSize, x_uEncode ) )
		return oexFALSE;

	// Commit buffer changes
	return EndFifoWrite();
}

oexBOOL CFifoSync::AllocateBuffers()
{
	// Lock the buffer
	oexAutoLock ll( *this );
	if ( !ll.IsLocked() )
        return oexFALSE;

    // Ensure size tag space
	if ( m_auSize.Size() > m_uMaxBuffers )
        return oexTRUE;

    // Can' resize a mapped buffer
    if ( m_pBi && m_auSize.IsMapped() )
        return oexTRUE;

//	if ( !m_auSize.ReSize( m_uMaxBuffers ) )        // +++ Would have to defrag the circular buffer
        if ( !m_auSize.OexNew( m_uMaxBuffers ).Ptr() )
    	    return oexFALSE;

    // Get buffer pointers
    m_pBuf = m_auSize.Ptr();

	// Get a block info
	// Why does it work this way? ... it's a long story ...
    m_pBi = &m_bi;

    // Save head and tail pointer
    m_pBi->uHeadPtr = 0;
    m_pBi->uTailPtr = 0;

	return oexTRUE;
}

oexBOOL CFifoSync::AddFifo( oexCPVOID x_pBuf, t_size x_uSize, oexUINT x_uEncode )
{
	// Lock the buffer
	oexAutoLock ll( *this );
	if ( !ll.IsLocked() )
        return oexFALSE;

    if ( !m_pBi )
        return oexFALSE;

    // Poke the data into the buffer
	if ( !Poke( x_pBuf, x_uSize, x_uEncode ) )
		return oexFALSE;

	// Keep track of the size of this sub buffer
	m_pBuf[ m_pBi->uHeadPtr ] += x_uSize;

	return oexTRUE;
}

oexBOOL CFifoSync::InitFifoWrite( t_size x_uSize )
{
	// Lock the buffer
	oexAutoLock ll( *this );
	if ( !ll.IsLocked() )
        return oexFALSE;

    // Ensure buffer space
    if ( !m_pBi )
        Allocate( x_uSize );

	// Do we have a buffer?
	if ( !GetMaxWrite( m_pBi->uTailPtr, m_pBi->uHeadPtr, m_uMaxBuffers ) )
    {
        // Can't grow shared memory
        if ( m_auSize.IsMapped() )
            return oexFALSE;

        // Attempt to get more space
        m_uMaxBuffers = cmn::NextPower2( m_uMaxBuffers * 2 );
        if ( !Allocate( x_uSize ) )
            return oexFALSE;

    } // end if

	// Normalize the head pointer
	m_pBi->uHeadPtr = NormalizePtr( m_pBi->uHeadPtr, m_uMaxBuffers );

	// Initialize buffer pointers
	m_pBuf[ m_pBi->uHeadPtr ] = 0;

	// Get ready to poke the buffer
	InitPoke();

	return oexTRUE;
}

oexBOOL CFifoSync::EndFifoWrite()
{
	// Lock the buffer
	oexAutoLock ll( *this );
	if ( !ll.IsLocked() )
        return oexFALSE;

    if ( !m_pBi )
        return oexFALSE;

    // Advance the head pointer
	m_pBi->uHeadPtr = AdvancePtr( m_pBi->uHeadPtr, 1, m_uMaxBuffers );

	return EndPoke();
}

oexBOOL CFifoSync::Read( oexPVOID x_pBuf, t_size x_uSize, t_size *x_puRead, oexUINT x_uEncode )
{
	// Lock the buffer
	oexAutoLock ll( *this );
	if ( !ll.IsLocked() )
        return oexFALSE;

    if ( !m_pBi )
        return oexFALSE;

    // Read the data block
	oexBOOL bRet = Peek( x_pBuf, x_uSize, x_puRead, 0, x_uEncode );

	// Next read block
	if ( x_pBuf && x_uSize )
        SkipBlock();

	return bRet;
}

oexBOOL CFifoSync::Read( CStr8 &x_sStr, t_size x_uMax )
{
	// Lock the buffer
	oexAutoLock ll( *this );
	if ( !ll.IsLocked() )
        return oexFALSE;

    if ( !m_pBi )
        return oexFALSE;

    t_size uRead = 0;
    oexBOOL bRet = Peek( oexNULL, 0, &uRead );

    if ( !bRet || !uRead )
    {   x_sStr.Destroy();
        return oexFALSE;
    } // end if

    // Range check
    if ( !x_uMax || x_uMax > uRead )
        x_uMax = uRead;

    // Allocate memory
    if ( !x_sStr.OexAllocate( x_uMax ) )
        return oexFALSE;

	// Read the string from the buffer
	Read( x_sStr._Ptr(), x_uMax, &uRead );

    // Ensure didn't read more than we should have
    oexASSERT( x_uMax >= uRead );

    // Don't assume it's NULL terminated
    x_sStr.SetLength( uRead );

    return oexTRUE;
}


oexBOOL CFifoSync::Peek( CStr8 &x_sStr, t_size x_uMax, oexBOOL x_bAppend )
{
	// Lock the buffer
	oexAutoLock ll( *this );
	if ( !ll.IsLocked() )
        return oexFALSE;

    if ( !m_pBi )
        return oexFALSE;

    t_size uRead = 0;
    oexBOOL bRet = Peek( oexNULL, 0, &uRead );

    if ( !bRet || !uRead )
    {   x_sStr.Destroy();
        return oexFALSE;
    } // end if

	t_size uOffset = 0;
	if ( x_bAppend )
		uOffset = x_sStr.Length();

    // Range check
    if ( !x_uMax || x_uMax > uRead )
        x_uMax = uRead;

    // Allocate memory
    if ( !x_sStr.OexAllocate( uOffset + x_uMax ) )
        return oexFALSE;

	// Read the string from the buffer
	Peek( x_sStr._Ptr( uOffset ), x_uMax, &uRead );

    // Ensure didn't read more than we should have
    oexASSERT( x_uMax >= uRead );

    // Don't assume it's NULL terminated
    x_sStr.SetLength( uOffset + uRead );

    return oexTRUE;
}

oexBOOL CFifoSync::Peek( oexPVOID x_pBuf, t_size x_uSize, t_size *x_puRead, oexLONG x_lOffset, oexUINT x_uEncode )
{
	// Lock the buffer
	oexAutoLock ll( *this );
	if ( !ll.IsLocked() )
        return oexFALSE;

    if ( !m_pBi )
        return oexFALSE;

	// How many buffers are available
	t_size uAvail = GetMaxRead( m_pBi->uTailPtr, m_pBi->uHeadPtr, m_uMaxBuffers );

	// Anything to read?
	if ( !uAvail )
		return oexFALSE;

	// Are they asking for the size?
	if ( !x_pBuf || !x_uSize )
	{
        if ( !x_puRead )
            return oexFALSE;

		// Get the buffer size
		*x_puRead = m_pBuf[ m_pBi->uTailPtr ];

		// Correct for offset
		if ( x_lOffset > (long)*x_puRead )
            return oexFALSE;

		*x_puRead -= x_lOffset;

		return oexTRUE;

	} // end if

	// What's the maximum amount of data in this block
	if ( x_uSize > m_pBuf[ m_pBi->uTailPtr ] )
		x_uSize = m_pBuf[ m_pBi->uTailPtr ];

	// Anything left to read?
	if ( x_uSize <= (t_size)x_lOffset )
        return oexFALSE;

	// Subtract the offset
	x_uSize -= (t_size)x_lOffset;

	// Peek the data
	return CCircBuf::Peek( x_pBuf, x_uSize, x_puRead, x_lOffset, x_uEncode );
}

oexBOOL CFifoSync::Peek( t_size x_uBuffer, CStr8 &x_sStr, t_size x_uMax, oexBOOL x_bAppend )
{
	// Lock the buffer
	oexAutoLock ll( *this );
	if ( !ll.IsLocked() )
    {   if ( !x_bAppend )
			x_sStr.Destroy();
        return oexFALSE;
    } // end if

    if ( !m_pBi )
    {   if ( !x_bAppend )
			x_sStr.Destroy();
        return oexFALSE;
    } // end if

    t_size uRead = 0;
    oexBOOL bRet = Peek( x_uBuffer, oexNULL, 0, &uRead );

    if ( !bRet || !uRead )
    {   if ( !x_bAppend )
			x_sStr.Destroy();
        return oexFALSE;
    } // end if

	t_size uOffset = 0;
	if ( x_bAppend )
		uOffset = x_sStr.Length();

    // Range check
    if ( !x_uMax || x_uMax > uRead )
        x_uMax = uRead;

    // Allocate memory
    if ( !x_sStr.OexAllocate( uOffset + x_uMax ) )
        return oexFALSE;

	// Read the string from the buffer
	Peek( x_uBuffer, x_sStr._Ptr( uOffset ), x_uMax, &uRead );

    // Ensure didn't read more than we should have
    oexASSERT( x_uMax >= uRead );

    // Don't assume it's NULL terminated
    x_sStr.SetLength( uOffset + uRead );

    return oexTRUE;
}

oexBOOL CFifoSync::Peek( t_size x_uBuffer, oexPVOID x_pBuf, t_size x_uSize, t_size *x_puRead, oexLONG x_lOffset, oexUINT x_uEncode )
{
	// Lock the buffer
	oexAutoLock ll( *this );
	if ( !ll.IsLocked() )
        return oexFALSE;

    if ( !m_pBi )
        return oexFALSE;

	// How many buffers are available
	t_size uAvail = GetMaxRead( m_pBi->uTailPtr, m_pBi->uHeadPtr, m_uMaxBuffers );

	// Anything to read?
	if ( x_uBuffer >= uAvail )
		return oexFALSE;

	// Get pointer to the buffer the user is interested in
//	t_size uBuffer = NormalizePtr( m_pBi->uTailPtr + x_uBuffer, m_uMaxBuffers );

	t_size uBuffer = m_pBi->uTailPtr;

	// Figure out the total offset into the buffer
	// +++ Could we figure out something else here please...
	t_size uOffset = 0;
	while ( x_uBuffer )
	{	x_uBuffer--;
		uOffset += m_pBuf[ uBuffer ];		
		uBuffer = NormalizePtr( ++uBuffer, m_uMaxBuffers );
	} // end while

	// Are they asking for the size?
	if ( !x_pBuf || !x_uSize )
	{
        if ( !x_puRead )
            return oexFALSE;

		// Get the buffer size
		*x_puRead = m_pBuf[ uBuffer ];

		// Correct for offset
		if ( x_lOffset > (long)*x_puRead )
            return oexFALSE;

		*x_puRead -= x_lOffset;

		return oexTRUE;

	} // end if

	// What's the maximum amount of data in this block
	if ( x_uSize > m_pBuf[ uBuffer ] )
		x_uSize = m_pBuf[ uBuffer ];

	// Anything left to read?
	if ( x_uSize <= (t_size)x_lOffset )
        return oexFALSE;

	// Subtract the offset
	x_uSize -= (t_size)x_lOffset;

	// Peek the data
	return CCircBuf::Peek( x_pBuf, x_uSize, x_puRead, (oexLONG)uOffset + x_lOffset, x_uEncode );
}

oexBOOL CFifoSync::SkipBlock()
{
	// Lock the buffer
	oexAutoLock ll( *this );
	if ( !ll.IsLocked() )
        return oexFALSE;

    if ( !m_pBi )
        return oexFALSE;

	// Anything to read?
	if ( !GetMaxRead( m_pBi->uTailPtr, m_pBi->uHeadPtr, m_uMaxBuffers ) )
		return oexFALSE;

	// Skip to the next block
	AdvanceReadPtr( m_pBuf[ m_pBi->uTailPtr ] );

	// Advance the tail pointer
	m_pBi->uTailPtr = NormalizePtr( AdvancePtr( m_pBi->uTailPtr, 1, m_uMaxBuffers ), m_uMaxBuffers );

	// Wrap pointers if buffer is empty
	if ( m_pBi->uTailPtr == NormalizePtr( m_pBi->uHeadPtr, m_uMaxBuffers ) && m_pBi->uTailPtr == 0 )
		m_pBi->uTailPtr = m_pBi->uHeadPtr = 0;

	return oexTRUE;
}

