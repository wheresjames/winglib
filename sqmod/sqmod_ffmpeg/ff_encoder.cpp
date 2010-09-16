// ff_encoder.cpp

#include "stdafx.h"

CFfEncoder::CFfEncoder()
{_STT();

	m_nFmt = 0;
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
		m_pCodecContext = oexNULL;
	} // end if

	m_nFmt = 0;
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

	if ( 0 >= width || 0 >= height )
		return 0;

	m_pCodec = avcodec_find_encoder( (CodecID)x_nCodec );
	if ( !m_pCodec )
	{	oexERROR( 0, oexMks( oexT( "avcodec_find_encoder() failed to find codec for id : " ), (int)x_nCodec ) );
		return 0;
	} // end if

	m_pCodecContext = avcodec_alloc_context();
	if ( !m_pCodecContext )
	{	oexERROR( 0, oexT( "avcodec_alloc_context() failed" ) );
		Destroy();
		return 0;
	} // end if

	avcodec_get_context_defaults( m_pCodecContext );

	m_pCodecContext->codec_id = (CodecID)x_nCodec;
	m_pCodecContext->codec_type = CODEC_TYPE_VIDEO;
	m_pCodecContext->bit_rate = brate;
	m_pCodecContext->bit_rate_tolerance = brate / 5;
	m_pCodecContext->width = width;
	m_pCodecContext->height = height;
	m_pCodecContext->gop_size = 12;
	m_pCodecContext->time_base.den = fps;
	m_pCodecContext->time_base.num = 1;
	m_pCodecContext->me_method = 1;
	m_pCodecContext->strict_std_compliance = ( ( m && m->isset( oexT( "cmp" ) ) ) ? (*m)[ oexT( "cmp" ) ].toint() : 0 );
	m_pCodecContext->pix_fmt = (PixelFormat)fmt;

	// Special h264 defaults
	if ( CODEC_ID_H264 == m_pCodecContext->codec_id )
	{
/*		m_pCodecContext->me_range = 16;
		m_pCodecContext->max_b_frames = 1;
		m_pCodecContext->max_qdiff = 4;
		m_pCodecContext->qmin = 10;
		m_pCodecContext->qmax = 51;
		m_pCodecContext->qcompress = 0.6f;
		m_pCodecContext->me_method = 2;
		m_pCodecContext->me_range = 16;
*/

		// For x264 / Don't ask me, i haven't a clue

		m_pCodecContext->refs = 3; // i_frame_reference
		m_pCodecContext->me_range = 16; // i_me_range
		m_pCodecContext->qmin = 10; // i_qp_min
		m_pCodecContext->qmax = 51; // i_qp_max
//		m_pCodecContext->gop_size = 250; // i_keyint_max
		m_pCodecContext->keyint_min = 6; // i_keyint_min
		m_pCodecContext->max_qdiff = 4; // i_qp_step
		m_pCodecContext->level = -1; // if(avctx->level > 0) x4->params.i_level_idc = avctx->level;
		m_pCodecContext->scenechange_threshold = 40; // i_scenecut_threshold
		m_pCodecContext->b_frame_strategy = 1; // i_bframe_adaptive
		m_pCodecContext->bframebias = 0; // i_bframe_bias
//		m_pCodecContext->flags2 = m_pCodecContext->flags2 & ~CODEC_FLAG2_BPYRAMID; // clear param->b_bframe_pyramid
//		m_pCodecContext->flags |= CODEC_FLAG_LOOP_FILTER; // set param->b_deblocking_filter
		m_pCodecContext->deblockalpha = 0; // i_deblocking_filter_alphac0
		m_pCodecContext->deblockbeta = 0; // i_deblocking_filter_beta
		m_pCodecContext->coder_type = FF_CODER_TYPE_AC; // b_cabac
		m_pCodecContext->crf = 23.0; // f_rf_constant
		m_pCodecContext->i_quant_factor = (float)(-1.0/1.4); // x4->params.rc.f_ip_factor
		m_pCodecContext->b_quant_factor = (float)1.3; // x4->params.rc.f_pb_factor
//		m_pCodecContext->flags = m_pCodecContext->flags & ~CODEC_FLAG_PASS1; // clear param->rc.b_stat_write
//		m_pCodecContext->flags = m_pCodecContext->flags & ~CODEC_FLAG_PASS2; // clear param->rc.b_stat_read
		m_pCodecContext->max_b_frames = 0;
		m_pCodecContext->partitions = X264_PART_I4X4 | X264_PART_I8X8 | X264_PART_P8X8 | X264_PART_B8X8;
//		m_pCodecContext->flags2 = m_pCodecContext->flags2 & ~CODEC_FLAG2_8X8DCT; // set b_transform_8x8 true
		m_pCodecContext->me_subpel_quality = 7; // i_subpel_refine
//		m_pCodecContext->flags2 = m_pCodecContext->flags2 | CODEC_FLAG2_MIXED_REFS; // set b_mixed_references
//		m_pCodecContext->trellis = 0; // i_trellis
//		m_pCodecContext->flags2 = m_pCodecContext->flags2 | CODEC_FLAG2_FASTPSKIP; // set b_fast_pskip
		m_pCodecContext->chromaoffset = 0; // i_chroma_qp_offset
		m_pCodecContext->me_cmp |= FF_CMP_CHROMA; // set b_chroma_me
//		m_pCodecContext->directpred = X264_DIRECT_PRED_SPATIAL;
		m_pCodecContext->me_method = ME_HEX; // set x4->params.analyse.i_me_method = X264_ME_HEX;
//		m_pCodecContext->flags = m_pCodecContext->flags & ~CODEC_FLAG_PASS1; // clear param->rc.b_stat_write
//		m_pCodecContext->flags2 = m_pCodecContext->flags2 | CODEC_FLAG2_WPRED; // set b_weighted_bipred
//		m_pCodecContext->flags2 = m_pCodecContext->flags2 | CODEC_FLAG2_8X8DCT; // set b_transform_8x8
		m_pCodecContext->trellis = 1; // set i_trellis
//		m_pCodecContext->flags = m_pCodecContext->flags & ~CODEC_FLAG_PSNR; // clear b_psnr
		m_pCodecContext->qcompress = 0.6f;
//		m_pCodecContext->flags2 = m_pCodecContext->flags2 & ~CODEC_FLAG2_AUD; // clear b_aud
		//m_pCodecContext->flags = m_pCodecContext->flags & ~CODEC_FLAG_GLOBAL_HEADER; // b_repeat_headers=1
//		m_pCodecContext->flags = m_pCodecContext->flags | CODEC_FLAG_GLOBAL_HEADER; // b_repeat_headers=0

	} // end if

#define M_DEF( k, f ) if ( m->isset( oexT( #k ) ) ) m_pCodecContext->k = (*m)[ oexT( #k ) ].f()

	if ( m && m->size() )
	{
		if ( m->isset( oexT( "codec_type" ) ) ) 
			m_pCodecContext->codec_type = (AVMediaType)(*m)[ oexT( "codec_type" ) ].toint();

		M_DEF( bit_rate_tolerance,		toint );
		M_DEF( gop_size,				toint );
		M_DEF( flags,					toint );
		M_DEF( flags2,					toint );
		M_DEF( refs,					toint );
		M_DEF( me_range,				toint );
		M_DEF( qmin,					toint );
		M_DEF( qmax,					toint );
		M_DEF( keyint_min,				toint );
		M_DEF( max_qdiff,				toint );
		M_DEF( level,					toint );
		M_DEF( scenechange_threshold,	toint );
		M_DEF( b_frame_strategy,		toint );
		M_DEF( bframebias,				toint );
		M_DEF( deblockalpha,			toint );
		M_DEF( deblockbeta,				toint );
		M_DEF( coder_type,				toint );
		M_DEF( max_b_frames,			toint );
		M_DEF( partitions,				toint );
		M_DEF( me_subpel_quality,		toint );
		M_DEF( trellis,					toint );
		M_DEF( chromaoffset,			toint );
		M_DEF( me_cmp,					toint );
		M_DEF( me_method,				toint );

		M_DEF( crf,						tofloat );
		M_DEF( i_quant_factor,			tofloat );
		M_DEF( b_quant_factor,			tofloat );
		M_DEF( qcompress,				tofloat );

	} // end if

	if ( m && m->isset( oexT( "quality" ) ) )
	{
		int q = (*m)[ oexT( "quality" ) ].toint();
		if ( !q ) 
			q = 5;

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
		m_pCodecContext = oexNULL;
		Destroy();
		return 0;
	} // end if

	m_nFmt = fmt;

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
	if ( (int)out->Size() < ( nSize << 1 ) && !out->Allocate( ( nSize << 1 ) ) )
		return 0;

	AVFrame *paf = avcodec_alloc_frame();
	if ( !paf )
		return 0;

	if ( !CFfConvert::FillAVFrame( paf, fmt, width, height, (void*)in ) )
		return 0;

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

	} // end if

	int nBytes = avcodec_encode_video( m_pCodecContext, (uint8_t*)out->Ptr(), out->Size(), paf );
	if ( !nBytes ) nBytes = avcodec_encode_video( m_pCodecContext, (uint8_t*)out->Ptr(), out->Size(), paf );
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
										  ? ( flags | PKT_FLAG_KEY )
										  : ( flags & ~PKT_FLAG_KEY ) ) ) );
		(*m)[ oexT( "dts" ) ].set( sqbind::oex2std( oexMks( m_pCodecContext->coded_frame->pts ) ) );

	} // end if

	av_free( paf );

	out->setUsed( nBytes );

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

