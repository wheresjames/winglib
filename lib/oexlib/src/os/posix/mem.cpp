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

//#include <crtdbg.h>

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;


oexPVOID CMem::New( oexUINT x_uSize, oexUINT x_uLine, oexCSTR x_pFile )
{
    oexUCHAR *pBuf;

    try
    {
        // +++ Figure out how to get the unicode file name into new without conversion

#if defined( oexUNICODE )

        // Allocate buffer
        pBuf = oexNEW oexUCHAR[ x_uSize ];

#else

        // Allocate buffer
        pBuf = oexNEWAT( x_uLine, x_pFile ) oexUCHAR[ x_uSize ];

#endif

        if ( !oexVERIFY( pBuf ) )
            return oexNULL;

    } // end try

    catch( ... )
    {
        return oexNULL;

    } // end catch

    return pBuf;
}

void CMem::Delete( oexPVOID x_pMem )
{
    if ( !oexVERIFY_PTR_NULL_OK( x_pMem ) )
        return;

    if ( x_pMem )
        oexDELETE_ARR( (oexUCHAR*)x_pMem );
}

static void /*_cdecl*/ oex_DumpHook( void * pUserData, size_t nBytes )
{
    CAlloc::ReportBlock( pUserData, nBytes );
}

oexBOOL CMem::MemReport()
{
#if defined( _DEBUG )

    _CrtMemState checkPt1;
    _CrtMemCheckpoint( &checkPt1 );
    _CrtMemDumpStatistics( &checkPt1 );       
    _CrtCheckMemory( );

#endif
    return oexTRUE;
}

oexBOOL CMem::DumpLeaks()
{
    // Custom dump function?
#if defined( _DEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
    
    _CRT_DUMP_CLIENT pPdc = _CrtSetDumpClient( oex_DumpHook );

#endif

    oexBOOL bLeaks = oexFALSE; //_CrtDumpMemoryLeaks() ? oexTRUE : oexFALSE;

#if defined( _DEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )

    _CrtSetDumpClient( pPdc );

#endif

    return bLeaks;
}


