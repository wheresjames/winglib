// ff_convert.cpp

#include "stdafx.h"

CFfConvert::CFfConvert()
{
}

int CFfConvert::CalcImageSize( int fmt, int width, int height )
{
	return avpicture_get_size( (PixelFormat)fmt, width, height );
}

int CFfConvert::FillAVPicture( AVPicture *pPic, int fmt, int width, int height, void *buf )
{
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
{
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
{
	if ( !dst )
		return 0;

	dst->setUsed( 0 );

	if ( 0 >= width || 0 >= height )
		return 0;

	if ( !src || !src->getUsed() )
		return 0;

	// How big is the destination image?
	int nSize = CalcImageSize( dst_fmt, width, height );
	if ( !nSize )
		return 0;

	// Allocate memory for destinatino image
	if ( dst->Size() < nSize && !dst->Obj().OexNew( nSize ).Ptr() )
		return 0;

	// Fill in picture data
	AVPicture apSrc, apDst;
	if ( !FillAVPicture( &apSrc, src_fmt, width, height, src )
	     || !FillAVPicture( &apDst, dst_fmt, width, height, dst ) )
		return 0;

	// Create conversion
	SwsContext *psc = sws_getContext( width, height, (PixelFormat)src_fmt,
									  width, height, (PixelFormat)dst_fmt,
									  alg, NULL, NULL, NULL);
	if ( !psc )
		return 0;

	sws_scale( psc, apSrc.data, apSrc.linesize, 0, height,
			   apDst.data, apDst.linesize );

	dst->setUsed( nSize );

	return 1;
}

int img_convert ( AVPicture *, int, const AVPicture *, int, int, int );

int CFfConvert::ConvertColorIB( sqbind::CSqImage *img, sqbind::CSqBinary *dst, int dst_fmt, int alg )
{
	if ( !dst )
		return 0;

	dst->setUsed( 0 );

	if ( !img )
		return 0;

	int width = img->getWidth();
	int height = img->getHeight();
	if ( !img->Obj().GetBits() || 0 >= width || 0 >= height )
		return 0;

oexSHOW( width );
oexSHOW( height );

	// How big is the destination image?
	int nSize = CalcImageSize( dst_fmt, width, height );
	if ( !nSize )
		return 0;

oexSHOW( nSize );

	// Allocate memory for destination image
	if ( dst->Size() < nSize && !dst->Obj().OexNew( nSize * 2 ).Ptr() )
		return 0;

oexSHOW( dst->Obj().Size() );
oexSHOW( (int)dst->Obj().Ptr() );

	// Fill in picture data
	AVPicture apSrc, apDst;
	if ( !FillAVPicture( &apSrc, PIX_FMT_RGB24, width, height, img->Obj().GetBits() )
	     || !FillAVPicture( &apDst, dst_fmt, width, height, dst ) )
		return 0;

/*	More overhead has been added to colorspace conversion

	// Convert the image
	if ( img_convert( &apDst, dst_fmt, &apSrc, PIX_FMT_RGB24, width, height ) )
		return 0;

	// Save the number of bytes used in the output buffer
	dst->setUsed( nSize );

	return 1;
*/


oexM();

	// Create conversion
	SwsContext *psc = sws_getContext( width, height, PIX_FMT_RGB24,
									  width, height, (PixelFormat)dst_fmt,
									  alg, NULL, NULL, NULL);
	if ( !psc )
		return 0;

oexM();

	for ( int i = 0; i < 4; i++ )
		oexSHOW( i ),
		oexSHOW( (int)apSrc.data[ i ] ),
		oexSHOW( (int)apSrc.linesize[ i ] ),
		oexSHOW( (int)apDst.data[ i ] ),
		oexSHOW( (int)apDst.linesize[ i ] );


	// http://gnunet.org/libextractor/doxygen/html/swscale-example_8c.html

//	int nRet = sws_scale( psc, apSrc.data, apSrc.linesize, 0, height,
//			   				   apDst.data, apDst.linesize );
	int nRet = height;

oexM();

oexSHOW( nRet );

//	sws_freeContext( psc );

	if ( 0 >= nRet )
		return 0;

oexM();

	dst->setUsed( nSize );

	return 1;
}
