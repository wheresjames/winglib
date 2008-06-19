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

oexBOOL CImage::Create( oexCSTR x_pShared, oexINT x_lWidth, oexINT x_lHeight, oexINT x_lBpp )
{
	// Lose any current image
	Destroy();

	// Calculate the size of the image
	oexINT lImageSize = GetScanWidth( x_lWidth, x_lBpp ) * cmn::Abs( x_lHeight );
	if ( !lImageSize )
		return oexFALSE;

	// Set shared name
	if ( x_pShared && *x_pShared )
		m_image.SetName( x_pShared );

	// Allocate memory for the shared memory
	if ( !m_image.OexNew( sizeof( SImageData ) + lImageSize ).Ptr() )
		return oexFALSE;

	return oexTRUE;
}

oexBOOL CImage::Create( os::CFMap::t_HFILEMAP x_hShared, oexINT x_lWidth, oexINT x_lHeight, oexINT x_lBpp )
{
	// Lose any current image
	Destroy();

	// Calculate the size of the image
	oexINT lImageSize = GetScanWidth( x_lWidth, x_lBpp ) * cmn::Abs( x_lHeight );
	if ( !lImageSize )
		return oexFALSE;

	// Set shared name
	if ( x_hShared )
		m_image.SetShareHandle( x_hShared );

	// Allocate memory for the shared memory
	if ( !m_image.OexNew( sizeof( SImageData ) + lImageSize ).Ptr() )
		return oexFALSE;

	return oexTRUE;
}

