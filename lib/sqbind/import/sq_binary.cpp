/*------------------------------------------------------------------
// sq_binary.cpp
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

int CSqBinary::FingerprintImage( CSqImage *img, CSqBinary *col, int scale )
{	if ( !img || !col ) return 0;
	CSqBinary buf; if ( !img->refPixels( &buf ) ) return 0;
	return oexFingerprint( &m_bin, &buf.Mem(), 0, img->getWidth(), img->getHeight(), &col->Mem(), scale );
}

int CSqBinary::GraphFloat( CSqImage *img, CSqColor *c, float scale )
{	if ( !img || !img->getUsed() || !getUsed() ) return 0;
	CSqBinary buf; if ( !img->refPixels( &buf ) ) return 0;
	return oex::CUtil::GraphFloat( &buf.Mem(), 0, img->getWidth(), img->getHeight(), img->getScanWidth(), c->Ptr(), (oex::oexFLOAT*)Ptr(), getUsed() / sizeof( oex::oexFLOAT ), scale );
}
