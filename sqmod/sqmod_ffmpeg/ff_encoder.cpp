// ff_encoder.cpp

#include "stdafx.h"

SQBIND_REGISTER_CLASS_BEGIN( CFfEncoder, CFfEncoder )

	SQBIND_MEMBER_FUNCTION( CFfEncoder, Destroy )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, Create )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, isValid )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, Encode )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, EncodeImage )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, getWidth )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, getHeight )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, getFps )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, getBitRate )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, getPktDts )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, getPktPts )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, getPts )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, setPts )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, getExtraData )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, setHeader )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, getHeader )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, calcPts )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, getFrame )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, setFrame )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, setTimeBase )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, getTimeBase )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, getLevel )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CFfEncoder );

void CFfEncoder::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CFfEncoder );
}

CFfEncoder::CFfEncoder()
{_STT();

	m_nFmt = 0;
	m_nFps = 0;
	m_nFrame = 0;
	m_nTimeBase = 0;
	m_pCodec = oexNULL;
	m_pCodecContext = oexNULL;
	m_pStream = oexNULL;
	m_pOutput = oexNULL;
	m_pFormatContext = oexNULL;

	// Register codecs
//	av_register_all();
}

void CFfEncoder::Destroy()
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
		av_free( m_pCodecContext );
		m_pCodecContext = oexNULL;
	} // end if

	m_nFmt = 0;
	m_nFrame = 0;
	m_pCodec = oexNULL;
	m_pOutput = oexNULL;
	m_pCodecContext = oexNULL;

	m_tmp.Free();
	m_tmp2.Free();
}

// http://lists.mplayerhq.hu/pipermail/libav-user/2009-June/003257.html
int CFfEncoder::Create( int x_nCodec, int fmt, int width, int height, int fps, int brate, sqbind::CSqMulti *m )
{_STT();

	oexAutoLock ll( _g_ffmpeg_lock );
	if ( !ll.IsLocked() ) return 0;

	// Lose previous codec
	Destroy();

	// Sanity checks
	if ( 0 >= width || 0 >= height || 0 >= fps )
		return 0;

	// Find an encoder
	m_pCodec = avcodec_find_encoder( (CodecID)x_nCodec );
	if ( !m_pCodec )
	{	// oexERROR( 0, oexMks( oexT( "avcodec_find_encoder() failed to find codec for id : " ), (int)x_nCodec ) );
		return 0;
	} // end if

	// Initialize codec context
	m_pCodecContext = avcodec_alloc_context();
	if ( !m_pCodecContext )
	{	oexERROR( 0, oexT( "avcodec_alloc_context() failed" ) );
		Destroy();
		return
		0;
	} // end if

	// Can't have a bit rate of zero
	if ( 0 >= brate )
		brate = width * height * fps / 3;

	avcodec_get_context_defaults( m_pCodecContext );

	// Get time base
	oex::oexINT64 nTimeBase = m_nTimeBase;
	if ( 0 >= nTimeBase )
		nTimeBase = fps;

	m_pCodecContext->codec_id = (CodecID)x_nCodec;
	m_pCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
	m_pCodecContext->bit_rate = brate;
	m_pCodecContext->bit_rate_tolerance = brate / 5;
	m_pCodecContext->width = width;
	m_pCodecContext->height = height;
	m_pCodecContext->gop_size = fps;
	m_pCodecContext->time_base.den = nTimeBase;
	m_pCodecContext->time_base.num = 1;
//	m_pCodecContext->me_method = 1;
	m_pCodecContext->strict_std_compliance = ( ( m && m->isset( oexT( "cmp" ) ) ) ? (*m)[ oexT( "cmp" ) ].toint() : 0 );
	m_pCodecContext->pix_fmt = (PixelFormat)fmt;

	// Global headers?
	if ( m && (*m)[ "global_headers" ].toint() )
		m_pCodecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;

	// Special h264 defaults
	if ( CODEC_ID_H264 == m_pCodecContext->codec_id )
	{
		// 'Real time'
//		m_pCodecContext->max_b_frames = 0;
//		m_pCodecContext->thread_count = 0;
//		m_pCodecContext->b_frame_strategy = 1;

		// Set approximate difficulti level
		if ( 320 >= width )
			m_pCodecContext->level = 20;
		else if ( 720 >= width )
			m_pCodecContext->level = 30;
		else if ( 1280 >= width )
			m_pCodecContext->level = 32;
		else if ( 2048 >= width )
			m_pCodecContext->level = 40;
		else
			m_pCodecContext->level = 50;

//		m_pCodecContext->profile = FF_PROFILE_H264_BASELINE;

		// Set H264 codec params
		av_opt_set( m_pCodecContext->priv_data, "profile",
					( m && (*m)[ "profile" ].len() ) ? (*m)[ "profile" ].c_str() : "baseline", AV_OPT_SEARCH_CHILDREN );
		av_opt_set( m_pCodecContext->priv_data, "preset",
					( m && (*m)[ "preset" ].len() ) ? (*m)[ "preset" ].c_str() : "ultrafast", AV_OPT_SEARCH_CHILDREN );
		av_opt_set( m_pCodecContext->priv_data, "tune",
					( m && (*m)[ "tune" ].len() ) ? (*m)[ "tune" ].c_str() : "zerolatency", AV_OPT_SEARCH_CHILDREN );
//		av_opt_set( m_pCodecContext->priv_data, "x264opts",
//					( m && (*m)[ "x264opts" ].len() ) ? (*m)[ "x264opts" ].c_str() : "no-mbtree:sliced-threads:sync-lookahead=0", 0 );

		if ( m )
			if ( (*m)[ "level" ].toint() )
				m_pCodecContext->level = (*m)[ "level" ].toint();
			else
				(*m)[ "level" ].set( sqbind::oex2std( oexMks( m_pCodecContext->level ) ) );

	} // end if

	if ( m && m->isset( oexT( "quality" ) ) )
	{
		float q = (*m)[ oexT( "quality" ) ].tofloat();
		if ( .1 > q )
			q = 5;
		else if ( 30 < q )
			q = 20;

		// Can't have quality and bitrate
		// m_pCodecContext->bit_rate = 0;
		m_pCodecContext->bit_rate_tolerance = 0;

		m_pCodecContext->qmin = m_pCodecContext->qmax = q;

		m_pCodecContext->mb_lmin = m_pCodecContext->lmin =
			m_pCodecContext->qmin * FF_QP2LAMBDA;

		m_pCodecContext->mb_lmax = m_pCodecContext->lmax =
			m_pCodecContext->qmax * FF_QP2LAMBDA;

		m_pCodecContext->flags |= CODEC_FLAG_QSCALE;
		m_pCodecContext->global_quality = m_pCodecContext->qmin * FF_QP2LAMBDA;

	} // end if

	int res = avcodec_open( m_pCodecContext, m_pCodec );
	if ( 0 > res )
	{	oexERROR( res, oexT( "avcodec_open() failed" ) );
		av_free( m_pCodecContext );
		m_pCodecContext = oexNULL;
		Destroy();
		return 0;
	} // end if

	// Start with invalid pts
//	if ( m_pCodecContext->coded_frame )
//		m_pCodecContext->coded_frame->pts = -1;

	m_nFmt = fmt;
	m_nFps = fps;

	return 1;
}

int CFfEncoder::EncodeRaw( int fmt, int width, int height, const void *in, int sz_in, sqbind::CSqBinary *out, sqbind::CSqMulti *m )
{_STT();

	// Ensure codec
	if ( !m_pCodecContext )
		return 0;

	// Validate parmeters
	if ( !in || !sz_in || !out || !width || !height )
		return 0;

	// Check for inverted image
	int flip = 0;
	if ( 0 > height )
	{	flip = 1; height = -height; }

	// Validate buffer size
	int nSize = CFfConvert::CalcImageSize( fmt, width, height );
	if ( nSize > sz_in )
		return 0;

	// How much room could we possibly need
	int nHeader = m_header.getUsed();
	if ( (int)out->Size() < nHeader + ( nSize << 1 ) && !out->Allocate( nHeader + ( nSize << 1 ) ) )
		return 0;

	AVFrame *paf = avcodec_alloc_frame();
	if ( !paf )
		return 0;

	if ( !CFfConvert::FillAVFrame( paf, fmt, width, height, (void*)in ) )
		return 0;

	// Assume key frame
	paf->key_frame = 1;

	if ( m )
	{
		if ( m->isset( oexT( "flags" ) ) )
		{	if ( (*m)[ oexT( "flags" ) ].toint() & 0x01 )
				paf->key_frame = 1;
			else
				paf->key_frame = 0;
		} // end if

		if ( m->isset( oexT( "quality" ) ) && ( (*m)[ oexT( "quality" ) ].toint() ) )
			paf->quality = (*m)[ oexT( "quality" ) ].toint();
		else
			paf->quality = m_pCodecContext->global_quality;

		// Set the PTS
		if ( !m_nFrame )
			paf->pts = 0;
		else if ( m->isset( oexT( "pts" ) ) )
			paf->pts = (*m)[ oexT( "pts" ) ].toint();
		else
			paf->pts = calcPts();

	} // end if

	else
		paf->pts = m_nFrame ? calcPts() : 0,
		paf->quality = m_pCodecContext->global_quality;

	// Ensure PTS match
	m_pCodecContext->coded_frame->pts = paf->pts;

	// Copy the header
	if ( 0 < nHeader )
		out->Copy( &m_header );

	// Encode the video frame
	int nBytes = avcodec_encode_video( m_pCodecContext, (uint8_t*)out->_Ptr( nHeader ), out->Size() - nHeader, paf );
	if ( 0 > nBytes )
	{	oexERROR( nBytes, oexT( "avcodec_encode_video() failed" ) );
		out->setUsed( 0 );
		av_free( paf );
		return 0;
	} // end if

	if ( m )
	{
		// Save key frame information
		int flags = (*m)[ oexT( "flags" ) ].toint();
		(*m)[ oexT( "flags" ) ]
			.set( sqbind::oex2std( oexMks( ( m_pCodecContext->coded_frame->key_frame )
										  ? ( flags | AV_PKT_FLAG_KEY )
										  : ( flags & ~AV_PKT_FLAG_KEY ) ) ) );
										  
		if ( 0 < m_pCodecContext->coded_frame->pts )
			(*m)[ oexT( "pts" ) ].set( sqbind::oex2std( oexMks( m_pCodecContext->coded_frame->pts ) ) );
		else
			(*m)[ oexT( "pts" ) ].set( sqbind::oex2std( oexMks( paf->pts ) ) );
//		(*m)[ oexT( "dts" ) ].set( sqbind::oex2std( oexMks( m_pCodecContext->coded_frame->pkt_dts ) ) );
//		if ( m_pCodecContext->pkt )
//			(*m)[ oexT( "pts" ) ].set( sqbind::oex2std( oexMks( m_pCodecContext->pkt->pts ) ) ),
//			(*m)[ oexT( "dts" ) ].set( sqbind::oex2std( oexMks( m_pCodecContext->pkt->dts ) ) );

	} // end if

	av_free( paf );

	out->setUsed( nBytes );

	// Count a frame
	m_nFrame++;

	return nBytes;
}

int CFfEncoder::Encode( int fmt, int width, int height, sqbind::CSqBinary *in, sqbind::CSqBinary *out, sqbind::CSqMulti *m )
{_STT();

	// Sanity checks
	if ( !in || !in->getUsed() )
		return 0;

	int flip = 0;
	if ( 0 > height )
		flip = 1;

	// Do we need to convert the colorspace?
	if ( !flip && fmt == m_nFmt )
		return EncodeRaw( fmt, width, height, in->Ptr(), in->getUsed(), out, m );

	// Must convert to input format
	if ( !CFfConvert::ConvertColorBB( width, height, in, fmt, &m_tmp, m_nFmt, SWS_FAST_BILINEAR ) )
		return 0;

	// Do the conversion
	return EncodeRaw( m_nFmt, width, height, m_tmp.Ptr(), m_tmp.getUsed(), out, m );
}

int CFfEncoder::EncodeImage( sqbind::CSqImage *img, sqbind::CSqBinary *out, sqbind::CSqMulti *m )
{_STT();
	// Ensure codec
	if ( !m_pCodecContext )
		return 0;

	// Validate params
	if ( !img || !out )
		return 0;

	// Do we need to convert the colorspace?
	if ( PIX_FMT_BGR24 == m_nFmt )
		return EncodeRaw( PIX_FMT_BGR24, img->getWidth(), img->getHeight(), img->Obj().GetBits(), img->Obj().GetImageSize(), out, m );

	// Must convert to input format
	if ( !CFfConvert::ConvertColorIB( img, &m_tmp, m_nFmt, SWS_FAST_BILINEAR, 1 ) )
		return 0;

	// Do the conversion
	return EncodeRaw( m_nFmt, img->getWidth(), img->getHeight(), m_tmp.Ptr(), m_tmp.getUsed(), out, m );
}

