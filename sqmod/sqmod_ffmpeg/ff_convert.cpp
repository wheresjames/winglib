// ff_convert.cpp

#include "stdafx.h"

CFfConvert::CFfConvert()
{_STT();
}

int CFfConvert::CalcImageSize( int fmt, int width, int height )
{_STT();
	return avpicture_get_size( (PixelFormat)fmt, width, height );
}

int CFfConvert::FillAVPicture( AVPicture *pPic, int fmt, int width, int height, void *buf )
{_STT();
	if ( !pPic || !buf )
		return 0;

	// Initialize structure
	oexZeroMemory( pPic, sizeof( AVPicture ) );

	// Fill in the picture data
	int nSize = avpicture_fill( pPic, (uint8_t*)buf, (PixelFormat)fmt, width, height );
	if ( 0 >= nSize )
		return 0;

	return nSize;
}

int CFfConvert::FillAVFrame( AVFrame *pAv, int fmt, int width, int height, void *buf )
{_STT();
	if ( !pAv || !buf )
		return 0;

	AVPicture avPic;
	oexZero( avPic );

	int nSize = avpicture_fill( &avPic, (uint8_t*)buf, (PixelFormat)fmt, width, height );
	if ( 0 >= nSize )
		return 0;

	for( int i = 0; i < (int)oexSizeOfArray( avPic.data ); i++ )
		pAv->data[ i ] = avPic.data[ i ];

	for( int i = 0; i < (int)oexSizeOfArray( avPic.linesize ); i++ )
		pAv->linesize[ i ] = avPic.linesize[ i ];

	return nSize;
}

int CFfConvert::ConvertColorBB( int width, int height, sqbind::CSqBinary *src, int src_fmt, sqbind::CSqBinary *dst, int dst_fmt, int alg )
{_STT();
	
	if ( !dst )
		return 0;

	dst->setUsed( 0 );

	int flip = 0;
	if ( 0 > height ) { flip = 1; height = -height; }

	if ( 0 >= width || 0 >= height || 0 != ( width % 4 ) )
		return 0;

	if ( !src || !src->getUsed() )
		return 0;
	
	// How big is the destination image?
	oexSIZE_T nSize = CalcImageSize( dst_fmt, width, height );
	if ( !nSize )
		return 0;

	// Allocate memory for destination image
	if ( dst->Size() < nSize && !dst->Mem().Mem().OexNew( nSize ).Ptr() )
		return 0;

	// Fill in picture data
	AVPicture apSrc, apDst;
	if ( !FillAVPicture( &apSrc, src_fmt, width, height, src->_Ptr() )
	     || !FillAVPicture( &apDst, dst_fmt, width, height, dst->_Ptr() ) )
		return 0;

	// Flip?
	if ( flip )
		for ( int z = 0; z < oexSizeOfArray( apSrc.linesize ); z++ )
			apSrc.data[ z ] = apSrc.data[ z ] + ( height - 1 ) * apSrc.linesize[ z ],
			apSrc.linesize[ z ] = -apSrc.linesize[ z ];

	// Create conversion
	SwsContext *psc = sws_getContext(	width, height, (PixelFormat)src_fmt,
										width, height, (PixelFormat)dst_fmt,
										alg, NULL, NULL, NULL );
	if ( !psc )
		return 0;

	int nRet = sws_scale(	psc, apSrc.data, apSrc.linesize, 0, height,
							apDst.data, apDst.linesize );

	sws_freeContext( psc );

	if ( 0 >= nRet )
		return 0;

	dst->setUsed( nSize );

	return 1;
}

int CFfConvert::ConvertColorIB( sqbind::CSqImage *img, sqbind::CSqBinary *dst, int dst_fmt, int alg, int flip )
{_STT();
	if ( !dst )
		return 0;

	dst->setUsed( 0 );

	if ( !img )
		return 0;

	PixelFormat src_fmt = PIX_FMT_BGR24;
	int width = img->getWidth();
	int height = img->getHeight();
	if ( !img->Obj().GetBits() || 0 >= width || 0 >= height )
		return 0;

	// How big is the destination image?
	oexSIZE_T nSize = CalcImageSize( dst_fmt, width, height );
	if ( !nSize )
		return 0;

	// Allocate memory for destination image
	if ( dst->Size() < nSize && !dst->Mem().Mem().OexNew( nSize ).Ptr() )
		return 0;

	// Fill in picture data
	AVPicture apSrc, apDst;
	if ( !FillAVPicture( &apSrc, src_fmt, width, height, img->Obj().GetBits() )
	     || !FillAVPicture( &apDst, dst_fmt, width, height, dst->_Ptr() ) )
		return 0;

	// Flip?
	if ( flip )
		for ( int z = 0; z < oexSizeOfArray( apSrc.linesize ); z++ )
			apSrc.data[ z ] = apSrc.data[ z ] + ( ( height - 1 ) * apSrc.linesize[ z ] ),
			apSrc.linesize[ z ] = -apSrc.linesize[ z ];

/*	More overhead has been added to colorspace conversion

	// Convert the image
	if ( img_convert( &apDst, dst_fmt, &apSrc, src_fmt, width, height ) )
		return 0;

	// Save the number of bytes used in the output buffer
	dst->setUsed( nSize );

	return 1;
*/

	// Create conversion
	SwsContext *psc = sws_getContext(	width, height, src_fmt,
										width, height, (PixelFormat)dst_fmt,
										alg, NULL, NULL, NULL );

	if ( !psc )
		return 0;

	// Do the conversion
	int nRet = sws_scale( psc, apSrc.data, apSrc.linesize, 0, height,
			   			  apDst.data, apDst.linesize );

	sws_freeContext( psc );

	if ( 0 >= nRet )
		return 0;

	dst->setUsed( nSize );

	return 1;
}

int CFfConvert::ConvertColorBI( sqbind::CSqBinary *src, int src_fmt, int width, int height, sqbind::CSqImage *img, int alg, int flip )
{_STT();
	// Sanity checks
	if ( !img || 0 >= width || 0 == height || !src || !src->getUsed() )
		return 0;

	if ( 0 > height ) { flip = 1; height = -height; }

	// Image format
	PixelFormat dst_fmt = PIX_FMT_BGR24;

	// Do we need to allocate destination image?
	if ( img->getWidth() != width || img->getHeight() != height )
		if ( !img->Create( width, height ) )
			return 0;

	// Fill in picture data
	AVPicture apSrc, apDst;
	if ( !FillAVPicture( &apSrc, src_fmt, width, height, src->_Ptr() )
	     || !FillAVPicture( &apDst, dst_fmt, width, height, img->Obj().GetBits() ) )
		return 0;

	// Flip?
	if ( flip )
		for ( int z = 0; z < oexSizeOfArray( apSrc.linesize ); z++ )
			apSrc.data[ z ] = apSrc.data[ z ] + ( ( height - 1 ) * apSrc.linesize[ z ] ),
			apSrc.linesize[ z ] = -apSrc.linesize[ z ];

	// Create conversion
	SwsContext *psc = sws_getContext(	width, height, (PixelFormat)src_fmt,
										width, height, dst_fmt,
										alg, NULL, NULL, NULL );
	if ( !psc )
		return 0;

	// Do the conversion
	int nRet = sws_scale( psc, apSrc.data, apSrc.linesize, 0, height,
			   			  apDst.data, apDst.linesize );

	sws_freeContext( psc );

	if ( 0 >= nRet )
		return 0;

	return 1;
}

int CFfConvert::ConvertColorRI( void *buf, int src_fmt, int width, int height, sqbind::CSqImage *img, int alg, int flip )
{_STT();
	// Sanity checks
	if ( !img || 0 >= width || 0 == height || !buf )
		return 0;

	if ( 0 > height ) { flip = 1; height = -height; }

	// Image format
	PixelFormat dst_fmt = PIX_FMT_BGR24;

	// Do we need to allocate destination image?
	if ( img->getWidth() != width || img->getHeight() != height )
		if ( !img->Create( width, height ) )
			return 0;

	// Fill in picture data
	AVPicture apSrc, apDst;
	if ( !FillAVPicture( &apSrc, src_fmt, width, height, buf )
	     || !FillAVPicture( &apDst, dst_fmt, width, height, img->Obj().GetBits() ) )
		return 0;

	// Flip?
	if ( flip )
		for ( int z = 0; z < oexSizeOfArray( apSrc.linesize ); z++ )
			apSrc.data[ z ] = apSrc.data[ z ] + ( ( height - 1 ) * apSrc.linesize[ z ] ),
			apSrc.linesize[ z ] = -apSrc.linesize[ z ];

	// Create conversion
	SwsContext *psc = sws_getContext(	width, height, (PixelFormat)src_fmt,
										width, height, dst_fmt,
										alg, NULL, NULL, NULL );
	if ( !psc )
		return 0;

	// Do the conversion
	int nRet = sws_scale( psc, apSrc.data, apSrc.linesize, 0, height,
			   			  apDst.data, apDst.linesize );

	sws_freeContext( psc );

	if ( 0 >= nRet )
		return 0;

	return 1;
}

int CFfConvert::ConvertColorFI( AVFrame* pAf, int src_fmt, int width, int height, sqbind::CSqImage *img, int alg, int flip )
{_STT();
	// Sanity checks
	if ( !img || 0 >= width || 0 == height || !pAf )
		return 0;

	if ( 0 > height ) { flip = 1; height = -height; }

	// Image format
	PixelFormat dst_fmt = PIX_FMT_BGR24;

	// Do we need to allocate destination image?
	if ( img->getWidth() != width || img->getHeight() != height )
		if ( !img->Create( width, height ) )
			return 0;

	// Fill in picture data
	AVPicture apSrc, apDst;

	// Copy source information
	for ( int i = 0; i < oexSizeOfArray( apSrc.linesize ); i++ )
		apSrc.data[ i ] = pAf->data[ i ],
		apSrc.linesize[ i ] = pAf->linesize[ i ];

	if ( !FillAVPicture( &apDst, dst_fmt, width, height, img->Obj().GetBits() ) )
		return 0;

	// Flip?
	if ( flip )
		for ( int z = 0; z < oexSizeOfArray( apSrc.linesize ); z++ )
			apSrc.data[ z ] = apSrc.data[ z ] + ( ( height - 1 ) * apSrc.linesize[ z ] ),
			apSrc.linesize[ z ] = -apSrc.linesize[ z ];

	// Create conversion
	SwsContext *psc = sws_getContext(	width, height, (PixelFormat)src_fmt,
										width, height, dst_fmt,
										alg, NULL, NULL, NULL );
	if ( !psc )
		return 0;

	// Do the conversion
	int nRet = sws_scale( psc, apSrc.data, apSrc.linesize, 0, height,
			   			  apDst.data, apDst.linesize );

	sws_freeContext( psc );

	if ( 0 >= nRet )
		return 0;

	return 1;
}

int CFfConvert::ConvertColorFB( AVFrame* pAf, int src_fmt, int width, int height, int dst_fmt, sqbind::CSqBinary *dst, int alg, int flip )
{_STT();
	// Sanity checks
	if ( !dst || 0 >= width || 0 == height || !pAf )
		return 0;

	if ( 0 > height ) { flip = 1; height = -height; }

	// Output size
	oexSIZE_T nSize = CFfConvert::CalcImageSize( dst_fmt, width, height );
	if ( dst->Size() < nSize && !dst->Mem().Mem().OexNew( nSize ).Ptr() )
		return 0;

	// Fill in picture data
	AVPicture apSrc, apDst;

	// Copy source information
	for ( int i = 0; i < 4; i++ )
		apSrc.data[ i ] = pAf->data[ i ],
		apSrc.linesize[ i ] = pAf->linesize[ i ];

	if ( !FillAVPicture( &apDst, dst_fmt, width, height, dst->_Ptr() ) )
		return 0;

	// Flip?
	if ( flip )
		for ( int z = 0; z < oexSizeOfArray( apDst.linesize ); z++ )
			apDst.data[ z ] = apDst.data[ z ] + ( height - 1 ) * apDst.linesize[ z ],
			apDst.linesize[ z ] = -apDst.linesize[ z ];

	// Create conversion
	SwsContext *psc = sws_getContext(	width, height, (PixelFormat)src_fmt,
										width, height, (PixelFormat)dst_fmt,
										alg, NULL, NULL, NULL );
	if ( !psc )
		return 0;

	// Do the conversion
	int nRet = sws_scale( psc, apSrc.data, apSrc.linesize, 0, height,
			   			  apDst.data, apDst.linesize );

	sws_freeContext( psc );

	if ( 0 >= nRet )
		return 0;

	dst->setUsed( nSize );

	return 1;
}

int CFfConvert::Rotate( int deg, sqbind::CSqBinary *src, int src_fmt, int width, int height, sqbind::CSqBinary *dst, int dst_fmt )
{
	// Sanity checks
	if ( !src || !dst || 0 >= width || 0 == height )
		return 0;

	// RGB24
	if ( ( PIX_FMT_BGR24 == src_fmt || PIX_FMT_RGB24 == src_fmt )
		 && ( PIX_FMT_BGR24 == dst_fmt || PIX_FMT_RGB24 == dst_fmt ) )
	{	
		int sw = oex::CImage::GetScanWidth( width, 24 );
		int sz = sw * height;

		// Ensure source buffer is large enough
		if ( src->getUsed() < sz )
			return 0;

		// Ensure destination buffer is large enough
		if ( dst->getUsed() < sz )
		{	if ( !dst->Allocate( sz ) )
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
			return Rotate90_3( src->_Ptr(), dst->_Ptr(), width, height );

	} // end if

	// RGB32
	else if ( ( PIX_FMT_BGR32 == src_fmt || PIX_FMT_RGB32 == src_fmt )
			  && ( PIX_FMT_BGR32 == dst_fmt || PIX_FMT_RGB32 == dst_fmt ) )
	{	
		int sw = oex::CImage::GetScanWidth( width, 32 );
		int sz = sw * height;

		// Ensure source buffer is large enough
		if ( src->getUsed() < sz )
			return 0;

		// Ensure destination buffer is large enough
		if ( dst->getUsed() < sz )
		{	if ( !dst->Allocate( sz ) )
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
			return Rotate90_4( src->_Ptr(), dst->_Ptr(), width, height );

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
	int sdiff = sstride - ( width * 3 );
	int dstride = oex::CImage::GetScanWidth( height, 24 );
	int ddiff = dstride - ( height * 3 );
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
	int sdiff = sstride - ( width * 3 );
	int dstride = oex::CImage::GetScanWidth( height, 24 );
	int ddiff = dstride - ( height * 3 );
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
	unsigned int *d = (unsigned int*)dst + height - 1;
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
