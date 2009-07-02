/*------------------------------------------------------------------
// base_16.cpp
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

oexBYTE CBase16::m_b16encode[] =
{
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

oexBYTE CBase16::m_b16decode[] =
{
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff,
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
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};


CBase16::CBase16()
{
}

CBase16::~CBase16()
{

}

oexINT CBase16::Decode( oexCSTR8 src, oexINT *srcsize, oexBYTE *dst, oexINT *dstsize, oexINT *done )
{
	oexINT x = 0, y = 0;
	oexINT b = 0;
	oexINT max = *srcsize;

	while ( x < max )
	{
		oexBYTE ch = m_b16decode[ (oexINT)src[ x++ ] ];

		if ( !( ch & 0x80 ) )
		{
			if ( !b )
				b++, dst[ y ] = ch << 4;
			else
			{	b = 0, dst[ y++ ] |= ch;
				*srcsize = x;
				*dstsize = y;
			} // end else

		} // end if

	} // end while

	return y;
}

CStr8 CBase16::Decode( oexCSTR8 src, oexINT srcsize )
{
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


oexINT CBase16::Encode( oexSTR8 dst, oexINT dstsize, oexCONST oexBYTE *src, oexINT srcsize )
{
	oexINT x = 0, y = 0;

	// Base 16 encoder
	while( x < srcsize && ( y + 1 ) < dstsize )
	{	dst[ y++ ] = m_b16encode[ 0xf & ( src[ x ] >> 4 ) ];
		dst[ y++ ] = m_b16encode[ 0xf & src[ x++ ] ];
	} // end while

	// NULL terminate for good measure
	if ( y < dstsize )
		dst[ y ] = 0;

	return y;
}

CStr8 CBase16::Encode( oexCPVOID src, oexINT srcsize )
{
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


