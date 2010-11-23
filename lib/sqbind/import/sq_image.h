/*------------------------------------------------------------------
// sq_image.h
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

// namespace
namespace sqbind
{

    class CSqImage
    {
	public:

		SQBIND_CLASS_CTOR_BEGIN( CSqImage )
		SQBIND_CLASS_CTOR_END( CSqImage )

		CSqImage() {}

		// Copy semantics
		CSqImage( const CSqImage &r )
		{
#if defined( OEX_ENABLE_XIMAGE )
			m_img = r.m_img;
#endif
		}
		CSqImage& operator=( const CSqImage &r )
		{
#if defined( OEX_ENABLE_XIMAGE )
			m_img = r.m_img;
#endif
			return *this;
		}

		void Destroy()
		{
#if defined( OEX_ENABLE_XIMAGE )
			m_img.Destroy();
#endif
		}

		int Create( int width, int height )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Create( width, height );
#endif
		}

		int Load( const stdString &sFile, const stdString &sType )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Load( sFile.c_str(), sType.c_str() );
#endif
		}

		int Save( const stdString &sFile, const stdString &sType )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Save( sFile.c_str(), sType.c_str() );
#endif
		}

		CSqBinary Encode( const stdString &sType )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return CSqBinary();
#else
			return m_img.Encode( sType.c_str() );
#endif
		}

		int Decode( const stdString &sType, CSqBinary *pData )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			if ( !pData )
				return 0;

			return m_img.Decode( (oex::oexBYTE*)pData->Ptr(), pData->getUsed(), sType.c_str() );
#endif
		}

		int EncodeJpg( CSqBinary *pSrc, CSqBinary *pDst, int w, int h, int q )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			if ( !pSrc || !pDst || 0 >= w || 0 >= h )
				return 0;

			if ( !q )
				q = 85;

			int nImageSize = h * oex::CImage::GetScanWidth( w, 24 );
			if ( pSrc->getUsed() < (oex::oexUINT)nImageSize )
				return 0;

			if ( pDst->Size() < (oex::oexUINT)nImageSize )
				if ( !pDst->Allocate( nImageSize ) )
					return 0;

			int nBytes = m_img.EncodeJpg( pSrc->Ptr(), w, h, pDst->_Ptr(), nImageSize, q );
			if ( 0 > nBytes )
				nBytes = 0;

			pDst->setUsed( nBytes );

			return nBytes;
#endif
		}

		int getWidth()
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.GetWidth();
#endif
		}

		int getHeight()
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.GetHeight();
#endif
		}

		int getScanWidth()
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.GetEffWidth();
#endif
		}

		CSqSize getSize()
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return CSqSize( m_img.GetWidth(), m_img.GetHeight() );
#endif
		}

		// Copy image
		int CopyImage( CSqImage *p )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			if ( !p )
				return 0;

			m_img.Copy( &p->m_img );

			return 1;
#endif
		}

		// Copy image
		int Resample( int x, int y, int fast )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Resample( x, y, fast ) ? 1 : 0;
#endif
		}

		int setPixels( CSqBinary *dat )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			if ( !dat )
				return 0;

			return m_img.CopyBits( dat->_Ptr(), dat->getUsed() );
#endif
		}

		int getPixels( CSqBinary *dat )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			if ( !dat )
				return 0;

			// How large is the image?
			int nSize = m_img.GetImageSize();
			if ( 0 >= nSize )
				return 0;

			// Copy the image data
			dat->MemCpy( (CSqBinary::t_byte*)m_img.GetBits(), nSize );

			// Return image size
			return nSize;
#endif
		}

		int refPixels( CSqBinary *dat )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			if ( !dat )
				return 0;

			// How large is the image?
			int nSize = m_img.GetImageSize();
			if ( 0 >= nSize )
				return 0;

			// Return reference to pixel buffer
			dat->setBuffer( (CSqBinary::t_byte*)m_img.GetBits(), nSize, 0, 0 );

			// Return image size
			return nSize;
#endif
		}

public:

		/// Dilates the image
		int Dilate( int x_Ksize )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Dilate( x_Ksize );
#endif
		}

		/// Erodes the image
		int Erode( int x_Ksize )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Erode( x_Ksize );
#endif
		}

		/// Lightens the image
		int Light( int x_level, int x_contrast )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Light( x_level, x_contrast );
#endif
		}

		/// Colorizes the image
		int Colorize( int x_hue, int x_sat )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Colorize( x_hue, x_sat );
#endif
		}

		/// Thresholds the image
		int Threshold( int x_level )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Threshold( x_level );
#endif
		}

		int ShiftRGB( int x_r, int x_g, int x_b )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.ShiftRGB( x_r, x_g, x_b );
#endif
		}

		int Gamma( float x_fGamma )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Gamma( x_fGamma );
#endif
		}

		int Median( int x_Ksize )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Median( x_Ksize );
#endif
		}

		int Noise( int x_level )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Noise( x_level );
#endif
		}

		int Crop( int x_left, int x_top, int x_right, int x_bottom, CSqImage *x_pDst )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			if ( !x_pDst )
				return 0;

			return m_img.Crop( x_left, x_top, x_right, x_bottom, &x_pDst->Obj() );
#endif
		}

		int Dither( int x_method )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Dither( x_method );
#endif
		}

		int Rotate( float x_angle, CSqImage *x_pDst )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			if ( !x_pDst )
				return 0;

			return m_img.Rotate( x_angle, &x_pDst->Obj() );
#endif
		}

		int Negative()
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Negative();
#endif
		}

		int GrayScale()
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.GrayScale();
#endif
		}

		int Flip()
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Flip();
#endif
		}

		int Mirror()
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Mirror();
#endif
		}

public:

		/// Returns non-zero if there is a valid image buffer
		int isValid()
		{
			return m_img.IsValid() ? 1 : 0;
		}

		/// Returns a const pointer to the image data
		const void* Ptr() { return m_img.GetBits(); }

		/// Returns a writable pointer to the image data
		void* _Ptr() { return m_img.GetBits(); }

		/// Returns the size of the image buffer
		int getUsed() { return m_img.GetImageSize(); }

		static void Register( sqbind::VM vm );

		/// Returns reference to the wrapped image object
		oex::CImage& Obj() { return m_img; }

	private:

#if defined( OEX_ENABLE_XIMAGE )
		/// Image object
		oex::CImage		m_img;
#endif

    };

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqImage, CSqImage )
