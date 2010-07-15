/*------------------------------------------------------------------
// mem_share.cpp
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

const CBin::t_byte* CBin::Resize( CBin::t_size x_nNewSize )
{_STT();

	// Ptr buffer?
	if ( m_ptr )
	{
		// Is the buffer just getting smaller?
		if ( m_nUsed > x_nNewSize )
			m_nUsed = x_nNewSize;

		// Must allocate more space
		else
		{
			// Make a new buffer and copy data
			if ( !m_buf.OexNew( x_nNewSize ).Ptr() )
				return 0;
			oexMemCpy( m_buf.Ptr(), m_ptr, m_nUsed );
			FreePtr();

		} // end else

	} // end if

	// Simple resize
	else
		m_buf.Resize( x_nNewSize );

	// How did it go?
	return Ptr();
}


CBin::t_size CBin::Copy( CBin *x_p )
{_STT();

	// Just ensuring our internal buffer is unshared?
	if ( !x_p )
	{
		// Ptr buffer?
		if ( m_ptr )
			m_buf.MemCpy( m_ptr, m_nUsed ),
			FreePtr();

		// Just unshare the buffer
		else
			m_buf.Unshare();

		return getUsed();

	} // end if

	// Valid buffer?
	if ( !x_p->Ptr() || !x_p->getUsed() )
	{	Destroy();
		return 0;
	} // end if

	// Lose Ptr buffer if any
	FreePtr();
	m_nUsed = x_p->getUsed();

	// Ptr buffer?
	m_buf.MemCpy( x_p->Ptr(), m_nUsed );

	return m_nUsed;
}

CBin::t_size CBin::Share( CBin *x_p )
{_STT();

	if ( !x_p || !x_p->getUsed() )
	{	Destroy();
		return 0;
	} // end if

	// Lose any Ptr buffer
	FreePtr();

	// Ptr buffer?
	if ( x_p->m_ptr )
		setBuffer( x_p->m_ptr, x_p->m_nUsed, x_p->m_nOffset, x_p->m_bFree );

	// Share native buffer
	else
		m_nUsed = x_p->m_nUsed,
		m_nOffset = x_p->m_nOffset,
		m_buf.Share( x_p->m_buf );

	return m_nUsed;
}

CBin::t_size CBin::AppendBuffer( const CBin::t_byte *x_pBuf, CBin::t_size x_nBytes )
{_STT();

	// Ptr pointer?
	if ( m_ptr )
	{
		// Copy Ptr buffer
		if ( m_nUsed )
			m_buf.MemCpy( m_ptr, m_nUsed );

		// Append new data
		m_buf.MemCpyAt( (t_byte*)x_pBuf, m_nUsed, x_nBytes );

		// Not Ptr buffer anymore
		FreePtr();
		m_nUsed = m_nUsed + x_nBytes;

		return m_nUsed;
	}

	// Verify m_nUsed
	if ( (unsigned int)m_nUsed > m_buf.Size() )
		m_nUsed = m_buf.Size();

	// Append new data
	m_buf.MemCpyAt( (t_byte*)x_pBuf, m_nUsed, x_nBytes );
	m_nUsed += x_nBytes;

	return m_nUsed;
}

CBin::t_size CBin::Insert( CBin::t_size x_nBytes, CBin::t_size x_nOffset = 0 )
{_STT();

	if ( !x_nBytes )
		return 0;

	// Is it past the end?
	if ( !getUsed() || x_nOffset >= getUsed() )
	{
		// Resize the buffer
		Resize( x_nOffset + x_nBytes );

		// Zero space between end of buffer and offset
		t_size over = x_nOffset - getUsed();
		if ( over ) oexZeroMemory( _Ptr( getUsed() ), over );

		// Update used value
		setUsed( x_nOffset + x_nBytes );

		return getUsed();

	} // end if

	// Allocate space
	Resize( getUsed() + x_nBytes );

	// Move block ( can't use memcpy because blocks overlap )
	t_size copy = getUsed() - x_nOffset;
    if ( copy )
    {   t_byte *dst = _Ptr( x_nOffset + x_nBytes + copy - 1 ), *src = _Ptr( x_nOffset + copy - 1 );
        while ( copy-- ) *dst-- = *src--;
    } // end if

	// Set new size
	setUsed( getUsed() + x_nBytes );

	return getUsed();
}


CBin::t_size CBin::LShift( CBin::t_size x_nBytes )
{_STT();

	// All of it?
	if ( x_nBytes >= getUsed() )
	{	FreePtr();
		m_nUsed = 0;
		m_nOffset = 0;
		return 0;
	} // end if

	// Account for the offset
	x_nBytes += m_nOffset;
	m_nOffset = 0;

	// Are we doing any work?
	if ( !x_nBytes )
		return m_nUsed;

	// Copy Ptr buffer if needed
	if ( m_ptr )
	{
		// What's the new size?
		m_nUsed -= x_nBytes;
		if ( !m_buf.OexNew( m_nUsed ).Ptr() )
			return 0;

		// Copy to private buffer
		m_buf.MemCpy( &m_ptr[ x_nBytes ], m_nUsed );

	} // end if
	else
	{
		// Just shift the buffer
		m_buf.LShift( x_nBytes, m_nUsed - x_nBytes ),
		m_nUsed -= x_nBytes;

	} // end else

	return m_nUsed;
}


void CBinShare::Destroy()
{_STT();

	oexAutoLock ll( m_lock );
	if ( !ll.IsLocked() )
		return;

#if defined( oexDEBUG )

	// Warn user if they are not running garbage collection
	if ( m_buffers.Size() && ( m_uTime + 3 ) < oexGetUnixTime() )
	{	oexERROR( 0, oexT( "You are using oexGetBin() / oexSetBin(), but not calling oexCleanupBin()" ) );
	} // edn if

#endif

	// Lose the whole list
	m_buffers.Destroy();
}

CBin CBinShare::GetBuffer( CStr x_sName, CBinShare::t_size x_uSize )
{_STT();

	oexAutoLock ll( m_lock );
	if ( !ll.IsLocked() )
		return CBin();

	// Existing?
	if ( m_buffers.IsKey( x_sName ) )
	{	CBinHolder &r = m_buffers[ x_sName ];
		r.to = oexGetUnixTime();
		return r.bin;
	} // end if

	// User only wants existing?
	else if ( !x_uSize )
		return CBin();

	// Get / Create share
	CBinHolder &r = m_buffers[ x_sName ];
	r.bin.Mem().OexNew( x_uSize );
	r.to = oexGetUnixTime();

	return r.bin;
}

oexBOOL CBinShare::SetBuffer( CStr x_sName, CBin *x_pBin )
{_STT();

	// You can't share a plain buffer
//	if ( x_pBin && x_pBin->IsPlainShare() )
//	{	oexERROR( 0, CStr() << oexT( "Attempt to shared plain buffer : " ) << x_sName );
//		oexASSERT( 0 );
//		return oexFALSE;
//	} // end if

	oexAutoLock ll( m_lock );
	if ( !ll.IsLocked() )
		return oexFALSE;

	// Are we dropping the buffer?
	if ( !x_pBin || !x_pBin->Size() )
	{	m_buffers.Unset( x_sName );
		return oexTRUE;
	} // end if

	// Create share
	CBinHolder &r = m_buffers[ x_sName ];
	r.bin = *x_pBin;
	r.to = oexGetUnixTime();

	return oexTRUE;
}

oexBOOL CBinShare::IsBuffer( CStr x_sName )
{_STT();

	oexAutoLock ll( m_lock );
	if ( !ll.IsLocked() )
		return oexFALSE;

	return m_buffers.IsKey( x_sName );
}


oexINT CBinShare::Cleanup()
{_STT();

	oexAutoLock ll( m_lock );
	if ( !ll.IsLocked() )
		return -1;

	// Save the time we did the cleanup
#if defined( oexDEBUG )
	m_uTime = oexGetUnixTime();
#endif

//	oexPrintf( "." ); oexFlush_stdout();

	oexUINT tm = oexGetUnixTime();
	oexUINT to = tm - eBufferTimeout;

	// Collect the garbage...
    for ( t_buffers::iterator it; m_buffers.Next( it ); )
	{
		// Remove invalid buffers
		if ( !it->bin.Ptr() )
			it = m_buffers.Erase( it );

		// Reset timeout if it's being used
		else if ( !it->bin.IsPlainShare() && 1 < oexGetRefCount( it->bin.Ptr() ) )
			it->to = tm;

		// Erase once it times out
		else if ( to > it->to )
			it = m_buffers.Erase( it );

	} // end for

	return (oexINT)m_buffers.Size();
}
