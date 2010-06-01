// ff_decoder.cpp

#include "stdafx.h"

/*
	.oo.ooo.oooooooo.
	 oU U U U U U Uo
	 |             |
	 |             |====\
	 | F F M P E G |    ||
	 |             |    ||
	 |   B E E R   |    ||
	 |             |    ||
	 |             |====/
	 |             |
	 ===============
*/

// av_open_input_file av_find_stream_info av_log_set_callback
// void av_log_default_callback(void* ptr, int level, const char* fmt, va_list vl)

extern "C"
{
#	include "libavformat/riff.h"
};

CFfDecoder::CFfDecoder()
{_STT();
	m_pCodec = oexNULL;
	m_pCodecContext = oexNULL;
	m_pFormatContext = oexNULL;
	m_pFrame = oexNULL;
	m_nFmt = 0;
	oexZero( m_pkt );

}

void CFfDecoder::Destroy()
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

	m_nFmt = 0;
	m_pCodec = oexNULL;
	oexZero( m_pkt );
}

int CFfDecoder::Create( int x_nCodec, int fmt, int width, int height, int fps, int brate, sqbind::CSqMulti *m )
{_STT();

	oexAutoLock ll( _g_ffmpeg_lock );
	if ( !ll.IsLocked() ) return 0;

	// Lose previous codec
	Destroy();

	m_pCodec = avcodec_find_decoder( (CodecID)x_nCodec );
	if ( !m_pCodec )
	{	oexERROR( 0, oexT( "avcodec_find_decoder() failed" ) );
		return 0;
	} // end if

//	CODEC_CAP_PARSE_ONLY CODEC_CAP_TRUNCATED
//	avcodec_parse_frame

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
    m_pCodecContext->codec_type = CODEC_TYPE_VIDEO;
    m_pCodecContext->bit_rate = brate;
    m_pCodecContext->width = width ? width : 320;
    m_pCodecContext->height = height ? height : 240;
    m_pCodecContext->time_base.den = fps;
    m_pCodecContext->time_base.num = 1;
    m_pCodecContext->strict_std_compliance = ( ( m && m->isset( oexT( "cmp" ) ) ) ? (*m)[ oexT( "cmp" ) ].toint() : 0 );
	m_pCodecContext->pix_fmt = (PixelFormat)fmt;

//	m_pCodecContext->flags = CODEC_FLAG_LOW_DELAY;


    if ( CODEC_ID_H264 == x_nCodec )
    {
		oexEcho( "!!! H264 Codec Settings" );
/*
		m_pCodecContext->extradata_size = 19;
		m_pCodecContext->extradata = (uint8_t*)av_mallocz( m_pCodecContext->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE );

        m_pCodecContext->profile = 66;

        m_pCodecContext->scenechange_factor = 0;

		m_pCodecContext->thread_count = 1;
		m_pCodecContext->ticks_per_frame = 2;
		m_pCodecContext->use_lpc = -1;
*/
//        m_pCodecContext->strict_std_compliance = -2;
        m_pCodecContext->workaround_bugs = FF_BUG_AUTODETECT;
        m_pCodecContext->error_recognition = FF_ER_AGGRESSIVE;
        m_pCodecContext->idct_algo = FF_IDCT_H264;
        m_pCodecContext->error_concealment = FF_EC_GUESS_MVS | FF_EC_DEBLOCK;
        m_pCodecContext->flags |= CODEC_FLAG_INPUT_PRESERVED | CODEC_FLAG_EMU_EDGE;
        m_pCodecContext->flags2 |= CODEC_FLAG2_BRDO | CODEC_FLAG2_MEMC_ONLY | CODEC_FLAG2_DROP_FRAME_TIMECODE | CODEC_FLAG2_SKIP_RD | CODEC_FLAG2_CHUNKS;

    } // end if

	if( 0 != ( m_pCodec->capabilities & CODEC_CAP_TRUNCATED ) )
		m_pCodecContext->flags |= CODEC_FLAG_TRUNCATED;

    int res = avcodec_open( m_pCodecContext, m_pCodec );
	if ( 0 > res )
	{	oexERROR( res, oexT( "avcodec_open() failed" ) );
		m_pCodecContext = oexNULL;
		Destroy();
		return 0;
	} // end if

	m_nFmt = fmt;

	return 1;
}

int CFfDecoder::FindStreamInfo( sqbind::CSqBinary *in )
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

int CFfDecoder::Decode( sqbind::CSqBinary *in, int fmt, sqbind::CSqBinary *out, sqbind::CSqMulti *m )
{_STT();
	// Ensure codec
	if ( !m_pCodecContext )
		return 0;

	// Validate params
	if ( !in || !in->getUsed() || !out )
		return 0;

	// How much padding is there in the buffer
	// If not enough, we assume the caller took care of it
	int nPadding = in->Size() - in->getUsed();
	if ( 0 < nPadding )
	{
		// Don't zero more than twice the padding size
		if ( nPadding > ( FF_INPUT_BUFFER_PADDING_SIZE * 2 ) )
			nPadding = FF_INPUT_BUFFER_PADDING_SIZE * 2;

		// Set end to zero to ensure no overreading on damaged blocks
		oexZeroMemory( &in->_Ptr()[ in->getUsed() ], nPadding );

	} // end if

//	if ( m_pFrame )
//	{	av_free( m_pFrame );
//		m_pFrame = oexNULL;
//	} // end if

	if ( !m_pFrame )
		m_pFrame = avcodec_alloc_frame();

	if ( !m_pFrame )
		return 0;

	// Init packet
	oexZero( m_pkt );
	av_init_packet( &m_pkt );

	if ( m )
	{
		if ( m->isset( oexT( "flags" ) ) )
			m_pkt.flags = (*m)[ oexT( "flags" ) ].toint();
		if ( m->isset( oexT( "size" ) ) )
			m_pkt.size = (*m)[ oexT( "size" ) ].toint();
		if ( m->isset( oexT( "stream_index" ) ) )
			m_pkt.stream_index = (*m)[ oexT( "stream_index" ) ].toint();
		if ( m->isset( oexT( "pos" ) ) )
			m_pkt.pos = (*m)[ oexT( "pos" ) ].toint();
		if ( m->isset( oexT( "dts" ) ) )
			m_pkt.dts = (*m)[ oexT( "dts" ) ].toint();
		if ( m->isset( oexT( "pts" ) ) )
			m_pkt.pts = (*m)[ oexT( "pts" ) ].toint();
		if ( m->isset( oexT( "duration" ) ) )
			m_pkt.duration = (*m)[ oexT( "duration" ) ].toint();

	} // end if

	m_pkt.data = (uint8_t*)in->_Ptr();
	m_pkt.size = in->getUsed();

	int gpp = 0;
//	int used =
#if defined( FFSQ_VIDEO2 )
		avcodec_decode_video2( m_pCodecContext, m_pFrame, &gpp, &m_pkt );
#else
		avcodec_decode_video( m_pCodecContext, m_pFrame, &gpp, (uint8_t*)in->_Ptr(), in->getUsed() );
#endif

	if ( 0 >= gpp )
		return 0;

	// Convert
	int res = CFfConvert::ConvertColorFB( m_pFrame, m_nFmt, m_pCodecContext->width, m_pCodecContext->height, fmt, out, SWS_FAST_BILINEAR );

	return res ? 1 : 0;
}

int CFfDecoder::DecodeImage( sqbind::CSqBinary *in, sqbind::CSqImage *img, sqbind::CSqMulti *m )
{_STT();
	// Ensure codec
	if ( !m_pCodecContext )
		return 0;

	// Validate params
	if ( !in || !in->getUsed() || !img )
		return 0;

	// How much padding is there in the buffer
	// If not enough, we assume the caller took care of it
	int nPadding = in->Size() - in->getUsed();
	if ( 0 < nPadding )
	{
		// Don't zero more than twice the padding size
		if ( nPadding > ( FF_INPUT_BUFFER_PADDING_SIZE * 2 ) )
			nPadding = FF_INPUT_BUFFER_PADDING_SIZE * 2;

		// Set end to zero to ensure no overreading on damaged blocks
		oexZeroMemory( &in->_Ptr()[ in->getUsed() ], nPadding );

	} // end if

	if ( m_pFrame )
	{	av_free( m_pFrame );
		m_pFrame = oexNULL;
	} // end if

	m_pFrame = avcodec_alloc_frame();
	if ( !m_pFrame )
		return 0;

	m_pkt.data = (uint8_t*)in->_Ptr();
	m_pkt.size = in->getUsed();

	int gpp = 0;
//	int used =
#if defined( FFSQ_VIDEO2 )
		avcodec_decode_video2( m_pCodecContext, m_pFrame, &gpp, &m_pkt );
#else
		avcodec_decode_video( m_pCodecContext, m_pFrame, &gpp, (uint8_t*)in->_Ptr(), in->getUsed() );
#endif

	if ( 0 >= gpp )
		return 0;

	// Convert
	int res = CFfConvert::ConvertColorFI( m_pFrame, m_nFmt, m_pCodecContext->width, m_pCodecContext->height, img, SWS_FAST_BILINEAR, 1 );

	return res ? 1 : 0;
}


static AVCodecTag g_ff_codec_map[] =
{
    { CODEC_ID_MPEG4,			MKTAG('M', 'P', '4', 'V') },

	{ CODEC_ID_NONE,			0 }
};


int CFfDecoder::LookupCodecId( const sqbind::stdString &sCodec )
{_STT();
	char c[ 5 ] = { ' ', ' ', ' ', ' ', 0 };
	for ( oexSIZE_T i = 0; i < 4 && i < sCodec.length(); i++ )
		c[ i ] = (char)sCodec[ i ];

	// Find a codec with that name
	for ( int i = 0; CODEC_ID_NONE != ff_codec_bmp_tags[ i ].id; i++ )
		if ( *(oex::oexUINT32*)c == ff_codec_bmp_tags[ i ].tag )
			return ff_codec_bmp_tags[ i ].id;

	// Extras
	for ( int i = 0; CODEC_ID_NONE != g_ff_codec_map[ i ].id; i++ )
		if ( *(oex::oexUINT32*)c == g_ff_codec_map[ i ].tag )
			return g_ff_codec_map[ i ].id;

	return 0;
}

sqbind::stdString CFfDecoder::LookupCodecName( int nId )
{_STT();
	// Find a codec with that id
	for ( int i = 0; CODEC_ID_NONE != ff_codec_bmp_tags[ i ].id; i++ )
		if ( ff_codec_bmp_tags[ i ].id == (CodecID)nId )
		{	char b[ 5 ] = { 0, 0, 0, 0, 0 };
			*(oex::oexUINT32*)b = ff_codec_bmp_tags[ i ].tag;
			return oexMbToStrPtr( b );
		} // end if

	// Find a codec with that id
	for ( int i = 0; CODEC_ID_NONE != g_ff_codec_map[ i ].id; i++ )
		if ( g_ff_codec_map[ i ].id == (CodecID)nId )
		{	char b[ 5 ] = { 0, 0, 0, 0, 0 };
			*(oex::oexUINT32*)b = g_ff_codec_map[ i ].tag;
			return oexMbToStrPtr( b );
		} // end if

	return oexT( "" );
}
