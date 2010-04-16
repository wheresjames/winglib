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

	/// Add string to output capture
	static oexBOOL AddOutput( oexCSTR x_pStr, oexUINT x_uSize, oexBOOL x_bNewLine );

	/// Returns the output circular buffer or 0 if ther is none
	static CFifoSync* getOutputBuffer();

	/// Creates a ascii view of a binary buffer
	static CStr BinToAsciiHexStr( CBin *x_pBin, oexSIZE_T x_uLen, oexSIZE_T x_nLineLen, oexSIZE_T x_nMaxLines );

	/// Creates a ascii view of a binary buffer
	static CStr BinToAsciiHexStr( oexCPVOID x_pBuf, oexSIZE_T x_uLen, oexSIZE_T x_nLineLen, oexSIZE_T x_nMaxLines );

	/// Creates an ascii 'fingerprint' of the specified binary buffer
	static CStr Fingerprint( CBin *buf, oexINT w, oexINT h, oexINT frame );

	/// Creates an ascii mandelbrot image
	static CStr Mandelbrot( int w, int h, int x1, int y1, int x2, int y2 );

};


