/*------------------------------------------------------------------
// sq_avi.cpp
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

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqAviFile, CSqAviFile )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqAviFile, Destroy )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqAviFile, Create )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqAviFile, AddVideoStream )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqAviFile, AddVideoFrame )
//	SQBIND_MEMBER_FUNCTION(  sqbind::CSqAviFile, getWidth )
//	SQBIND_MEMBER_FUNCTION(  sqbind::CSqAviFile, getHeight )
//	SQBIND_MEMBER_FUNCTION(  sqbind::CSqAviFile, getFps )	
//	SQBIND_MEMBER_FUNCTION(  sqbind::CSqAviFile, isOpen )	

	SQBIND_STATIC_FUNCTION(  sqbind::CSqAviFile, fourCC )

SQBIND_REGISTER_CLASS_END()

void CSqAviFile::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqAviFile );
}

int CSqAviFile::Destroy()
{_STT();

	// Close the capture device
	m_avi.Destroy();

	return 1;
}

int CSqAviFile::Create( const sqbind::stdString &sFile )
{_STT();

	return m_avi.Create( sFile.c_str() ) ? 1 : 0;
}

int CSqAviFile::AddVideoStream( int nCodecId, int nWidth, int nHeight, float fFps )
{_STT();

	return m_avi.AddVideoStream( nCodecId, nWidth, nHeight, fFps );
}

int CSqAviFile::fourCC( const sqbind::stdString &sCodec )
{_STT();

	// Must be at least four characters
	if ( 4 > sCodec.length() )
		return 0;

	// Convert the codec id
	return oex::vid::CAviFile::fourCC( sCodec.c_str() );
}

int CSqAviFile::AddVideoFrame( int nType, int nStream, CSqBinary *pFrame )
{_STT();

	if ( !pFrame || 0 >= pFrame->getUsed() )
		return 0;

	if ( !nType )
		nType = oex::vid::CAviFile::eFtCompressedVideo;
		
	return m_avi.AddFrame( nType, nStream, pFrame->Ptr(), pFrame->getUsed() );
}
