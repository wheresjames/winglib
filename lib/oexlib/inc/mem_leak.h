/*------------------------------------------------------------------
// mem_leak.h
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
#	define OEX_MEMLEAK_DEBUG
#else
#	undef OEX_MEMLEAK_DEBUG
#endif


/// Memory leak tracking
class CMemLeak
{
public:

	enum
	{
		// Pool size in bits ( 16 = 65536, 18 = 262144, 20 = 1048576 )
		eDefaultPoolBits = 20
	};

	enum
	{
		// Slot mask
		eDefaultSlotMask = 0x3f
	};

	// Size type
	typedef oexSIZE_T		t_size;

public:

	/// Constructor
	CMemLeak();

	/// Destructor
	virtual ~CMemLeak();

	/// Releases all resources
	void Destroy();

	/// Creates the leak detection pool
	oexBOOL Create( t_size x_nPoolBits = eDefaultPoolBits, t_size x_nSlotMask = eDefaultSlotMask );

	/// Adds a pointer to the tracking pool
	oexINT Add( oexCPVOID p );

	/// Verify's that a pointer is in the pool
	oexINT Check( oexCPVOID p );

	/// Removes a pointer from the pool
	oexINT Remove( oexCPVOID p );

	/// Creates a report of the memory situation
	CStr Report( t_size *pLeaks = oexNULL );

	/// Freezes memory allocation statistics
	void Freeze() { m_bFreeze = oexTRUE; }

	/// Unfreezes memory allocation statistics
	void Unfreeze() { m_bFreeze = oexFALSE; }

private:

	/// Non-zero to freeze memory tracking
	oexBOOL					m_bFreeze;

	/// The pointer pool
	oexCPVOID				*m_pPool;

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

#if defined( OEX_MEMLEAK_DEBUG )

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
