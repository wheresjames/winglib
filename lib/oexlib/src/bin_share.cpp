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

#include "oexlib.h"

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

			// Remember how many are used
			t_size nUsed = m_nUsed;

			// Copy the data
			oexMemCpy( m_buf.Ptr(), m_ptr, nUsed );

			// Free the user pointer
			FreePtr();

			// Restore used amount
			m_nUsed = nUsed;

		} // end else

	} // end if

	// Simple resize
	else
		m_buf.OexResize( x_nNewSize );

	// How did it go?
	return Ptr();
}


CBin::t_size CBin::Copy( CBin *x_p )
{_STT();

	// Just ensuring our internal buffer is unshared?
	if ( !x_p )
	{
		// Where's the buffer we're dealing with?
		t_size nUsed = getUsed();
		const CBin::t_byte* ptr = Ptr();

		// Valid buffer?
		if ( 0 >= nUsed || !ptr )
		{	Destroy();
			return 0;
		} // end if

		// User ptr buffer?
		if ( m_ptr )
			m_buf.MemCpy( ptr, nUsed ), FreePtr();

		// Make a copy of shared pointer
		else
		{	t_buffer buf;
			buf.MemCpy( ptr, nUsed );
			m_buf.Assume( buf );
		} // end else if

		// Restore used count
		m_nUsed = nUsed;
		m_nOffset = 0;

		return nUsed;

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

	// Null Append?
	if ( !x_pBuf || !x_nBytes )
		return m_nUsed;

	// Ptr pointer?
	if ( m_ptr )
	{
		// Remember how many bytes are being used
		t_size nUsed = m_nUsed;
	
		// Copy Ptr buffer
		if ( nUsed )
			m_buf.MemCpy( m_ptr, nUsed );

		// Append new data
		m_buf.MemCpyAt( (t_byte*)x_pBuf, nUsed, x_nBytes );

		// No Ptr buffer anymore
		FreePtr();

		// New used amount
		m_nUsed = nUsed + x_nBytes;

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

	// Anything to do?
	if ( !x_nBytes && !m_nOffset )
		return 0;

	// All of it?
	if ( x_nBytes >= getUsed() )
	{	FreePtr();
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

#define OEX_CBIN_SCALE_TYPE( ot, t, e )	case obj::ot : \
	{	oex##t *p = (oex##t*)_Ptr( uOffset ); if ( !p ) return 0; \
		if ( 1 == e ) while ( uSamples ) { p[ uOffset ] = oexLE_##t( (oex##t)( oexLE_##t( p[ uOffset ] ) * dScale ) ); uOffset++; uSamples--; } \
		else if ( 2 == e ) while ( uSamples ) { p[ uOffset ] = oexBE_##t( (oex##t)( oexBE_##t( p[ uOffset ] ) * dScale ) ); uOffset++; uSamples--; } \
		else if ( 3 == e ) while ( uSamples ) { p[ uOffset ] = (oex##t)( oexLE_##t( p[ uOffset ] ) * dScale ); uOffset++; uSamples--; } \
		else if ( 4 == e ) while ( uSamples ) { p[ uOffset ] = (oex##t)( oexBE_##t( p[ uOffset ] ) * dScale ); uOffset++; uSamples--; } \
		else while ( uSamples ) { p[ uOffset ] = (oex##t)( p[ uOffset ] * dScale ); uOffset++; uSamples--; } \
	} break;

oexUINT CBin::Scale( oexINT nType, oexUINT uOffset, oexUINT uSamples, oexUINT uE, oexDOUBLE dScale )
{_STT();
	// Get element size
	oexUINT uESize = obj::StaticSize( nType );
	if ( !uESize )
		return 0;

	// Starting in the buffer?
	if ( ( uOffset + uESize ) >= getUsed() )
		return 0;

	// Boundry limit samples
	oexUINT uMaxSamples = ( getUsed() - uOffset ) / uESize;
	if ( !uSamples || uSamples > uMaxSamples )
		uSamples = uMaxSamples;

	switch( nType )
	{
		OEX_CBIN_SCALE_TYPE( tInt8, CHAR, uE );
		OEX_CBIN_SCALE_TYPE( tUInt8, UCHAR, uE );
		OEX_CBIN_SCALE_TYPE( tInt16, SHORT, uE );
		OEX_CBIN_SCALE_TYPE( tUInt16, USHORT, uE );
		OEX_CBIN_SCALE_TYPE( tInt32, INT, uE );
		OEX_CBIN_SCALE_TYPE( tUInt32, UINT, uE );
		OEX_CBIN_SCALE_TYPE( tInt64, INT64, uE );
		OEX_CBIN_SCALE_TYPE( tUInt64, UINT64, uE );
		OEX_CBIN_SCALE_TYPE( tFloat, FLOAT, uE );
		OEX_CBIN_SCALE_TYPE( tDouble, DOUBLE, uE );

		default : break;

	} // end switch

	return uESize;
}

#define OEX_CBIN_TH_TYPE( ot, t, e )	case obj::ot : \
	{	oex##t *p = (oex##t*)_Ptr( uOffset ); if ( !p ) return 0; \
		oex##t t, lo = (oex##t)dLower, hi = (oex##t)dUpper; \
		if ( 1 == e ) { oex##t lo_def = oexLE_##t( (oex##t)dLowerDef ), hi_def = oexLE_##t( (oex##t)dUpperDef ); \
			while ( uSamples ) { t = oexLE_##t( p[ uOffset ] ); if ( t < lo ) p[ uOffset ] = lo_def; else if ( t > hi ) p[ uOffset ] = hi_def; uSamples--; uOffset++; } } \
		else if ( 2 == e ) { oex##t lo_def = oexBE_##t( (oex##t)dLowerDef ), hi_def = oexBE_##t( (oex##t)dUpperDef ); \
			while ( uSamples ) { t = oexBE_##t( p[ uOffset ] ); if ( t < lo ) p[ uOffset ] = lo_def; else if ( t > hi ) p[ uOffset ] = hi_def; uSamples--; uOffset++; } } \
		else { oex##t lo_def = (oex##t)dLowerDef, hi_def = (oex##t)dUpperDef; \
			while ( uSamples ) { t = oexBE_##t( p[ uOffset ] ); if ( t < lo ) p[ uOffset ] = lo_def; else if ( t > hi ) p[ uOffset ] = hi_def; uSamples--; uOffset++; } } \
	} break;

oexUINT CBin::Threshold( oexINT nType, oexUINT uOffset, oexUINT uSamples, oexUINT uE, oexDOUBLE dUpper, oexDOUBLE dUpperDef, oexDOUBLE dLower, oexDOUBLE dLowerDef )
{_STT();
	// Get element size
	oexUINT uESize = obj::StaticSize( nType );
	if ( !uESize )
		return 0;

	// Starting in the buffer?
	if ( ( uOffset + uESize ) >= getUsed() )
		return 0;

	// Boundry limit samples
	oexUINT uMaxSamples = ( getUsed() - uOffset ) / uESize;
	if ( !uSamples || uSamples > uMaxSamples )
		uSamples = uMaxSamples;

	switch( nType )
	{
		OEX_CBIN_TH_TYPE( tInt8, CHAR, uE );
		OEX_CBIN_TH_TYPE( tUInt8, UCHAR, uE );
		OEX_CBIN_TH_TYPE( tInt16, SHORT, uE );
		OEX_CBIN_TH_TYPE( tUInt16, USHORT, uE );
		OEX_CBIN_TH_TYPE( tInt32, INT, uE );
		OEX_CBIN_TH_TYPE( tUInt32, UINT, uE );
		OEX_CBIN_TH_TYPE( tInt64, INT64, uE );
		OEX_CBIN_TH_TYPE( tUInt64, UINT64, uE );
		OEX_CBIN_TH_TYPE( tFloat, FLOAT, uE );
		OEX_CBIN_TH_TYPE( tDouble, DOUBLE, uE );

		default : break;

	} // end switch

	return uESize;
}


oexUINT CBin::GroupAvg( oexINT nType, oexUINT uOffset, oexUINT uInterval, oexUINT uGroups, CBin &bin, oexINT nFlags )
{
	// Sanity check
	if ( 0 >= uGroups || !getUsed() )
		return 0;

	// Get skip value
	oexUINT uSkip = nFlags & 0x0f;
	if ( uSkip >= uGroups )
		uSkip = 0;

	// Get element size
	oexUINT uESize = obj::StaticSize( nType );
	if ( !uESize )
		return 0;

	// Total samples
	oexUINT uSamples = getUsed() / uESize;
	if ( uSamples < uInterval )
		return 0;

	// How many samples per group
	oexUINT nSamplesPerGroup = ( uInterval - uSkip ) / ( uGroups - uSkip );
	if ( !nSamplesPerGroup )
		return 0;

	// Allocate memory
	oexUINT uBytes = uGroups * uESize;
	if ( bin.Size() < uBytes )
		if ( !bin.Allocate( uBytes ) )
			return oexFALSE;

	// Allocate an array to hold the counts
	TMem< oexUINT > aCnts;
	if ( !aCnts.OexNew( uGroups ).Ptr() )
		return oexFALSE;

	// Memory to hold acc
	TMem< oexLONGDOUBLE > aAcc;
	if ( !aAcc.OexNew( uGroups ).Ptr() )
		return oexFALSE;

	// Initialize group averages
	for ( oexUINT i = 0; i < uGroups; i++ )
		aAcc[ i ] = 0, aCnts[ i ] = 0;

	// Accumulate all samples
	while( ( uOffset + uInterval ) < uSamples )
	{
		// Accumulate samples in this interval
		for ( oexUINT s = 0; s < uInterval; s++ )
		{
			// Get the value
			oexLONGDOUBLE dV;
			switch( nType )
			{	case obj::tInt : dV = getINT( uOffset + s ); break;
				case obj::tFloat : dV = getFLOAT( uOffset + s ); break;
				case obj::tDouble : dV = getDOUBLE( uOffset + s ); break;
				default : dV = 0;
			} // end switch

			// Range check and add
//			if ( 1000000 > oex::cmn::Abs( dV ) && .0000001 < oex::cmn::Abs( dV ) )

			if ( s < uSkip )
				aAcc[ s ] = dV, aCnts[ s ]++;
			else
			{	oexUINT o = uSkip + s / nSamplesPerGroup;
				if ( o >= uGroups ) o = uGroups - 1;
				aAcc[ o ] += dV; aCnts[ o ]++;
			} // end else

		} // end for

		uOffset += uInterval;

	} // end while

	// Calculate averages
	switch( nType )
	{
		case obj::tInt :
			for ( oexUINT g = 0; g < uGroups; g++ )
				bin.setINT( g, aCnts[ g ] ? oexINT( aAcc[ g ] / aCnts[ g ] ) : 0 );
			break;

		case obj::tFloat :
			for ( oexUINT g = 0; g < uGroups; g++ )
				bin.setFLOAT( g, aCnts[ g ] ? oexFLOAT( aAcc[ g ] / aCnts[ g ] ) : 0 );
			break;

		case obj::tDouble :
			for ( oexUINT g = 0; g < uGroups; g++ )
				bin.setDOUBLE( g, aCnts[ g ] ? oexDOUBLE( aAcc[ g ] / aCnts[ g ] ) : 0 );
			break;

		default :
			return 0;

	} // end switch

	// Number of bytes we used
	bin.setUsed( uBytes );

	return uOffset;
}

CBin::t_size CBin::Randomize( int nStart, int nEnd )
{
	if ( nStart < 0 )
		nStart = 0;

	if ( nEnd <= 0 )
		nEnd = getUsed();
	else if  ( nEnd > getUsed() )
		nEnd = getUsed();

	if ( nStart >= nEnd )
		return 0;

	os::CSys::Rand( _Ptr( nStart ), nEnd - nStart );

	return nEnd - nStart;
}

CStr8 CBin::base64_encode()
{_STT();
	return CBase64::Encode( Ptr(), getUsed() );
}

CStr8 CBin::base64_decode()
{_STT();
	return CBase64::Decode( Ptr(), getUsed() );
}

CStr8 CBin::base16_encode()
{_STT();
	return CBase16::Encode( Ptr(), getUsed() );
}

CStr8 CBin::base16_decode()
{_STT();
	return CBase16::Decode( Ptr(), getUsed() );
}

CStr8 CBin::compress()
{_STT();
	return oexCompress( CStr8( Ptr(), getUsed() ) );
}

CStr8 CBin::uncompress()
{_STT();
	return oexUncompress( CStr8( Ptr(), getUsed() ) );
}

CBin& CBin::base64_encode( oexCONST CStr8 &s )
{_STT();
	setString( CBase64::Encode( s ) );
	return *this;
}

CBin& CBin::base64_decode( oexCONST CStr8 &s )
{_STT();
	setString( CBase64::Decode( s ) );
	return *this;
}

CBin& CBin::base16_encode( oexCONST CStr8 &s )
{_STT();
	setString( CBase16::Encode( s ) );
	return *this;
}

CBin& CBin::base16_decode( oexCONST CStr8 &s )
{_STT();
	setString( CBase16::Decode( s ) );
	return *this;
}

CBin& CBin::compress( oexCONST CStr8 &s )
{_STT();
	setString( oexCompress( s ) );
	return *this;
}

CBin& CBin::uncompress( oexCONST CStr8 &s )
{_STT();
	setString( oexUncompress( s ) );
	return *this;
}

void CBinShare::Destroy()
{_STT();

	if ( !m_lock.IsValid() )
		return;

	{ // Scope lock

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

	} // end scope

	// +++ Leaking to avoid crash on ctrl+c, please fix one day!
	//m_lock.Detach();

	// Lose the lock
	 m_lock.Destroy();
}

CBin CBinShare::GetBuffer( CStr x_sName, CBinShare::t_size x_uSize )
{_STT();

	if ( !m_lock.IsValid() )
		return CBin();

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

	if ( !m_lock.IsValid() )
		return oexFALSE;

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

	if ( !m_lock.IsValid() )
		return oexFALSE;

	oexAutoLock ll( m_lock );
	if ( !ll.IsLocked() )
		return oexFALSE;

	return m_buffers.IsKey( x_sName );
}


oexINT CBinShare::Cleanup()
{_STT();

	if ( !m_lock.IsValid() )
		return -1;

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

