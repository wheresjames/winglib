// ff_encoder.cpp

#include "stdafx.h"

CFfEncoder::CFfEncoder()
{
	m_nFmt = 0;
	m_pCodec = oexNULL;
	m_pCodecContext = oexNULL;
	m_pStream = oexNULL;
	m_pOutput = oexNULL;
	m_pFormatContext = oexNULL;

	// Register codecs
	oexAlignCall( 16, av_register_all() );
}

void CFfEncoder::Destroy()
{
	if ( m_pStream )
	{	av_free( m_pStream );
		m_pStream = oexNULL;
	} // end if

	if ( m_pFormatContext )
	{	av_free( m_pFormatContext );
		m_pFormatContext = oexNULL;
	} // end if

	m_nFmt = 0;
	m_pCodec = oexNULL;
	m_pOutput = oexNULL;
	m_pCodecContext = oexNULL;

	m_tmp.Free();
}

// http://lists.mplayerhq.hu/pipermail/libav-user/2009-June/003257.html
int CFfEncoder::Create( int x_nCodec, int fmt, int width, int height )
{
	m_nFmt = fmt;

	if ( 0 >= width || 0 >= height )
		return 0;

	int res = 0;
	sqbind::stdString x_sContainer = oexT( "avi" );

	// Lose previous codec
	Destroy();

	m_pOutput = guess_format( x_sContainer.c_str(), 0, 0 );
	if ( !m_pOutput )
	{	oexERROR( 0, oexMks( oexT( "guess_format( " ), x_sContainer.c_str(), oexT( " ) failed " ) ) );
		Destroy();
		return 0;
	} // end if

	m_pFormatContext = avformat_alloc_context();
	if ( !m_pFormatContext )
	{	oexERROR( 0, oexT( "av_alloc_format_context() failed " ) );
		Destroy();
		return 0;
	} // end if

	m_pFormatContext->oformat = m_pOutput;
	strcpy( m_pFormatContext->filename, "test.avi" );
	m_pFormatContext->max_delay = (int)( 0.5 * AV_TIME_BASE );
	m_pFormatContext->loop_output = AVFMT_NOOUTPUTLOOP;
	m_pFormatContext->flags |= AVFMT_FLAG_NONBLOCK;

	AVFormatParameters params;
    params.prealloced_context = 1;
    params.video_codec_id = (CodecID)x_nCodec;
    params.audio_codec_id = CODEC_ID_NONE;
    params.width = width;
    params.height = height;
    params.time_base.num = 1;
    params.time_base.den = 30;
	params.pix_fmt = (PixelFormat)m_nFmt;

	res = av_set_parameters( m_pFormatContext, &params );
	if ( 0 > res )
	{	oexERROR( res, oexT( "av_set_parameters() failed " ) );
		Destroy();
		return 0;
	} // end if

	m_pStream = av_new_stream( m_pFormatContext, 0 );
	if ( !m_pStream )
	{	oexERROR( 0, oexT( "av_new_stream() failed " ) );
		Destroy();
		return 0;
	} // end if

	m_pFormatContext->streams[ 0 ] = m_pStream;
	m_pFormatContext->nb_streams = 1;

	m_pCodec = avcodec_find_encoder( (CodecID)x_nCodec );
	if ( !m_pCodec )
	{	oexERROR( 0, oexMks( oexT( "avcodec_find_encoder() failed to find codec for id : " ), (int)x_nCodec ) );
		Destroy();
		return 0;
	} // end if

	m_pCodecContext = m_pStream->codec;
	if ( !m_pCodecContext )
	{	oexERROR( 0, oexT( "codec field is NULL" ) );
		Destroy();
		return 0;
	} // end if

    m_pCodecContext->codec_id = (CodecID)x_nCodec;
    m_pCodecContext->codec_type = CODEC_TYPE_VIDEO;
    m_pCodecContext->bit_rate = 400000;
    m_pCodecContext->width = width;
    m_pCodecContext->height = height;
    m_pCodecContext->gop_size = 12;
    m_pCodecContext->time_base.den = 30;
    m_pCodecContext->time_base.num = 1;
    m_pCodecContext->me_method = 1;
    m_pCodecContext->strict_std_compliance = 1;
	m_pCodecContext->pix_fmt = (PixelFormat)m_nFmt;

	res = avcodec_open( m_pCodecContext, m_pCodec );
	if ( 0 > res )
	{	oexERROR( res, oexT( "avcodec_open() failed" ) );
		Destroy();
		return 0;
	} // end if

	return 1;
}

int CFfEncoder::EncodeRaw( int fmt, int width, int height, const void *in, int sz_in, sqbind::CSqBinary *out )
{
	// Ensure codec
	if ( !m_pCodecContext )
		return 0;

	// Validate parmeters
	if ( !in || !sz_in || !out || !width || !height )
		return 0;

	// Validate buffer size
	if ( CFfConvert::CalcImageSize( fmt, width, height ) > sz_in )
		return 0;

	// How much room could we possibly need
	int nSize = CFfConvert::CalcImageSize( fmt, width, height ) * 2;
	if ( out->Size() < nSize && !out->Obj().OexNew( nSize ).Ptr() )
		return 0;

	AVFrame *paf = avcodec_alloc_frame();
	if ( !paf )
		return 0;

	if ( !CFfConvert::FillAVFrame( paf, fmt, width, height, (void*)in ) )
		return 0;

	paf->key_frame = 1;
//	paf->pts = AV_NOPTS_VALUE;
//	paf->motion_val = { 0, 0 };

	int nBytes = avcodec_encode_video( m_pCodecContext, out->Obj().Ptr(), nSize, paf );
	if ( 0 > nBytes )
	{	oexERROR( nBytes, oexT( "avcodec_encode_video() failed" ) );
		out->setUsed( 0 );
		av_free( paf );
		return 0;
	} // end if

	av_free( paf );

	out->setUsed( nBytes );

	return nBytes;
}

int CFfEncoder::Encode( int fmt, int width, int height, sqbind::CSqBinary *in, sqbind::CSqBinary *out )
{
	// Sanity checks
	if ( !in || !in->getUsed() )
		return 0;

	return EncodeRaw( fmt, width, height, in->Ptr(), in->getUsed(), out );
}

int CFfEncoder::EncodeImage( sqbind::CSqImage *img, sqbind::CSqBinary *out, int alg )
{
	// Ensure codec
	if ( !m_pCodecContext )
		return 0;

	// Validate params
	if ( !img || !out )
		return 0;

	// Do we need to convert the colorspace?
	if ( PIX_FMT_RGB24 == m_nFmt )
		return EncodeRaw( PIX_FMT_RGB24, img->getWidth(), img->getHeight(), img->Obj().GetBits(), img->Obj().GetImageSize(), out );

	// Must convert to input format
	if ( !CFfConvert::ConvertColorIB( img, &m_tmp, m_nFmt, alg ) )
		return 0;

	// Do the conversion
	return EncodeRaw( m_nFmt, img->getWidth(), img->getHeight(), m_tmp.Ptr(), m_tmp.getUsed(), out );
}

