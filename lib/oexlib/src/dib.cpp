/*------------------------------------------------------------------
// image.cpp
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

// +++ Packing on the some arm compilers are broken!!!
#ifndef OEX_PACKBROKEN
	oexSTATIC_ASSERT( 14 == sizeof( CDib::SDIBFileHeader ) );
#	define sizeof_SDIBFileHeader sizeof( CDib::SDIBFileHeader )
#else
#	define sizeof_SDIBFileHeader 14
#endif

oexBOOL CDib::Destroy()
{_STT();
	/// Destroy the image buffer
	m_image.Destroy();

	return oexTRUE;
}

oexBOOL CDib::Create( oexCSTR x_pShared, os::CFMap::t_HFILEMAP x_hShared, oexINT x_lWidth, oexINT x_lHeight, oexINT x_lBpp, oexBOOL x_bInit )
{_STT();
	// Lose any current image
	Destroy();

	// Calculate the size of the image
	oexINT lImageSize = GetScanWidth( x_lWidth, x_lBpp ) * cmn::Abs( x_lHeight );
	if ( !lImageSize )
		return oexFALSE;

	// Set shared name
	if ( x_pShared && *x_pShared )
	{	m_image.PlainShare( oexTRUE );
		m_image.SetName( x_pShared );
	} // end if

	// Set shared name
	else if ( x_hShared )
	{	m_image.PlainShare( oexTRUE );
		m_image.SetShareHandle( x_hShared );
	} // end else if

	// Allocate memory for the shared memory
	if ( !m_image.OexNew( sizeof( SImageData ) + lImageSize ).Ptr() )
	{	oexERROR( 0, oexMks( oexT( "Error allocating image data : s=" ), x_pShared,
							 oexT( ", fm=" ), oexPtrToInt( x_hShared ),
							 oexT( ", w=" ), x_lWidth,
							 oexT( ", h=" ), x_lHeight,
							 oexT( ", bpp=" ), x_lBpp ) );
		return oexFALSE;
	} // end if

	if ( x_bInit )
	{
		// Get a pointer to the image data
		SImageData *pId = Image();
		if ( !oexCHECK_PTR( pId ) )
	{	oexERROR( 0, oexMks( oexT( "Error allocating image data : s=" ), x_pShared,
							 oexT( ", fm=" ), oexPtrToInt( x_hShared ),
							 oexT( ", w=" ), x_lWidth,
							 oexT( ", h=" ), x_lHeight,
							 oexT( ", bpp=" ), x_lBpp ) );
			return oexFALSE;
		} // end if

		// Assume zero
		oexZeroMemory( pId, sizeof( SImageData ) );

		// Initialize image metrics
		pId->bih.biSize = sizeof( SBitmapInfoHeader );
		pId->bih.biWidth = x_lWidth;
		pId->bih.biHeight = x_lHeight;
		pId->bih.biPlanes = 1;
		pId->bih.biBitCount = x_lBpp;
		pId->bih.biCompression = SBitmapInfoHeader::eBiRgb;
		pId->bih.biSizeImage = lImageSize;

	} // end if

	return oexTRUE;
}

#define u8		char
#define u32	unsigned long

#define R(x,y,w) pRGB24[0 + 3 * ((x) + w * (y))]
#define G(x,y,w) pRGB24[1 + 3 * ((x) + w * (y))]
#define B(x,y,w) pRGB24[2 + 3 * ((x) + w * (y))]

#define Bay(x,y,w) pBay[(x) + w * (y)]

static void bayer_copy(u8 *pBay, u8 *pRGB24, int x, int y, int w)
{
	G(x + 0, y + 0, w) = Bay(x + 0, y + 0, w);
	G(x + 1, y + 1, w) = Bay(x + 1, y + 1, w);
	G(x + 0, y + 1, w) = G(x + 1, y + 0, w) = ((u32)Bay(x + 0, y + 0, w) + (u32)Bay(x + 1, y + 1, w)) >> 1;
	R(x + 0, y + 0, w) = R(x + 1, y + 0, w) = R(x + 1, y + 1, w) = R(x + 0, y + 1, w) = Bay(x + 0, y + 1, w);
	B(x + 1, y + 1, w) = B(x + 0, y + 0, w) = B(x + 0, y + 1, w) = B(x + 1, y + 0, w) = Bay(x + 1, y + 0, w);
}

static void bayer_bilinear(u8 *pBay, u8 *pRGB24, int x, int y, int w)
{
	R(x + 0, y + 0, w) = ((u32)Bay(x + 0, y + 1, w) + (u32)Bay(x + 0, y - 1, w)) >> 1;
	G(x + 0, y + 0, w) = Bay(x + 0, y + 0, w);
	B(x + 0, y + 0, w) = ((u32)Bay(x - 1, y + 0, w) + (u32)Bay(x + 1, y + 0, w)) >> 1;

	R(x + 0, y + 1, w) = Bay(x + 0, y + 1, w);
	G(x + 0, y + 1, w) = ((u32)Bay(x + 0, y + 0, w) + (u32)Bay(x + 0, y + 2, w)
			 			 + (u32)Bay(x - 1, y + 1, w) + (u32)Bay(x + 1, y + 1, w)) >> 2;
	B(x + 0, y + 1, w) = ((u32)Bay(x + 1, y + 0, w) + (u32)Bay(x - 1, y + 0, w)
			 			 + (u32)Bay(x + 1, y + 2, w) + (u32)Bay(x - 1, y + 2, w)) >> 2;

	R(x + 1, y + 0, w) = ((u32)Bay(x + 0, y + 1, w) + (u32)Bay(x + 2, y + 1, w)
			 			 + (u32)Bay(x + 0, y - 1, w) + (u32)Bay(x + 2, y - 1, w)) >> 2;
	G(x + 1, y + 0, w) = ((u32)Bay(x + 0, y + 0, w) + (u32)Bay(x + 2, y + 0, w)
			 			 + (u32)Bay(x + 1, y - 1, w) + (u32)Bay(x + 1, y + 1, w)) >> 2;
	B(x + 1, y + 0, w) = Bay(x + 1, y + 0, w);

	R(x + 1, y + 1, w) = ((u32)Bay(x + 0, y + 1, w) + (u32)Bay(x + 2, y + 1, w)) >> 1;
	G(x + 1, y + 1, w) = Bay(x + 1, y + 1, w);
	B(x + 1, y + 1, w) = ((u32)Bay(x + 1, y + 0, w) + (u32)Bay(x + 1, y + 2, w)) >> 1;
}

static void bayer_to_rgb24(u8 *pBay, u8 *pRGB24, int w, int h)
{_STT();
	int i, j;

	for (i = 0; i < w; i += 2)
		for (j = 0; j < h; j += 2)
			if (i == 0 || j == 0 || i >= w - 2 || j >= h - 2)
				bayer_copy(pBay, pRGB24, i, j, w);
			else
				bayer_bilinear(pBay, pRGB24, i, j, w);
}

oexBOOL CDib::SBGGR8toRGB24( oexPVOID x_pDst, oexPVOID x_pSrc, oexINT x_nWidth, oexINT x_nHeight )
{_STT();
	if ( !oexCHECK_PTR( x_pDst ) || !oexCHECK_PTR( x_pSrc ) || !x_nWidth || !x_nHeight )
		return oexFALSE;

	bayer_to_rgb24( (u8*)x_pSrc, (u8*)x_pDst, x_nWidth, x_nHeight );

	return oexTRUE;
}

oexBOOL CDib::Copy( oexPVOID x_pData, oexINT x_nSize )
{_STT();
	if ( !oexCHECK_PTR( x_pData ) || !x_nSize )
		return oexFALSE;

	if ( !m_image.IsValid() )
		return oexFALSE;

	if ( GetBufferSize() < x_nSize )
		x_nSize = GetBufferSize();

	oexMemCpy( GetBuffer(), x_pData, x_nSize );

	return oexTRUE;
}


oexBOOL CDib::CopySBGGR8( oexPVOID x_pData )
{_STT();
	if ( !m_image.IsValid() )
		return oexFALSE;

	// Copy data
	bayer_to_rgb24( (u8*)x_pData, (u8*)GetBuffer(), GetWidth(), GetHeight() );

	return oexTRUE;
}

oexBOOL CDib::CopyGrey( oexPVOID x_pData )
{_STT();
	if ( !m_image.IsValid() )
		return oexFALSE;

	oexCHAR *pSrc = (oexCHAR*)x_pData;
	oexCHAR *pDst = (oexCHAR*)GetBuffer();

	int w = GetWidth(), h = GetHeight();//, sw = GetScanWidth( w, 24 ) - w;

	for( int y = 0; y < h; y++ /*, pDst += sw */ )
		for( int x = 0; x < w; x++, pSrc++, pDst += 3 )
		{
			pDst[ 0 ] = pSrc[ 0 ];
			pDst[ 1 ] = pSrc[ 0 ];
			pDst[ 2 ] = pSrc[ 0 ];

		} // end for

	return oexTRUE;
}

oexBOOL CDib::SaveDibFile( oexCSTR x_pFile, SImageData *x_pId, oexCPVOID x_pData, oexINT x_nData )
{_STT();
	if ( !oexCHECK_PTR( x_pFile ) || !*x_pFile || !oexCHECK_PTR( x_pId ) || !oexCHECK_PTR( x_pData ) || !x_nData )
	{	oexERROR( -1, oexT( "Invalid parameter" ) );
		return oexFALSE;
	} // end if

	oex::CFile cFile;
	if ( !cFile.CreateAlways( x_pFile ).IsOpen() )
	{	oexERROR( cFile.GetLastError(), oexMks( oexT( "Error creating file : " ), x_pFile ) );
		return oexFALSE;
	} // end if


	// Fill in header info
	SDIBFileHeader dfh;
	dfh.uMagicNumber = SDIBFileHeader::eMagicNumber;
	dfh.uSize = sizeof_SDIBFileHeader + x_pId->bih.biSize + x_nData;
	dfh.uReserved1 = 0;
	dfh.uReserved2 = 0;
	dfh.uOffset = sizeof_SDIBFileHeader + x_pId->bih.biSize;

//	os::CSys::printf( "sizeof( SDIBFileHeader ) = %d\n", (int)sizeof( SDIBFileHeader ) );

	if ( !cFile.Write( &dfh, sizeof_SDIBFileHeader ) )
	{	oexERROR( cFile.GetLastError(), oexMks( oexT( "Error writing DIB header : " ), x_pFile ) );
		return oexFALSE;
	} // end if

	// In case the size is not right
	OEX_TRY
	{	if ( !cFile.Write( x_pId, x_pId->bih.biSize ) )
		{	oexERROR( cFile.GetLastError(), oexMks( oexT( "Error writing image header : " ), x_pFile ) );
			return oexFALSE;
		} // end if
	} // end try
	OEX_CATCH_ALL
	{	oexERROR( cFile.GetLastError(), oexMks( oexT( "Exception!  Perhaps biSize is not set correctly? : " ), x_pFile ) );
		return oexFALSE;
	} // end catchall

	OEX_TRY
	{
		if ( !cFile.Write( x_pData, x_nData ) )
		{	oexERROR( cFile.GetLastError(), oexMks( oexT( "Error writing image data : " ), x_pFile ) );
			return oexFALSE;
		} // end if
	}
	OEX_CATCH_ALL
	{	oexERROR( cFile.GetLastError(), oexMks( oexT( "Exception! Bad image buffer pointer? : " ), x_pFile ) );
		return oexFALSE;
	} // end catchall

	return oexTRUE;
}

// Integer conversion coefficients for Yuv2Rgb()
const oexINT	c_uShift = 12;
const oexINT	c_u2PowShift = 4096; // 2 ^ c_uShift
const oexINT	c_uUScale = (oexUINT)( (oexDOUBLE)1.772 * c_u2PowShift );
const oexINT	c_uVScale = (oexUINT)( (oexDOUBLE)1.402 * c_u2PowShift );
const oexINT	c_uYuScale = (oexUINT)( (oexDOUBLE)0.34414 * c_u2PowShift );
const oexINT	c_uYvScale = (oexUINT)( (oexDOUBLE)0.71414 * c_u2PowShift );

/**
	\param [in] lWidth			-	Width of video frame.
	\param [in] lHeight			-	Height of video frame.
	\param [in] pSrc			-	Pointer to YUV encoded buffer.
	\param [in] pDst			-	Pointer to DWORD aligned RGB buffer.
	\param [in] bGrayscale		-	Set to true for grayscale image.

	<b> YUV Format: </b>

	Each Y, U, and V are one byte.
	U and V are sub-sampled.

	\code

	| Y[ width * height ] | U[ width * height / 4 ] | V[ width * height / 4 ] |

	\endcode

	\return Zero if failure, otherwise non-zero.
*/
oexBOOL CDib::YUV_RGB_1(oexLONG lWidth, oexLONG lHeight, oexBYTE *pSrc, oexBYTE *pDst, oexBOOL bGrayscale)
{_STT();
	// Image params
	oexINT lScanWidth = GetScanWidth( lWidth, 24 );
//	oexINT lScanWidthDif = lScanWidth - ( lWidth * 3 );

	// Access image elements (image is top down)
	oexBYTE *pPixDst = pDst + ( lScanWidth * ( lHeight - 1 ) );

	// Source index offset
	oexUINT i = 0;

	// Grayscale
	if ( bGrayscale )

		// Just use Y
		for ( oexINT y = 0; y < lHeight; y++, pPixDst -= lScanWidth * 2 )
			for ( oexINT x = 0; x < lWidth; x++, pPixDst += 3, i++ )
				pPixDst[ 0 ] = pPixDst[ 1 ] = pPixDst[ 2 ] = pSrc[ i ];

	else
	{
		// Field params
		oexINT lFieldWidth = lWidth * lHeight;
		oexINT lUOffset = lFieldWidth;
		oexINT lVOffset = lFieldWidth + ( lFieldWidth >> 2 );

		oexINT lY, lT;
		oexCHAR lU, lV;

		oexBYTE *pU1 = &pSrc[ lUOffset ], *pU2 = &pSrc[ lUOffset ];
		oexBYTE *pV1 = &pSrc[ lVOffset ], *pV2 = &pSrc[ lVOffset ];

		// Color
		for ( oexINT y = 0; y < lHeight; y++, pPixDst -= lScanWidth * 2 )
		{
			for ( oexINT x = 0; x < lWidth; x++, pPixDst += 3, i++ )
			{
				// Get YUV values
				lY = pSrc[ i ];

				if ( !( y & 1 ) )
				{	lU = *pU1; lV = *pV1;
					if ( x & 1 ) pU1++, pV1++;
				} // end if
				else
				{	lU = *pU2; lV = *pV2;
					if ( x & 1 ) pU2++, pV2++;
				} // end if

				// Prescale
				lY <<= c_uShift; lU -= 128; lV -= 128;

				// Blue
				lT = ( lY + c_uUScale * lU ) >> c_uShift;
				pPixDst[ 0 ] = cmn::Range( lT, 0, 255 );

				// Green
				lT = ( lY - c_uYuScale * lU - c_uYvScale * lV ) >> c_uShift;
				pPixDst[ 1 ] = cmn::Range( lT, 0, 255 );

				// Red
				lT = ( lY + c_uVScale * lV ) >> c_uShift;
				pPixDst[ 2 ] = cmn::Range( lT, 0, 255 );

			} // end for

		} // end for

	} // end else

	return oexTRUE;
}

/**
	\param [in] lWidth			-	Width of video frame.
	\param [in] lHeight			-	Height of video frame.
	\param [in] pSrc			-	Pointer to YUV encoded buffer.
	\param [in] pDst			-	Pointer to DWORD aligned RGB buffer.
	\param [in] bGrayscale		-	Set to true for grayscale image.

	YUV Format:

	Each Y, U, and V are one byte.
	U and V are sub-sampled.
	For each Y[ i ] use U[ i / 2 ] and V[ i / 2 ].

	\code

	| Y[ width * height ] | U[ width * height / 2 ] | V[ width * height / 2 ] |

	\endcode

	\return Zero if failure, otherwise non-zero.
*/
oexBOOL CDib::YUV_RGB_2(oexLONG lWidth, oexLONG lHeight, oexBYTE *pSrc, oexBYTE *pDst, oexBOOL bGrayscale)
{_STT();
	// Image params
	oexINT lScanWidth = GetScanWidth( lWidth, 24 );
//	oexINT lScanWidthDif = lScanWidth - ( lWidth * 3 );

	// Access image elements (image is top down)
	oexBYTE *pPixDst = pDst + ( lScanWidth * ( lHeight - 1 ) );

	// Source index offset
	oexUINT i = 0;

	// Grayscale
	if ( bGrayscale )

		// Just use Y
		for ( oexINT y = 0; y < lHeight; y++, pPixDst -= lScanWidth * 2 )
			for ( oexINT x = 0; x < lWidth; x++, pPixDst += 3, i++ )
				pPixDst[ 0 ] = pPixDst[ 1 ] = pPixDst[ 2 ] = pSrc[ i ];

	else
	{
		// Field params
		oexINT lFieldWidth = lWidth * lHeight;
		oexINT lUOffset = lFieldWidth;
		oexINT lVOffset = lFieldWidth + ( lFieldWidth >> 1 );

		register oexINT lY, lT;
		register oexCHAR lU, lV;

		// Color
		for ( oexINT y = 0; y < lHeight; y++, pPixDst -= lScanWidth * 2 )
			for ( oexINT x = 0; x < lWidth; x++, pPixDst += 3, i++ )
			{
				// Get YUV values
				lY = pSrc[ i ];
				lU = pSrc[ lUOffset + ( i >> 1 ) ];
				lV = pSrc[ lVOffset + ( i >> 1 ) ];

				// Prescale
				lY <<= c_uShift; lU -= 128; lV -= 128;

				// Blue
				lT = ( lY + c_uUScale * lU ) >> c_uShift;
				pPixDst[ 0 ] = cmn::Range( lT, 0, 255 );

				// Green
				lT = ( lY - c_uYuScale * lU - c_uYvScale * lV ) >> c_uShift;
				pPixDst[ 1 ] = cmn::Range( lT, 0, 255 );

				// Red
				lT = ( lY + c_uVScale * lV ) >> c_uShift;
				pPixDst[ 2 ] = cmn::Range( lT, 0, 255 );

			} // end for

	} // end else

	return oexTRUE;
}

oexBOOL CDib::YUYV_RGB(oexLONG lWidth, oexLONG lHeight, oexPVOID pSrc, oexPVOID pDst, oexBOOL bGrayscale)
{_STT();
	// Image params
	oexINT lScanWidth = GetScanWidth( lWidth, 24 );
	oexINT lScanWidthDif = lScanWidth - ( lWidth * 3 );

	// Access image
	oexBYTE *pPixDst = (oexBYTE*)pDst;
	oexBYTE *pPixSrc = (oexBYTE*)pSrc;

	// Grayscale
	if ( bGrayscale )

		// Gray scale copy
		for ( oexINT y = 0; y < lHeight; y++, pPixDst += lScanWidthDif )
			for ( oexINT x = 0; x < lWidth; x += 2, pPixDst += 6, pPixSrc += 4 )
				pPixDst[ 0 ] = pPixDst[ 1 ] = pPixDst[ 2 ] = pPixSrc[ 0 ],
				pPixDst[ 3 ] = pPixDst[ 4 ] = pPixDst[ 5 ] = pPixSrc[ 2 ];

	else
		for ( oexINT y = 0; y < lHeight; y++, pPixDst += lScanWidthDif )
			for ( oexINT x = 0; x < lWidth; x += 2, pPixDst += 6, pPixSrc += 4 )
			{
				// Get YUV values
				oexINT lY1 = pPixSrc[ 0 ];
				oexINT lU = pPixSrc[ 1 ];
				oexINT lY2 = pPixSrc[ 2 ];
				oexINT lV = pPixSrc[ 3 ];

				// Prescale
				lY1 <<= c_uShift;
				lY2 <<= c_uShift;
				lU -= 128;
				lV -= 128;

				// Blue
				oexINT lT = ( lY1 + c_uUScale * lU ) >> c_uShift;
				pPixDst[ 0 ] = cmn::Range( lT, 0, 255 );

				// Green
				lT = ( lY1 - c_uYuScale * lU - c_uYvScale * lV ) >> c_uShift;
				pPixDst[ 1 ] = cmn::Range( lT, 0, 255 );

				// Red
				lT = ( lY1 + c_uVScale * lV ) >> c_uShift;
				pPixDst[ 2 ] = cmn::Range( lT, 0, 255 );


				// Blue
				lT = ( lY2 + c_uUScale * lU ) >> c_uShift;
				pPixDst[ 3 ] = cmn::Range( lT, 0, 255 );

				// Green
				lT = ( lY2 - c_uYuScale * lU - c_uYvScale * lV ) >> c_uShift;
				pPixDst[ 4 ] = cmn::Range( lT, 0, 255 );

				// Red
				lT = ( lY2 + c_uVScale * lV ) >> c_uShift;
				pPixDst[ 5 ] = cmn::Range( lT, 0, 255 );

			} // end for

	return oexTRUE;
}
