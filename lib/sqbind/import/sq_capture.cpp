/*------------------------------------------------------------------
// sq_capture.cpp
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

#include "../stdafx.h"

using namespace sqbind;

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqCapture, CSqCapture )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqCapture, Init )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqCapture, Destroy )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqCapture, Capture )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqCapture, ReleaseFrame )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqCapture, GetSupportedFormats )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqCapture, GetFormatDescription )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqCapture, getWidth )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqCapture, getHeight )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqCapture, getFps )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqCapture, isOpen )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqCapture, isRunning )	

	SQBIND_STATIC_FUNCTION(  sqbind::CSqCapture, GetNextAvailableSystem )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqCapture, GetDevices )	

SQBIND_REGISTER_CLASS_END()

void CSqCapture::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqCapture );
}

int CSqCapture::GetNextAvailableSystem( int i, sqbind::CSqMulti *inf )
{
	if ( !inf )
		return 0;

	inf->clear();

	// Get next info block
	oexCONST oex::vid::SVideoSystemInfo* pVsi = oex::vid::CCapture::GetNextAvailableSystem( i );
	if ( !pVsi )
		return 0;

	(*inf)[ "index" ].setstr( oexMks( pVsi->nIndex ) );
	(*inf)[ "type" ].setstr( oexMks( pVsi->uType ) );
	(*inf)[ "supported" ].setstr( oexMks( pVsi->bSupported ) );
	if ( pVsi->pTag ) (*inf)[ "tag" ].setstr( pVsi->pTag );
	if ( pVsi->pName ) (*inf)[ "name" ].setstr( pVsi->pName );

	return 1;
}

int CSqCapture::GetDevices( int type, sqbind::CSqMulti *inf )
{
	if ( !inf )
		return 0;

	inf->clear();

	oex::CPropertyBag pb;
	int n = oex::vid::CCapture::GetDevices( type, &pb );
	if ( !n )
		return 0;

	inf->fromPb( pb );
	return inf->size();
}


int CSqCapture::Destroy()
{_STT();

	// Close the capture device
	m_cap.Destroy();

	// Lose any image buffer we may have
	m_img.Destroy();

	// Lose temporary buffer
	m_buf.Destroy();

	return 1;
}

int CSqCapture::Init( int nDevice, int nChannel, int nWidth, int nHeight, int nFps, const stdString &sFormat, int nInit )
{_STT();

	oex::CStr sFmt = sFormat.c_str();

	if ( sFmt.Length() < 4 )
		sFmt = oexT( "JPEG" );

	// Open the capture device
	if ( !m_cap.Open( 0, nDevice, nChannel, nWidth, nHeight, *(const oex::oexUINT*)sFmt.Ptr(), nFps, 0 ) )
		return -1;

//	if ( !m_cap.Open( 0, nDevice, nChannel, nWidth, nHeight, oexFOURCC( 'Y', 'U', 'Y', 'V' ), nFps, 0 ) )
//	CStr().Fmt( oexT( "VIDIOC_S_FMT : Unsupported format: %x - suggested type: %x" ),
//												(unsigned int)m_uFormat

	if ( !nInit )
		return 1;

	if ( !m_cap.Init() )
		return -2;

	if ( !m_cap.StartCapture() )
		return -3;

	return 1;
}

stdString SaveImage( void *pBuf, long lSize, long lWidth, long lHeight )
{_STT();

	if ( !pBuf || !lSize )
		return stdString();

	oex::CImage img;
	if ( !img.Create( lWidth, lHeight ) )
		return stdString();

	memcpy( img.GetBits(), pBuf, lSize / 2 );

	unsigned char *pEnc = 0;
	int lEnc = 0;
	if ( !img.Encode( &pEnc, &lEnc, oexT( "jpg" ) ) || !pEnc || !lEnc )
		return stdString();

	oex::CStr s = oexMbToStr( oex::CStr8( (char*)pEnc, lEnc ) );

	return stdString().assign( s.Ptr(), s.Length() );
}

stdString CSqCapture::GetSupportedFormats()
{_STT();

	return m_cap.GetSupportedFormats().Ptr();
}

stdString CSqCapture::GetFormatDescription( const stdString &sFormat )
{_STT();

	// Convert to multi-byte
	oex::CStr8 fmt = oexStrToMb( sFormat.c_str() );
	if ( fmt.Length() < 4 )
		return oexT( "" );

	// Convert to integer
	return m_cap.GetFormatDescription( *(oex::oexUINT*)fmt.Ptr() ).Ptr();
}


int CSqCapture::Capture( sqbind::CSqBinary *pBuf, int nMaxWait )
{_STT();

	if ( !pBuf )
		return -1;

	// No data yet
	pBuf->setUsed( 0 );

	// Wait for a frame
	if ( !m_cap.WaitForFrame( nMaxWait ) )
		return -1;

	// Did we get valid info?
	if ( m_cap.GetBuffer() && m_cap.GetBufferSize() )
		pBuf->setBuffer( (sqbind::CSqBinary::t_byte*)m_cap.GetBuffer(), (CSqBinary::t_size)m_cap.GetBufferSize(), 0, 0 );

	return pBuf->getUsed();
}

int CSqCapture::ReleaseFrame()
{_STT();

	// Release the memory
	m_cap.ReleaseFrame();

	return 1;
}


/*
	// Convert to multi-byte
	oex::CStr8 fmt = oexStrToMb( sEncode.c_str() );
	if ( fmt.Length() < 4 )
		return oexT( "" );

	// What format does the user want?
	oex::oexUINT uReq = 0;
	if ( 4 <= sEncode.length() )
		uReq = *(oex::oexUINT*)fmt.Ptr();

	// What format do we have
	oex::oexUINT uFormat = m_cap.GetFormat();

	// Is the video already in the required format?
	stdString ret;
	if ( !uReq || uReq == uFormat )
	{
		if ( m_cap.GetBuffer() && m_cap.GetBufferSize() )
		{	oex::CStr s = oexMbToStr( oex::CStr8( (char*)m_cap.GetBuffer(), m_cap.GetBufferSize() ) );
			ret.assign( s.Ptr(), s.Length() );
		} // end if

	} // end if

	else
	{
		switch( uReq )
		{
			// JPEG
			case oexFOURCC( 'J', 'P', 'E', 'G' ) :

				switch( uFormat )
				{
					// RGB3 -> JPEG
					case oexFOURCC( 'R', 'G', 'B', '3' ) :
					{
						if ( m_img.Create( m_cap.GetWidth(), m_cap.GetHeight(), 24 ) )
						{
							memcpy( m_img.GetBits(), m_cap.GetBuffer(), m_cap.GetBufferSize() );

							oex::oexPBYTE pEnc;
							oex::oexINT nEnc;
							if ( 0 < m_img.Encode( &pEnc, &nEnc, oexT( "JPG" ) ) )
							{	oex::CStr s = oexMbToStr( oex::CStr8( (char*)pEnc, nEnc ) );
								ret.assign( s.Ptr(), s.Length() );
							} // end if

						} // end if

					} // end case
					break;

					// UYVY -> JPEG
					case oexFOURCC( 'U', 'Y', 'V', 'Y' ) :
					{

//oexEcho( "Creating image" );

						if ( m_img.Create( m_cap.GetWidth(), m_cap.GetHeight(), 24 ) )
						{
//oexEcho( "Starting conversion" );

							// YUYV conversion
							oex::CDib::YUYV_RGB( m_cap.GetWidth(), m_cap.GetHeight(),
												 m_cap.GetBuffer(), m_img.GetBits(), 0 );

							oex::oexPBYTE pEnc;
							oex::oexINT nEnc;

//oexEcho( "Starting image encoding" );
							if ( 0 < m_img.Encode( &pEnc, &nEnc, oexT( "JPG" ) ) )
							{	oex::CStr s = oexMbToStr( oex::CStr8( (char*)pEnc, nEnc ) );
								ret.assign( s.Ptr(), s.Length() );
							} // end if
							else
								oexERROR( 0, oexT( "Error encoding image" ) );

						} // end if

						else
							oexERROR( 0, oexT( "Unable to create image buffer" ) );

					} // end case
					break;

				} // end switch

				break;

		} // end switch

	} // end else

	// Release video buffer
	m_cap.ReleaseFrame();

	return ret;
*/
