/*------------------------------------------------------------------
// base_64.cpp
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

OEX_USING_NAMESPACE

oexBYTE CBase64::m_b64encode[] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
};

oexBYTE CBase64::m_b64decode[] =
{
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0x3e, 0xff, 0xff, 0xff, 0x3f,
	0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
	0x3c, 0x3d, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff,
	0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
	0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
	0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
	0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
	0x31, 0x32, 0x33, 0xff, 0xff, 0xff, 0xff, 0xff,

	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

CBase64::CBase64()
{_STT();
}

CBase64::~CBase64()
{_STT();

}

oexINT CBase64::Decode( oexCSTR8 src, oexINT *srcsize, oexBYTE *dst, oexINT *dstsize, oexINT *done )
{_STT();

	oexINT x = 0, y = 0;
	oexINT b = 0;
	oexINT max = *srcsize;

	while( x < max )
	{
		oexBYTE ch = m_b64decode[ (oexINT)src[ x++ ] ];

		if ( !( ch & 0x80 ) )
		{
			switch ( b )
			{
				case 0 :
					dst[ y ] = ch << 2;
					break;

				case 1 :
					dst[ y++ ] |= ( ch & 0x30 ) >> 4;
					dst[ y ] = ( ch & 0x0f ) << 4;
					break;

				case 2 :
					dst[ y++ ] |= ( ch & 0x3c ) >> 2;
					dst[ y ] = ( ch & 0x03 ) << 6;
					break;

				case 3 :
					dst[ y++ ] |= ( ch & 0x3f );
					break;

			} // end switch

			// increment state machine
			if ( ++b > 3 )
			{	b = 0;
				*srcsize = x;
				*dstsize = y;
			} // end if

		} // end if

		// Is this the end of the stream?
		else if ( ch == 0xfe )
		{
			*srcsize = x;
			*dstsize = y;
			if ( done )
				*done = 1;
			return y;
		} // end else if

	} // end while

	return y;
}

CStr8 CBase64::Decode( oexCSTR8 src, oexINT srcsize )
{_STT();

	// Verify pointers
	if ( !oexCHECK_PTR( src ) || !srcsize )
		return CStr8();

	CStr8 	sRet;
	oexINT 	nDecodedSize = GetDecodedSize( srcsize );
	if ( !sRet.OexAllocate( nDecodedSize ) )
		return CStr8();

	// Do the decode
	nDecodedSize = Decode( src, &srcsize, (oexBYTE*)sRet._Ptr(), &nDecodedSize, oexNULL );
	sRet.SetLength( nDecodedSize );

	return sRet;
}


oexINT CBase64::Encode( oexSTR8 dst, oexINT dstsize, oexCONST oexBYTE *src, oexINT srcsize )
{_STT();

	oexINT x = 0, y = 0;
	oexINT b = 0;
	oexINT line = 0;

	oexINT val = 0;

	while( x < srcsize && y < dstsize )
	{
		switch( b )
		{
			case 0 :
				val = src[ x ] >> 2;
				break;

			case 1 :
				val = ( src[ x++ ] & 0x03 ) << 4;
				if ( x < srcsize ) val |= src[ x ] >> 4;
				break;

			case 2 :
				val = ( src[ x++ ] & 0x0f ) << 2;
				if ( x < srcsize ) val |= src[ x ] >> 6;
				break;

			case 3 :
				val = src[ x++ ] & 0x3f;
				break;

		} // end switch

		// Next byte mask
		if ( ++b > 3 ) b = 0;

		// Save encoded value
		dst[ y++ ] = m_b64encode[ val ];

		// Line Wrap
		if ( ++line >= eMaxLineLength && ( y + 2 ) < dstsize )
		{	line = 0;
			dst[ y++ ] = '\r';
			dst[ y++ ] = '\n';
		} // end if

	} // end while

	// Add padding
	switch ( srcsize % 3 )
	{
		case 1 :
			dst[ y++ ] = '=';
		case 2 :
			dst[ y++ ] = '=';

	} // end switch

	// NULL terminate for good measure
	if ( y < dstsize )
		dst[ y ] = 0;

	return y;
}

CStr8 CBase64::Encode( oexCPVOID src, oexINT srcsize )
{_STT();

	// Verify pointers
	if ( !oexCHECK_PTR( src ) || !srcsize )
		return CStr8();

	CStr8 sRet;
	oexINT 	nEncodedSize = GetEncodedSize( srcsize );
	if ( !sRet.OexAllocate( nEncodedSize ) )
		return CStr8();

	// Do the decode
	nEncodedSize = Encode( sRet._Ptr(), nEncodedSize, (oexCONST oexBYTE*)src, srcsize );
	sRet.SetLength( nEncodedSize );

	return sRet;
}


