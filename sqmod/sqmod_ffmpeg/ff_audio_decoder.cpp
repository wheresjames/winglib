// ff_audio_decoder.cpp

#include "stdafx.h"

extern "C"
{
#	include "libavformat/riff.h"
};

CFfAudioDecoder::CFfAudioDecoder()
{_STT();

	m_pCodec = oexNULL;
	m_pCodecContext = oexNULL;
	m_pFormatContext = oexNULL;
	m_pFrame = oexNULL;
	m_lFrames = 0;
	oexZero( m_pkt );
}

void CFfAudioDecoder::Destroy()
{_STT();

	oexAutoLock ll( _g_ffmpeg_lock );
	if ( !ll.IsLocked() ) return;

	if ( m_pFrame )
	{	av_free( m_pFrame );
		m_pFrame = oexNULL;
	} // end if

	if ( m_pCodecContext )
	{	avcodec_close( m_pCodecContext );
		m_pCodecContext = oexNULL;
	} // end if

	if ( m_pFormatContext )
	{	av_free( m_pFormatContext );
		m_pFormatContext = oexNULL;
	} // end if

	m_pCodec = oexNULL;
	oexZero( m_pkt );
	m_lFrames = 0;
}

int CFfAudioDecoder::Create( int x_nCodec )
{_STT();

	oexAutoLock ll( _g_ffmpeg_lock );
	if ( !ll.IsLocked() ) return 0;

	// Lose previous codec
	Destroy();

	m_pCodec = avcodec_find_decoder( (CodecID)x_nCodec );
	if ( !m_pCodec )
	{	oexERROR( 0, oexMks( oexT( "avcodec_find_decoder() failed : " ),
					 (int)x_nCodec, oexT( " - " ), LookupCodecName( x_nCodec ).c_str() ) );
		return 0;
	} // end if

	// Allocate context
	m_pCodecContext = avcodec_alloc_context();
	if ( !m_pCodecContext )
	{	oexERROR( 0, oexT( "avcodec_alloc_context() failed" ) );
		Destroy();
		return 0;
	} // end if

//	avcodec_get_context_defaults( m_pCodecContext );
	avcodec_get_context_defaults2( m_pCodecContext, CODEC_TYPE_VIDEO );

    m_pCodecContext->codec_id = (CodecID)x_nCodec;
    m_pCodecContext->codec_type = CODEC_TYPE_AUDIO;
//	m_pCodecContext->strict_std_compliance = cmp;
//	m_pCodecContext->pix_fmt = (PixelFormat)fmt;

	if( 0 != ( m_pCodec->capabilities & CODEC_CAP_TRUNCATED ) )
		m_pCodecContext->flags |= CODEC_FLAG_TRUNCATED;

	int res = avcodec_open( m_pCodecContext, m_pCodec );
	if ( 0 > res )
	{	oexERROR( res, oexT( "avcodec_open() failed" ) );
		m_pCodecContext = oexNULL;
		Destroy();
		return 0;
	} // end if

	return 1;
}

int CFfAudioDecoder::FindStreamInfo( sqbind::CSqBinary *in )
{_STT();
	if ( !in )
		return 0;

	if ( m_pFormatContext )
	{	av_free( m_pFormatContext );
		m_pFormatContext = oexNULL;
	} // end if

	m_pFormatContext = avformat_alloc_context();
	if ( !m_pFormatContext )
		return 0;

	if ( 0 > av_find_stream_info( m_pFormatContext ) )
	{	av_free( m_pFormatContext );
		m_pFormatContext = oexNULL;
		return 0;
	} // end if

	return 1;
}

int CFfAudioDecoder::Decode( sqbind::CSqBinary *in, sqbind::CSqBinary *out, sqbind::CSqMulti *m )
{_STT();
	if ( !in || !out )
		return 0;

	int in_size = in->getUsed();
	const uint8_t* in_ptr = (const uint8_t*)in->Ptr();

	int out_size = oex::cmn::Max( (int)(in->getUsed() * 2), (int)AVCODEC_MAX_AUDIO_FRAME_SIZE );
	if ( (int)out->Size() < out_size )
		out->Allocate( out_size );

	int used = avcodec_decode_audio2( m_pCodecContext,
									  (int16_t*)out->_Ptr(), &out_size,
									  (const uint8_t*)in_ptr, in_size );

	out->setUsed( out_size );

	if ( 0 >= used )
//	{	oexSHOW( used );
		return -1;
//	} // end if

	if ( 0 >= out_size )
		return 0;

	// Frame count
	m_lFrames++;

	return 1;
}

static AVCodecTag g_ff_audio_codec_map[] =
{
    { CODEC_ID_AAC,				MKTAG('A', 'M', 'R', ' ') },
    { CODEC_ID_AMR_NB,			MKTAG('A', 'M', 'R', ' ') },
    { CODEC_ID_AMR_WB,			MKTAG('A', 'M', 'R', ' ') },
    { CODEC_ID_AAC,				MKTAG('M', 'P', '4', 'A') },

    { CODEC_ID_MP2,				MKTAG('M', 'P', '2', '_') },
    { CODEC_ID_MP3,				MKTAG('M', 'P', '3', '_') },
    { CODEC_ID_AAC,				MKTAG('A', 'A', 'C', '_') },
    { CODEC_ID_AC3,				MKTAG('A', 'C', '3', '_') },

    
	{ CODEC_ID_NONE,			0 }
};

int CFfAudioDecoder::LookupCodecId( const sqbind::stdString &sCodec )
{_STT();
	char c[ 5 ] = { ' ', ' ', ' ', ' ', 0 };
	for ( unsigned int i = 0; i < 4 && i < sCodec.length(); i++ )
		c[ i ] = (char)sCodec[ i ];

	// Find a codec with that name
	for ( int i = 0; CODEC_ID_NONE != ff_codec_wav_tags[ i ].id; i++ )
		if ( *(oex::oexUINT32*)c == ff_codec_wav_tags[ i ].tag )
			return ff_codec_wav_tags[ i ].id;

	// Extras
	for ( int i = 0; CODEC_ID_NONE != g_ff_audio_codec_map[ i ].id; i++ )
		if ( *(oex::oexUINT32*)c == g_ff_audio_codec_map[ i ].tag )
			return g_ff_audio_codec_map[ i ].id;

	return 0;
}

sqbind::stdString CFfAudioDecoder::LookupCodecName( int nId )
{_STT();
	// Find a codec with that id
	for ( int i = 0; CODEC_ID_NONE != ff_codec_wav_tags[ i ].id; i++ )
		if ( ff_codec_wav_tags[ i ].id == (CodecID)nId )
		{	char b[ 5 ] = { 0, 0, 0, 0, 0 };
			*(oex::oexUINT32*)b = ff_codec_wav_tags[ i ].tag;
			return oexMbToStrPtr( b );
		} // end if

	return oexT( "" );
}

