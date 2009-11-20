// ff_audio_decoder.cpp

#include "stdafx.h"

extern "C" 
{
#	include "libavformat/riff.h"
};

CFfAudioDecoder::CFfAudioDecoder()
{
	m_pCodec = oexNULL;
	m_pCodecContext = oexNULL;
	m_pFormatContext = oexNULL;
	m_pFrame = oexNULL;
	m_nFmt = 0;
	oexZero( m_pkt );

}

void CFfAudioDecoder::Destroy()
{
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

int CFfAudioDecoder::Create( int x_nCodec, int fmt )
{
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

int CFfAudioDecoder::FindStreamInfo( sqbind::CSqBinary *in )
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

int CFfAudioDecoder::Decode( sqbind::CSqBinary *in, int fmt, sqbind::CSqBinary *out, sqbind::CSqMulti *m )
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

#if defined( FFSQ_VIDEO2 )

	int gpp = 0;
	int used = avcodec_decode_video2( m_pCodecContext, m_pFrame, &gpp, &m_pkt );

#else

	int gpp = 0;
	int used = avcodec_decode_video( m_pCodecContext, m_pFrame, &gpp, in->_Ptr(), in->getUsed() );
	if ( 0 >= used )
	{	oexEcho( "!used" );
		return -1;
	} // end if

#endif

	if ( used != m_pkt.size )
		oexEcho( "Unsed data!!!" );

//	int gpp = 0;
//	int used = avcodec_decode_video( m_pCodecContext, m_pFrame, &gpp, in->_Ptr(), in->getUsed() );

	if ( 0 >= gpp )
		return 0;

	// Convert
	int res = CFfConvert::ConvertColorFB( m_pFrame, m_nFmt, m_pCodecContext->width, m_pCodecContext->height, fmt, out, SWS_FAST_BILINEAR );

	return res ? 1 : 0;
}

static AVCodecTag g_ff_audio_codec_map[] = 
{
    { CODEC_ID_AAC,				MKTAG('A', 'M', 'R', ' ') },
    { CODEC_ID_AMR_NB,			MKTAG('A', 'M', 'R', ' ') },
    { CODEC_ID_AMR_WB,			MKTAG('A', 'M', 'R', ' ') },

	{ CODEC_ID_NONE,			0 }
};

int CFfAudioDecoder::LookupCodecId( const sqbind::stdString &sCodec )
{
	char c[ 5 ] = { ' ', ' ', ' ', ' ', 0 };
	for ( int i = 0; i < 4 && i < sCodec.length(); i++ )
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
{
	// Find a codec with that id
	for ( int i = 0; CODEC_ID_NONE != ff_codec_wav_tags[ i ].id; i++ )
		if ( ff_codec_wav_tags[ i ].id == (CodecID)nId )
		{	char b[ 5 ] = { 0, 0, 0, 0, 0 };
			*(oex::oexUINT32*)b = ff_codec_wav_tags[ i ].tag;
			return oexMbToStrPtr( b );
		} // end if

	return oexT( "" );
}
