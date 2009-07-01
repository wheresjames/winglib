// ff_decoder.cpp

#include "stdafx.h"

extern "C" void assert(int expression)
{
}


CFfDecoder::CFfDecoder()
{
	m_pCodec = oexNULL;
	m_pCodecContext = oexNULL;

	// Initialize ffmpeg	
    avcodec_init();

	// Register all available codecs
    avcodec_register_all();	

}

void CFfDecoder::Destroy()
{

}

int CFfDecoder::Create( int x_nCodec )
{
	// Lose previous codec
	Destroy();
	
	m_pCodec = avcodec_find_decoder( (CodecID)x_nCodec );
	if ( !m_pCodec )
		return 0;
	
	
	return 1;
}


