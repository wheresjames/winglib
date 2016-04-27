// ff_convert.cpp

#include "stdafx.h"

SQBIND_REGISTER_CLASS_BEGIN( CFfConvert, CFfConvert )

	SQBIND_MEMBER_FUNCTION( CFfConvert, ConvertBB )
	SQBIND_MEMBER_FUNCTION( CFfConvert, ConvertIB )
	SQBIND_MEMBER_FUNCTION( CFfConvert, ConvertBI )
	SQBIND_MEMBER_FUNCTION( CFfConvert, ConvertII )

	SQBIND_MEMBER_FUNCTION( CFfConvert, getSrcWidth )
	SQBIND_MEMBER_FUNCTION( CFfConvert, getSrcHeight )
	SQBIND_MEMBER_FUNCTION( CFfConvert, getSrcSize )
	SQBIND_MEMBER_FUNCTION( CFfConvert, getSrcFmt )
	SQBIND_MEMBER_FUNCTION( CFfConvert, getDstWidth )
	SQBIND_MEMBER_FUNCTION( CFfConvert, getDstHeight )
	SQBIND_MEMBER_FUNCTION( CFfConvert, getDstSize )
	SQBIND_MEMBER_FUNCTION( CFfConvert, getDstFmt )

	SQBIND_STATIC_FUNCTION( CFfConvert, Rotate )
	SQBIND_STATIC_FUNCTION( CFfConvert, FlipVert )

	// Format types
	SQBIND_GLOBALCONST( AV_PIX_FMT_RGB24 )
	SQBIND_GLOBALCONST( AV_PIX_FMT_BGR24 )
	SQBIND_GLOBALCONST( AV_PIX_FMT_RGB32 )
	SQBIND_GLOBALCONST( AV_PIX_FMT_BGR32 )

	SQBIND_GLOBALCONST( AV_PIX_FMT_MONOWHITE )
	SQBIND_GLOBALCONST( AV_PIX_FMT_MONOBLACK )
	SQBIND_GLOBALCONST( AV_PIX_FMT_GRAY8 )
	SQBIND_GLOBALCONST( AV_PIX_FMT_GRAY16LE )
	SQBIND_GLOBALCONST( AV_PIX_FMT_GRAY16BE )

	SQBIND_GLOBALCONST( AV_PIX_FMT_YUV410P )
	SQBIND_GLOBALCONST( AV_PIX_FMT_YUV411P )
	SQBIND_GLOBALCONST( AV_PIX_FMT_YUV420P )
	SQBIND_GLOBALCONST( AV_PIX_FMT_YUV422P )
	SQBIND_GLOBALCONST( AV_PIX_FMT_YUV440P )
	SQBIND_GLOBALCONST( AV_PIX_FMT_YUV444P )

	SQBIND_GLOBALCONST( AV_PIX_FMT_YUYV422 )

	SQBIND_GLOBALCONST( AV_PIX_FMT_YUVJ420P )
	SQBIND_GLOBALCONST( AV_PIX_FMT_YUVJ422P )
	SQBIND_GLOBALCONST( AV_PIX_FMT_YUVJ444P )
	SQBIND_GLOBALCONST( AV_PIX_FMT_YUVJ440P )
	SQBIND_GLOBALCONST( AV_PIX_FMT_YUVA420P )

	SQBIND_GLOBALCONST( AV_PIX_FMT_UYVY422 )
	SQBIND_GLOBALCONST( AV_PIX_FMT_UYYVYY411 )

	// Processing types
	SQBIND_GLOBALCONST( SWS_FAST_BILINEAR )
	SQBIND_GLOBALCONST( SWS_BILINEAR )
	SQBIND_GLOBALCONST( SWS_BICUBIC )
	SQBIND_GLOBALCONST( SWS_X )
	SQBIND_GLOBALCONST( SWS_POINT )
	SQBIND_GLOBALCONST( SWS_AREA )
	SQBIND_GLOBALCONST( SWS_BICUBLIN )
	SQBIND_GLOBALCONST( SWS_GAUSS )
	SQBIND_GLOBALCONST( SWS_SINC )
	SQBIND_GLOBALCONST( SWS_LANCZOS )
	SQBIND_GLOBALCONST( SWS_SPLINE )
	SQBIND_GLOBALCONST( SWS_SRC_V_CHR_DROP_MASK )
	SQBIND_GLOBALCONST( SWS_SRC_V_CHR_DROP_SHIFT )
	SQBIND_GLOBALCONST( SWS_PARAM_DEFAULT )
	SQBIND_GLOBALCONST( SWS_PRINT_INFO )
	SQBIND_GLOBALCONST( SWS_FULL_CHR_H_INT )
	SQBIND_GLOBALCONST( SWS_FULL_CHR_H_INP )
	SQBIND_GLOBALCONST( SWS_DIRECT_BGR )
	SQBIND_GLOBALCONST( SWS_ACCURATE_RND )
//	SQBIND_GLOBALCONST( SWS_CPU_CAPS_MMX )
//	SQBIND_GLOBALCONST( SWS_CPU_CAPS_MMX2 )
//	SQBIND_GLOBALCONST( SWS_CPU_CAPS_3DNOW )
//	SQBIND_GLOBALCONST( SWS_CPU_CAPS_ALTIVEC )
//	SQBIND_GLOBALCONST( SWS_CPU_CAPS_BFIN )
	SQBIND_GLOBALCONST( SWS_MAX_REDUCE_CUTOFF )
	SQBIND_GLOBALCONST( SWS_CS_ITU709 )
	SQBIND_GLOBALCONST( SWS_CS_FCC )
	SQBIND_GLOBALCONST( SWS_CS_ITU601 )
	SQBIND_GLOBALCONST( SWS_CS_ITU624 )
	SQBIND_GLOBALCONST( SWS_CS_SMPTE170M )
	SQBIND_GLOBALCONST( SWS_CS_SMPTE240M )
	SQBIND_GLOBALCONST( SWS_CS_DEFAULT )

SQBIND_REGISTER_CLASS_END()

void CFfConvert::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CFfConvert );
}

CFfConvert::CFfConvert()
{_STT();

	m_psc = 0;
	m_src_width = 0;
	m_src_height = 0;
	m_src_size = 0;
	m_src_fmt = 0;
	m_dst_width = 0;
	m_dst_height = 0;
	m_dst_size = 0;
	m_dst_fmt = 0;
	m_flip = 0;

}

CFfConvert::~CFfConvert()
{
	Destroy();
}

void CFfConvert::Destroy()
{_STT();

	if ( m_psc )
	{	sws_freeContext( m_psc );
		m_psc = 0;
	} // end if
	
	m_src_width = 0;
	m_src_height = 0;
	m_src_size = 0;
	m_src_fmt = 0;
	m_dst_width = 0;
	m_dst_height = 0;
	m_dst_size = 0;
	m_dst_fmt = 0;
	m_flip = 0;
}

#define FLIP_AVLINE( p, i, h ) \
	if ( i < oexSizeOfArray( p->linesize ) && p->data[ i ] && p->linesize[ i ] ) \
		p->data[ i ] = p->data[ i ] + ( ( h ) - 1 ) * p->linesize[ i ], \
		p->linesize[ i ] = -p->linesize[ i ];

int CFfConvert::Flip( int fmt, int h, AVPicture *p )
{_STT();

	if ( !p )
		return 0;

	// For all formats
	FLIP_AVLINE( p, 0, h );

	switch( fmt )
	{
		case AV_PIX_FMT_YUV410P :
		case AV_PIX_FMT_YUV411P :
		case AV_PIX_FMT_YUV420P :
		case AV_PIX_FMT_YUV422P :
		case AV_PIX_FMT_YUV440P :
		case AV_PIX_FMT_YUV444P :
		case AV_PIX_FMT_YUYV422 :
		case AV_PIX_FMT_YUVJ420P :
		case AV_PIX_FMT_YUVJ422P :
		case AV_PIX_FMT_YUVJ444P :
		case AV_PIX_FMT_YUVJ440P :
		case AV_PIX_FMT_YUVA420P :
			FLIP_AVLINE( p, 1, h / 2 );
			FLIP_AVLINE( p, 2, h / 2 );
			break;

	} // end for

	return 1;
}

int CFfConvert::Create( int src_width, int src_height, int src_fmt, int dst_width, int dst_height, int dst_fmt, int alg, int flip )
{_STT();

	// Lose the old transform
	Destroy();
	
_STT_SET_CHECKPOINT( 1 );

	// Sanity check
	if ( !src_width || ( src_width & 3 ) || !src_height || !dst_width || ( dst_width & 3 ) || !dst_height )
		return 0;
	
	// Fix broken formats
	src_fmt = CFfFmt::FmtEquiv( src_fmt );
	dst_fmt = CFfFmt::FmtEquiv( dst_fmt );

	m_flip = flip ? 1 : 0;
	
	if ( 0 > src_height ) 
	{ 	m_flip = m_flip ? 0 : 1; 
		src_height = -src_height; 
	} // end if
	
	if ( 0 > dst_height ) 
	{ 	m_flip = m_flip ? 0 : 1;
		dst_height = -dst_height; 
	} // end if

	// How big is the source image?
	m_src_size = CFfFmt::CalcImageSize( src_fmt, src_width, src_height );
	if ( !m_src_size )
	{	Destroy();
		return 0;
	} // end if

	// How big is the destination image
	m_dst_size = CFfFmt::CalcImageSize( dst_fmt, dst_width, dst_height );
	if ( !m_dst_size )
	{	Destroy();
		return 0;
	} // end if
	
_STT_SET_CHECKPOINT( 2 );

	// Create conversion
	m_psc = sws_getContext(	src_width, src_height, (AVPixelFormat)src_fmt,
							dst_width, dst_height, (AVPixelFormat)dst_fmt,
							alg, NULL, NULL, NULL );

_STT_SET_CHECKPOINT( 3 );

	if ( !m_psc )
	{	Destroy();
		return 0;
	} // end if
	
	m_src_width = src_width;
	m_src_height = src_height;
	m_src_fmt = src_fmt; 

	m_dst_width = dst_width;
	m_dst_height = dst_height;
	m_dst_fmt = dst_fmt;

	return 1;
}

int CFfConvert::ConvertBB( sqbind::CSqBinary *src, sqbind::CSqBinary *dst )
{_STT();

	if ( !m_psc )
		return 0;
	
_STT_SET_CHECKPOINT( 10 );

	// Ensure source buffer is large enough
	if ( src->getUsed() < m_src_size )
		return 0;

	// Allocate memory for destination image
	if ( dst->Size() < ( m_dst_size + FF_INPUT_BUFFER_PADDING_SIZE * 2 )
		 && !dst->Allocate( m_dst_size + FF_INPUT_BUFFER_PADDING_SIZE * 2 ) )
		return 0;
		
_STT_SET_CHECKPOINT( 11 );

	// Fill in picture data
	AVPicture apSrc, apDst;
	if ( !CFfFmt::FillAVPicture( &apSrc, m_src_fmt, m_src_width, m_src_height, src->_Ptr() )
	     || !CFfFmt::FillAVPicture( &apDst, m_dst_fmt, m_dst_width, m_dst_height, dst->_Ptr() ) )
		return 0;

_STT_SET_CHECKPOINT( 12 );

	if ( m_flip )
		Flip( m_src_fmt, m_src_height, &apSrc );

_STT_SET_CHECKPOINT( 13 );

	int nRet = sws_scale(	m_psc, apSrc.data, apSrc.linesize, 0, m_src_height,
							apDst.data, apDst.linesize );

_STT_SET_CHECKPOINT( 14 );

	if ( 0 >= nRet )
		return 0;

	dst->setUsed( m_dst_size );

	return 1;
}


int CFfConvert::ConvertIB( sqbind::CSqImage *src, sqbind::CSqBinary *dst )
{_STT();

	if ( !m_psc )
		return 0;
	
_STT_SET_CHECKPOINT( 10 );

	// Ensure source buffer is large enough
	if ( src->getUsed() < m_src_size || src->getWidth() != m_src_width || src->getHeight() != m_src_height )
		return 0;

	// Allocate memory for destination image
	if ( dst->Size() < ( m_dst_size + FF_INPUT_BUFFER_PADDING_SIZE * 2 )
		 && !dst->Allocate( m_dst_size + FF_INPUT_BUFFER_PADDING_SIZE * 2 ) )
		return 0;
		
_STT_SET_CHECKPOINT( 11 );

	// Fill in picture data
	AVPicture apSrc, apDst;
	if ( !CFfFmt::FillAVPicture( &apSrc, m_src_fmt, m_src_width, m_src_height, src->Obj().GetBits() )
	     || !CFfFmt::FillAVPicture( &apDst, m_dst_fmt, m_dst_width, m_dst_height, dst->_Ptr() ) )
		return 0;

_STT_SET_CHECKPOINT( 12 );

	if ( m_flip )
		Flip( m_src_fmt, m_src_height, &apSrc );

_STT_SET_CHECKPOINT( 13 );

	int nRet = sws_scale(	m_psc, apSrc.data, apSrc.linesize, 0, m_src_height,
							apDst.data, apDst.linesize );

_STT_SET_CHECKPOINT( 14 );

	if ( 0 >= nRet )
		return 0;

	dst->setUsed( m_dst_size );

	return 1;
}

int CFfConvert::ConvertBI( sqbind::CSqBinary *src, sqbind::CSqImage *dst )
{_STT();

	if ( !m_psc )
		return 0;
	
_STT_SET_CHECKPOINT( 10 );

	// Ensure source buffer is large enough
	if ( src->getUsed() < m_src_size )
		return 0;

	// Do we need to allocate destination image?
	if ( dst->getWidth() != m_dst_width || dst->getHeight() != m_dst_height )
		if ( !dst->Create( m_dst_width, m_dst_height ) )
			return 0;
		
_STT_SET_CHECKPOINT( 11 );

	// Fill in picture data
	AVPicture apSrc, apDst;
	if ( !CFfFmt::FillAVPicture( &apSrc, m_src_fmt, m_src_width, m_src_height, src->_Ptr() )
	     || !CFfFmt::FillAVPicture( &apDst, m_dst_fmt, m_dst_width, m_dst_height, dst->Obj().GetBits() ) )
		return 0;

_STT_SET_CHECKPOINT( 12 );

	if ( m_flip )
		Flip( m_src_fmt, m_src_height, &apSrc );

_STT_SET_CHECKPOINT( 13 );

	int nRet = sws_scale(	m_psc, apSrc.data, apSrc.linesize, 0, m_src_height,
							apDst.data, apDst.linesize );

_STT_SET_CHECKPOINT( 14 );

	if ( 0 >= nRet )
		return 0;

	return 1;
}

int CFfConvert::ConvertII( sqbind::CSqImage *src, sqbind::CSqImage *dst )
{_STT();

	if ( !m_psc )
		return 0;
	
_STT_SET_CHECKPOINT( 10 );

	// Ensure source buffer is large enough
	if ( src->getUsed() < m_src_size || src->getWidth() != m_src_width || src->getHeight() != m_src_height )
		return 0;

	// Do we need to allocate destination image?
	if ( dst->getWidth() != m_dst_width || dst->getHeight() != m_dst_height )
		if ( !dst->Create( m_dst_width, m_dst_height ) )
			return 0;
		
_STT_SET_CHECKPOINT( 11 );

	// Fill in picture data
	AVPicture apSrc, apDst;
	if ( !CFfFmt::FillAVPicture( &apSrc, m_src_fmt, m_src_width, m_src_height, src->Obj().GetBits() )
	     || !CFfFmt::FillAVPicture( &apDst, m_dst_fmt, m_dst_width, m_dst_height, dst->Obj().GetBits() ) )
		return 0;

_STT_SET_CHECKPOINT( 12 );

	if ( m_flip )
		Flip( m_src_fmt, m_src_height, &apSrc );

_STT_SET_CHECKPOINT( 13 );

	int nRet = sws_scale(	m_psc, apSrc.data, apSrc.linesize, 0, m_src_height,
							apDst.data, apDst.linesize );

_STT_SET_CHECKPOINT( 14 );

	if ( 0 >= nRet )
		return 0;

	return 1;
}

int CFfConvert::ConvertRI( void *src, sqbind::CSqImage *dst )
{_STT();

	if ( !m_psc )
		return 0;
	
_STT_SET_CHECKPOINT( 10 );

	// Ensure source buffer is large enough
	if ( !src )
		return 0;

	// Do we need to allocate destination image?
	if ( dst->getWidth() != m_dst_width || dst->getHeight() != m_dst_height )
		if ( !dst->Create( m_dst_width, m_dst_height ) )
			return 0;
		
_STT_SET_CHECKPOINT( 11 );

	// Fill in picture data
	AVPicture apSrc, apDst;
	if ( !CFfFmt::FillAVPicture( &apSrc, m_src_fmt, m_src_width, m_src_height, src )
	     || !CFfFmt::FillAVPicture( &apDst, m_dst_fmt, m_dst_width, m_dst_height, dst->Obj().GetBits() ) )
		return 0;

_STT_SET_CHECKPOINT( 12 );

	if ( m_flip )
		Flip( m_src_fmt, m_src_height, &apSrc );

_STT_SET_CHECKPOINT( 13 );

	int nRet = sws_scale(	m_psc, apSrc.data, apSrc.linesize, 0, m_src_height,
							apDst.data, apDst.linesize );

_STT_SET_CHECKPOINT( 14 );

	if ( 0 >= nRet )
		return 0;

	return 1;
}

int CFfConvert::ConvertFI( AVFrame* src, sqbind::CSqImage *dst )
{_STT();

	if ( !m_psc )
		return 0;
	
_STT_SET_CHECKPOINT( 10 );

	// Ensure source buffer is large enough
	if ( !src )
		return 0;

	// Do we need to allocate destination image?
	if ( dst->getWidth() != m_dst_width || dst->getHeight() != m_dst_height )
		if ( !dst->Create( m_dst_width, m_dst_height ) )
			return 0;
		
_STT_SET_CHECKPOINT( 11 );

	// Fill in picture data
	AVPicture apSrc, apDst;

	// Copy source information
	oexZero( apSrc );
	for ( int i = 0; i < (int)oexSizeOfArray( apSrc.linesize ); i++ )
		apSrc.data[ i ] = src->data[ i ],
		apSrc.linesize[ i ] = src->linesize[ i ];
	
	if ( !CFfFmt::FillAVPicture( &apDst, m_dst_fmt, m_dst_width, m_dst_height, dst->Obj().GetBits() ) )
		return 0;

_STT_SET_CHECKPOINT( 12 );

	if ( m_flip )
		Flip( m_src_fmt, m_src_height, &apSrc );

_STT_SET_CHECKPOINT( 13 );

	int nRet = sws_scale(	m_psc, apSrc.data, apSrc.linesize, 0, m_src_height,
							apDst.data, apDst.linesize );

_STT_SET_CHECKPOINT( 14 );

	if ( 0 >= nRet )
		return 0;

	return 1;
}

int CFfConvert::ConvertFB( AVFrame* src, sqbind::CSqBinary *dst )
{_STT();

	if ( !m_psc )
		return 0;
	
_STT_SET_CHECKPOINT( 10 );

	// Ensure source buffer is large enough
	if ( !src )
		return 0;

	// Allocate memory for destination image
	if ( dst->Size() < ( m_dst_size + FF_INPUT_BUFFER_PADDING_SIZE * 2 )
		 && !dst->Allocate( m_dst_size + FF_INPUT_BUFFER_PADDING_SIZE * 2 ) )
		return 0;
				
_STT_SET_CHECKPOINT( 11 );

	// Fill in picture data
	AVPicture apSrc, apDst;
	
	// Copy source information
	oexZero( apSrc );
	for ( int i = 0; i < (int)oexSizeOfArray( apSrc.linesize ); i++ )
		apSrc.data[ i ] = src->data[ i ],
		apSrc.linesize[ i ] = src->linesize[ i ];
	
	if ( !CFfFmt::FillAVPicture( &apDst, m_dst_fmt, m_dst_width, m_dst_height, dst->_Ptr() ) )
		return 0;

_STT_SET_CHECKPOINT( 12 );

	if ( m_flip )
		Flip( m_src_fmt, m_src_height, &apSrc );

_STT_SET_CHECKPOINT( 13 );

	int nRet = sws_scale(	m_psc, apSrc.data, apSrc.linesize, 0, m_src_height,
							apDst.data, apDst.linesize );

_STT_SET_CHECKPOINT( 14 );

	if ( 0 >= nRet )
		return 0;

	dst->setUsed( m_dst_size );

	return 1;
}

int CFfConvert::Rotate( int deg, sqbind::CSqBinary *src, int src_fmt, int width, int height, sqbind::CSqBinary *dst, int dst_fmt )
{_STT();

	// Sanity checks
	if ( !src || !dst || 0 >= width || 0 == height )
		return 0;

	// RGB24
	if ( ( AV_PIX_FMT_BGR24 == src_fmt || AV_PIX_FMT_RGB24 == src_fmt )
		 && ( AV_PIX_FMT_BGR24 == dst_fmt || AV_PIX_FMT_RGB24 == dst_fmt ) )
	{
		int sw = oex::CImage::GetScanWidth( width, 24 );
		int sz = sw * height;

		// Ensure source buffer is large enough
		if ( (int)src->getUsed() < sz )
			return 0;

		// Ensure destination buffer is large enough
		if ( (int)dst->getUsed() < sz )
		{	if ( !dst->Allocate( sz + FF_INPUT_BUFFER_PADDING_SIZE * 2 ) )
				return 0;
			else
				dst->setUsed( sz );
		} // end if

		// Do the rotation
		if ( 90 == deg )
			return Rotate90_3( src->_Ptr(), dst->_Ptr(), width, height );

		else if ( 180 == deg )
			return Rotate180_3( src->_Ptr(), dst->_Ptr(), width, height );

		else if ( 270 == deg )
			return Rotate270_3( src->_Ptr(), dst->_Ptr(), width, height );

	} // end if

	// RGB32
	else if ( ( AV_PIX_FMT_BGR32 == src_fmt || AV_PIX_FMT_RGB32 == src_fmt )
			  && ( AV_PIX_FMT_BGR32 == dst_fmt || AV_PIX_FMT_RGB32 == dst_fmt ) )
	{
		int sw = oex::CImage::GetScanWidth( width, 32 );
		int sz = sw * height;

		// Ensure source buffer is large enough
		if ( (int)src->getUsed() < sz )
			return 0;

		// Ensure destination buffer is large enough
		if ( (int)dst->getUsed() < sz )
		{	if ( !dst->Allocate( sz + FF_INPUT_BUFFER_PADDING_SIZE * 2 ) )
				return 0;
			else
				dst->setUsed( sz );
		} // end if

		// Do the rotation
		if ( 90 == deg )
			return Rotate90_4( src->_Ptr(), dst->_Ptr(), width, height );

		else if ( 180 == deg )
			return Rotate180_4( src->_Ptr(), dst->_Ptr(), width, height );

		else if ( 270 == deg )
			return Rotate270_4( src->_Ptr(), dst->_Ptr(), width, height );

	} // end if

	return 0;
}

int CFfConvert::Rotate90_3( void *src, void *dst, int width, int height )
{_STT();

	// Sanity checks
	if ( !src || !dst || 0 >= width || 0 == height )
		return 0;

	// Rotate the image
	int sstride = oex::CImage::GetScanWidth( width, 24 );
	int dstride = oex::CImage::GetScanWidth( height, 24 );
	unsigned char *s = (unsigned char*)src + sstride - 3;
	for ( int y = 0; y < height; y++, s += sstride << 1 )
	{	unsigned char *d = (unsigned char*)dst + ( y * 3 );
		for ( int x = 0; x < width; x++, s -= 3, d += dstride )
			d[ 0 ] = s[ 0 ], d[ 1 ] = s[ 1 ], d[ 2 ] = s[ 2 ];
	} // end for

	return 1;
}

int CFfConvert::Rotate180_3( void *src, void *dst, int width, int height )
{_STT();

	// Sanity checks
	if ( !src || !dst || 0 >= width || 0 == height )
		return 0;

	// Rotate the image
	int stride = oex::CImage::GetScanWidth( width, 24 );
	int diff = stride - ( width * 3 );
	unsigned char *s = (unsigned char*)src;
	unsigned char *d = (unsigned char*)dst + ( height * stride ) - 3;
	for ( int y = 0; y < height; y++, s += diff, d -= diff )
		for ( int x = 0; x < width; x++, s += 3, d -= 3 )
			d[ 0 ] = s[ 0 ], d[ 1 ] = s[ 1 ], d[ 2 ] = s[ 2 ];

	return 1;
}

int CFfConvert::Rotate270_3( void *src, void *dst, int width, int height )
{_STT();

	// Sanity checks
	if ( !src || !dst || 0 >= width || 0 == height )
		return 0;

	// Rotate the image
	int sstride = oex::CImage::GetScanWidth( width, 24 );
	int dstride = oex::CImage::GetScanWidth( height, 24 );
	unsigned char *s = (unsigned char*)src + sstride - 3;
	for ( int y = 0; y < height; y++, s += sstride << 1 )
	{	unsigned char *d = (unsigned char*)dst + ( dstride * width ) - ( y * 3 ) - 3;
		for ( int x = 0; x < width; x++, s -= 3, d -= dstride )
			d[ 0 ] = s[ 0 ], d[ 1 ] = s[ 1 ], d[ 2 ] = s[ 2 ];
	} // end for

	return 1;
}

int CFfConvert::Rotate90_4( void *src, void *dst, int width, int height )
{_STT();

	// Sanity checks
	if ( !src || !dst || 0 >= width || 0 == height )
		return 0;

	// Rotate the image
	unsigned int *s = (unsigned int*)src + width - 1;
	for ( int y = 0; y < height; y++, s += width << 1 )
	{	unsigned int *d = (unsigned int*)dst + y;
		for ( int x = 0; x < width; x++, s--, d += height )
			*d = *s;
	} // end for

	return 1;
}

int CFfConvert::Rotate180_4( void *src, void *dst, int width, int height )
{_STT();

	// Sanity checks
	if ( !src || !dst || 0 >= width || 0 == height )
		return 0;

	// Rotate the image
	unsigned int *s = (unsigned int*)src;
	unsigned int *d = (unsigned int*)dst + ( height * width ) - 1;
	for ( int i = width * height; i; i--, s++, d-- )
		*d = *s;

	return 1;
}

int CFfConvert::Rotate270_4( void *src, void *dst, int width, int height )
{_STT();

	// Sanity checks
	if ( !src || !dst || 0 >= width || 0 == height )
		return 0;

	// Rotate the image
	unsigned int *s = (unsigned int*)src + width - 3;
	for ( int y = 0; y < height; y++, s += width << 1 )
	{	unsigned int *d = (unsigned int*)dst + ( height * width ) - y - 1;
		for ( int x = 0; x < width; x++, s--, d -= height )
			*d = *s;
	} // end for

	return 1;
}

int CFfConvert::FlipVert_3( void *src, void *dst, int width, int height )
{_STT();

	// Sanity checks
	if ( !src || !dst || 0 >= width || 0 == height )
		return 0;

	// Flip the image
	int stride = oex::CImage::GetScanWidth( width, 24 );
	int diff = stride - ( width * 3 );
	unsigned char *s = (unsigned char*)src;
	unsigned char *d = (unsigned char*)dst + ( ( height - 1 ) * stride );
	for ( int y = 0; y < height; y++, s += diff, d -= ( stride * 2 ) )
		for ( int x = 0; x < width; x++, s += 3, d += 3 )
			d[ 0 ] = s[ 0 ], d[ 1 ] = s[ 1 ], d[ 2 ] = s[ 2 ];

	return 1;
}

int CFfConvert::FlipVert_4( void *src, void *dst, int width, int height )
{_STT();

	// Sanity checks
	if ( !src || !dst || 0 >= width || 0 == height )
		return 0;

	// Flip the image
	unsigned int *s = (unsigned int*)src;
	unsigned int *d = (unsigned int*)dst + ( ( height - 1 ) * width );
	for ( int y = 0; y < height; y++, d -= ( width * 2 ) )
		for ( int x = 0; x < width; x++, s++, d++ )
			*d = *s;

	return 1;
}

int CFfConvert::FlipVert( sqbind::CSqBinary *src, int src_fmt, int width, int height, sqbind::CSqBinary *dst, int dst_fmt )
{_STT();

	// Sanity checks
	if ( !src || !dst || 0 >= width || 0 == height )
		return 0;

	// RGB24
	if ( ( AV_PIX_FMT_BGR24 == src_fmt || AV_PIX_FMT_RGB24 == src_fmt )
		 && ( AV_PIX_FMT_BGR24 == dst_fmt || AV_PIX_FMT_RGB24 == dst_fmt ) )
	{
		int sw = oex::CImage::GetScanWidth( width, 24 );
		int sz = sw * height;

		// Ensure source buffer is large enough
		if ( (int)src->getUsed() < sz )
			return 0;

		// Ensure destination buffer is large enough
		if ( (int)dst->getUsed() < sz )
		{	if ( !dst->Allocate( sz + FF_INPUT_BUFFER_PADDING_SIZE * 2 ) )
				return 0;
			else
				dst->setUsed( sz );
		} // end if

		return FlipVert_3( src->_Ptr(), dst->_Ptr(), width, height );

	} // end if

	// RGB32
	else if ( ( AV_PIX_FMT_BGR32 == src_fmt || AV_PIX_FMT_RGB32 == src_fmt )
			  && ( AV_PIX_FMT_BGR32 == dst_fmt || AV_PIX_FMT_RGB32 == dst_fmt ) )
	{
		int sw = oex::CImage::GetScanWidth( width, 32 );
		int sz = sw * height;

		// Ensure source buffer is large enough
		if ( (int)src->getUsed() < sz )
			return 0;

		// Ensure destination buffer is large enough
		if ( (int)dst->getUsed() < sz )
		{	if ( !dst->Allocate( sz + FF_INPUT_BUFFER_PADDING_SIZE * 2 ) )
				return 0;
			else
				dst->setUsed( sz );
		} // end if

		return FlipVert_4( src->_Ptr(), dst->_Ptr(), width, height );

	} // end if

	return 0;
}
