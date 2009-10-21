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

		stdString Encode( const stdString &sType )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return stdString();
#else
			oex::oexPBYTE pBuf = oexNULL;
			oex::oexINT nSize = 0;
			if ( !m_img.Encode( &pBuf, &nSize, sType.c_str() )
			     || !oexCHECK_PTR( pBuf ) || !nSize )
				return oexT( "" );

			return stdString().assign( (oex::oexCSTR)pBuf, nSize );
#endif
		}

		int Decode( const stdString &sType, const stdString &sData )
		{
#if !defined( OEX_ENABLE_XIMAGE )
			return 0;
#else
			return m_img.Decode( (oex::oexBYTE*)sData.c_str(), sData.length(), sType.c_str() );
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
