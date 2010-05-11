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

#define _STTEX()

#if defined( OEX_NOIMAGE )
#	undef OEX_ENABLE_XIMAGE
#endif

#if defined( OEX_ENABLE_XIMAGE )
#define CXIMAGE_CUSTOM_ALLOCATOR	1 
//#define CX_FORCE_CUSTOM_ALLOCATOR	1
#	include "../../../../CxImage/ximage.h"
#endif

#if !defined( CXIMAGE_SUPPORT_JPG )
#	undef OEX_ENABLE_XIMAGE
#endif

#if defined( OEX_ENABLE_XIMAGE )

#include <setjmp.h>

extern "C" {
 #include "../../../../jpeg/jpeglib.h"
 #include "../../../../jpeg/jerror.h"
}

// Just ensure our structures are at least the same size
oexSTATIC_ASSERT( sizeof( RECT) == sizeof( oexSRect ) );
oexSTATIC_ASSERT( sizeof( RGBQUAD ) == sizeof( oexSRgbQuad ) );
oexSTATIC_ASSERT( sizeof( COLORREF ) == sizeof( oexSColorRef ) );


//==================================================================
// CCxCustomImg
//
/// Custom CxImage class
/**
	This class is completely encapsulated and hidden by CImage
*/
//==================================================================
class CCxCustomImg : public CxImage
{
public:

};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static void* _cximage_malloc( size_t size )
{
	return oex::CMem::New( size, oexLINE, oexTEXT( oexFILE ) );
}

static void* _cximage_calloc( size_t num, size_t size )
{
	void *ptr = oex::CMem::New( num * size, oexLINE, oexTEXT( oexFILE ) );
	oexZeroMemory( ptr, num * size );
	return ptr;
}

static void* _cximage_realloc( void *ptr, size_t size )
{
	return oex::CMem::Resize( ptr, size, oexLINE, oexTEXT( oexFILE ) );
}

static void _cximage_free( void *ptr )
{
	return oex::CMem::Delete( ptr );
}

extern "C"
{	t_cx_malloc cximage_malloc = _cximage_malloc;
	t_cx_calloc cximage_calloc = _cximage_calloc;
	t_cx_realloc cximage_realloc = _cximage_realloc;
	t_cx_free cximage_free = _cximage_free;
};

#endif

CImage::CImage()
{_STT();

#if defined( OEX_ENABLE_XIMAGE )

	m_pimg = OexAllocConstruct< CCxCustomImg >();

	m_pEncoderState = oexNULL;

	m_bTransparent = oexFALSE;
	m_rgbTransparent = 0; //RGB( 0, 0, 0 );

	oexZeroMemory( &m_rect, sizeof( m_rect ) );

	m_pMem = oexNULL;
	m_uMemSize = 0;

#endif

}

CImage::CImage( const CImage &img )
{
#if defined( OEX_ENABLE_XIMAGE )

	m_pimg = OexAllocConstruct< CCxCustomImg >();

	m_pEncoderState = oexNULL;

	m_bTransparent = oexFALSE;
	m_rgbTransparent = 0; //RGB( 0, 0, 0 );

	oexZeroMemory( &m_rect, sizeof( m_rect ) );

    Copy( (CImage*)&img );

	m_pMem = oexNULL;
	m_uMemSize = 0;
#endif
}


CImage::~CImage()
{_STT();

	Destroy();

}

void CImage::Destroy()
{_STT();

#if defined( OEX_ENABLE_XIMAGE )

	// Lose
	m_filename.Destroy();

	// Release encode memory
	ReleaseEncodeMemory();

	// Release encoder memory
	ReleaseEncodeJpg();

	// Get image object
	if ( oexCHECK_PTR( m_pimg ) )
		OexAllocDestruct( (CCxCustomImg*)m_pimg );
	m_pimg = oexNULL;

#endif
}

CStr CImage::GetLastError()
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return CStr();
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexT( "Invalid Object" );
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Return last error string
	return oexMbToStr( pimg->GetLastError() );
#endif
}

oexINT CImage::GetFileType( oexCSTR x_pFile )
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return -1;
#else

// Sanity check
	if ( !oexCHECK_PTR( x_pFile ) )
		return -1;

	oexINT type = -1;
	CStr sExt = CStr( x_pFile ).GetFileExtension();
	if ( !sExt.Length() )
		sExt = x_pFile;

	sExt.ToLower();

	if ( 0 );

	// Check for bitmap
#if defined( CXIMAGE_SUPPORT_JPG ) && CXIMAGE_SUPPORT_JPG
	else if ( sExt == oexT( "jpg" ) || sExt == oexT( "jpeg" ) ) type = CXIMAGE_FORMAT_JPG;
#endif
#if defined( CXIMAGE_SUPPORT_BMP ) && CXIMAGE_SUPPORT_BMP
	else if ( sExt == oexT( "bmp" ) || sExt == oexT( "dib" ) ) type = CXIMAGE_FORMAT_BMP;
#endif
#if defined( CXIMAGE_SUPPORT_PNG ) && CXIMAGE_SUPPORT_PNG
	else if ( sExt == oexT( "png" ) ) type = CXIMAGE_FORMAT_PNG;
#endif
#if defined( CXIMAGE_SUPPORT_WMF ) && CXIMAGE_SUPPORT_WMF
	else if ( sExt == oexT( "wmf" ) ) type = CXIMAGE_FORMAT_WMF;
	else if ( sExt == oexT( "emf" ) ) type = CXIMAGE_FORMAT_WMF;
#endif
#if defined( CXIMAGE_SUPPORT_GIF ) && CXIMAGE_SUPPORT_GIF
	else if ( sExt == oexT( "gif" ) ) type = CXIMAGE_FORMAT_GIF;
#endif
//#if defined( CXIMAGE_SUPPORT_MNG ) && CXIMAGE_SUPPORT_MNG
//	else if ( sExt == oexT( "mng" ) ) type = CXIMAGE_FORMAT_MNG;
//#endif
#if defined( CXIMAGE_SUPPORT_ICO ) && CXIMAGE_SUPPORT_ICO
	else if ( sExt == oexT( "ico" ) ) type = CXIMAGE_FORMAT_ICO;
#endif
#if defined( CXIMAGE_SUPPORT_TIF ) && CXIMAGE_SUPPORT_TIF
	else if ( sExt == oexT( "tif" ) || sExt == oexT( "tiff" ) ) type = CXIMAGE_FORMAT_TIF;
#endif
#if defined( CXIMAGE_SUPPORT_TGA ) && CXIMAGE_SUPPORT_TGA
	else if ( sExt == oexT( "tga" ) ) type = CXIMAGE_FORMAT_TGA;
#endif
#if defined( CXIMAGE_SUPPORT_PCX ) && CXIMAGE_SUPPORT_PCX
	else if ( sExt == oexT( "pcx" ) ) type = CXIMAGE_FORMAT_PCX;
#endif
//#if defined( CXIMAGE_SUPPORT_JP2 ) && CXIMAGE_SUPPORT_JP2
//	else if ( sExt == oexT( "jp2" ) ) type = CXIMAGE_FORMAT_JP2;
//#endif

	return type;

#endif
}

oexBOOL CImage::Load( oexCSTR x_pFile, oexCSTR x_pType )
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	if ( !oexCHECK_PTR( x_pFile ) )
		return oexFALSE;

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Copy filename
	m_filename = x_pFile;

	// Get the file type
	DWORD type = GetFileType( ( oexCHECK_PTR( x_pType ) && *x_pType ) ? x_pType : x_pFile );

	if ( (DWORD)-1 == type )
		return oexFALSE;

#if defined( OEX_WINCE )

	// Load the file
	oexBOOL bRet = pimg->Load( x_pFile, type );

#else

	// Load the file
	oexBOOL bRet = pimg->Load( oexStrToMbPtr( x_pFile ), type );

#endif

	GetRect();

	return bRet;

#endif
}

oexBOOL CImage::Save( oexCSTR x_pFile, oexCSTR x_pType )
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Copy filename
	m_filename = x_pFile;

	// Get the file type
	DWORD type = GetFileType( ( oexCHECK_PTR( x_pType ) && *x_pType ) ? x_pType : x_pFile );

	if ( (DWORD)-1 == type )
		return oexFALSE;

#if defined( OEX_WINCE )

	// Load the file
	return pimg->Save( x_pFile, type );

#else

	// Load the file
	return pimg->Save( oexStrToMbPtr( x_pFile ), type );

#endif

#endif
}

oexBOOL CImage::Decode( oexPBYTE x_buf, oexINT x_size, oexCSTR x_pType )
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Copy filename
	if ( oexCHECK_PTR( x_pType ) )
		m_filename = x_pType;

	// Get the file type
	DWORD type = ( oexCHECK_PTR( x_pType ) && *x_pType ) ? GetFileType( x_pType ) : -1;
	if ( (DWORD)-1 == type )
		return oexFALSE;

	// Decode the image
	if ( !pimg->Decode( x_buf, x_size, type ) )
		return oexFALSE;

	GetRect();

	return TRUE;
#endif
}

CBin& CImage::Encode( oexCSTR x_pType )
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return m_mem;
#else

	// Encode the image
	oexPBYTE pBuf; oexINT nSize;
	if ( !Encode( &pBuf, &nSize, x_pType ) || !pBuf || 0 >= nSize )
		return m_mem;

	// +++ Be nice to replace this copy with the CBin::setBuffer()
	//     mentioned a few lines down.

	// Copy the image
	m_mem.MemCpy( (CBin::t_byte*)pBuf, nSize );

	return m_mem;

#endif
}

oexINT CImage::Encode( oexPBYTE *x_buf, oexINT *x_pnSize, oexCSTR x_pType )
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return 0;
#else

	// Lose old memory
	ReleaseEncodeMemory();

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return 0;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Copy filename
	if ( oexCHECK_PTR( x_pType ) )
		m_filename = x_pType;

	// Get the file type
	DWORD type = ( oexCHECK_PTR( x_pType ) && *x_pType ) ? GetFileType( x_pType ) : -1;
	if ( (DWORD)-1 == type )
		return 0;

	// Encode the image
	BYTE *pMem = 0;
	long lSize = 0;
	if ( !pimg->Encode( pMem, lSize, type ) || !oexCHECK_PTR( pMem ) || 0 >= lSize )
		return 0;

	// Save pointers
	m_pMem = pMem;
	m_uMemSize = (oexUINT)lSize;

	// +++ This would be nice, it would save a copy, but to make it work
	//     we must ensure that xImage uses oex_malloc() to allocate
	//     the memory.
//	m_mem.setBuffer( (CBin::t_byte*)pMem, lSize );

	// Save data
	if ( oexCHECK_PTR( x_buf ) )
		 *x_buf = (oexPBYTE)m_pMem;
	if ( oexCHECK_PTR( x_pnSize ) )
		*x_pnSize = m_uMemSize;

	return lSize;

#endif
}

void CImage::ReleaseEncodeMemory()
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return;
#else

	// Hmmm.... not releasing this means we won't have to realloc
	// it all the time
//	m_mem.Destroy();

	// Release memory buffer
	if ( oexCHECK_PTR( m_pimg ) && oexCHECK_PTR( m_pMem ) )
			((CCxCustomImg*)m_pimg)->FreeMemory( (void*)m_pMem );

	m_uMemSize = 0;
	m_pMem = oexNULL;

#endif
}

/*
BOOL CImage::Draw(HDC hDC, LPRECT pRect, BOOL bStretch)
{_STT();
	// Sanity checks
	if ( hDC == NULL || pRect == NULL )
		return oexFALSE;

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Stretch?
	if ( bStretch )
	{
		// use decent looking stretch mode for color
		int oldsbm = SetStretchBltMode( hDC, HALFTONE );
		SetBrushOrgEx( hDC, 0, 0, NULL );

		// Draw the image
		BOOL ret = pimg->Draw( hDC, *pRect );

		// Restore stretch mode
		SetStretchBltMode( hDC, oldsbm );

		return ret;

	} // end if

	// Ironically, it is actually more trouble not to stretch
	RECT rect;
	CopyRect( &rect, pRect );

	// Set true width and height
	rect.right = rect.left + pimg->GetWidth();
	rect.bottom = rect.top + pimg->GetHeight();

	// Do it
	return pimg->Draw( hDC, rect );
}
*/

oexINT CImage::GetWidth()
{_STTEX();

#if !defined( OEX_ENABLE_XIMAGE )
	return 0;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return 0;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->GetWidth();

#endif
}

oexINT CImage::GetHeight()
{_STTEX();

#if !defined( OEX_ENABLE_XIMAGE )
	return 0;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return 0;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->GetHeight();

#endif
}

oexBOOL CImage::IsValid()
{_STTEX();

#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->IsValid();
#endif
}
/*
BOOL CImage::CreateFromHBITMAP(HBITMAP hBmp)
{_STT();
	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Create the image
	pimg->CreateFromHBITMAP( hBmp );

	GetRect();

	return pimg->IsValid();
}
*/
oexBOOL CImage::SetQuality(oexINT q)
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Create the image
	pimg->SetJpegQuality( (BYTE)q );

	return oexTRUE;
#endif
}
/*
HICON CImage::CreateIcon(long w, long h, COLORREF bck, HDC hDC)
{_STT();
	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexNULL;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	HICON hIcon = NULL;

	// Get a template DC
	BOOL bReleaseDC = FALSE;
	if ( hDC == NULL )
	{	bReleaseDC = TRUE;
		hDC = GetDC( NULL );
	} // end if

	// Create a bitmap
	HBITMAP hBmp = CreateCompatibleBitmap( hDC, w, h );
	if ( hBmp == NULL )
	{	if ( bReleaseDC ) ReleaseDC( NULL, hDC );
		return FALSE;
	} // end if

	// Create the mask
	HBITMAP hMask = CreateCompatibleBitmap( hDC, w, h );
	if ( hMask == NULL )
	{	DeleteObject( hBmp );
		if ( bReleaseDC ) ReleaseDC( NULL, hDC );
		return FALSE;
	} // end if

	// Create a DC
	HDC hCDC = CreateCompatibleDC( hDC );
	if ( hCDC == NULL )
	{	DeleteObject( hBmp );
		DeleteObject( hMask );
		if ( bReleaseDC ) ReleaseDC( NULL, hDC );
		return FALSE;
	} // end if

	// Select the bitmap
	HBITMAP oldbmp = (HBITMAP)SelectObject( hCDC, hBmp );

	// Prepare the icon bitmap
	RECT	rect;
	SetRect( &rect, 0, 0, w, h );

	// Fill in the background
	CGrDC::FillSolidRect( hCDC, &rect, bck );

	// Correct aspect ratio for image
	CGrDC::AspectCorrect( &rect, pimg->GetWidth(), pimg->GetHeight() );

	// Draw the metafile into the icon bitmap
	Draw( hCDC, &rect );

	// Fill in the mask with zeros
	SelectObject( hCDC, hMask );
	FillRect( hCDC, &rect, (HBRUSH)GetStockObject( BLACK_BRUSH ) );

	// Lose the DC
	SelectObject( hCDC, oldbmp );
	DeleteDC( hCDC );

	// Fill in icon information
	ICONINFO	ii;
	ii.fIcon = TRUE;
	ii.xHotspot = w / 2;
	ii.yHotspot = h / 2;
	ii.hbmMask = hMask;
	ii.hbmColor = hBmp;

	// Create a new icon
	hIcon = CreateIconIndirect( &ii );

	// Clean up
	DeleteObject( hBmp );
	DeleteObject( hMask );
	if ( bReleaseDC ) ReleaseDC( NULL, hDC );

	return hIcon;
}
*/
oexBOOL CImage::Mirror()
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->Mirror();
#endif
}

oexBOOL CImage::Flip()
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->Flip();
#endif
}

oexBOOL CImage::GrayScale()
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->GrayScale();
#endif
}

oexBOOL CImage::Negative()
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->Negative();
#endif
}

oexBOOL CImage::Rotate(oexFLOAT x_angle, CImage *x_pDst)
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	CCxCustomImg *pdst = oexNULL;
	if ( oexCHECK_PTR( x_pDst ) && oexCHECK_PTR( x_pDst->m_pimg ) )
		pdst = (CCxCustomImg*)x_pDst->m_pimg;

	return pimg->Rotate( x_angle, pdst );
#endif
}

oexBOOL CImage::Resample(oexINT x_newx, oexINT x_newy, oexINT x_fast)
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !IsValid() )
		return oexFALSE;

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->Resample( x_newx, x_newy, x_fast );

#endif
}

oexBOOL CImage::Dither(oexINT x_method)
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !IsValid() )
		return oexFALSE;

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->Dither( x_method );

#endif
}

oexBOOL CImage::Crop(oexINT x_left, oexINT x_top, oexINT x_right, oexINT x_bottom, CImage *x_pDst)
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	CCxCustomImg *pdst = oexNULL;
	if ( oexCHECK_PTR( x_pDst ) && oexCHECK_PTR( x_pDst->m_pimg ) )
		pdst = (CCxCustomImg*)x_pDst->m_pimg;

	return pimg->Crop( x_left, x_top, x_right, x_bottom, pdst );

#endif
}

oexBOOL CImage::Noise(oexINT x_level)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->Noise( x_level );

#endif
}

oexBOOL CImage::Median(oexINT x_Ksize)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->Median( x_Ksize );

#endif
}

oexBOOL CImage::Gamma(oexFLOAT x_gamma)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->Gamma( x_gamma );

#endif
}

oexBOOL CImage::ShiftRGB(oexINT x_r, oexINT x_g, oexINT x_b)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->ShiftRGB( x_r, x_g, x_b );
#endif
}

oexBOOL CImage::Threshold(oexINT x_level)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->Threshold( (oexBYTE)x_level );
#endif
}

oexBOOL CImage::Colorize(oexINT x_hue, oexINT x_sat)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->Colorize( (oexBYTE)x_hue, (oexBYTE)x_sat );
#endif
}

oexBOOL CImage::Light(oexINT x_level, oexINT x_contrast)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->Light( x_level, x_contrast );
#endif
}


oexBOOL CImage::Erode(oexINT x_Ksize)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->Erode( x_Ksize );
#endif
}

oexBOOL CImage::Dilate(oexINT x_Ksize)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->Dilate( x_Ksize );
#endif
}

oexINT CImage::Histogram(oexLONG *x_red, oexLONG *x_green, oexLONG *x_blue, oexLONG *x_gray, oexLONG x_colorspace)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return 0;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return 0;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->Histogram( x_red, x_green, x_blue, x_gray, x_colorspace );
#endif
}

/*
oexBOOL CImage::LoadFromResource(LPCTSTR pResource, LPCTSTR pResType, LPCTSTR pImgType, HMODULE hModule)
{_STT();
	// Sanity checks
	if ( pResource == NULL || pResType == NULL || pImgType == NULL )
		return oexFALSE;

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	DWORD	size = 0;
	char	*buf = NULL;

	// Get resource size
	if ( !CWinFile::LoadResource( pResource, NULL, &size, pResType, hModule ) || size == 0 )
		return FALSE;

	// Allocate memory
	buf = new char[ size + 1 ];
	if ( buf == NULL ) return FALSE;

	// Load resource
	if ( !CWinFile::LoadResource( pResource, (LPBYTE)buf, &size, pResType, hModule ) )
	{	delete [] buf;
		return FALSE;
	} // end if

	// Decode the image
	oexBOOL bRet = Decode( (LPBYTE)buf, size, pImgType );

	// Release memory
	delete [] buf;

	return bRet;
}
*/

oexPVOID CImage::GetBits()
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexNULL;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexNULL;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->GetBits();
#endif
}

oexINT CImage::GetEffWidth()
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return 0;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return 0;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->GetEffWidth();
#endif
}

oexBOOL CImage::Create(oexINT x_w, oexINT x_h, oexINT x_bpp)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Just just return if we already have the image they want
	if ( (oexINT)pimg->GetWidth() == x_w
	     && (oexINT)pimg->GetHeight() == x_h
	     && pimg->GetBpp() == x_bpp )
		return TRUE;

	return ( pimg->Create( x_w, x_h, x_bpp ) != NULL );
#endif
}

oexBOOL CImage::Copy(CImage *pImg)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Ensure image to copy
	if ( !oexCHECK_PTR( pImg ) || !oexCHECK_PTR( pImg->m_pimg ) )
		return oexFALSE;

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Copy the thing
	pimg->Copy( *( (CCxCustomImg*)pImg->m_pimg ) );

	return IsValid();
#endif
}

oexBOOL CImage::Resample( oexSRect *x_pRect, oexINT x_fast)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return pimg->Resample( cmn::RW( x_pRect ), cmn::RH( x_pRect ), x_fast );
#endif
}

/*
HBITMAP CImage::CreateHBITMAP( HDC hDC )
{_STT();
	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexNULL;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Create bitmap
	return pimg->MakeBitmap( hDC );
}
*/

oexBOOL CImage::IsTransparent()
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Create bitmap
	return pimg->IsTransparent();
#endif
}

oexBOOL CImage::SetTransColor( oexSRgbQuad x_rgb )
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Create bitmap
	pimg->SetTransColor( *(RGBQUAD*)&x_rgb );

	return TRUE;
#endif
}

oexSRgbQuad CImage::GetTransColor()
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexSRgbQuad();
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
	{	oexSRgbQuad rgb = { 0, 0, 0, 0 }; return rgb; }
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Create bitmap
	register RGBQUAD rgb = pimg->GetTransColor();
	return *(oexSRgbQuad*)&rgb;
#endif
}

oexINT CImage::GetBpp()
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Create bitmap
	return pimg->GetBpp();
#endif
}

oexBOOL CImage::IncreaseBpp(oexINT x_bpp)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Create bitmap
	return pimg->IncreaseBpp( x_bpp );
#endif
}

oexSRgbQuad CImage::GetPixel(oexINT x_x, oexINT x_y)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexSRgbQuad();
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
	{	oexSRgbQuad rgb = { 0, 0, 0, 0 }; return rgb; }
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Create bitmap
	register RGBQUAD rgb = pimg->GetPixelColor( x_x, x_y );
	return *(oexSRgbQuad*)&rgb;
#endif
}

oexBOOL CImage::SetPixel(oexINT x_x, oexINT x_y, oexSRgbQuad x_rgb)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Create bitmap
	pimg->SetPixelColor( x_x, x_y, *(RGBQUAD*)&x_rgb );

	return oexTRUE;
#endif
}

oexINT CImage::GetTransIndex()
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return 0;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Create bitmap
	return pimg->GetTransIndex();
#endif
}

oexBOOL CImage::SetTransIndex(oexINT x_index)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Create bitmap
	pimg->SetTransIndex( x_index );

	return oexTRUE;
#endif
}

oexINT CImage::GetNearestIndex(oexSRgbQuad x_rgb)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return 0;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Create bitmap
	return pimg->GetNearestIndex( *(RGBQUAD*)&x_rgb );
#endif
}

oexBOOL CImage::DecreaseBpp(oexINT x_bpp, oexBOOL x_bErrorDiffusion, oexSRgbQuad *x_ppal)
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Create bitmap
	return pimg->DecreaseBpp( x_bpp, x_bErrorDiffusion, (RGBQUAD*)x_ppal );
#endif
}

/*
oexBOOL CImage::FromClipboard( HWND hWnd )
{_STT();
	// Lose current image
	Destroy();

	// Is there an image on the clipboard
	if ( !IsClipboardFormatAvailable( CF_DIB ) )
		return FALSE;

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Open clipboard
	if ( !::OpenClipboard( hWnd ) ) return FALSE;

	HANDLE hData = GetClipboardData( CF_DIB );
	if ( hData != NULL )

		// Load the image from the handle
		pimg->CreateFromHANDLE( hData );


	// Close the clipboard
	::CloseClipboard();

	// Did we get a valid image
	return pimg->IsValid();
}

oexBOOL CImage::ToClipboard( HWND hWnd )
{_STT();
	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Open clipboard
	if ( !::OpenClipboard( hWnd ) ) return FALSE;

	// Lose current clipboard contents
	EmptyClipboard();

	// Get image handle
	HANDLE hData = pimg->CopyToHandle();
	if ( hData != NULL )

		// Set the data to the clipboard
		::SetClipboardData( CF_DIB, hData );

	// Close the clipboard
	::CloseClipboard();

	return TRUE;
}
*/

oexBOOL CImage::HasAlpha()
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Create bitmap
	return pimg->IsTransparent();
#endif
}

oexBOOL CImage::AlphaDelete()
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Create bitmap
	pimg->AlphaDelete();

	return oexTRUE;
#endif
}
/*
oexBOOL CImage::Tile(HDC hDC, LPRECT pRect)
{_STT();
	// Sanity checks
	if ( hDC == NULL || pRect == NULL ) return FALSE;

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return oexFALSE;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	pimg->Tile( hDC, pRect );

	return TRUE;

}
*/

#ifdef ENABLE_OPENCV

IplImage* CImage::InitIplHeader( CImage *pImg, IplImage *pIpl )
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return oexNULL;
#else

	ZeroMemory( pIpl, sizeof( IplImage ) );

	// Fill in the structure
	pIpl->nSize = sizeof( IplImage );
	pIpl->nChannels = 3;
	pIpl->depth = IPL_DEPTH_8U;
	pIpl->dataOrder = 0;
	pIpl->origin = 1;
	pIpl->align = 4;
	pIpl->width = pImg->GetWidth();
	pIpl->height = pImg->GetHeight();
	pIpl->imageSize = CWinDc::GetScanWidth( pImg->GetWidth() ) * pImg->GetHeight();
	pIpl->imageData = (char*)pImg->GetBits();
	pIpl->widthStep = CWinDc::GetScanWidth( pImg->GetWidth() );
	pIpl->imageDataOrigin = (char*)pImg->GetBits();

	return pIpl;
#endif
}

#endif

#if defined( OEX_ENABLE_XIMAGE )

typedef class s_tagCxMemFile : public CxMemFile
{public:
	s_tagCxMemFile() : CxMemFile() {};
	s_tagCxMemFile( oexPBYTE x_pBuf, oexINT x_nSize ) : CxMemFile()
	{	Attach( x_pBuf, x_nSize ); }
	virtual void Alloc(oexINT nBytes) { oexASSERT( 0 ); }
	void Attach( oexPBYTE x_pBuf, oexINT x_nSize )
	{	m_pBuffer = x_pBuf; m_Edge = x_nSize; m_Size = m_Position = 0; m_bFreeOnClose = false; }
	void Detach() { GetBuffer( true ); }
}s_CxMemFile;

#endif

oexINT CImage::Encode( oexPBYTE x_pBuf, oexINT x_nSize, oexCSTR x_pType )
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return 0;
#else

	// Sanity checks
	if ( oexCHECK_PTR( x_pBuf ) || !x_nSize )
		return 0;

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return 0;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	// Use filename if not supplied
	if ( !oexCHECK_PTR( x_pType ) )
		x_pType = m_filename.Ptr();

	// Get the file type
	DWORD type = GetFileType( x_pType );
	if ( (DWORD)-1 == type )
		return 0;

	// Wrap the memory pointer
	s_CxMemFile mf( x_pBuf, x_nSize );

	// Encode the image
	if ( !pimg->Encode( &mf, type ) )
	{	mf.Detach(); return 0; }

	// Get the number of bytes encoded
	oexINT nEncoded = mf.Size();

	// Detach from the buffer
	mf.Detach();

	return nEncoded;

#endif
}

oexINT CImage::GetImageSize()
{_STT();
#if !defined( OEX_ENABLE_XIMAGE )
	return 0;
#else

	// Get image object
	if ( !oexCHECK_PTR( m_pimg ) )
		return 0;
	CCxCustomImg *pimg = (CCxCustomImg*)m_pimg;

	return ( pimg->GetEffWidth() * pimg->GetHeight() );
#endif
}

oexINT CImage::CopyBits( oexPVOID x_buf, oexINT x_nSize )
{
#if !defined( OEX_ENABLE_XIMAGE )
	return 0;
#else

	// Sanity checks
	if ( !x_buf || !x_nSize )
		return 0;

	// Get image size
	oexINT nSize = GetImageSize();
	if ( nSize > x_nSize )
		nSize = x_nSize;

	// Get destination buffer
	oexPVOID pBits = GetBits();
	if ( !pBits )
		return 0;

	// Copy the data
	memcpy( pBits, x_buf, nSize );

	return nSize;
#endif
}

oexINT CImage::GetBits( oexPVOID x_buf, oexINT x_nSize )
{
#if !defined( OEX_ENABLE_XIMAGE )
	return 0;
#else

	// Sanity checks
	if ( !x_buf || !x_nSize )
		return 0;

	// Get image size
	oexINT nSize = GetImageSize();
	if ( nSize > x_nSize )
		nSize = x_nSize;

	// Get destination buffer
	oexPVOID pBits = GetBits();
	if ( !pBits )
		return 0;

	// Copy the data
	memcpy( x_buf, pBits, nSize );

	return nSize;
#endif
}

#if defined( OEX_ENABLE_XIMAGE )

class CJpegErr : public jpeg_error_mgr
{
public:

	CJpegErr() {}

	void Init( jpeg_compress_struct *pJcs )
	{_STT();

		// Error string
		*m_szError = 0;

		// Error function
		error_exit = JpegErrorExit;
		pJcs->err = jpeg_std_error( this );

		// Set jump address
		setjmp( m_jbErr );
	}

	static void JpegErrorExit( j_common_ptr cinfo )
	{_STT();

		CJpegErr *pJe = (CJpegErr*)cinfo;
		if ( pJe == NULL ) return;

		pJe->format_message( cinfo, pJe->m_szError );

	}

private:

	jmp_buf			m_jbErr;
	char			m_szError[ 256 ];
};

class CJpegDst : public jpeg_destination_mgr
{
public:

	CJpegDst()
	{	next_output_byte = NULL; free_in_buffer = 0;
		init_destination = &InitDestination;
		empty_output_buffer = &EmptyOutputBuffer;
		term_destination = &TermDestination;
	}

	void Init( oexPBYTE pBuf, oexULONG dwSize )
	{	next_output_byte = pBuf;
		free_in_buffer = dwSize;
	}

	// Buffer functions
	static void InitDestination(j_compress_ptr cinfo) { }
	static boolean EmptyOutputBuffer(j_compress_ptr cinfo) { return true; }
	static void TermDestination(j_compress_ptr cinfo) { }

};

typedef struct tagSJpegEncState
{
	struct jpeg_compress_struct		cinfo;
	CJpegErr						jerr;
	CJpegDst						dmgr;

	long							lWidth;
	long							lHeight;

} SJpegEncState;

#endif

oexBOOL CImage::InitEncodeJpg( oexINT x_nWidth, oexINT x_nHeight, oexINT x_nQuality )
{
#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Lose old encoder
	ReleaseEncodeJpg();

	// Allocate memory for encoder
	SJpegEncState *pState = new SJpegEncState;
	m_pEncoderState = (void*)pState;

	// Save image size
	pState->lWidth = x_nWidth;
	pState->lHeight = x_nHeight;

	// Initialize cinfo structure
	oexZeroMemory( &pState->cinfo, sizeof( pState->cinfo ) );

	// Initialize error handling
	pState->jerr.Init( &pState->cinfo );

	// Create compressor
	jpeg_create_compress( &pState->cinfo );

	// Initialize encode structure
	pState->cinfo.dest = &pState->dmgr;
	pState->cinfo.image_width = x_nWidth;
	pState->cinfo.image_height = x_nHeight;
	pState->cinfo.input_components = 3;
	pState->cinfo.in_color_space = JCS_RGB;
	pState->cinfo.density_unit = 1;
	pState->cinfo.X_density = 0;
	pState->cinfo.Y_density = 0;

	// Set jpeg defaults
	jpeg_set_defaults( &pState->cinfo );

	// Set jpeg quality
	jpeg_set_quality( &pState->cinfo, x_nQuality, true );

	return oexTRUE;
#endif
}

void CImage::ReleaseEncodeJpg()
{
#if !defined( OEX_ENABLE_XIMAGE )
	return;
#else

	if ( m_pEncoderState == NULL ) return;
	SJpegEncState *pState = (SJpegEncState*)m_pEncoderState;
	m_pEncoderState = NULL;

	// Release compressor resources
	jpeg_destroy_compress( &pState->cinfo );

	delete pState;
#endif
}

oexINT CImage::EncodeJpg(oexPBYTE x_pSrc, oexINT x_nWidth, oexINT x_nHeight, oexPBYTE x_pDst, oexINT x_nDst, oexINT x_nQuality )
{_STT();

#if !defined( OEX_ENABLE_XIMAGE )
	return oexFALSE;
#else

	// Sanity checks
	if ( !oexCHECK_PTR( x_pDst ) || !x_nDst )
		return oexFALSE;

	// Initialize encoder
	SJpegEncState *pState = (SJpegEncState*)m_pEncoderState;
	if ( oexCHECK_PTR( pState ) || x_nWidth != pState->lWidth || x_nHeight != pState->lHeight )
		if ( !InitEncodeJpg( x_nWidth, x_nHeight, x_nQuality ) )
		return oexFALSE;

	// Get structure
	pState = (SJpegEncState*)m_pEncoderState;

	// Scan width
	oexLONG lScanWidth = GetScanWidth( x_nWidth, 24 );

	pState->dmgr.Init( x_pDst, x_nDst );

	// Initialize compressor
	jpeg_start_compress( &pState->cinfo, true );

	// Compress each line
	oexPBYTE pLine = &x_pSrc[ ( x_nHeight - 1 ) * lScanWidth ];
	for ( oexINT y = 0; y < x_nHeight; y++, pLine -= lScanWidth )
	{	SwapRB( pLine, lScanWidth );
		jpeg_write_scanlines( &pState->cinfo, (JSAMPROW*)&pLine, 1 );
	} // end for

	// Complete image
	jpeg_finish_compress( &pState->cinfo );

	// Return number of bytes writen
	return x_nDst - pState->dmgr.free_in_buffer;
#endif
}

CStr CImage::GetUserImageStr()
{
#if !defined( OEX_ENABLE_XIMAGE )
	return CStr();
#else

#if defined( OEX_WINDOWS ) && !defined( OEX_WINCE )

	return oexT( "All Images ("	"" )

#if defined( CXIMAGE_SUPPORT_JPG ) && CXIMAGE_SUPPORT_JPG
											oexT( "*.jpg;*.jpeg;" )
#endif
#if defined( CXIMAGE_SUPPORT_PNG ) && CXIMAGE_SUPPORT_PNG
											oexT( "*.png;" )
#endif
#if defined( CXIMAGE_SUPPORT_GIF ) && CXIMAGE_SUPPORT_GIF
											oexT( "*.gif;" )
#endif
#if defined( CXIMAGE_SUPPORT_ICO ) && CXIMAGE_SUPPORT_ICO
											oexT( "*.ico;" )
#endif
#if defined( CXIMAGE_SUPPORT_TIF ) && CXIMAGE_SUPPORT_TIF
											oexT( "*.tif;" )
#endif
#if defined( CXIMAGE_SUPPORT_TGA ) && CXIMAGE_SUPPORT_TGA
											oexT( "*.tga;" )
#endif
#if defined( CXIMAGE_SUPPORT_WMF ) && CXIMAGE_SUPPORT_WMF
											oexT( "*.wmf;*.emf;" )
#endif
#if defined( CXIMAGE_SUPPORT_JP2 ) && CXIMAGE_SUPPORT_JP2
											oexT( "*.jp2;" )
#endif
#if defined( CXIMAGE_SUPPORT_BMP ) && CXIMAGE_SUPPORT_BMP
											oexT( "*.bmp;*.dib" )
#endif

											oexT( ")\x0" )

#if defined( CXIMAGE_SUPPORT_JPG ) && CXIMAGE_SUPPORT_JPG
											oexT( "*.jpg;*.jpeg;" )
#endif
#if defined( CXIMAGE_SUPPORT_PNG ) && CXIMAGE_SUPPORT_PNG
											oexT( "*.png;" )
#endif
#if defined( CXIMAGE_SUPPORT_GIF ) && CXIMAGE_SUPPORT_GIF
											oexT( "*.gif;" )
#endif
#if defined( CXIMAGE_SUPPORT_ICO ) && CXIMAGE_SUPPORT_ICO
											oexT( "*.ico;" )
#endif
#if defined( CXIMAGE_SUPPORT_TIF ) && CXIMAGE_SUPPORT_TIF
											oexT( "*.tif;" )
#endif
#if defined( CXIMAGE_SUPPORT_TGA ) && CXIMAGE_SUPPORT_TGA
											oexT( "*.tga;" )
#endif
#if defined( CXIMAGE_SUPPORT_WMF ) && CXIMAGE_SUPPORT_WMF
											oexT( "*.wmf;*.emf;" )
#endif
#if defined( CXIMAGE_SUPPORT_JP2 ) && CXIMAGE_SUPPORT_BMP
											oexT( "*.jp2;" )
#endif
#if defined( CXIMAGE_SUPPORT_BMP ) && CXIMAGE_SUPPORT_BMP
											oexT( "*.bmp;*.dib" )
#endif

											oexT( "\x0" )

#if defined( CXIMAGE_SUPPORT_JPG ) && CXIMAGE_SUPPORT_JPG
											oexT( "JPEG (*.jpg;*.jpeg)\x0*.jpg;*.jpeg\x0" )
#endif
#if defined( CXIMAGE_SUPPORT_PNG ) && CXIMAGE_SUPPORT_PNG
											oexT( "Portable Network Graphic (*.png)\x0*.png\x0" )
#endif
#if defined( CXIMAGE_SUPPORT_GIF ) && CXIMAGE_SUPPORT_GIF
											oexT( "Graphic Interchange Format (*.gif)\x0*.png\x0" )
#endif
#if defined( CXIMAGE_SUPPORT_ICO ) && CXIMAGE_SUPPORT_ICO
											oexT( "Windows Icon (*.ico)\x0*.ico\x0" )
#endif
#if defined( CXIMAGE_SUPPORT_TIF ) && CXIMAGE_SUPPORT_TIF
											oexT( "Tagged Image File (*.tif; *.tiff)\x0*.tif;*.tiff\x0" )
#endif
#if defined( CXIMAGE_SUPPORT_TGA ) && CXIMAGE_SUPPORT_TGA
											oexT( "Targa (*.tga)\x0*.tga\x0" )
#endif
#if defined( CXIMAGE_SUPPORT_WMF ) && CXIMAGE_SUPPORT_WMF
											oexT( "Windows Metafile (*.wmf;*.emf)\x0*.wmf;*.emf\x0" )
#endif
#if defined( CXIMAGE_SUPPORT_JP2 ) && CXIMAGE_SUPPORT_JP
											oexT( "JPEG 2000 (*.jp2)\x0*.jp2\x0" )
#endif
#if defined( CXIMAGE_SUPPORT_BMP ) && CXIMAGE_SUPPORT_BMP
											oexT( "Windows Bitmap (*.bmp;*.dib)\x0*.bmp;*.dib\x0" )
#endif
											;

#else // OEX_WINDOWS

	return oexT( "All Images ("	""

#if defined( CXIMAGE_SUPPORT_JPG ) && CXIMAGE_SUPPORT_JPG
											"*.jpg;*.jpeg;"
#endif
#if defined( CXIMAGE_SUPPORT_PNG ) && CXIMAGE_SUPPORT_PNG
											"*.png;"
#endif
#if defined( CXIMAGE_SUPPORT_GIF ) && CXIMAGE_SUPPORT_GIF
											"*.gif;"
#endif
#if defined( CXIMAGE_SUPPORT_ICO ) && CXIMAGE_SUPPORT_ICO
											"*.ico;"
#endif
#if defined( CXIMAGE_SUPPORT_TIF ) && CXIMAGE_SUPPORT_TIF
											"*.tif;"
#endif
#if defined( CXIMAGE_SUPPORT_TGA ) && CXIMAGE_SUPPORT_TGA
											"*.tga;"
#endif
#if defined( CXIMAGE_SUPPORT_WMF ) && CXIMAGE_SUPPORT_WMF
											"*.wmf;*.emf;"
#endif
#if defined( CXIMAGE_SUPPORT_JP2 ) && CXIMAGE_SUPPORT_JP
											"*.jp2;"
#endif
#if defined( CXIMAGE_SUPPORT_BMP ) && CXIMAGE_SUPPORT_BMP
											"*.bmp;*.dib"
#endif

											")\x0"

#if defined( CXIMAGE_SUPPORT_JPG ) && CXIMAGE_SUPPORT_JPG
											"*.jpg;*.jpeg;"
#endif
#if defined( CXIMAGE_SUPPORT_PNG ) && CXIMAGE_SUPPORT_PNG
											"*.png;"
#endif
#if defined( CXIMAGE_SUPPORT_GIF ) && CXIMAGE_SUPPORT_GIF
											"*.gif;"
#endif
#if defined( CXIMAGE_SUPPORT_ICO ) && CXIMAGE_SUPPORT_ICO
											"*.ico;"
#endif
#if defined( CXIMAGE_SUPPORT_TIF ) && CXIMAGE_SUPPORT_TIF
											"*.tif;"
#endif
#if defined( CXIMAGE_SUPPORT_TGA ) && CXIMAGE_SUPPORT_TGA
											"*.tga;"
#endif
#if defined( CXIMAGE_SUPPORT_WMF ) && CXIMAGE_SUPPORT_WMF
											"*.wmf;*.emf;"
#endif
#if defined( CXIMAGE_SUPPORT_JP2 ) && CXIMAGE_SUPPORT_JP2
											"*.jp2;"
#endif
#if defined( CXIMAGE_SUPPORT_BMP ) && CXIMAGE_SUPPORT_BMP
											"*.bmp;*.dib"
#endif

											"\x0"

#if defined( CXIMAGE_SUPPORT_JPG ) && CXIMAGE_SUPPORT_JPG
											"JPEG (*.jpg;*.jpeg)\x0*.jpg;*.jpeg\x0"
#endif
#if defined( CXIMAGE_SUPPORT_PNG ) && CXIMAGE_SUPPORT_PNG
											"Portable Network Graphic (*.png)\x0*.png\x0"
#endif
#if defined( CXIMAGE_SUPPORT_GIF ) && CXIMAGE_SUPPORT_GIF
											"Graphic Interchange Format (*.gif)\x0*.png\x0"
#endif
#if defined( CXIMAGE_SUPPORT_ICO ) && CXIMAGE_SUPPORT_ICO
											"Windows Icon (*.ico)\x0*.ico\x0"
#endif
#if defined( CXIMAGE_SUPPORT_TIF ) && CXIMAGE_SUPPORT_TIF
											"Tagged Image File (*.tif; *.tiff)\x0*.tif;*.tiff\x0"
#endif
#if defined( CXIMAGE_SUPPORT_TGA ) && CXIMAGE_SUPPORT_TGA
											"Targa (*.tga)\x0*.tga\x0"
#endif
#if defined( CXIMAGE_SUPPORT_WMF ) && CXIMAGE_SUPPORT_WMF
											"Windows Metafile (*.wmf;*.emf)\x0*.wmf;*.emf\x0"
#endif
#if defined( CXIMAGE_SUPPORT_JP2 ) && CXIMAGE_SUPPORT_JP2
											"JPEG 2000 (*.jp2)\x0*.jp2\x0"
#endif
#if defined( CXIMAGE_SUPPORT_BMP ) && CXIMAGE_SUPPORT_BMP
											"Windows Bitmap (*.bmp;*.dib)\x0*.bmp;*.dib\x0"
#endif
											);
#endif // OEX_WINDOWS

#endif // OEX_ENABLE_XIMAGE
}
