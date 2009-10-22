// ff_decoder.cpp

#include "stdafx.h"


// av_open_input_file av_find_stream_info av_log_set_callback
// void av_log_default_callback(void* ptr, int level, const char* fmt, va_list vl)

CFfDecoder::CFfDecoder()
{
	m_pCodec = oexNULL;
	m_pCodecContext = oexNULL;
	m_pFormatContext = oexNULL;
	m_nFmt = 0;

	// Register codecs
	av_register_all();
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

	// Allocate context
	m_pCodecContext = avcodec_alloc_context();
	if ( !m_pCodecContext )
	{	oexERROR( 0, oexT( "avcodec_alloc_context() failed" ) );
		Destroy();
		return 0;
	} // end if

	avcodec_get_context_defaults2( m_pCodecContext, CODEC_TYPE_VIDEO );

    m_pCodecContext->codec_id = (CodecID)x_nCodec;
//    m_pCodecContext->codec_type = CODEC_TYPE_VIDEO;
//    m_pCodecContext->bit_rate = 400000;
    m_pCodecContext->width = width;
    m_pCodecContext->height = height;
//    m_pCodecContext->gop_size = 12;
//    m_pCodecContext->time_base.den = 30;
//    m_pCodecContext->time_base.num = 1;
//    m_pCodecContext->me_method = 1;
    m_pCodecContext->strict_std_compliance = cmp;
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

int CFfDecoder::DecodeImage( sqbind::CSqBinary *in, sqbind::CSqImage *img, int alg )
{
	// Ensure codec
	if ( !m_pCodecContext )
		return 0;

	int width = m_pCodecContext->width;
	int height = m_pCodecContext->height;

	// Validate params
	if ( !in || !in->getUsed() || !img || 0 >= width || 0 >= height )
		return 0;

	// Ensure padding
	if ( in->getUsed() + FF_INPUT_BUFFER_PADDING_SIZE > in->Size() )
		in->Resize( in->getUsed() + FF_INPUT_BUFFER_PADDING_SIZE );

	// Get stream info if needed
//	if ( !m_pFormatContext )
//		FindStreamInfo( in );

	// Temp buffer
	int nSize = CFfConvert::CalcImageSize( m_nFmt, width, height );
	if ( !nSize )
		return 0;

	if ( !m_tmp.Allocate( nSize ) )
		return 0;

	if ( !oex::cmn::IsAligned16( (long)in->_Ptr() ) )
		return 0;

	if ( !oex::cmn::IsAligned16( (long)m_tmp._Ptr() ) )
		return 0;

	AVFrame *paf = avcodec_alloc_frame();
	if ( !paf )
		return 0;

	if ( !CFfConvert::FillAVFrame( paf, m_nFmt, width, height, (void*)m_tmp._Ptr() ) )
	{	av_free( paf );
		return 0;
	} // end if

	int gpp = 0;
	int used = avcodec_decode_video( m_pCodecContext, paf, &gpp, in->_Ptr(), in->Size() );

//	AVPacket pkt;
//	av_init_packet( &pkt );
//	pkt.data = in->_Ptr();
//	pkt.size = in->Size();
//	int used = avcodec_decode_video2( m_pCodecContext, paf, &gpp, &pkt );

	av_free( paf );

	if ( 0 >= used || 0 >= gpp )
		return 0;

	m_tmp.setUsed( nSize );

	// Must convert to input format
	if ( !CFfConvert::ConvertColorBI( &m_tmp, m_nFmt, width, height, img, alg ) )
		return 0;

	return 1;
}


