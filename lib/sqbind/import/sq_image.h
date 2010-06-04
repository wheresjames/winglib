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

		/// Returns non-zero if there is a valid image buffer
		int isValid()
		{
			return m_img.IsValid() ? 1 : 0;
		}


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
