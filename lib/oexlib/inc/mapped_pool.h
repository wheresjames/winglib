/*------------------------------------------------------------------
// mapped_pool.h
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

#if defined( oexDEBUG )
#	define OEX_MAPPEDPOOL_DEBUG
#endif

/// Mapped memory pool
template< typename T_VAL > class TMappedPool
{
public:

	enum
	{
		/// Pool size in bits ( 16 = 65536, 18 = 262144, 20 = 1048576 )
		eDefaultPoolBits = 18
	};

	enum
	{
		/// Slot mask
		eDefaultSlotMask = 0x1f
	};

	/// Size type
	typedef oexSIZE_T		t_size;

	/// Key type
	typedef oexULONG		t_key;

	/// Mapping structure
	struct SMap
	{	t_key			key;
		T_VAL			*val;
	};

public:

	/// Constructor
	TMappedPool()
	{
#if defined( OEX_MAPPEDPOOL_DEBUG )
		m_nSlotOverflows = 0;
#endif
		m_nPoolOverflows = 0;
		m_uTotalAllocations = 0;
		m_uCurrentAllocations = 0;
		m_uPeakAllocations = 0;
		m_nPoolBits = 0;
		m_nSlotMask = 0;
		m_nSlotSize = 0;
		m_nPoolMask = 0;
		m_nPoolSize = 0;
		m_pPool = oexNULL;
	}

	/// Destructor
	virtual ~TMappedPool()
	{
		Destroy();
	}

	/// Releases all resources
	void Destroy()
	{
		m_nPoolBits = 0;
		m_nSlotMask = 0;
		m_nSlotSize = 0;
		m_nPoolMask = 0;
		m_nPoolSize = 0;

		if ( m_pPool )
		{
			SRawAllocator sa = CMem::GetDefaultAllocator();
			if ( sa.fFree )
				sa.fFree( m_pPool );
			m_pPool = oexNULL;
		} // end if

#if defined( OEX_MAPPEDPOOL_DEBUG )
		m_nSlotOverflows = 0;
#endif

		m_nPoolOverflows = 0;
		m_uPeakAllocations = 0;
		m_uTotalAllocations = 0;
		m_uCurrentAllocations = 0;
	}

	/// Creates the leak detection pool
	oexBOOL Create( t_size x_nPoolBits = eDefaultPoolBits, t_size x_nSlotMask = eDefaultSlotMask )
	{
		Destroy();

		// Ensure valid slot mask
		if ( !cmn::IsPowerOf2( x_nSlotMask + 1 ) )
			x_nSlotMask = eDefaultSlotMask;

		// Number of bits in the pool
		m_nPoolBits = x_nPoolBits;

		// Save the slot mask
		m_nSlotMask = x_nSlotMask;

		// How big is a slot?
		m_nSlotSize = cmn::BitsToValue( cmn::CountBits( m_nSlotMask ) );

		// Get the pool mask
		m_nPoolMask = cmn::BitsToMask( m_nPoolBits );

		// What's the pool size?
		m_nPoolSize = cmn::BitsToValue( m_nPoolBits );

		// Get the default allocator
		SRawAllocator sa = CMem::GetDefaultAllocator();
		if ( !sa.fMalloc )
		{	Destroy();
			return oexFALSE;
		} // end if

		// Allocate pool
		m_pPool = (SMap*)sa.fMalloc( m_nPoolSize * sizeof( SMap ) );
		if ( !m_pPool )
		{	Destroy();
			return oexFALSE;
		} // end if

		// Initialize the pool
		oexZeroMemory( m_pPool, m_nPoolSize * sizeof( SMap ) );

		return oexTRUE;
	}

	/// Returns non-zero if there is a valid pool
	oexBOOL isValid()
	{	return ( m_pPool ) ? oexTRUE : oexFALSE; }

	/// How many slots are left in the pool?
	t_size getFreeSlots() { return m_nPoolSize - m_uCurrentAllocations; }

	/// How many slots are left in the pool?
	t_size getUsedSlots() { return m_uCurrentAllocations; }

	/// How many slots are in the pool?
	t_size getTotalSlots() { return m_nPoolSize; }

	/// Adds a pointer to the tracking pool
	oexBOOL Add( t_key key, T_VAL *val )
	{
		if ( !m_pPool || !key )
			return oexFALSE;

		// Track numbers
		m_uTotalAllocations++;
		m_uCurrentAllocations++;
		if ( m_uCurrentAllocations > m_uPeakAllocations )
			m_uPeakAllocations = m_uCurrentAllocations;

		// Calculate starting offset
		t_size uOffset = ( (t_size)key >> 4 ) & m_nPoolMask;

		t_size uStart = uOffset;
		do
		{
			// Already exists, or it's vacant?
			if ( m_pPool[ uOffset ].key == key || !m_pPool[ uOffset ].key )
			{
				// Save pointer
				// !!! Subtle thing, set the value before the key 
				m_pPool[ uOffset ].val = val;
				m_pPool[ uOffset ].key = key;

				return oexTRUE;

			} // end if

#if defined( OEX_MAPPEDPOOL_DEBUG )

			// This isn't so bad, just less efficient
			t_size u = ( uStart <= uOffset ) ? uOffset - uStart : m_nPoolSize - uOffset;
			if ( u > m_nSlotSize )
				m_nSlotOverflows++,
				oexEcho( "TMappedPool : Slot overflow" );

#endif

			// Calculate next offset
			uOffset = ( uOffset + 1 ) & m_nPoolMask;

		} while ( uStart != uOffset );

		// Count a pool overflow
		m_nPoolOverflows++;

#if defined( OEX_MEMLEAK_DEBUG )

		// This means leak detection isn't working for this pointer
		oexEcho( oexT( "TMappedPool : Tracking pool is full!" ) );

#endif

		return oexFALSE;
	}

	/// Verify's that a pointer is in the pool
	T_VAL* Next( oexUINT *i )
	{
		if ( !i || !m_pPool )
			return oexNULL;

		// Search for the next empty slot
		while ( (*i) < m_nPoolSize )
			if ( !m_pPool[ (*i) ].key )
				(*i)++;
			else
				return m_pPool[ (*i)++ ].val;

		return oexNULL;

	}

	/// Verify's that a pointer is in the pool
	T_VAL* Ptr( t_key key )
	{
		if ( !m_pPool || !key )
			return oexNULL;

		// Calculate starting offset
		t_size uOffset = ( (t_size)key >> 4 ) & m_nPoolMask;

		t_size uStart = uOffset;
		do
		{
			// Is this the one we're looking for?
			if ( m_pPool[ uOffset ].key == key )

				return m_pPool[ uOffset ].val;

			// Calculate next offset
			uOffset = ( uOffset + 1 ) & m_nPoolMask;

		} while ( uStart != uOffset );

		return oexNULL;
	}

	/// Removes a pointer from the pool
	oexBOOL Remove( t_key key )
	{
		if ( !m_pPool )
			return oexFALSE;

		// Track allocations
		if ( m_uCurrentAllocations )
			m_uCurrentAllocations--;
#if defined( OEX_MAPPEDPOOL_DEBUG )
		else
			oexEcho( oexT( "TMappedPool : m_uCurrentAllocations wants to go negative?" ) );
#endif

		// Calculate starting offset
		t_size uOffset = ( (t_size)key >> 4 ) & m_nPoolMask;

		t_size uStart = uOffset;
		do
		{
			// Is this the one we're looking for?
			if ( m_pPool[ uOffset ].key == key )
			{
				// Remove from pool
				// !!! Subtle thing, set the value before the key 
				m_pPool[ uOffset ].val = 0;
				m_pPool[ uOffset ].key = 0;

				return oexTRUE;

			} // end if

			// Calculate next offset
			uOffset = ( uOffset + 1 ) & m_nPoolMask;

		} while ( uStart != uOffset );

#if defined( OEX_MAPPEDPOOL_DEBUG )
		oexEcho( oexT( "TMappedPool : Block not found" ) );
	//	oexASSERT( 0 );
#endif

		return oexFALSE;
	}

private:

	/// The pointer pool
	SMap					*m_pPool;

	/// Pool size
	t_size					m_nPoolSize;

	/// Pool bits
	t_size					m_nPoolBits;

	/// Pool mask
	t_size					m_nPoolMask;

	/// Slot mask
	t_size					m_nSlotMask;

	/// Slot size
	t_size					m_nSlotSize;

	/// Total number of allocations
	t_size					m_nAllocations;

#if defined( OEX_MAPPEDPOOL_DEBUG )

	/// Tracks the number of slot overflows
	t_size					m_nSlotOverflows;

#endif

	/// Tracks the number of pool overflows
	t_size					m_nPoolOverflows;

	/// Total number of allocations
	t_size					m_uTotalAllocations;

	/// Current number of allocations
	t_size					m_uCurrentAllocations;

	/// Maximum number of allocations
	t_size					m_uPeakAllocations;
};
