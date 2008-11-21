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

// Include capture classes
//#include "vfw1.hpp"
//#include "vfw2.hpp"

CCapture::CCapture()
{
	m_uType = oexVIDSUB_AUTO;
	m_pDevice = oexNULL;
}

/// Destructor
CCapture::~CCapture()
{
	Destroy();
}

oexBOOL CCapture::Destroy()
{
	/// Lose device if any
	if ( oexCHECK_PTR( m_pDevice ) )
	{
		switch( m_uType )
		{
			case oexVIDSUB_VFW :
//				OexAllocDelete< CV4l1 >( (CV4l1*)m_pDevice );
				break;

			case oexVIDSUB_DSHOW :
//				OexAllocDelete< CV4l2 >( (CV4l2*)m_pDevice );
				break;

		} // end switch

	} // end if

	m_uType = oexVIDSUB_AUTO;
	m_pDevice = oexNULL;

	return oexTRUE;
}

oexBOOL CCapture::Open( oexUINT x_uType, oexCSTR x_sDevice, oexINT x_nWidth, oexINT x_nHeight, oexINT x_nBpp, oexFLOAT x_fFps )
{
	// Lose previous device
	Destroy();

	// Allocate a new capture device
	switch( x_uType )
	{
		case oexVIDSUB_AUTO :

//			m_pDevice = OexAllocConstruct< CV4w2 >();
			if ( !oexCHECK_PTR( m_pDevice ) )
			{	Destroy();
				return oexFALSE;
			} // end if

			// Try VFL2
			if ( m_pDevice->Open( x_uType, x_sDevice, x_nWidth, x_nHeight, x_nBpp, x_fFps ) )
			{	m_uType = oexVIDSUB_VFL2;
				return oexTRUE;
			} // end if

//			OexAllocDelete< CV4w2 >( (CV4w2*)m_pDevice );
			m_pDevice = oexNULL;

		case oexVIDSUB_VFW :
			m_uType = x_uType;
//			m_pDevice = OexAllocConstruct< CV4w1 >();
			break;

		case oexVIDSUB_DSHOW :
			m_uType = x_uType;
//			m_pDevice = OexAllocConstruct< CV4w2 >();
			break;

	} // end switch

	if ( !oexCHECK_PTR( m_pDevice ) )
	{	Destroy();
		return oexFALSE;
	} // end if

	// Attempt to open the capture device
	if ( !m_pDevice->Open( x_uType, x_sDevice, x_nWidth, x_nHeight, x_nBpp, x_fFps ) )
	{	Destroy();
		return oexFALSE;
	} // end if

	return oexTRUE;
}

