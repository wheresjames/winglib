/*------------------------------------------------------------------
// capture.cpp
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

#include "../../../oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;
using namespace OEX_NAMESPACE::vid;


CCapture::CCapture()
{
	m_pDevice = oexNULL;
}

/// Destructor
CCapture::~CCapture()
{
	Destroy();
}

void CCapture::Destroy()
{
	/// Lose device if any
	if ( m_pDevice )
	{
//		OexAllocDelete( (CV4lCapture*)m_pDevice );
		m_pDevice = oexNULL;
	} // end if
}

//	CV4lCapture				m_v4lCap;

oexBOOL CCapture::Open( oexCSTR x_sDevice, oexINT x_nWidth, oexINT x_nHeight, oexINT x_nBpp, oexFLOAT x_fFps )
{
	// Lose previous device
	Destroy();

	// Allocate a new capture device
//	m_pDevice = OexAllocNew< CV4lCapture >( 1 );
	if ( !m_pDevice )
		return oexFALSE;

	return oexTRUE;
}

oexBOOL CCapture::WaitForFrame( oexUINT x_uTimeout )
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return oexFALSE;
}

oexBOOL CCapture::StartCapture()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return oexFALSE;
}

oexBOOL CCapture::StopCapture()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return oexFALSE;
}

oexPVOID CCapture::GetBuffer()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return oexFALSE;
}

oexINT CCapture::GetBufferSize()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return 0;
}

CImage* CCapture::GetImage()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return oexFALSE;
}

oexINT CCapture::GetWidth()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return oexFALSE;
}

oexINT CCapture::GetHeight()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return oexFALSE;
}

oexINT CCapture::GetBpp()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return oexFALSE;
}

oexFLOAT CCapture::GetFps()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return oexFALSE;
}

oexINT64 CCapture::GetFrame()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return oexFALSE;
}

CStr CCapture::GetSupportedFormats()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return CStr();
	} // end if

	return oexFALSE;
}

