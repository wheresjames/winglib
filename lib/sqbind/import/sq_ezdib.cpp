/*------------------------------------------------------------------
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

_SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqEzdib, CSqEzdib )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqEzdib, Destroy )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqEzdib, Init )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqEzdib, Save )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqEzdib, GetPixel )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqEzdib, SetPixel )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqEzdib, Line )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqEzdib, FillRect )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqEzdib, Rect )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqEzdib, Arc )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqEzdib, Circle )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqEzdib, FloodFill )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqEzdib, Fill )
_SQBIND_REGISTER_CLASS_END()

void CSqEzdib::Register( sqbind::VM vm )
{_STT();
	_SQBIND_EXPORT( vm, CSqEzdib );
}

CSqEzdib::CSqEzdib()
{
	m_ezimg = 0;
	
}

CSqEzdib::~CSqEzdib()
{
	Destroy();
}

void CSqEzdib::Destroy()
{_STT();

	if ( m_ezimg )
		oss::ezd_destroy( m_ezimg ), m_ezimg = 0;

}

int CSqEzdib::Init( CSqBinary *pBuf, int lWidth, int lHeight, int lBpp )
{_STT();

	if ( !pBuf )
		return 0;

	// Share the buffer so it doesn't dissapear on us
	if ( !m_buf.Share( pBuf ) )
		return 0;
	
	// Create the image
    m_ezimg = oss::ezd_create( lWidth, lHeight, lBpp, EZD_FLAG_USER_IMAGE_BUFFER );
	if ( !m_ezimg )
		return 0;
	
	// Set image buffer pointer
	if ( !oss::ezd_set_image_buffer( m_ezimg, m_buf._Ptr(), m_buf.getUsed() ) )
	{	Destroy();
		return 0;
	} // end if
	
	return 1;
}

int CSqEzdib::Save( const stdString &sFile )
{_STT();
	
	if ( !m_ezimg || !sFile.length() )
		return 0;
	
	return oss::ezd_save( m_ezimg, sFile.c_str() );
}

int CSqEzdib::Fill( int col )
{_STT();

	if ( !m_ezimg )
		return 0;

	return oss::ezd_fill( m_ezimg, col );
}

int CSqEzdib::SetPixel( int x, int y, int col )
{
	if ( !m_ezimg )
		return 0;

	return oss::ezd_set_pixel( m_ezimg, x, y, col );
}

int CSqEzdib::GetPixel( int x, int y )
{
	if ( !m_ezimg )
		return 0;

	return oss::ezd_get_pixel( m_ezimg, x, y );
}

int CSqEzdib::Line( int x1, int y1, int x2, int y2, int col )
{
	if ( !m_ezimg )
		return 0;

	return oss::ezd_line( m_ezimg, x1, y1, x2, y2, col );
}

int CSqEzdib::FillRect( int x1, int y1, int x2, int y2, int col )
{
	if ( !m_ezimg )
		return 0;

	return oss::ezd_fill_rect( m_ezimg, x1, y1, x2, y2, col );
}

int CSqEzdib::Rect( int x1, int y1, int x2, int y2, int col )
{
	if ( !m_ezimg )
		return 0;

	return oss::ezd_rect( m_ezimg, x1, y1, x2, y2, col );
}

int CSqEzdib::Arc( int x, int y, int rad, double start, double end, int col )
{
	if ( !m_ezimg )
		return 0;

	return oss::ezd_arc( m_ezimg, x, y, rad, start, end, col );
}

int CSqEzdib::Circle( int x, int y, int rad, int col )
{
	if ( !m_ezimg )
		return 0;

	return oss::ezd_circle( m_ezimg, x, y, rad, col );
}

int CSqEzdib::FloodFill( int x, int y, int bcol, int col )
{
	if ( !m_ezimg )
		return 0;

	return oss::ezd_flood_fill( m_ezimg, x, y, bcol, col );
}


