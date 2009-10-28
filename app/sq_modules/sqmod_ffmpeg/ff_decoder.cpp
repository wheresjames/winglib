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

CFfDecoder::CFfDecoder()
{
	m_pCodec = oexNULL;
	m_pCodecContext = oexNULL;
	m_pFormatContext = oexNULL;
	m_nFmt = 0;

	// Register codecs
//	av_register_all();
}

void CFfDecoder::Destroy()
{
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
}

int CFfDecoder::Create( int x_nCodec, int fmt, int width, int height, int cmp )
{
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
    m_pCodecContext->bit_rate = 400000;
    m_pCodecContext->width = width;
    m_pCodecContext->height = height;
//	m_pCodecContext->strict_std_compliance = cmp;
	m_pCodecContext->pix_fmt = (PixelFormat)fmt;

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
{
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

int CFfDecoder::Decode( sqbind::CSqBinary *in, int fmt, sqbind::CSqBinary *out, int alg )
{
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
		// Don't zero mor than twice the padding size
		if ( nPadding > ( FF_INPUT_BUFFER_PADDING_SIZE * 2 ) )
			nPadding = FF_INPUT_BUFFER_PADDING_SIZE * 2;

		// Set end to zero to ensure no overreading on damaged blocks
		oexZeroMemory( &in->_Ptr()[ in->getUsed() ], nPadding );

	} // end if

	AVFrame *paf = avcodec_alloc_frame();
	if ( !paf )
		return 0;

	int gpp = 0;
	int used = avcodec_decode_video( m_pCodecContext, paf, &gpp, in->_Ptr(), in->getUsed() );

	if ( 0 >= gpp )
	{//	av_free( paf );
		return 0;
	} // end if

	// Read out
	int width = m_pCodecContext->width;
	int height = m_pCodecContext->height;
	int res = CFfConvert::ConvertColorFB( paf, m_nFmt, width, height, fmt, out, alg );

	// +++ Not sure if this is right or not?
//	av_free( paf );

	return res ? 1 : 0;
}

int CFfDecoder::DecodeImage( sqbind::CSqBinary *in, sqbind::CSqImage *img, int alg )
{
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
		// Don't zero mor than twice the padding size
		if ( nPadding > ( FF_INPUT_BUFFER_PADDING_SIZE * 2 ) )
			nPadding = FF_INPUT_BUFFER_PADDING_SIZE * 2;

		// Set end to zero to ensure no overreading on damaged blocks
		oexZeroMemory( &in->_Ptr()[ in->getUsed() ], nPadding );

	} // end if

	AVFrame *paf = avcodec_alloc_frame();
	if ( !paf )
		return 0;

	int gpp = 0;
	int used = avcodec_decode_video( m_pCodecContext, paf, &gpp, in->_Ptr(), in->getUsed() );

	if ( 0 >= gpp )
	{	av_free( paf );
		return 0;
	} // end if

	// Read out
	int width = m_pCodecContext->width;
	int height = m_pCodecContext->height;
	int res = CFfConvert::ConvertColorFI( paf, m_nFmt, width, height, img, alg, 1 );

	// +++ Not sure if this is right or not?
//	av_free( paf );

	return res ? 1 : 0;
}


