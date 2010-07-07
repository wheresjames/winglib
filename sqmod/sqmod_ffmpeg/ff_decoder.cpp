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

	// Truncated data
	if( 0 != ( m_pCodec->capabilities & CODEC_CAP_TRUNCATED ) )
		m_pCodecContext->flags |= CODEC_FLAG_TRUNCATED;

	// Codec context
	if ( m_extra.getUsed() )
	{	m_pCodecContext->extradata_size = m_extra.getUsed();
		m_pCodecContext->extradata = (uint8_t*)m_extra._Ptr();
		m_pCodecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;
	} // end if

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

/*
	AVPacket pkt; oexZero( pkt );
	pkt.data = (uint8_t*)in->_Ptr();
	pkt.size = in->getUsed();

	// Ensure buffer size
	if ( ( m_tmp.Size() - m_tmp.getUsed() ) < (sqbind::CSqBinary::t_size)( pkt.size + FF_INPUT_BUFFER_PADDING_SIZE ) )
		m_tmp.Allocate( 2 * ( m_tmp.Size() + pkt.size + FF_INPUT_BUFFER_PADDING_SIZE ) );

	// Add new data
	m_tmp.AppendBuffer( (sqbind::CSqBinary::t_byte*)pkt.data, pkt.size );

oexSHOW( m_tmp.getUsed() );

	// Create packet
	pkt.data = (uint8_t*)m_tmp._Ptr();
	pkt.size = m_tmp.getUsed();

	// Zero padding
	int nPadding = m_tmp.Size() - m_tmp.getUsed();
	if ( 0 < nPadding )
	{
		// Don't zero more than twice the padding size
		if ( nPadding > ( FF_INPUT_BUFFER_PADDING_SIZE * 2 ) )
			nPadding = FF_INPUT_BUFFER_PADDING_SIZE * 2;

		// Set end to zero to ensure no overreading on damaged blocks
		oexZeroMemory( &m_tmp._Ptr()[ m_tmp.getUsed() ], nPadding );

	} // end if
*/
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

	if ( !m_pFrame )
		m_pFrame = avcodec_alloc_frame();

	if ( !m_pFrame )
		return 0;

	int gpp = 0;
	int used = 0;
#if defined( FFSQ_VIDEO2 )

		// Init packet
		AVPacket pkt; oexZero( pkt );
//		av_init_packet( &pkt );

		if ( m )
		{
			if ( m->isset( oexT( "flags" ) ) )
				pkt.flags = (*m)[ oexT( "flags" ) ].toint();
			if ( m->isset( oexT( "stream_index" ) ) )
				pkt.stream_index = (*m)[ oexT( "stream_index" ) ].toint();
			if ( m->isset( oexT( "pos" ) ) )
				pkt.pos = (*m)[ oexT( "pos" ) ].toint64();
			if ( m->isset( oexT( "dts" ) ) )
				pkt.dts = (*m)[ oexT( "dts" ) ].toint64();
			if ( m->isset( oexT( "pts" ) ) )
				pkt.pts = (*m)[ oexT( "pts" ) ].toint64();
			if ( m->isset( oexT( "duration" ) ) )
				pkt.duration = (*m)[ oexT( "duration" ) ].toint();
			if ( m->isset( oexT( "convergence_duration" ) ) )
				pkt.duration = (*m)[ oexT( "convergence_duration" ) ].toint();

//oexSHOW( m->serialize().c_str() );

		} // end if

		pkt.data = (uint8_t*)in->_Ptr();
		pkt.size = in->getUsed();

		used = avcodec_decode_video2( m_pCodecContext, m_pFrame, &gpp, &pkt );
		if ( 0 >= used )
		{	oexSHOW( used );
			return 0;
		} // end if

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
