// ff_transcode.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CFfTranscode, CFfTranscode )

	SQBIND_MEMBER_FUNCTION( CFfTranscode, Init )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, Destroy )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, Transcode )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, GetRaw )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, GetImage )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, getDecoderCodecId )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, getEncoderCodecId )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, getWidth )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, getHeight )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, getFps )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, getBitRate )
	SQBIND_MEMBER_FUNCTION( CFfTranscode, isValid )

SQBIND_REGISTER_CLASS_END()

void CFfTranscode::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CFfTranscode );
}

CFfTranscode::CFfTranscode()
{_STT();
	m_dec_id = 0;
	m_enc_id = 0;
}

void CFfTranscode::Destroy()
{_STT();
	m_dec_id = 0;
	m_enc_id = 0;
	m_enc.Destroy();
	m_dec.Destroy();
	m_cvt.Destroy();
}

int CFfTranscode::Init( int width, int height, int fps, int brate, int src_codec, int dst_codec )
{_STT();
	Destroy();

	if ( 0 >= width || 0 >= height )
		return 0;

	m_dec_id = src_codec;
	m_enc_id = dst_codec;

	// Are we actually transcoding?
	if ( m_dec_id == m_enc_id )
		return 1;

	// Attempt to create a decoder
	if ( !m_dec.Create( m_dec_id, AV_PIX_FMT_YUV420P, width, height, fps, brate, 0 ) )
	{	Destroy();
		return 0;
	} // end if

	// Create encoder
	if ( !m_enc.Create( m_enc_id, AV_PIX_FMT_YUV420P, width, height, fps, brate, 0 ) )
	{	Destroy();
		return 0;
	} // end if

	return 1;
}

int CFfTranscode::Transcode( sqbind::CSqBinary *src, sqbind::CSqBinary *dst, sqbind::CSqMulti *fi, int flip )
{_STT();
	if ( !isValid() )
		return 0;

	if ( !src || !dst )
		return 0;

	// Does it need transcoding?
	if ( m_dec_id == m_enc_id )
	{	dst->Share( src );
		return 1;
	} // end if

	// Decode incoming
	if ( !m_dec.Decode( src, AV_PIX_FMT_YUV420P, &m_tmp, fi, flip ) )
		return 0;

	// Encode in new format
	if ( !m_enc.Encode( AV_PIX_FMT_YUV420P, m_dec.getWidth(), m_dec.getHeight(), &m_tmp, dst, fi ) )
		return 0;

	return 1;
}

int CFfTranscode::GetRaw( sqbind::CSqBinary *buf )
{_STT();
	if ( !buf || !m_tmp.getUsed() )
		return 0;

	buf->Share( &m_tmp );

	return 1;
}

int CFfTranscode::GetImage( sqbind::CSqImage *img )
{_STT();
	if ( !img || !m_tmp.getUsed() || !isValid() )
		return 0;

	// Create converter if needed
	if ( m_cvt.getSrcWidth() != getWidth() || m_cvt.getSrcHeight() != getHeight() 
		 || m_cvt.getDstWidth() != getWidth() || m_cvt.getDstHeight() != getHeight()
		 || m_cvt.getSrcFmt() != AV_PIX_FMT_YUV420P || m_cvt.getDstFmt() != AV_PIX_FMT_BGR24 )
		if ( !m_cvt.Create( getWidth(), getHeight(), AV_PIX_FMT_YUV420P, 
							getWidth(), getHeight(), AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, 0 ) )
			return 0;
	
	if ( !m_cvt.ConvertBI( &m_tmp, img ) )
		return 0;
	
	return 1;
}

