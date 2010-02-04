// ff_transcode.cpp

#include "stdafx.h"

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
	if ( !m_dec.Create( m_dec_id, PIX_FMT_YUV420P, width, height, fps, brate, 0 ) )
	{	Destroy();
		return 0;
	} // end if

	// Create encoder
	if ( !m_enc.Create( m_enc_id, PIX_FMT_YUV420P, width, height, fps, brate, 0 ) )
	{	Destroy();
		return 0;
	} // end if

	return 1;
}

int CFfTranscode::Transcode( sqbind::CSqBinary *src, sqbind::CSqBinary *dst, sqbind::CSqMulti *fi )
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

	// Decode in comming
	if ( !m_dec.Decode( src, PIX_FMT_YUV420P, &m_tmp, fi ) )
		return 0;

	// Encode in new format
	if ( !m_enc.Encode( PIX_FMT_YUV420P, m_dec.getWidth(), m_dec.getHeight(), &m_tmp, dst, fi ) )
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

	// Convert to image format
	return CFfConvert::ConvertColorBI( &m_tmp, PIX_FMT_YUV420P, m_dec.getWidth(), m_dec.getHeight(), img, SWS_FAST_BILINEAR, 1 );
}

