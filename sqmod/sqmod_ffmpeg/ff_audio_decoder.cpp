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

int CFfAudioDecoder::Create( int x_nCodec, int x_nChannels, int x_nSampleRate, int x_nBps )
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
	avcodec_get_context_defaults2( m_pCodecContext, AVMEDIA_TYPE_AUDIO );

    m_pCodecContext->codec_id = (CodecID)x_nCodec;
    m_pCodecContext->codec_type = AVMEDIA_TYPE_AUDIO;
    
    m_pCodecContext->channels = x_nChannels;
	m_pCodecContext->sample_rate = x_nSampleRate;
    m_pCodecContext->bits_per_coded_sample = x_nBps;
    m_pCodecContext->bit_rate = m_pCodecContext->sample_rate * m_pCodecContext->channels * 8;
    m_pCodecContext->block_align = 0;

//    m_pCodecContext->strict_std_compliance = ( ( m && m->isset( oexT( "cmp" ) ) ) ? (*m)[ oexT( "cmp" ) ].toint() : 0 );

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

int CFfAudioDecoder::BufferData( sqbind::CSqBinary *in, sqbind::CSqMulti *m )
{
	// Init packet
	oexZero( m_pkt );
//	av_init_packet( &m_pkt );

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
		{	oex::oexUINT uMin = 2 * ( m_tmp.Size() + in->getUsed() + FF_INPUT_BUFFER_PADDING_SIZE );
			if ( 32000 > uMin )
				uMin = 32000;
	        m_tmp.Allocate( uMin );
		} // end if

		// Add new data to buffer
		m_tmp.Append( in );

	} // end if

	// Is there a sync sequence?
	if ( m_sync.getUsed() )
	{
		oexSHOW( m_sync.getUsed() );
	
		const char *s = m_sync.Ptr();
		sqbind::CSqBinary::t_size ls = m_sync.getUsed();

		const char *p = m_tmp.Ptr();
		sqbind::CSqBinary::t_size lp = m_tmp.getUsed();
		
		// Look for the sync
		while ( lp > ls && oexMemCmp( p, s, ls ) )
			p++, lp--;

		// Shift out unsynced data			
		if ( lp < m_tmp.getUsed() )
		{	oexSHOW( lp );
			oexSHOW( m_tmp.getUsed() );
			m_tmp.LShift( m_tmp.getUsed() - lp );
		} // end if
	
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

int CFfAudioDecoder::UnBufferData( int uUsed )
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

int CFfAudioDecoder::getBufferSize()
{	if ( !m_pkt.data )
		return 0;
	return m_pkt.size;
}

int CFfAudioDecoder::Decode( sqbind::CSqBinary *in, sqbind::CSqBinary *out, sqbind::CSqMulti *m )
{_STT();

	// Ensure codec
	if ( !m_pCodecContext )
		return 0;

	// Add data to buffer
	if ( !in || !out || !BufferData( in, m ) )
		return 0;

	int out_size = oex::cmn::Max( (int)(in->getUsed() * 2), (int)AVCODEC_MAX_AUDIO_FRAME_SIZE );
	if ( (int)out->Size() < out_size )
		out->Allocate( out_size );

	// Attempt to decode packet
	int used = avcodec_decode_audio3( m_pCodecContext, 
									  (int16_t*)out->_Ptr(), &out_size,
									  &m_pkt );
									 
	
//	if ( AVERROR_INVALIDDATA == used )
//		return 0;
	
	if ( AVERROR( EAGAIN ) == used )
		return 0;

	if ( 0 > used )
	{	UnBufferData( -1 );
		return 0;
	} // end if

	if ( 0 < used )
		UnBufferData( used );

	if ( 0 >= out_size )
	{	out->setUsed( 0 );
		return 0;
	} // end if

	out->setUsed( out_size );

	// Frame count
	m_lFrames++;

	return out_size;
}

static AVCodecTag g_ff_audio_codec_map[] =
{
    { CODEC_ID_AAC,				MKTAG('A', 'M', 'R', ' ') },
    { CODEC_ID_AMR_NB,			MKTAG('A', 'M', 'R', ' ') },
    { CODEC_ID_AMR_WB,			MKTAG('A', 'M', 'R', ' ') },
    { CODEC_ID_AAC_LATM,		MKTAG('L', 'A', 'T', 'M') },

    { CODEC_ID_MP2,				MKTAG('M', 'P', '2', '_') },
    { CODEC_ID_MP3,				MKTAG('M', 'P', '3', '_') },
    { CODEC_ID_AAC,				MKTAG('A', 'A', 'C', '_') },
    { CODEC_ID_AC3,				MKTAG('A', 'C', '3', '_') },

    
	{ CODEC_ID_NONE,			0 }
};

struct SFfAudioCodecInfo
{
	int			id;
	const char 	*tag;
};

static SFfAudioCodecInfo g_ff_audio_codec_info[] =
{
	{ CODEC_ID_AAC,				oexT( "MP4A-LATM" ) },
	{ CODEC_ID_AAC,				oexT( "MPEG4-GENERIC" ) },

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

	// Extras by name
	oex::CStr8 s = oexStrToStr8( sqbind::std2oex( sCodec ) );
	for ( int i = 0; CODEC_ID_NONE != g_ff_audio_codec_info[ i ].id; i++ )
		if ( !oex::str::ICompare( s.Ptr(), s.Length(), 
								  g_ff_audio_codec_info[ i ].tag, 
								  oex::zstr::Length( g_ff_audio_codec_info[ i ].tag ) ) )
			return g_ff_audio_codec_info[ i ].id;

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

	// Find a codec with that id
	for ( int i = 0; CODEC_ID_NONE != g_ff_audio_codec_map[ i ].id; i++ )
		if ( g_ff_audio_codec_map[ i ].id == (CodecID)nId )
		{	char b[ 5 ] = { 0, 0, 0, 0, 0 };
			*(oex::oexUINT32*)b = g_ff_audio_codec_map[ i ].tag;
			return oexMbToStrPtr( b );
		} // end if
		
	// Find a codec with that id
	for ( int i = 0; CODEC_ID_NONE != g_ff_audio_codec_info[ i ].id; i++ )
		if ( g_ff_audio_codec_info[ i ].id == (CodecID)nId )
			return oexMbToStrPtr( g_ff_audio_codec_info[ i ].tag );
		
	return oexT( "" );
}

