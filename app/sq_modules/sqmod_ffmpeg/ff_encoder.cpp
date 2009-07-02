// ff_encoder.cpp

#include "stdafx.h"

CFfEncoder::CFfEncoder()
{
	m_pCodec = oexNULL;
	m_pCodecContext = oexNULL;
	m_pStream = oexNULL;
	m_pOutput = oexNULL;
	m_pFormatContext = oexNULL;

	// Initialize ffmpeg
    avcodec_init();

	// Register all available codecs
//    avcodec_register_all();

	av_register_all();

}

void CFfEncoder::Destroy()
{
/*
	if ( m_pStream )
	{	av_free( m_pStream );
		m_pStream = oexNULL;
	} // end if

	if ( m_pFormatContext )
	{	av_free( m_pFormatContext );
		m_pFormatContext = oexNULL;
	} // end if

	m_pCodec = oexNULL;
*/
}

// http://lists.mplayerhq.hu/pipermail/libav-user/2009-June/003257.html
int CFfEncoder::Create( int x_nCodec )
{
	// Lose previous codec
	Destroy();

	m_pOutput = guess_format( oexT( "avi" ), 0, 0 );
	if ( !m_pOutput )
		return 0;

	m_pFormatContext = av_alloc_format_context();
	if ( !m_pFormatContext )
		return 0;

	m_pFormatContext->oformat = m_pOutput;
	strcpy( m_pFormatContext->filename, "test.avi" );
	m_pFormatContext->max_delay = (int)( 0.5 * AV_TIME_BASE );
	m_pFormatContext->loop_output = AVFMT_NOOUTPUTLOOP;
	m_pFormatContext->flags |= AVFMT_FLAG_NONBLOCK;

	AVFormatParameters params;
    params.prealloced_context = 1;
    params.video_codec_id = CODEC_ID_MPEG4;
    params.audio_codec_id = CODEC_ID_NONE;
    params.width = 320;
    params.height = 240;
    params.time_base.num = 1;
    params.time_base.den = 30;
    params.pix_fmt = PIX_FMT_YUV420P;
    if ( 0 > av_set_parameters( m_pFormatContext, &params ) )
		return 0;

	m_pStream = av_new_stream( m_pFormatContext, 0 );
	if ( !m_pStream )
		return 0;

	m_pFormatContext->streams[ 0 ] = m_pStream;
	m_pFormatContext->nb_streams = 1;

	m_pCodec = avcodec_find_encoder( (CodecID)x_nCodec );
	if ( !m_pCodec )
		return 0;

	m_pCodecContext = m_pStream->codec;
	if ( !m_pCodecContext )
		return 0;

    m_pCodecContext->codec_id = (CodecID)x_nCodec;
    m_pCodecContext->codec_type = CODEC_TYPE_VIDEO;
    m_pCodecContext->bit_rate = 400000;
    m_pCodecContext->width = 320;
    m_pCodecContext->height = 240;
    m_pCodecContext->gop_size = 12;
    m_pCodecContext->time_base.den = 30;
    m_pCodecContext->time_base.num = 1;
    m_pCodecContext->pix_fmt = PIX_FMT_YUV420P;
    m_pCodecContext->me_method = 1;
    m_pCodecContext->strict_std_compliance = 1;

	return 1;
}


