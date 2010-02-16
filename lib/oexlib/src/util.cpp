/*------------------------------------------------------------------
// util.cpp
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

CStr8 CUtil::md5( CStr8 s )
{_STT();
	oexGUID hash;
	return CBase16::Encode( oss::CMd5::Transform( &hash, s.Ptr(), s.Length() ), sizeof( hash ) );

}

CStrW CUtil::md5( CStrW s )
{_STT();
#if !defined( OEX_NOWCHAR )
	oexGUID hash;
	CStr8 sMb = oexStrWToMb( s );
	CStrW _s = oexMbToStrW( CBase16::Encode( OEX_NAMESPACE::oss::CMd5::Transform( &hash, sMb.Ptr(), sMb.Length() ), sizeof( hash ) ) );
  	return _s;
#endif
}

// +++ This isn't really thread safe, it's ok if we assume that
//     Output capture will only be turned off after all other threads
//	   have been released, which is currently the case.
//	   PS : CCircBuf is internally thread safe.
//static CCircBuf *g_pCircBuf = oexNULL;
static CFifoSync *g_pFifoSync = oexNULL;
oexBOOL CUtil::EnableOutputCapture( oexUINT x_uBuffers, oexUINT x_uSize )
{
	if ( x_uBuffers && x_uSize && !g_pFifoSync )
	{	if ( 4 > x_uSize ) x_uSize = 4;
		g_pFifoSync = OexAllocConstruct< CFifoSync >( oexFALSE, x_uSize, 
													  CCircBuf::eWmOverwrite, x_uBuffers );
	} // end if
	else if ( ( !x_uBuffers || !x_uSize ) && g_pFifoSync )
		OexAllocDelete( g_pFifoSync ), g_pFifoSync = oexNULL;
	return oexTRUE;

/*
	if ( x_uSize && !g_pCircBuf )
	{	if ( 32 > x_uSize ) x_uSize = 32;
		g_pCircBuf = OexAllocConstruct< CCircBuf >( oexFALSE, x_uSize, CCircBuf::eWmOverwrite );
	} // end if
	else if ( !x_uSize )
		OexAllocDelete( g_pCircBuf ), g_pCircBuf = oexNULL;
	return oexTRUE;
*/	
}

//CCircBuf* CUtil::getOutputBuffer()
CFifoSync* CUtil::getOutputBuffer()
{
	return g_pFifoSync;
//	return g_pCircBuf;
}

oexBOOL CUtil::AddOutput( oexCSTR x_pStr, oexUINT x_uSize, oexBOOL x_bNewLine )
{
	// Sanity checks
	if ( !g_pFifoSync || !x_pStr || !*x_pStr )
		return oexFALSE;

	if ( !x_uSize )
		x_uSize = zstr::Length( x_pStr );

	if ( !x_uSize )
		return oexFALSE;

	// Write the data to the buffer
	g_pFifoSync->Write( x_pStr, x_uSize * sizeof( x_pStr[ 0 ] ) );

	return oexTRUE;

/*
	// Sanity checks
	if ( !g_pCircBuf || !x_pStr || !*x_pStr )
		return oexFALSE;

	if ( !x_uSize )
		x_uSize = zstr::Length( x_pStr );

	if ( !x_uSize )
		return oexFALSE;

	// Write the data to the buffer
	g_pCircBuf->Write( x_pStr, x_uSize * sizeof( x_pStr[ 0 ] ) );

	if ( x_bNewLine )
		g_pCircBuf->Write( oexNL, oexNL_LEN * sizeof( oexCHAR ) );

	return oexTRUE;
*/
}

