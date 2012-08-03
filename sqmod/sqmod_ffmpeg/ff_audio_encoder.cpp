// ff_encoder.cpp

#include "stdafx.h"

SQBIND_REGISTER_CLASS_BEGIN( CFfAudioEncoder, CFfAudioEncoder )

	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, Destroy )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, Create )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, isValid )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, Encode )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, getChannels )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, getSampleRate )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, getBps )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, getBitRate )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, getPktDts )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, getPktPts )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, getPts )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, getFrameSize )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, BufferData )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, UnbufferData )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, getCodecId )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, getFmtCnv )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, setFmtCnv )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, getTimeBase )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, setTimeBase )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, getFrame )
	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder, setFrame )
//	SQBIND_MEMBER_FUNCTION( CFfAudioEncoder,  )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CFfAudioEncoder );

void CFfAudioEncoder::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CFfAudioEncoder );
}

CFfAudioEncoder::CFfAudioEncoder()
{_STT();

	m_nFmt = 0;
	m_nFrame = 0;
	m_nTimeBase = 0;
	m_nCodecId = 0;
	m_pCodec = oexNULL;
	m_pCodecContext = oexNULL;
	m_pStream = oexNULL;
	m_pOutput = oexNULL;
	m_pFormatContext = oexNULL;
}

void CFfAudioEncoder::Destroy()
{_STT();

	oexAutoLock ll( _g_ffmpeg_lock );
	if ( !ll.IsLocked() ) return;

	if ( m_pStream )
	{	av_free( m_pStream );
		m_pStream = oexNULL;
	} // end if

	if ( m_pFormatContext )
	{	av_free( m_pFormatContext );
		m_pFormatContext = oexNULL;
	} // end if

	if ( m_pCodecContext )
	{	avcodec_close( m_pCodecContext );
		m_pCodecContext = oexNULL;
	} // end if

	m_nFmt = 0;
	m_nFrame = 0;
	m_nCodecId = 0;
	m_pCodec = oexNULL;
	m_pOutput = oexNULL;
	m_pCodecContext = oexNULL;
}

int CFfAudioEncoder::Create( int x_nCodec, int x_nFmt, int x_nChannels, int x_nSampleRate, int x_nBps, sqbind::CSqMulti *m )
{_STT();

	oexAutoLock ll( _g_ffmpeg_lock );
	if ( !ll.IsLocked() ) return 0;

	// Lose previous codec
	Destroy();

	// Check params
	if ( 0 >= x_nChannels || 0 >= x_nSampleRate )
		return 0;

	m_pCodec = avcodec_find_encoder( (CodecID)x_nCodec );
	if ( !m_pCodec )
	{	// oexERROR( 0, oexMks( oexT( "avcodec_find_encoder() failed to find codec for id : " ), (int)x_nCodec ) );
		return 0;
	} // end if

	m_pCodecContext = avcodec_alloc_context();
	if ( !m_pCodecContext )
	{	oexERROR( 0, oexT( "avcodec_alloc_context() failed" ) );
		Destroy();
		return 0;
	} // end if

	avcodec_get_context_defaults( m_pCodecContext );

//	MAX_AUDIO_PACKET_SIZE
//	AVCODEC_MAX_AUDIO_FRAME_SIZE

	m_pCodecContext->codec_id = (CodecID)x_nCodec;
	m_pCodecContext->codec_type = AVMEDIA_TYPE_AUDIO;

#	define CNVTYPE( t, v ) case oex::obj::t : m_pCodecContext->sample_fmt = v; break;
	switch( x_nFmt )
	{	CNVTYPE( tUInt8, AV_SAMPLE_FMT_U8 );
		CNVTYPE( tInt16, AV_SAMPLE_FMT_S16 );
		CNVTYPE( tInt32, AV_SAMPLE_FMT_S32 );
		CNVTYPE( tFloat, AV_SAMPLE_FMT_FLT );
		CNVTYPE( tDouble, AV_SAMPLE_FMT_DBL );
		default :
			m_pCodecContext->sample_fmt = AV_SAMPLE_FMT_NONE; break;
			oexERROR( 0, oexMks( oexT( "Invalid sample format : " ), x_nFmt ) );
			Destroy();
			return 0;
	} // end switch

	// Get time base
	oex::oexINT64 nTimeBase = m_nTimeBase;
	if ( 0 >= nTimeBase )
		nTimeBase = x_nSampleRate;

	// Set codec parameters
    m_pCodecContext->channels = x_nChannels;
	m_pCodecContext->sample_rate = x_nSampleRate;
	m_pCodecContext->bits_per_coded_sample = ( x_nFmt & 0xf ) * x_nChannels * 8;
    m_pCodecContext->block_align = 0;
    m_pCodecContext->bit_rate = m_pCodecContext->sample_rate * ( x_nFmt & 0xf ) * m_pCodecContext->channels * 8;
	m_pCodecContext->channel_layout = ( 1 == x_nChannels ) ? AV_CH_LAYOUT_MONO : AV_CH_LAYOUT_STEREO;
	m_pCodecContext->time_base.num = 1;
	m_pCodecContext->time_base.den = nTimeBase;
	m_pCodecContext->strict_std_compliance = ( ( m && m->isset( oexT( "cmp" ) ) ) ? (*m)[ oexT( "cmp" ) ].toint() : 0 );

	// Check profile for aac
	if ( CODEC_ID_AAC == x_nCodec )
//		m_pCodecContext->profile = FF_PROFILE_AAC_MAIN;
		m_pCodecContext->profile = FF_PROFILE_AAC_LOW,
		m_pCodecContext->bit_rate = m_pCodecContext->sample_rate * ( x_nFmt & 0xf ) * m_pCodecContext->channels;

	if ( m && m->isset( oexT( "quality" ) ) )
	{
		float q = (*m)[ oexT( "quality" ) ].tofloat();
		if ( .1 > q )
			q = 5;
		else if ( 30 < q )
			q = 20;

		m_pCodecContext->bit_rate_tolerance = 0;

		m_pCodecContext->qmin = m_pCodecContext->qmax = q;

		m_pCodecContext->mb_lmin = m_pCodecContext->lmin =
			m_pCodecContext->qmin * FF_QP2LAMBDA;

		m_pCodecContext->mb_lmax = m_pCodecContext->lmax =
			m_pCodecContext->qmax * FF_QP2LAMBDA;

		m_pCodecContext->flags |= CODEC_FLAG_QSCALE;
		m_pCodecContext->global_quality = m_pCodecContext->qmin * FF_QP2LAMBDA;

	} // end if

	// Open the codec
	int res = avcodec_open( m_pCodecContext, m_pCodec );
	if ( 0 > res )
	{	oexERROR( res, oexT( "avcodec_open() failed" ) );
		m_pCodecContext = oexNULL;
		Destroy();
		return 0;
	} // end if

	// Save info
	m_nFmt = x_nFmt;
	m_nCodecId = x_nCodec;

	return 1;
}

int CFfAudioEncoder::BufferData( sqbind::CSqBinary *in )
{_STT();

	// Are we adding data?
	if ( in && in->getUsed() )
	{
		// Ensure buffer size
		if ( ( m_buf.Size() - m_buf.getUsed() ) < (sqbind::CSqBinary::t_size)( 4 * in->getUsed() + FF_INPUT_BUFFER_PADDING_SIZE ) )
			m_buf.Resize( 4 * ( m_buf.Size() + in->getUsed() + FF_INPUT_BUFFER_PADDING_SIZE ) );

		// Add new data to buffer
		if ( 0 >= m_nCnv || m_nFmt == m_nCnv )
			m_buf.Append( in );

		// Data needs conversion
		else
		{	double dOffset = 0, dScale = 1;
			if ( oex::obj::tFloat == m_nFmt )
			{	if ( m_nCnv == oex::obj::tInt8 )
					dScale = (double)1 / (double)0x7f;
				else if ( m_nCnv == oex::obj::tInt16 )
					dScale = (double)1 / (double)0x7fff;
				else if ( m_nCnv == oex::obj::tInt32 )
					dScale = (double)1 / (double)0x7fffffff;
			} // end if

			// Convert data types
			long written = oex::obj::Convert( m_buf._Ptr( m_buf.getUsed() ), m_buf.Size() - m_buf.getUsed(), m_nFmt,
											in->Ptr(), in->getUsed(), m_nCnv, dOffset, dScale );

			// Number of new bytes
			if ( 0 < written )
				m_buf.setUsed( m_buf.getUsed() + written );

		} // end else

	} // end if

	// Get buffer pointers
	m_pkt.data = (uint8_t*)m_buf._Ptr();
	m_pkt.size = m_buf.getUsed();

	// Zero padding
	int nPadding = m_buf.Size() - m_buf.getUsed();
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

int CFfAudioEncoder::UnbufferData( int uUsed )
{
	if ( 0 > uUsed )
		m_buf.setUsed( 0 );
	else if ( 0 < uUsed )
		m_buf.LShift( uUsed );

	// Ensure packet pointers are valid
	m_pkt.data = (uint8_t*)m_buf._Ptr();
	m_pkt.size = m_buf.getUsed();

	return m_pkt.size;
}

int CFfAudioEncoder::Encode( sqbind::CSqBinary *in, sqbind::CSqBinary *out, sqbind::CSqMulti *m )
{_STT();

	// Sanity check
	if ( !m_pCodecContext || !out )
		return 0;

	// We need at least one frame of data
	unsigned int nIn = BufferData( in );
	unsigned int fs = getFrameSize();
	unsigned int bs = fs * ( getBps() / 8 );
	if ( 0 >= nIn || bs > nIn )
		return 0;

	// Allocate space for output data
	unsigned int nOutPtr = out->getUsed();
	unsigned int nOut = oex::cmn::NextPower2( nOutPtr + FF_MIN_BUFFER_SIZE + nIn );
	if ( out->Size() < nOut && !out->Allocate( nOut ) )
		return 0;

	// Output buffer pointer
	uint8_t *pOut = (uint8_t*)out->_Ptr();

	// While we have input data
	//while ( nIn >= bs )
	if ( nIn >= bs )
	{
		// Ensure a reasonable output buffer
		while ( ( nOut - nOutPtr ) < ( bs + FF_MIN_BUFFER_SIZE ) )
			nOut <<= 1, out->Resize( nOut );

		// Check for memory issue
		if ( out->Size() < nOut || nOut <= nOutPtr || ( nOut - nOutPtr ) < ( bs + FF_MIN_BUFFER_SIZE ) )
		{	oexERROR( nOut, oexT( "Memory allocation failed" ) );
			return 0;
		} // end if

#if defined( DEVEL_USE_AVCODEC_ENCODE_AUDIO2 )

// ############### 2

		AVFrame *pDst = avcodec_alloc_frame();
		if ( !pDst )
		{	oexERROR( nOut, oexT( "avcodec_alloc_frame() failed" ) );
			return 0;
		} // end if

		// Get frame defaults
		avcodec_get_frame_defaults( pDst );

		pDst->pts = calcPts();
		pDst->nb_samples = fs;
		pDst->format = m_pCodecContext->sample_fmt;
//		pDst->channels = m_pCodecContext->channels;
		pDst->sample_rate = m_pCodecContext->sample_rate;
		pDst->channel_layout = m_pCodecContext->channel_layout;
		pDst->type = 0;
		void *data = &pOut[ nOutPtr ];
		pDst->extended_data = (uint8_t**)&data;
		pDst->linesize[ 0 ] = bs;

//		if ( 0 > avcodec_fill_audio_frame( pDst, m_pCodecContext->channels, m_pCodecContext->sample_fmt,
//										   &pOut[ nOutPtr ], nOut - nOutPtr, 1 ) )
//		{	oexERROR( nOut, oexT( "avcodec_fill_audio_frame() failed" ) );
//			return 0;
//		} // end if

		// Encode a frame
		int gop = 0;
		int nBytes = avcodec_encode_audio2( m_pCodecContext, &m_pkt, pDst, &gop );
		if ( 0 > nBytes )
		{	oexERROR( nBytes, oexT( "avcodec_encode_audio2() failed" ) );
			return 0;
		} // end if

		av_free( pDst );

// ############### 2
#else

		int nBytes = avcodec_encode_audio( m_pCodecContext, &pOut[ nOutPtr ], nOut - nOutPtr, (const short*)m_pkt.data );
		if ( 0 > nBytes )
		{	oexERROR( nBytes, oexT( "avcodec_encode_audio() failed" ) );
			return 0;
		} // end if

#endif

		// Add bytes
		if ( 0 < nBytes )
			nOutPtr += nBytes;

		// Unbuffer used data
		nIn = UnbufferData( bs );

	} // end while

	// Set total bytes in the output queue
	out->setUsed( nOutPtr );

	if ( m )
	{
		// Save key frame information
		int flags = (*m)[ oexT( "flags" ) ].toint();
		(*m)[ oexT( "flags" ) ]
			.set( sqbind::oex2std( oexMks( ( m_pCodecContext->coded_frame->key_frame )
										  ? ( flags | AV_PKT_FLAG_KEY )
										  : ( flags & ~AV_PKT_FLAG_KEY ) ) ) );
		(*m)[ oexT( "pts" ) ].set( sqbind::oex2std( oexMks( m_pCodecContext->coded_frame->pkt_pts ) ) );
		(*m)[ oexT( "dts" ) ].set( sqbind::oex2std( oexMks( m_pCodecContext->coded_frame->pkt_dts ) ) );

	} // end if

	// Count a frame
	m_nFrame++;

	return nOutPtr;
}


