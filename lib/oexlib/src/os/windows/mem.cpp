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

#include "../../../oexlib.h"
#include "std_os.h"

#include <stdlib.h>

#if !defined( OEX_NOCRTDEBUG )
#	include <crtdbg.h>
#endif

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

static oexPVOID oex_malloc( oexINT x_nSize )
{	return malloc( (size_t)x_nSize ); }

static oexPVOID oex_realloc( oexPVOID x_ptr, oexINT x_nSize )
{	return realloc( x_ptr, x_nSize ); }

static void oex_free( oexPVOID x_ptr )
{	return free( x_ptr ); }

/// Raw allocator
SRawAllocator	CMem::m_def = { oex_malloc, oex_realloc, oex_free };
SRawAllocator	CMem::m_ra = { oex_malloc, oex_realloc, oex_free };

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

static void __cdecl oex_DumpHook( void * pUserData, size_t nBytes )
{
    CAlloc::ReportBlock( pUserData, nBytes );
}

oexBOOL CMem::MemReport()
{
#if defined( oexDEBUG ) && !defined( OEX_NOCRTDEBUG )

    _CrtMemState checkPt1;
    _CrtMemCheckpoint( &checkPt1 );
    _CrtMemDumpStatistics( &checkPt1 );
    _CrtCheckMemory( );
    return oexTRUE;

#else

	return oexFALSE;

#endif
}

oexBOOL CMem::DumpLeaks()
{
#if defined( OEX_NOCRTDEBUG )
	return oexFALSE;
#else

    // Custom dump function?
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )

    _CRT_DUMP_CLIENT pPdc = _CrtSetDumpClient( oex_DumpHook );

#endif

    oexBOOL bLeaks = _CrtDumpMemoryLeaks() ? oexTRUE : oexFALSE;

#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )

    _CrtSetDumpClient( pPdc );

#endif

    return bLeaks;

#endif
}


