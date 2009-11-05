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
	m_pFrame = oexNULL;
	m_nFmt = 0;
	oexZero( m_pkt );

}

void CFfDecoder::Destroy()
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

int CFfDecoder::Decode( sqbind::CSqBinary *in, int fmt, sqbind::CSqBinary *out, sqbind::CSqMulti *m )
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

	m_pkt.data = in->_Ptr();
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

int CFfDecoder::DecodeImage( sqbind::CSqBinary *in, sqbind::CSqImage *img, sqbind::CSqMulti *m )
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

	if ( m_pFrame )
	{	av_free( m_pFrame );
		m_pFrame = oexNULL;
	} // end if

	m_pFrame = avcodec_alloc_frame();
	if ( !m_pFrame )
		return 0;

	int gpp = 0;
	int used = avcodec_decode_video( m_pCodecContext, m_pFrame, &gpp, in->_Ptr(), in->getUsed() );

	if ( 0 >= gpp )
		return 0;

	// Convert
	int res = CFfConvert::ConvertColorFI( m_pFrame, m_nFmt, m_pCodecContext->width, m_pCodecContext->height, img, SWS_FAST_BILINEAR, 1 );

	return res ? 1 : 0;
}


struct SCodecMap
{
	enum 
	{
		// Video is flipped
		eFlipped = 1
	};

	int			id;
	char		name[ 16 ];
	int			flags;
};

static SCodecMap g_ff_codec_map[] = 
{
	{ CODEC_ID_MPEG4,		"mpg4",		0 },
	{ CODEC_ID_MPEG4,		"fmp4",		0 },
	{ CODEC_ID_MPEG4,		"divx",		0 },
	{ CODEC_ID_MPEG4,		"dx50",		0 },
	{ CODEC_ID_MPEG4,		"xvid",		0 },
	{ CODEC_ID_MPEG4,		"mp4s",		0 },
	{ CODEC_ID_MPEG4,		"m4s2",		0 },
	{ CODEC_ID_MPEG4,		"div1",		0 },
	{ CODEC_ID_MPEG4,		"blz0",		0 },
	{ CODEC_ID_MPEG4,		"mp4v",		0 },
	{ CODEC_ID_MPEG4,		"ump4",		0 },
	{ CODEC_ID_MPEG4,		"wv1f",		0 },
	{ CODEC_ID_MPEG4,		"sedg",		0 },
	{ CODEC_ID_MPEG4,		"rmp4",		0 },
	{ CODEC_ID_MPEG4,		"3iv2",		0 },
	{ CODEC_ID_MPEG4,		"ffds",		0 },
	{ CODEC_ID_MPEG4,		"fvfw",		0 },
	{ CODEC_ID_MPEG4,		"dcod",		0 },
	{ CODEC_ID_MPEG4,		"mvxm",		0 },
	{ CODEC_ID_MPEG4,		"pm4v",		0 },
	{ CODEC_ID_MPEG4,		"smp4",		0 },
	{ CODEC_ID_MPEG4,		"dxgm",		0 },
	{ CODEC_ID_MPEG4,		"vidm",		0 },
	{ CODEC_ID_MPEG4,		"m4t3",		0 },
	{ CODEC_ID_MPEG4,		"geox",		0 },
	{ CODEC_ID_MPEG4,		"hdx4",		SCodecMap::eFlipped },
	{ CODEC_ID_MPEG4,		"dmk2",		0 },
	{ CODEC_ID_MPEG4,		"digi",		0 },
	{ CODEC_ID_MPEG4,		"inmc",		0 },
	{ CODEC_ID_MPEG4,		"ephv",		0 },
	{ CODEC_ID_MPEG4,		"em4a",		0 },
	{ CODEC_ID_MPEG4,		"m4cc",		0 },

	{ CODEC_ID_H264,		"h264",		0 },
	{ CODEC_ID_H264,		"x264",		0 },
	{ CODEC_ID_H264,		"avc1",		0 },
	{ CODEC_ID_H264,		"vssh",		0 },

	{ CODEC_ID_H263,		"h263",		0 },
	{ CODEC_ID_H263,		"x263",		0 },
	{ CODEC_ID_H263,		"t263",		0 },
	{ CODEC_ID_H263,		"l263",		0 },
	{ CODEC_ID_H263,		"vx1k",		0 },

	{ CODEC_ID_H263P,		"p263",		0 },
	{ CODEC_ID_H263I,		"i263",		0 },

	{ CODEC_ID_MSMPEG4V3,	"div3",		0 },
	{ CODEC_ID_MSMPEG4V3,	"mp43",		0 },
	{ CODEC_ID_MSMPEG4V3,	"mpg3",		0 },
	{ CODEC_ID_MSMPEG4V3,	"div6",		0 },
	{ CODEC_ID_MSMPEG4V3,	"div5",		0 },
	{ CODEC_ID_MSMPEG4V3,	"div4",		0 },
	{ CODEC_ID_MSMPEG4V3,	"dvx3",		0 },
	{ CODEC_ID_MSMPEG4V3,	"ap41",		0 },
	{ CODEC_ID_MSMPEG4V3,	"col1",		0 },
	{ CODEC_ID_MSMPEG4V3,	"col2",		0 },

	{ CODEC_ID_MSMPEG4V2,	"mp42",		0 },
	{ CODEC_ID_MSMPEG4V2,	"div2",		0 },

	{ CODEC_ID_MSMPEG4V1,	"mpg4",		0 },
	{ CODEC_ID_MSMPEG4V1,	"mp41",		0 },

	{ CODEC_ID_WMV1,		"wmv1",		0 },
	{ CODEC_ID_WMV2,		"wmv2",		0 },
	{ CODEC_ID_WMV3,		"wmv3",		0 },

	{ CODEC_ID_DVVIDEO,		"dvsd",		0 },
	{ CODEC_ID_DVVIDEO,		"dvhd",		0 },
	{ CODEC_ID_DVVIDEO,		"dvs1",		0 },
	{ CODEC_ID_DVVIDEO,		"dv25",		0 },
	{ CODEC_ID_DVVIDEO,		"dv50",		0 },
	{ CODEC_ID_DVVIDEO,		"cdvc",		0 },
	{ CODEC_ID_DVVIDEO,		"dvc ",		0 },

	{ CODEC_ID_MPEG1VIDEO,	"mpg1",		0 },
	{ CODEC_ID_MPEG2VIDEO,	"mpg2",		0 },

	{ CODEC_ID_MJPEG,		"mjpg",		0 },
	{ CODEC_ID_MJPEG,		"ljpg",		0 },
	{ CODEC_ID_MJPEG,		"dmb1",		0 },
	{ CODEC_ID_MJPEG,		"jpgl",		0 },
	{ CODEC_ID_MJPEG,		"mjls",		0 },
	{ CODEC_ID_MJPEG,		"jpeg",		0 },
	{ CODEC_ID_MJPEG,		"ijpg",		0 },
	{ CODEC_ID_MJPEG,		"avrn",		0 },
	{ CODEC_ID_MJPEG,		"acdv",		0 },
	{ CODEC_ID_MJPEG,		"qivg",		0 },
	{ CODEC_ID_MJPEG,		"slmj",		0 },
	{ CODEC_ID_MJPEG,		"cjpg",		0 },

	{ CODEC_ID_INDEO3,		"rt21",		0 },
	{ CODEC_ID_INDEO3,		"iv31",		0 },
	{ CODEC_ID_INDEO3,		"iv32",		0 },
	{ CODEC_ID_INDEO4,		"iv41",		0 },
	{ CODEC_ID_INDEO5,		"iv50",		0 },

	{ CODEC_ID_MSVIDEO1,	"msvc",		0 },
	{ CODEC_ID_MSVIDEO1,	"cram",		0 },
	{ CODEC_ID_MSVIDEO1,	"wham",		0 },

	{ CODEC_ID_CINEPAK,		"cvid",		0 },
	{ CODEC_ID_ZLIB,		"zlib",		0 },
	{ CODEC_ID_SNOW,		"snow",		0 },
	{ CODEC_ID_FLV1,		"flv1",		0 },
	{ CODEC_ID_FLASHSV,		"fsv1",		0 },
	{ CODEC_ID_THEORA,		"theo",		0 },
	{ CODEC_ID_JPEG2000,	"mj2c",		0 },
	{ CODEC_ID_PNG,			"mpng",		0 },

	{ 0, 0, 0 }
};

int CFfDecoder::LookupCodecId( const sqbind::stdString &sCodec )
{
	if ( 4 > sCodec.length() )
		return 0;

	// Find a codec with that name
	for ( int i = 0; g_ff_codec_map[ i ].name; i++ )
		if ( !strnicmp( sCodec.c_str(), g_ff_codec_map[ i ].name, 4 ) )
			return g_ff_codec_map[ i ].id;

	return 0;
}

sqbind::stdString CFfDecoder::LookupCodecName( int nId )
{
	// Find a codec with that id
	for ( int i = 0; g_ff_codec_map[ i ].name; i++ )
		if ( nId == g_ff_codec_map[ i ].id )
			return g_ff_codec_map[ i ].name;

	return oexT( "" );
}
