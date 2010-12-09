/*------------------------------------------------------------------
// util.h
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

class CUtil
{
public:

	// Graph types
	enum
	{
		eGtNone		= 0x010000,
		eGtHorz		= 0x020000,
		eGtVert		= 0x030000,
		eGtHorzFft	= 0x040000,
		eGtVertFft	= 0x050000,

		eDtMask		= 0x0000ff,
		eGtMask		= 0xff0000
	};

private:

	CUtil() {}

	virtual ~CUtil() {}

public:

	/// Creates md5 of char8 string
	static CStr8 md5( CStr8 s );

	/// Creaets md5 of wchar string
	static CStrW md5( CStrW s );

	/// Enable / disable output capture
	static oexBOOL EnableOutputCapture( oexUINT x_uBuffers, oexUINT x_uSize );

	/// Enable / disable output capture to file
	static oexBOOL EnableOutputFileCapture( oexCSTR x_pFile, oexUINT x_uTimestampFreq );

	/// Add string to output capture
	static oexBOOL AddOutput( oexCSTR x_pStr, oexUINT x_uSize, oexBOOL x_bNewLine );

	/// Returns the output circular buffer or 0 if there is none
	static CFifoSync* getOutputBuffer();

	/// Creates a ascii view of a binary buffer
	static CStr BinToAsciiHexStr( CBin *x_pBin, oexSIZE_T x_uLen, oexSIZE_T x_nLineLen, oexSIZE_T x_nMaxLines );
	static CStr BinToAsciiHexStr( const CBin &x_rBin, oexSIZE_T x_uLen, oexSIZE_T x_nLineLen, oexSIZE_T x_nMaxLines )
	{	return BinToAsciiHexStr( &x_rBin, x_uLen, x_nLineLen, x_nMaxLines ); }

	/// Creates a ascii view of a binary buffer
	static CStr BinToAsciiHexStr( oexCPVOID x_pBuf, oexSIZE_T x_uLen, oexSIZE_T x_nLineLen, oexSIZE_T x_nMaxLines );

	/// Creates an ascii 'fingerprint' of the specified binary buffer
	static CStr Fingerprint( CBin *buf, oexINT w, oexINT h, oexINT frame );

	/// Creates a fingerprint image (currently only supports RGB3 )
	static oexBOOL Fingerprint( CBin *buf, CBin *img, oexINT fmt, oexINT w, oexINT h, CBin *col, oexINT scale );

	/// Creates an ascii mandelbrot image
	static CStr Mandelbrot( int w, int h, int x1, int y1, int x2, int y2 );

    /// Concatinates two strings into a path
	static CStr BuildPath( oexINT x_nId, oexCONST CStr &x_sPath, oexTCHAR tSep = oexTCPathSep( oexTCHAR ) );

	/// Bresenham's line drawing
	static oexBOOL DrawLine( CBin *img, oexINT fmt, oexINT w, oexINT h, oexINT sw, oexINT *pc, oexINT x1, oexINT y1, oexINT x2, oexINT y2 );

	/// Draws a graph of an array of float values
	static oexBOOL GraphFloat( CBin *img, oexINT fmt, oexINT w, oexINT h, oexINT sw, oexINT *pc, oexFLOAT *pf, oexINT n, oexFLOAT scale, oexFLOAT min, oexFLOAT max );

	/// Draws the specified graph
	static oexBOOL Graph( oexINT nSamples, oexINT nInterval, oexINT nType, CBin *img, oexINT fmt, oexINT w, oexINT h, oexINT sw, oexINT *fg, oexINT *bg, oexCPVOID p, oexINT n, oexCSTR pParams );

};


