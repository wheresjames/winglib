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

CStr CUtil::BinToAsciiHexStr( CBin *x_pBin, oexSIZE_T x_uLen, oexSIZE_T x_nLineLen, oexSIZE_T x_nMaxLines )
{_STT();

	if ( !x_pBin || !x_pBin->getUsed() )
		return oexT( "" );

	if ( !x_uLen || x_pBin->getUsed() < x_uLen )
		x_uLen = x_pBin->getUsed();

	return BinToAsciiHexStr( x_pBin->Ptr(), x_pBin->getUsed(), x_nLineLen, x_nMaxLines );
}

CStr CUtil::BinToAsciiHexStr( oexCPVOID x_pBuf, oexSIZE_T x_uLen, oexSIZE_T x_nLineLen, oexSIZE_T x_nMaxLines )
{_STT();

	// Sanity checks
	if ( !x_pBuf || !x_uLen || !x_nLineLen || !x_nMaxLines )
		return oexT( "" );

	// Allocate buffer for string
	oexSIZE_T nBytes = ( ( x_nLineLen * 4 ) + x_nLineLen + 4 ) * x_nMaxLines;
	CStr s; CStr::t_char *p = s.OexAllocate( nBytes );
	if ( !p )
		return oexT( "" );

	oexSIZE_T i = 0, b = 0;
	oexBYTE *pBuf = (oexBYTE*)x_pBuf;
	oexSIZE_T szNL = zstr::Length( oexNL8 );
	for ( oexSIZE_T l = 0; 0 < nBytes && i < x_uLen && ( !x_nMaxLines || l < x_nMaxLines ); l++, i += x_nLineLen )
	{
		for ( b = 0; 0 < nBytes && ( i + b ) < x_uLen && b < x_nLineLen; b++ )
			os::CSys::StrFmt( p, nBytes, oexT( "%0.2X " ), (oexUINT)pBuf[ i + b ] ),
			p += 3, nBytes -= 3;

		while ( 0 < nBytes && b < x_nLineLen )
			zstr::Copy( p, nBytes, oexT( "   " ) ),
			p += 3, nBytes -= 3, b++;

		// Append space
		zstr::Copy( p, nBytes, oexT( "  " ) ),
		p += 2, nBytes -= 2;

		for ( b = 0; 0 < nBytes && ( i + b ) < x_uLen && b < x_nLineLen; b++ )
			if ( pBuf[ i + b ] >= oexT( ' ' ) && pBuf[ i + b ] <= oexT( '~' ) )
				*( p++ ) = pBuf[ i + b ], nBytes--;
			else
				*( p++ ) = oexT( '.' ), nBytes--;

		// Append new line
		zstr::Copy( p, nBytes, oexNL ),
		p += szNL, nBytes -= szNL;

	} // end for

	return s;
}

CStr CUtil::Fingerprint( CBin *buf, oexINT w, oexINT h, oexINT frame )
{
	if ( !buf || !buf->getUsed() )
		return oexT( "" );

	// Glyphs
	int bits = 3;
	int mask = 0x07;
	oex::oexCSTR glyph = oexT( " .-+=|ox^*$@&#0()%!~:" );

	// Allocate space

	int sz = w * h;
	int snl = oex::zstr::Length( oexNL );

	oex::CStr s;
	oex::oexSTR p = s.OexAllocate( sz + ( h * snl ) );

	for ( int i = 0, y = 0; y < h; y++ )
	{
		// Init frame
		for ( int x = 0; x < w; x++ )
			if ( !frame || ( ( !y || ( y + 1 ) == h ) && ( !x || ( x + 1 ) == w ) ) )
				p[ i++ ] = oexT( ' ' );
			else if ( !y || ( y + 1 ) == h )
				p[ i++ ] = oexT( '-' );
			else if ( !x || ( x + 1 ) == w )
				p[ i++ ] = oexT( '|' );
			else
				p[ i++ ] = oexT( glyph[ 0 ] );

		// Terminate
		if ( y >= ( h - 1 ) )
			p[ i++ ] = 0;

		// New line
		else
		{	p[ i++ ] = oexNL[ 0 ];
			if ( 1 < snl ) p[ i++ ] = oexNL[ 1 ];
		} // end else

	} // end for

	// Hash the data
	oex::oexGUID hash;
	oex::oss::CMd5::Transform( &hash, buf->Ptr(), buf->getUsed() );
	oex::oexUCHAR *psrc = (oex::oexUCHAR*)&hash;
	oex::oexINT nsrc = sizeof( hash );

	// Draw something
	int x =psrc[ 0 ] % ( w - 2 );
	int y = psrc[ 1 ] % ( h - 2 );
	int d = 0, m = 0;
	int n = 16, max = nsrc * 8;

	while ( n < max )
	{
		// Keep cursor in bounds
		if ( x <= 1 ) x = w - 2;
		else if ( x >= ( w - 2 ) ) x = 1;
		if ( y <= 1 ) y = h - 2;
		else if ( y >= ( h - 2 ) ) y = 1;

		// Pick the glyph
		int g = 0, o = x + ( y * ( w + snl ) );
		if ( max > n ) oex::cmn::CopyBits( &g, 0, psrc, n, ( ( max - n ) < bits ) ? ( max - n ) : bits ), n += bits;
//		p[ o ] = glyph[ g ];

		// Increment colliding glyphs
		int u = g & mask;
		while ( glyph[ u ] && p[ o ] != glyph[ u ] ) u++;
		if ( glyph[ u ] && glyph[ u + 1 ] ) p[ o ] = glyph[ u + 1 ];
		else p[ o ] = glyph[ g & mask ];

		// Get new speed and direction
		if ( !m )
		{	if ( max > n )
				oex::cmn::CopyBits( &m, 0,psrc, n, ( ( max - n ) < 2 ) ? ( max - n ) : 2 ), n += 2;
			if ( max > n )
				oex::cmn::CopyBits( &d, 0, psrc, n, ( ( max - n ) < 2 ) ? ( max - n ) : 2 ), n += 2;
		} // end if
		else
			m--;

		// Move cursor
		switch( d )
		{	case 0 : x += 1; break;
			case 1 : x -= 1; break;
			case 2 : y += 1; break;
			default : y -= 1; break;
		} // end switch

	} // end while

	return s;
}


CStr CUtil::Mandelbrot( int w, int h, int x1, int y1, int x2, int y2 )
{
	CStr s;

	// Cool link
	// http://mrl.nyu.edu/~perlin/

	int k = 2;
	float mx = x2 - x1, my = y2 - y1;
	float sy = my / (float)h;
	float sx = mx / (float)w;

	float i, j, r, x = 0, y = y1;
	for ( int py = 0; py < h; py++ )
	{
		x = x1;
		for ( int px = 0; px < w; px++ )
		{
			i = j = k = r = 0;
			do
			{
				r = j;

				j = r * r - i * i - 2 + x / 25;

				i = 2 * r * i + y / 10;

			} while ( j * j + i * i < 11 && k++ < 111 );

//			s += oexT( " .:-;!/>)|&IH%*#" )[ k & 15 ];
			s += oexT( " .-:=!+coeCO80&#" )[ k & 15 ];

			x += sx;

		} // end for

		y += sy;
		s += oexNL;

	} // end while

	return s;
}

