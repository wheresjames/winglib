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
		pId->bih.biSize = sizeof( SImageData );
		pId->bih.biWidth = x_lWidth;
		pId->bih.biHeight = x_lHeight;
		pId->bih.biPlanes = 1;
		pId->bih.biBitCount = x_lBpp;
		pId->bih.biCompression = SBitmapInfoHeader::eBiRgb;
		pId->bih.biSizeImage = lImageSize;

	} // end if

	return oexTRUE;
}

