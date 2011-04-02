/*------------------------------------------------------------------
// mem.cpp
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

//#include <cstdarg>
//#include <stdio.h>
//#include "stdint.h"
//#include <crtdefs.h>
//#include <varargs.h>
#include <stdarg.h>
#include <stdlib.h>

OEX_USING_NAMESPACE

namespace OEX_NAMESPACE {

#if !defined( OEX_ALIGNEDMEM ) || 0 == OEX_ALIGNEDMEM

oexPVOID oex_malloc( oexNEWSIZE_T x_nSize )
{
#if defined( oexDEBUG )
	oexPVOID p = malloc( (size_t)x_nSize );
	COex::GetMemLeak().Add( p );
	return p;
#else
	return malloc( (size_t)x_nSize );
#endif
}

oexPVOID oex_realloc( oexPVOID x_ptr, oexNEWSIZE_T x_nSize )
{
#if defined( oexDEBUG )
	oexPVOID p = realloc( x_ptr, x_nSize );
	COex::GetMemLeak().Remove( x_ptr );
	COex::GetMemLeak().Add( p );
	return p;
#else
	return realloc( x_ptr, x_nSize );
#endif
}

void oex_free( oexPVOID x_ptr )
{
#if defined( oexDEBUG )
	COex::GetMemLeak().Remove( x_ptr );
#endif
	return free( x_ptr );
}

#else

// Ensure OEX_ALIGNEDMEM is a power of 2
oexSTATIC_ASSERT( 0 == ( OEX_ALIGNEDMEM & ( OEX_ALIGNEDMEM - 1 ) ) );

oexPVOID oex_malloc( oexNEWSIZE_T x_nSize )
{
	// Allocate memory
	void *ptr = malloc( (size_t)x_nSize + OEX_ALIGNEDMEM + sizeof( void* ) );
	if ( !ptr )
		return oexNULL;

	// Align pointer
	oexBYTE *ptr2 = (oexBYTE*)ptr + sizeof( void* );
	ptr2 = (oexBYTE*)cmn::AlignN( (oexPtrType)ptr2, (oexPtrType)OEX_ALIGNEDMEM );

	// Save original pointer
	*( (void**)ptr2 - 1 ) = ptr;

#if defined( oexDEBUG )
	COex::GetMemLeak().Add( ptr2 );
#endif

	return ptr2;
}

oexPVOID oex_realloc( oexPVOID x_ptr, oexNEWSIZE_T x_nSize )
{
	if ( !x_ptr )
		return oex_malloc( x_nSize );

	// Get original pointer
	void *ptr = *( (void**)x_ptr - 1 );

	// Ensure it's sane
	oexASSERT( cmn::Dif( (oexULONG)ptr, (oexULONG)x_ptr ) <= OEX_ALIGNEDMEM + sizeof( void* ) );

	// Attempt realloc
	void *ptr2 = realloc( ptr, x_nSize + OEX_ALIGNEDMEM + sizeof( void* ) );
	if ( ptr2 == ptr )
		return x_ptr;

#if defined( oexDEBUG )
	COex::GetMemLeak().Remove( x_ptr );
#endif

	// Great, the os moved the block
	void *ptr3 = (oexBYTE*)ptr2 + ( (oexPtrType)x_ptr - (oexPtrType)ptr );
	*( (void**)ptr3 - 1 ) = ptr2;

	// Is it still aligned?
	if ( 0 == ( (oexPtrType)ptr3 & ( OEX_ALIGNEDMEM - 1 ) ) )
	{
#if defined( oexDEBUG )
		COex::GetMemLeak().Add( ptr3 );
#endif
		return ptr3;
	} // end if

//	oexEcho( "+++ inefficient realloc()" );

	// Well, now life sux
	// I don't know what to do except allocate another aligned block and copy
	void *ptr4 = oex_malloc( x_nSize );
	oexMemCpy( ptr4, ptr3, x_nSize );

	// Free the realloc() pointer
	free( ptr2 );

	// Did you get all that?
	return ptr4;
}

void oex_free( oexPVOID x_ptr )
{
	if ( !x_ptr )
		return;

#if defined( oexDEBUG )
	COex::GetMemLeak().Remove( x_ptr );
#endif

	// Get original pointer
	void *ptr = *( (void**)x_ptr - 1 );

	// Ensure it's sane
	oexASSERT( cmn::Dif( (oexULONG)ptr, (oexULONG)x_ptr ) <= OEX_ALIGNEDMEM + sizeof( void* ) );

	return free( ptr );
}

#endif

oexPVOID oex_calloc( oexSIZE_T x_nNum, oexNEWSIZE_T x_nSize )
{	oexSIZE_T sz = x_nNum * x_nSize;
	if ( !sz ) return 0;
	oexPVOID p = oex_malloc( sz );
	if ( !p ) return 0;
	oexZeroMemory( p, sz );
	return p;
}

}

/// Binary share object
static CBinShare				g_cBinShare;

/// Memory leak tracker
static CMemLeak					g_cMemLeak;

/// Logging
static CLog						g_cLog;

/// Resource object
static COexResourceHelper		g_cResourceHelper;

/// Stack trace object
static CStackTrace				g_cStaticInstance;


/// Raw allocator
SRawAllocator		CMem::m_def = { oex_malloc, oex_realloc, oex_free, &g_cBinShare, &g_cMemLeak, &g_cLog, &g_cResourceHelper, &g_cStaticInstance };
SRawAllocator		CMem::m_ra = { oex_malloc, oex_realloc, oex_free, &g_cBinShare, &g_cMemLeak, &g_cLog, &g_cResourceHelper, &g_cStaticInstance };

CStackTrace& CMem::GetStackTrace()
{
	CStackTrace *p = CMem::GetRawAllocator().pStackTrace;
	oexASSERT_PTR( p );
	return *p;
}


oexPVOID CMem::New( oexUINT x_uSize, oexUINT x_uLine, oexCSTR x_pFile )
{
    oexUCHAR *pBuf;

    _oexTRY
    {
		if ( m_ra.fMalloc )
			pBuf = (oexUCHAR*)m_ra.fMalloc( x_uSize );

		else
		{
			// +++ Figure out how to get the unicode file name into new without conversion

#if defined( oexUNICODE )

	        // Allocate buffer
    	    pBuf = oexNEW oexUCHAR[ x_uSize ];

#else

        	// Allocate buffer
        	pBuf = oexNEWAT( x_uLine, x_pFile ) oexUCHAR[ x_uSize ];

#endif

		} // end else

        if ( !oexVERIFY( pBuf ) )
            return oexNULL;

    } // end try

    _oexCATCH_ALL()
    {
		oexASSERT( 0 );

        return oexNULL;

    } // end catch

    return pBuf;
}

oexPVOID CMem::Resize( oexPVOID x_pMem, oexUINT x_uSize, oexUINT x_uLine, oexCSTR x_pFile )
{
	if ( !x_pMem )
		return New( x_uSize, x_uLine, x_pFile );

    _oexTRY
    {
		if ( m_ra.fRealloc )
			return m_ra.fRealloc( x_pMem, x_uSize );

	} // end try

	_oexCATCH_ALL()
	{
		oexASSERT( 0 );

	} // end catch

	return oexNULL;
}

void CMem::Delete( oexPVOID x_pMem )
{
    if ( !oexVERIFY_PTR_NULL_OK( x_pMem ) )
        return;

    if ( x_pMem )
    {

    	_oexTRY
	    {

			if ( m_ra.fFree )
				m_ra.fFree( x_pMem );
			else
				oexDELETE_ARR( (oexUCHAR*)x_pMem );

		} // end try

		_oexCATCH_ALL()
		{
			oexASSERT( 0 );

		} // end catch

	} // end if
}

