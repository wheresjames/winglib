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

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CFfDecoder, CFfDecoder )

	SQBIND_MEMBER_FUNCTION( CFfDecoder, Destroy )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, Create )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, isValid )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, FindStreamInfo )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, Decode )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, DecodeImage )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, getWidth )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, getHeight )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, getFps )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, getBitRate )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, setExtraData )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, getExtraData )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, BufferData )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, UnBufferData )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, getBufferSize )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, getH264FrameType )
//	SQBIND_MEMBER_FUNCTION( CFfDecoder,  )
//	SQBIND_MEMBER_FUNCTION( CFfDecoder,  )
//	SQBIND_MEMBER_FUNCTION( CFfDecoder,  )
//	SQBIND_MEMBER_FUNCTION( CFfDecoder,  )
//	SQBIND_MEMBER_FUNCTION( CFfDecoder,  )


	SQBIND_STATIC_FUNCTION( CFfDecoder, LookupCodecId )
	SQBIND_STATIC_FUNCTION( CFfDecoder, LookupCodecName )


SQBIND_REGISTER_CLASS_END()

void CFfDecoder::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CFfDecoder );
}

CFfDecoder::CFfDecoder()
{_STT();
	m_pCodec = oexNULL;
	m_pCodecContext = oexNULL;
	m_pFormatContext = oexNULL;
	m_pFrame = oexNULL;
	m_nFmt = 0;
	m_nFps = 0;
	m_nFrame = 0;
	m_nFlags = 0;
	m_nWaitKeyFrame = 0;
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
	m_nFps = 0;
	m_nFrame = 0;
	m_nFlags = 0;
	m_pCodec = oexNULL;
	m_nWaitKeyFrame = 0;
	oexZero( m_pkt );
}

int CFfDecoder::Create( int x_nCodec, int fmt, int width, int height, int fps, int brate, sqbind::CSqMulti *m )
{_STT();

	oexAutoLock ll( _g_ffmpeg_lock );
	if ( !ll.IsLocked() ) return 0;

	// Lose previous codec
	Destroy();

	m_pCodec = avcodec_find_decoder( (AVCodecID)x_nCodec );
	if ( !m_pCodec )
	{	oexERROR( 0, oexT( "avcodec_find_decoder() failed" ) );
		return 0;
	} // end if

//	CODEC_CAP_PARSE_ONLY CODEC_CAP_TRUNCATED
//	avcodec_parse_frame

	// Allocate context
//	m_pCodecContext = avcodec_alloc_context();
	m_pCodecContext = avcodec_alloc_context3( m_pCodec );
	if ( !m_pCodecContext )
	{	oexERROR( 0, oexT( "avcodec_alloc_context() failed" ) );
		Destroy();
		return 0;
	} // end if

//	ff_p->fmt = avformat_alloc_context();
//	ff_p->fmt->max_delay = 100000;

//	avcodec_get_context_defaults( m_pCodecContext );
//	avcodec_get_context_defaults2( m_pCodecContext, AVMEDIA_TYPE_VIDEO );
	avcodec_get_context_defaults3( m_pCodecContext, m_pCodec );

    m_pCodecContext->codec_id = (AVCodecID)x_nCodec;
    m_pCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
    m_pCodecContext->bit_rate = brate;
    m_pCodecContext->width = width; // ( 0 < width ) ? width : 320;
    m_pCodecContext->height = height; // ( 0 < height ) ? height : 240;
    m_pCodecContext->time_base.den = fps; // ( 0 < fps ) ? fps : 30;
    m_pCodecContext->time_base.num = 1;
    m_pCodecContext->strict_std_compliance = ( ( m && m->isset( oexT( "cmp" ) ) ) ? (*m)[ oexT( "cmp" ) ].toint() : 0 );
	m_pCodecContext->pix_fmt = (AVPixelFormat)fmt;
//	m_pCodecContext->max_delay = 1000000;

	// Truncated data
	if( 0 != ( m_pCodec->capabilities & CODEC_CAP_TRUNCATED ) )
		m_pCodecContext->flags |= CODEC_FLAG_TRUNCATED;

	// Codec context
	if ( m_extra.getUsed() )
	{	m_nFlags |= 3;
		m_extra.Resize( m_extra.getUsed() + FF_INPUT_BUFFER_PADDING_SIZE * 2 );
		m_pCodecContext->extradata_size = m_extra.getUsed();
		m_pCodecContext->extradata = (uint8_t*)m_extra._Ptr();
		m_pCodecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;
	} // end if

	// Can we go ahead and open the codec?
	if ( AV_CODEC_ID_H264 != x_nCodec || 3 == ( 3 & m_nFlags ) )
	{
		// Open the codec
		int res = avcodec_open2( m_pCodecContext, m_pCodec, 0 );
		if ( 0 > res )
		{	oexERROR( res, oexT( "avcodec_open() failed" ) );
			m_pCodecContext = oexNULL;
			Destroy();
			return 0;
		} // end if

	} // end if

	else
		oexEcho( "iii Missing SEI Headers, will search the input stream for them." );

	m_nFmt = fmt;
	m_nFps = fps;

	// +++ H264 crashes sometimes if you don't lead in with a key frame
	if ( AV_CODEC_ID_H264 == x_nCodec )
		m_nWaitKeyFrame = m_nFps;

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

	if ( 0 > avformat_find_stream_info( m_pFormatContext, 0 ) )
	{	av_free( m_pFormatContext );
		m_pFormatContext = oexNULL;
		return 0;
	} // end if

	return 1;
}

int CFfDecoder::BufferData( sqbind::CSqBinary *in, sqbind::CSqMulti *m )
{
	// Init packet
	oexZero( m_pkt );

	// Init other packet data
	if ( m )
	{
		if ( m->isset( oexT( "flags" ) ) )
			m_pkt.flags = (*m)[ oexT( "flags" ) ].toint();
		if ( m->isset( oexT( "stream_index" ) ) )
			m_pkt.stream_index = (*m)[ oexT( "stream_index" ) ].toint();
		if ( m->isset( oexT( "pos" ) ) )
			m_pkt.pos = (*m)[ oexT( "pos" ) ].toint64();
		if ( m->isset( oexT( "dts" ) ) )
			m_pkt.dts = (*m)[ oexT( "dts" ) ].toint64();
		if ( m->isset( oexT( "pts" ) ) )
			m_pkt.pts = (*m)[ oexT( "pts" ) ].toint64();
		if ( m->isset( oexT( "duration" ) ) )
			m_pkt.duration = (*m)[ oexT( "duration" ) ].toint();
		if ( m->isset( oexT( "convergence_duration" ) ) )
			m_pkt.duration = (*m)[ oexT( "convergence_duration" ) ].toint();

	} // end if

	// Are we adding data?
	if ( in && in->getUsed() )
	{
		// Ensure buffer size
		if ( ( m_tmp.Size() - m_tmp.getUsed() ) < (sqbind::CSqBinary::t_size)( in->getUsed() + FF_INPUT_BUFFER_PADDING_SIZE ) )
			m_tmp.Resize( 2 * ( m_tmp.Size() + in->getUsed() + FF_INPUT_BUFFER_PADDING_SIZE ) );

		// Add new data to buffer
		m_tmp.Append( in );

	} // end if

	// Ensure we have SEI headers if needed
	if ( AV_CODEC_ID_H264 == m_pCodecContext->codec_id && 3 > m_nFlags )
	{
		// While we need headers
		while ( 3 != ( 3 & m_nFlags ) )
		{
			// Find a start nal header
			int s = FindH264Nal( m_tmp.Ptr(), m_tmp.getUsed() );

			// Do we have enough data?
			if ( 0 > s || ( s + 3 ) >=  m_tmp.getUsed() )
				return 0;

			// Find an end nal header
			int e = FindH264Nal( m_tmp.Ptr( s + 3 ), m_tmp.getUsed() - ( s + 3 ) );
			if ( 0 > e )
				return 0;

			// Attempt to read headers
			ReadSEI( m_tmp.Ptr( s ), e + 3 );

			// Chunk this frame
			UnBufferData( s + 3 + e );

		} // end while

	} // end if

	// Get buffer pointers
	m_pkt.data = (uint8_t*)m_tmp._Ptr();
	m_pkt.size = m_tmp.getUsed();

	// Zero padding
	int nPadding = m_tmp.Size() - m_tmp.getUsed();
	if ( 0 < nPadding )
	{
		// Don't zero more than twice the padding size
		if ( nPadding > ( FF_INPUT_BUFFER_PADDING_SIZE * 2 ) )
			nPadding = FF_INPUT_BUFFER_PADDING_SIZE * 2;

		// Set end to zero to ensure no overreading on damaged blocks
		oexZeroMemory( &m_pkt.data[ m_pkt.size ], nPadding );

	} // end if

	return m_pkt.size;
}

int CFfDecoder::UnBufferData( int uUsed )
{
	if ( 0 > uUsed )
		m_tmp.setUsed( 0 );
	else if ( 0 < uUsed )
		m_tmp.LShift( uUsed );

	// Ensure packet pointers are valid
	m_pkt.data = (uint8_t*)m_tmp._Ptr();
	m_pkt.size = m_tmp.getUsed();

	return m_pkt.size;
}

int CFfDecoder::getBufferSize()
{	if ( !m_pkt.data )
		return 0;
	return m_pkt.size;
}

int CFfDecoder::ReadSEI( const void *p, int len )
{
	if ( !m_pCodecContext )
		return 0;

	// Currently, this only applies to H264
	if ( AV_CODEC_ID_H264 != m_pCodecContext->codec_id || 3 == ( 3 & m_nFlags ) )
		return 1;

#if defined( DEBUG )
//	oexEcho( "!!! Missing SEI Information !!!" );
//	oexEcho( oexBinToAsciiHexStr( sqbind::CSqBinary( p, len ), 0, 16, 16 ).Ptr() );
#endif

	// Process frame type
	switch( GetH264FrameId( p, len ) )
	{
		case 0x67 :
			m_nFlags |= 1;
			m_extra.AppendBuffer( p, len );
			break;

		case 0x68 :
			m_nFlags |= 2;
			m_extra.AppendBuffer( p, len );
			break;

		default :
			break;
	}

	// Did we get complete SEI information?
	if ( 3 == ( 3 & m_nFlags ) )
	{
		// Fill in the missing SEI data
		m_extra.Resize( m_extra.getUsed() + FF_INPUT_BUFFER_PADDING_SIZE * 2 );
		m_pCodecContext->extradata_size = m_extra.getUsed();
		m_pCodecContext->extradata = (uint8_t*)m_extra._Ptr();
		m_pCodecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;

		// Must lock ffmpeg
		oexAutoLock ll( _g_ffmpeg_lock );
		if ( !ll.IsLocked() ) return 0;

		// Attempt to open the codec
		int res = avcodec_open2( m_pCodecContext, m_pCodec, 0 );
		if ( 0 > res )
		{	oexERROR( res, oexT( "avcodec_open2() failed" ) );
			m_pCodecContext = oexNULL;
			Destroy();
			return 0;
		} // end if

		return 1;

	} // end if

	return 0;
}

int CFfDecoder::Decode( sqbind::CSqBinary *in, int fmt, sqbind::CSqBinary *out, sqbind::CSqMulti *m, int flip )
{_STT();

	// Ensure codec
	if ( !m_pCodecContext )
		return 0;

	// Get data packet
	if ( !in || !out || !BufferData( in, m ) )
		return 0;

	// Do we have packet data?
	if ( !m_pkt.data || !m_pkt.size )
		return 0;
		
	// Allocate frame
	if ( !m_pFrame )
	{	m_pFrame = av_frame_alloc();
		if ( !m_pFrame )
			return 0;
	} // end if

	// Waiting for key frame?
	if ( m_nWaitKeyFrame )
	{	if ( AV_CODEC_ID_H264 != m_pCodecContext->codec_id
			 || 0 == GetH264FrameType( m_pkt.data, m_pkt.size ) )
			m_nWaitKeyFrame = 0;
		else
		{	m_nWaitKeyFrame--;
			return 0;
		} // end else
	} // end if

	int gpp = 0, used = 0;
#if defined( FFSQ_VIDEO2 )
	used = avcodec_decode_video2( m_pCodecContext, m_pFrame, &gpp, &m_pkt );
#else
	used = avcodec_decode_video( m_pCodecContext, m_pFrame, &gpp, (uint8_t*)m_pkt.data, m_pkt.size );
#endif

	// Dequeue used data
	UnBufferData( used );

	// Check for error
	if ( 0 > used )
		return 0;

	// Did we get a frame
	if ( 0 >= gpp )
		return 0;

	// Validate image size
	if ( 0 >= m_pCodecContext->width || 0 >= m_pCodecContext->height )
		return 0;

	// Convert
	int res = CFfConvert::ConvertColorFB( m_pFrame, m_nFmt, m_pCodecContext->width, m_pCodecContext->height, fmt, out, SWS_FAST_BILINEAR, flip );
	if ( !res )
		return 0;

	// Count a frame
	m_nFrame++;

	return 1;
}

int CFfDecoder::DecodeImage( sqbind::CSqBinary *in, sqbind::CSqImage *img, sqbind::CSqMulti *m, int flip )
{_STT();
	// Ensure codec
	if ( !m_pCodecContext )
		return 0;

	// Ensure codec
	if ( !m_pCodecContext )
		return 0;

	// Get data packet
	if ( !in || !img || !BufferData( in, m ) )
		return 0;

	// Allocate frame
	if ( !m_pFrame )
	{	m_pFrame = av_frame_alloc();
		if ( !m_pFrame )
			return 0;
	} // end if

	// Waiting for key frame?
	if ( m_nWaitKeyFrame )
	{	if ( AV_CODEC_ID_H264 != m_pCodecContext->codec_id
			 || 0 == GetH264FrameType( m_pkt.data, m_pkt.size ) )
			m_nWaitKeyFrame = 0;
		else
		{	m_nWaitKeyFrame--;
			return 0;
		} // end else
	} // end if

	int gpp = 0, used = 0;
#if defined( FFSQ_VIDEO2 )
		used = avcodec_decode_video2( m_pCodecContext, m_pFrame, &gpp, &m_pkt );
#else
		used = avcodec_decode_video( m_pCodecContext, m_pFrame, &gpp, (uint8_t*)in->_Ptr(), in->getUsed() );
#endif

	// Dequeue used data
	UnBufferData( used );

	// Check for error
	if ( 0 > used )
		return 0;

	if ( 0 >= gpp )
		return 0;

	// Validate image size
	if ( 0 >= m_pCodecContext->width || 0 >= m_pCodecContext->height )
		return 0;

	// Convert
	int res = CFfConvert::ConvertColorFI( m_pFrame, m_nFmt, m_pCodecContext->width, m_pCodecContext->height, img, SWS_FAST_BILINEAR, flip );
	if ( !res )
		return 0;

	// Count a frame
	m_nFrame++;

	return 1;
}

static AVCodecTag g_ff_codec_map[] =
{
    { AV_CODEC_ID_MPEG4,		MKTAG('M', 'P', 'G', '4') },
    { AV_CODEC_ID_MPEG4,		MKTAG('M', 'P', '4', 'V') },

	{ AV_CODEC_ID_NONE,			0 }
};

struct SFfVideoCodecInfo
{
	int			id;
	const char 	*tag;
};

/*
static SFfVideoCodecInfo g_ff_video_codec_info[] =
{
	{ AV_CODEC_ID_H263P,		oexT( "H263-2000" ) },
	{ AV_CODEC_ID_H263P,		oexT( "H263-1998" ) },

	{ AV_CODEC_ID_NONE,			0 }
};
*/

int CFfDecoder::LookupCodecId( const sqbind::stdString &sCodec )
{_STT();
/*
	// Extras by name
	oex::CStr8 s = oexStrToStr8( sqbind::std2oex( sCodec ) );
	for ( int i = 0; AV_CODEC_ID_NONE != g_ff_video_codec_info[ i ].id; i++ )
		if ( !oex::str::ICompare( s.Ptr(), s.Length(),
								  g_ff_video_codec_info[ i ].tag,
								  oex::zstr::Length( g_ff_video_codec_info[ i ].tag ) ) )
			return g_ff_video_codec_info[ i ].id;
*/
	char c[ 5 ] = { ' ', ' ', ' ', ' ', 0 };
	for ( oexSIZE_T i = 0; i < 4 && i < sCodec.length(); i++ )
		c[ i ] = (char)sCodec[ i ];

	// Find a codec with that name
	for ( int i = 0; AV_CODEC_ID_NONE != ff_codec_bmp_tags[ i ].id; i++ )
		if ( *(oex::oexUINT32*)c == ff_codec_bmp_tags[ i ].tag )
			return ff_codec_bmp_tags[ i ].id;

	// Extras
	for ( int i = 0; AV_CODEC_ID_NONE != g_ff_codec_map[ i ].id; i++ )
		if ( *(oex::oexUINT32*)c == g_ff_codec_map[ i ].tag )
			return g_ff_codec_map[ i ].id;

	return 0;
}

sqbind::stdString CFfDecoder::LookupCodecName( int nId )
{_STT();
/*
	// Find a codec with that id
	for ( int i = 0; AV_CODEC_ID_NONE != g_ff_video_codec_info[ i ].id; i++ )
		if ( g_ff_video_codec_info[ i ].id == (AVCodecID)nId )
			return oexMbToStrPtr( g_ff_video_codec_info[ i ].tag );
*/
	// Find a codec with that id
	for ( int i = 0; AV_CODEC_ID_NONE != ff_codec_bmp_tags[ i ].id; i++ )
		if ( ff_codec_bmp_tags[ i ].id == (AVCodecID)nId )
		{	char b[ 5 ] = { 0, 0, 0, 0, 0 };
			*(oex::oexUINT32*)b = ff_codec_bmp_tags[ i ].tag;
			return oexMbToStrPtr( b );
		} // end if

	// Find a codec with that id
	for ( int i = 0; AV_CODEC_ID_NONE != g_ff_codec_map[ i ].id; i++ )
		if ( g_ff_codec_map[ i ].id == (AVCodecID)nId )
		{	char b[ 5 ] = { 0, 0, 0, 0, 0 };
			*(oex::oexUINT32*)b = g_ff_codec_map[ i ].tag;
			return oexMbToStrPtr( b );
		} // end if

	return oexT( "" );
}

int CFfDecoder::FindH264Nal( const void *p, int len )
{
	// Sanity check
	if ( !p || 0 >= len )
		return -1;

	// Find a NAL marker
	unsigned char *b = (unsigned char*)p;
	for ( int i = 0; ( i + 3 ) < len; i++ )
		if ( !b[ i + 0 ] && !b[ i + 1 ] && 0x01 == b[ i + 2 ] )
			return i;

	return -1;
}


int CFfDecoder::GetH264FrameType( const void *p, int len )
{
	if ( !p )
		return -1;

	// Verify the nal marker
	int i = FindH264Nal( p, 4 );
	if ( 0 > i || ( i + 5 ) >= len )
		return -1;

	unsigned char *b = ( (unsigned char*)p ) + i + 3;

	// Check for VOP id
	if ( 0xb6 == *b )
	{	b++;
		return ( *b & 0xc0 ) >> 6;
	} // end if

	switch( *b )
	{	case 0x65 : return 0;
		case 0x61 : return 1;
		case 0x01 : return 2;
	} // end switch

	return -1;
}

int CFfDecoder::GetH264FrameId( const void *p, int len )
{
	if ( !p )
		return -1;

	// Verify the nal marker
	int i = FindH264Nal( p, 4 );
	if ( 0 > i || ( i + 5 ) >= len )
		return -1;

	unsigned char *b = ( (unsigned char*)p ) + i + 3;

	// Verify VOP id
	if ( 0xb6 == *b )
	{	b++;
		return ( *b & 0xc0 ) >> 6;
	} // end if

	return *b;
}
