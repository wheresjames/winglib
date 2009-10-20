// ff_decoder.cpp

#include "stdafx.h"


// av_open_input_file av_find_stream_info

CFfDecoder::CFfDecoder()
{
	m_pCodec = oexNULL;
	m_pCodecContext = oexNULL;

	// Register codecs
	av_register_all();

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


