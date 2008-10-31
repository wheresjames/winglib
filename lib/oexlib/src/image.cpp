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

// +++ Packing on the arm compiler is broken!!!
#ifndef OEX_ARM
	oexSTATIC_ASSERT( 14 == sizeof( CImage::SDIBFileHeader ) );
#	define sizeof_SDIBFileHeader sizeof( CImage::SDIBFileHeader )
#else
#	define sizeof_SDIBFileHeader 14
#endif

oexBOOL CImage::Destroy()
{
	/// Destroy the image buffer
	m_image.Destroy();

	return oexTRUE;
}

oexBOOL CImage::Create( oexCSTR x_pShared, os::CFMap::t_HFILEMAP x_hShared, oexINT x_lWidth, oexINT x_lHeight, oexINT x_lBpp, oexBOOL x_bInit )
{
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
	{	oexERROR( -1, CStr().Fmt( oexT( "Error allocating image data : s=%s, fm=%d, w=%d, h=%d, bpp=%d" ), oexStrToMbPtr( x_pShared ), (oexINT)x_hShared, x_lWidth, x_lHeight, x_lBpp ) );
		return oexFALSE;
	} // end if

	if ( x_bInit )
	{
		// Get a pointer to the image data
		SImageData *pId = Image();
		if ( !oexCHECK_PTR( pId ) )
		{	oexERROR( -1, CStr().Fmt( oexT( "Invalid image data pointer : s=%s, fm=%d, w=%d, h=%d, bpp=%d" ), oexStrToMbPtr( x_pShared ), (oexINT)x_hShared, x_lWidth, x_lHeight, x_lBpp ) );
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
	G(x + 0, y + 1, w) = G(x + 1, y + 0, w) = ((u32)Bay(x + 0, y + 0, w) + (u32)Bay(x + 1, y + 1, w)) / 2;
	R(x + 0, y + 0, w) = R(x + 1, y + 0, w) = R(x + 1, y + 1, w) = R(x + 0, y + 1, w) = Bay(x + 0, y + 1, w);
	B(x + 1, y + 1, w) = B(x + 0, y + 0, w) = B(x + 0, y + 1, w) = B(x + 1, y + 0, w) = Bay(x + 1, y + 0, w);
}

static void bayer_bilinear(u8 *pBay, u8 *pRGB24, int x, int y, int w)
{
	R(x + 0, y + 0, w) = ((u32)Bay(x + 0, y + 1, w) + (u32)Bay(x + 0, y - 1, w)) / 2;
	G(x + 0, y + 0, w) = Bay(x + 0, y + 0, w);
	B(x + 0, y + 0, w) = ((u32)Bay(x - 1, y + 0, w) + (u32)Bay(x + 1, y + 0, w)) / 2;

	R(x + 0, y + 1, w) = Bay(x + 0, y + 1, w);
	G(x + 0, y + 1, w) = ((u32)Bay(x + 0, y + 0, w) + (u32)Bay(x + 0, y + 2, w)
			 			 + (u32)Bay(x - 1, y + 1, w) + (u32)Bay(x + 1, y + 1, w)) / 4;
	B(x + 0, y + 1, w) = ((u32)Bay(x + 1, y + 0, w) + (u32)Bay(x - 1, y + 0, w)
			 			 + (u32)Bay(x + 1, y + 2, w) + (u32)Bay(x - 1, y + 2, w)) / 4;

	R(x + 1, y + 0, w) = ((u32)Bay(x + 0, y + 1, w) + (u32)Bay(x + 2, y + 1, w)
			 			 + (u32)Bay(x + 0, y - 1, w) + (u32)Bay(x + 2, y - 1, w)) / 4;
	G(x + 1, y + 0, w) = ((u32)Bay(x + 0, y + 0, w) + (u32)Bay(x + 2, y + 0, w)
			 			 + (u32)Bay(x + 1, y - 1, w) + (u32)Bay(x + 1, y + 1, w)) / 4;
	B(x + 1, y + 0, w) = Bay(x + 1, y + 0, w);

	R(x + 1, y + 1, w) = ((u32)Bay(x + 0, y + 1, w) + (u32)Bay(x + 2, y + 1, w)) / 2;
	G(x + 1, y + 1, w) = Bay(x + 1, y + 1, w);
	B(x + 1, y + 1, w) = ((u32)Bay(x + 1, y + 0, w) + (u32)Bay(x + 1, y + 2, w)) / 2;
}

static void bayer_to_rgb24(u8 *pBay, u8 *pRGB24, int w, int h)
{
	int i, j;

	for (i = 0; i < w; i += 2)
		for (j = 0; j < h; j += 2)
			if (i == 0 || j == 0 || i == w - 2 || j == h - 2)
				bayer_copy(pBay, pRGB24, i, j, w);
			else
				bayer_bilinear(pBay, pRGB24, i, j, w);
}

oexBOOL CImage::CopySBGGR8( oexPVOID x_pData )
{
	if ( !m_image.IsValid() )
		return oexFALSE;

	// Copy data
	bayer_to_rgb24( (u8*)x_pData, (u8*)GetBuffer(), GetWidth(), GetHeight() );

	return oexTRUE;
}

oexBOOL CImage::CopyGrey( oexPVOID x_pData )
{
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

oexBOOL CImage::SaveDibFile( oexCSTR x_pFile, SImageData *x_pId, oexCPVOID x_pData, oexINT x_nData )
{
	if ( !oexCHECK_PTR( x_pFile ) || !*x_pFile || !oexCHECK_PTR( x_pId ) || !oexCHECK_PTR( x_pData ) || !x_nData )
	{	oexERROR( -1, oexT( "Invalid parameter" ) );
		return oexFALSE;
	} // end if

	oex::CFile cFile;
	if ( !cFile.CreateAlways( x_pFile ).IsOpen() )
	{	oexERROR( cFile.GetLastError(), CStr().Fmt( oexT( "Error creating file : %s" ), oexStrToMbPtr( x_pFile ) ) );
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
	{	oexERROR( cFile.GetLastError(), CStr().Fmt( oexT( "Error writing DIB header: %s" ), oexStrToMbPtr( x_pFile ) ) );
		return oexFALSE;
	} // end if

	// In case the size is not right
	OEX_TRY
	{	if ( !cFile.Write( x_pId, x_pId->bih.biSize ) )
		{	oexERROR( cFile.GetLastError(), CStr().Fmt( oexT( "Error writing image header: %s" ), oexStrToMbPtr( x_pFile ) ) );
			return oexFALSE;
		} // end if
	} // end try
	OEX_CATCH_ALL
	{	oexERROR( cFile.GetLastError(), CStr().Fmt( oexT( "Exception!  Perhaps biSize is not set correctly?: %s" ), oexStrToMbPtr( x_pFile ) ) );
		return oexFALSE;
	} // end catchall

	OEX_TRY
	{
		if ( !cFile.Write( x_pData, x_nData ) )
		{	oexERROR( cFile.GetLastError(), CStr().Fmt( oexT( "Error writing image data: %s" ), oexStrToMbPtr( x_pFile ) ) );
			return oexFALSE;
		} // end if
	}
	OEX_CATCH_ALL
	{	oexERROR( cFile.GetLastError(), CStr().Fmt( oexT( "Exception! Bad image buffer pointer?: %s" ), oexStrToMbPtr( x_pFile ) ) );
		return oexFALSE;
	} // end catchall

	return oexTRUE;
}

