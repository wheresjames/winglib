/*------------------------------------------------------------------
// sq_binary.cpp
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

#include "../stdafx.h"

using namespace sqbind;

#define REGISTER_TYPE_ACCESS( t )	\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, set##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, get##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, setAbs##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, getAbs##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, LE_set##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, LE_get##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, LE_setAbs##t )		\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, LE_getAbs##t )		\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, BE_set##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, BE_get##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, BE_setAbs##t )		\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, BE_getAbs##t )		\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, find##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, size##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Allocate##t )

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqBinary, CSqBinary )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Allocate )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Free )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Existing )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Resize )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Zero )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Copy )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Append )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, LShift )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Insert )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, InsertBin )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, MemCpyAt )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Share )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Size )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, RawSize )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, PlainShare )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, IsPlainShare )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, AsciiHexStr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Fingerprint )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, FingerprintBin )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, FingerprintImage )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, GroupAvg )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, GraphFloat )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Graph )		
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Scale )		
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Threshold )		
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, get )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, set )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, getOffset )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, setOffset )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, SetName )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, GetName )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, setUsed )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, getUsed )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, setString )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, getString )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, appendString )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, setSubString )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, getSubString )	
	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, FindBin )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Sub )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, getSub )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, failed )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, base64_encode )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, base64_decode )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, base16_encode )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, base16_decode )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, compress )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, uncompress )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, base64_encode_str )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, base64_decode_str )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, base16_encode_str )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, base16_decode_str )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, compress_str )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, uncompress_str )

	REGISTER_TYPE_ACCESS( CHAR )
	REGISTER_TYPE_ACCESS( UCHAR )
	REGISTER_TYPE_ACCESS( SHORT )
	REGISTER_TYPE_ACCESS( USHORT )
	REGISTER_TYPE_ACCESS( INT )
	REGISTER_TYPE_ACCESS( UINT )
	REGISTER_TYPE_ACCESS( LONG )
	REGISTER_TYPE_ACCESS( ULONG )
	REGISTER_TYPE_ACCESS( FLOAT )
	REGISTER_TYPE_ACCESS( DOUBLE )

	SQBIND_ENUM( oex::CUtil::eGtHorz,		eGtHorz )
	SQBIND_ENUM( oex::CUtil::eGtVert,		eGtVert )
	SQBIND_ENUM( oex::CUtil::eGtHorzFft,	eGtHorzFft )
	SQBIND_ENUM( oex::CUtil::eGtVertFft,	eGtVertFft )

SQBIND_REGISTER_CLASS_END()

void CSqBinary::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqBinary );
}

int CSqBinary::FingerprintImage( CSqImage *img, CSqBinary *col, int scale )
{	if ( !img || !col ) return 0;
	CSqBinary buf; if ( !img->refPixels( &buf ) ) return 0;
	return oexFingerprint( &m_bin, &buf.Mem(), 0, img->getWidth(), img->getHeight(), &col->Mem(), scale );
}

int CSqBinary::GraphFloat( CSqImage *img, CSqColor *c, float scale, float min, float max )
{	if ( !img || !img->getUsed() || !getUsed() ) return 0;
	CSqBinary buf; if ( !img->refPixels( &buf ) ) return 0;
	return oex::CUtil::GraphFloat( &buf.Mem(), 0, img->getWidth(), img->getHeight(), img->getScanWidth(), c->Ptr(), (oex::oexFLOAT*)Ptr(), getUsed() / sizeof( oex::oexFLOAT ), scale, min, max );
}

int CSqBinary::Graph( int nSamples, int nInterval, int nType, CSqImage *img, CSqColor *bg, CSqColor *fg, const stdString &sParams )
{	if ( !img || !img->getUsed() || !getUsed() ) return 0;
	CSqBinary buf; if ( !img->refPixels( &buf ) ) return 0;
	return oex::CUtil::Graph( nSamples, nInterval, nType, &buf.Mem(), 0,
							  img->getWidth(), img->getHeight(), img->getScanWidth(), 
							  fg->Ptr(), bg->Ptr(), Ptr(), getUsed(), sParams.c_str() );
}
