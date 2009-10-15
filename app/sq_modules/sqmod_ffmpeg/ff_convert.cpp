// ff_convert.cpp

#include "stdafx.h"

CFfConvert::CFfConvert()
{
}

int CFfConvert::CalcImageSize( int fmt, int width, int height )
{
	return avpicture_get_size( fmt, width, height );
}

int CFfConvert::FillAVFrame( AVFrame *pAv, int fmt, int width, int height, void *buf )
{
	if ( !pAv )
		return 0;

	switch( fmt )
	{
		case PIX_FMT_RGB24 :

oexSHOW( width );
oexSHOW( height );

			pAv->data[ 0 ] = pAv->data[ 1 ] =  pAv->data[ 2 ] = (uint8_t*)buf;
			pAv->base[ 0 ] = pAv->base[ 1 ] =  pAv->base[ 2 ] = (uint8_t*)buf;
			pAv->linesize[ 0 ] = pAv->linesize[ 1 ] = pAv->linesize[ 2 ] = oex::CImage::GetScanWidth( width, 24 );
			pAv->linesize[ 3 ] = 0;
			pAv->data[ 3 ] = pAv->base[ 3 ] = 0;
			break;

		case PIX_FMT_YUV420P :
			pAv->linesize[ 0 ] = width;
			pAv->linesize[ 1 ] = width / 4;
			pAv->linesize[ 2 ] = width / 4;
			pAv->data[ 0 ] = (uint8_t*)buf;
			pAv->data[ 1 ] = pAv->data[ 0 ] + ( pAv->linesize[ 0 ] * height );
			pAv->data[ 0 ] = pAv->data[ 1 ] + ( pAv->linesize[ 1 ] * height );
			pAv->base[ 0 ] = pAv->data[ 0 ];
			pAv->base[ 1 ] = pAv->data[ 1 ];
			pAv->base[ 2 ] = pAv->data[ 2 ];
			pAv->linesize[ 3 ] = 0;
			pAv->data[ 3 ] = pAv->base[ 3 ] = 0;
			break;

	} // end switch

	return 1;
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

	AVFrame avSrc, avDst;

	if ( !FillAVFrame( &avSrc, src_fmt, width, height, src ) )
		return 0;

	if ( !FillAVFrame( &avDst, dst_fmt, width, height, dst ) )
		return 0;

	// Create conversion
	SwsContext *psc = sws_getContext( width, height, (PixelFormat)src_fmt,
									  width, height, (PixelFormat)dst_fmt,
									  alg, NULL, NULL, NULL);
	if ( !psc )
		return 0;

	sws_scale( psc, avSrc.data, avSrc.linesize, 0, height,
			   avDst.data, avDst.linesize );

	dst->setUsed( nSize );

	return 1;
}

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

	// How big is the destination image?
	int nSize = CalcImageSize( dst_fmt, width, height );
	if ( !nSize )
		return 0;

oexSHOW( width );
oexSHOW( height );
oexSHOW( nSize );

	// Allocate memory for destinatino image
	if ( dst->Size() < nSize && !dst->Obj().OexNew( nSize ).Ptr() )
		return 0;

	AVFrame avSrc, avDst;

	if ( !FillAVFrame( &avSrc, PIX_FMT_RGB24, width, height, img->Obj().GetBits() ) )
		return 0;

	if ( !FillAVFrame( &avDst, dst_fmt, width, height, dst ) )
		return 0;

oexM();

	// Create conversion
	SwsContext *psc = sws_getContext( width, height, PIX_FMT_RGB24,
									  width, height, (PixelFormat)dst_fmt,
									  alg, NULL, NULL, NULL);
	if ( !psc )
		return 0;

oexM();

	sws_scale( psc, avSrc.data, avSrc.linesize, 0, height,
			   avDst.data, avDst.linesize );

	dst->setUsed( nSize );

oexM();

	sws_freeContext( psc );

oexM();

	return 1;
}
