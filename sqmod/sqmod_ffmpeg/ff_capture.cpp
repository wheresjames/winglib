// ff_container.cpp

#include "stdafx.h"

CFfCapture::CFfCapture()
{_STT();
	m_pFormatContext = oexNULL;
	m_pCodecContext = oexNULL;
	m_pFrame = oexNULL;
	m_nFrames = 0;
	m_nVideoStream = -1;
	m_bKeyRxd = 0;
	oexZero( m_pkt );
}

void CFfCapture::Destroy()
{_STT();
	if ( m_pkt.data )
		av_free_packet( &m_pkt );

	if ( m_pCodecContext )
		avcodec_close( m_pCodecContext );

	if ( m_pFormatContext )
		av_close_input_file( m_pFormatContext );

	m_pFormatContext = oexNULL;
	m_pCodecContext = oexNULL;
	m_pFrame = oexNULL;
	m_nFrames = 0;
	m_nVideoStream = -1;
	m_bKeyRxd = 0;
	oexZero( m_pkt );
}

int CFfCapture::Open( const sqbind::stdString &sFormat, const sqbind::stdString &sDevice, sqbind::CSqMulti *m )
{_STT();
	// Lose old container
	Destroy();

//	AVInputFormat *p = 0;
//	while ( p = av_iformat_next(p) )
//		printf( "%s: %s:\n", p->name, p->long_name );

	// Did we get a valid string?
	if ( !sFormat.length() || !sDevice.length() )
		return 0;

	AVFormatParameters fp; oexZero( fp );
	fp.channel = 0;
	fp.standard = 0;
	fp.width = 320;
	fp.height = 240;
	fp.pix_fmt = PIX_FMT_NONE; //PIX_FMT_YUV420P;
	fp.time_base.num = 15;
	fp.time_base.den = 1;
//	fp.device = sDevice.c_str();
	fp.channels = 0;

	AVInputFormat *pFormat = av_find_input_format( oexStrToMbPtr( sFormat.c_str() ) );
	if ( !pFormat )
	{	oexERROR( 0, oexMks( oexT( "av_find_input_format() failed : " ), sFormat.c_str() ) );
		Destroy();
		return 0;
	} // end if

	// Attempt to open it
	int res = av_open_input_file( &m_pFormatContext, oexStrToMbPtr( sDevice.c_str() ),
								  pFormat, 0, &fp );
	if ( 0 > res )
	{	oexERROR( res, oexT( "av_open_input_file() failed" ) );
		Destroy();
		return 0;
	} // end if

	res = av_find_stream_info( m_pFormatContext );
	if ( 0 > res )
	{	oexERROR( res, oexT( "av_find_stream_info() failed" ) );
		Destroy();
		return 0;
	} // end if

	if ( m )
	{
		(*m)[ oexT( "filename" ) ].set( oexMbToStrPtr( m_pFormatContext->filename ) );
		(*m)[ oexT( "timestamp" ) ].set( oexMks( m_pFormatContext->timestamp ).Ptr() );
		(*m)[ oexT( "title" ) ].set( oexMbToStrPtr( m_pFormatContext->title ) );
		(*m)[ oexT( "author" ) ].set( oexMbToStrPtr( m_pFormatContext->author ) );
		(*m)[ oexT( "copyright" ) ].set( oexMbToStrPtr( m_pFormatContext->copyright ) );
		(*m)[ oexT( "comment" ) ].set( oexMbToStrPtr( m_pFormatContext->comment ) );
		(*m)[ oexT( "album" ) ].set( oexMbToStrPtr( m_pFormatContext->album ) );
		(*m)[ oexT( "year" ) ].set( oexMks( m_pFormatContext->year ).Ptr() );
		(*m)[ oexT( "track" ) ].set( oexMks( m_pFormatContext->track ).Ptr() );
		(*m)[ oexT( "genre" ) ].set( oexMbToStrPtr( m_pFormatContext->genre ) );
		(*m)[ oexT( "ctx_flags" ) ].set( oexMks( m_pFormatContext->ctx_flags ).Ptr() );
		(*m)[ oexT( "start_time" ) ].set( oexMks( m_pFormatContext->start_time ).Ptr() );
		(*m)[ oexT( "duration" ) ].set( oexMks( m_pFormatContext->duration ).Ptr() );
		(*m)[ oexT( "file_size" ) ].set( oexMks( m_pFormatContext->file_size ).Ptr() );
		(*m)[ oexT( "bit_rate" ) ].set( oexMks( m_pFormatContext->bit_rate ).Ptr() );
		(*m)[ oexT( "index_built" ) ].set( oexMks( m_pFormatContext->index_built ).Ptr() );
		(*m)[ oexT( "mux_rate" ) ].set( oexMks( m_pFormatContext->mux_rate ).Ptr() );
		(*m)[ oexT( "packet_size" ) ].set( oexMks( m_pFormatContext->packet_size ).Ptr() );
		(*m)[ oexT( "preload" ) ].set( oexMks( m_pFormatContext->preload ).Ptr() );
		(*m)[ oexT( "max_delay" ) ].set( oexMks( m_pFormatContext->max_delay ).Ptr() );
		(*m)[ oexT( "loop_output" ) ].set( oexMks( m_pFormatContext->loop_output ).Ptr() );
		(*m)[ oexT( "loop_input" ) ].set( oexMks( m_pFormatContext->loop_input ).Ptr() );
		(*m)[ oexT( "probesize" ) ].set( oexMks( m_pFormatContext->probesize ).Ptr() );
		(*m)[ oexT( "max_analyze_duration" ) ].set( oexMks( m_pFormatContext->max_analyze_duration ).Ptr() );
		(*m)[ oexT( "keylen" ) ].set( oexMks( m_pFormatContext->keylen ).Ptr() );

	} // end if

	// Find audio and video stream indices
	for ( unsigned int i = 0; i < m_pFormatContext->nb_streams; i++ )
		if ( CODEC_TYPE_VIDEO == m_pFormatContext->streams[ i ]->codec->codec_type )
			m_nVideoStream = i;
//		else if ( CODEC_TYPE_AUDIO == m_pFormatContext->streams[ i ]->codec->codec_type )
//			m_nAudioStream = i;

	// Did we find a stream?
	if ( 0 > m_nVideoStream /*&& 0 > m_nAudioStream */ )
	{	oexERROR( 0, oexT( "file contains no video or audio streams" ) );
		Destroy();
		return 0;
	} // end if

	// Video stream
	if ( 0 <= m_nVideoStream
	     && m_pFormatContext->streams[ m_nVideoStream ]
		 &&  m_pFormatContext->streams[ m_nVideoStream ]->codec )
	{
		m_pCodecContext = m_pFormatContext->streams[ m_nVideoStream ]->codec;
//		m_pCodecContext->workaround_bugs |= FF_BUG_AUTODETECT;
//		m_pCodecContext->error_concealment = FF_EC_GUESS_MVS;
//		m_pCodecContext->error_recognition = FF_ER_CAREFUL;
		AVCodec *pCodec = avcodec_find_decoder( m_pCodecContext->codec_id );
		if ( !pCodec )
			m_pCodecContext = oexNULL;

		else if ( 0 > avcodec_open( m_pCodecContext, pCodec ) )
				m_pCodecContext = oexNULL;

	} // end if

//	dump_format( m_pFormatContext, 0, 0, 0 );

	return 1;
}

int CFfCapture::ReadFrame( sqbind::CSqBinary *dat, sqbind::CSqMulti *m )
{_STT();
	if ( !m_pFormatContext )
		return -1;

	int res = av_read_frame( m_pFormatContext, &m_pkt );
	if ( res )
		return -1;

	if ( m )
	{
		(*m)[ oexT( "flags" ) ].set( oexMks( m_pkt.flags ).Ptr() );
		(*m)[ oexT( "size" ) ].set( oexMks( m_pkt.size ).Ptr() );
		(*m)[ oexT( "stream_index" ) ].set( oexMks( m_pkt.stream_index ).Ptr() );
		(*m)[ oexT( "pos" ) ].set( oexMks( m_pkt.pos ).Ptr() );
		(*m)[ oexT( "dts" ) ].set( oexMks( m_pkt.dts ).Ptr() );
		(*m)[ oexT( "pts" ) ].set( oexMks( m_pkt.pts ).Ptr() );
		(*m)[ oexT( "duration" ) ].set( oexMks( m_pkt.duration ).Ptr() );
//		(*m)[ oexT( "convergence_duration" ) ].set( oexMks( pkt.convergence_duration ).Ptr() );

	} // end if

	// Waiting key frame?
	if ( !m_bKeyRxd )
	{	if ( 0 == ( m_pkt.flags & PKT_FLAG_KEY ) )
			return -1;
		m_bKeyRxd = 1;
	} // end if

	if ( dat )
		dat->setBuffer( (sqbind::CSqBinary::t_byte*)m_pkt.data, m_pkt.size, 0 );

	return m_pkt.stream_index;
}
/*
int CFfCapture::DecodeFrame( int stream, int fmt, sqbind::CSqBinary *dat, sqbind::CSqMulti *m )
{_STT();
	// Read a frame from the packet
	int res = -1;

	oexPrintf( oexMks( "\r",  m_nFrames, " : " ).Ptr() );

	do
	{
		// Read frames from input stream
		res = ReadFrame( oexNULL, m );
		if ( 0 > res )
			return res;

	} while ( res != stream );

	oexPrintf( oexMks( m_pkt.flags, " : ", m_pkt.size, " : ", m_buf.getUsed(), "    " ).Ptr() );
	oex::os::CSys::Flush_stdout();

	if ( 0 != ( m_pkt.flags & PKT_FLAG_KEY ) )
		oexEcho( " key frame" );
//	else if ( m_pkt.size > 2000 )
//		oexEcho( " large" );

	// Data left over from last time?
	if ( m_buf.getUsed() )
	{	m_buf.AppendBuffer( (sqbind::CSqBinary::t_byte*)m_pkt.data, m_pkt.size );
		m_pkt.data = (uint8_t*)m_buf._Ptr();
		m_pkt.size = m_buf.getUsed();
	} // end if

	// Video only atm
	if ( !dat || stream != m_nVideoStream || !m_pCodecContext )
		return -1;

	if ( !m_pFrame )
		m_pFrame = avcodec_alloc_frame();
	if ( !m_pFrame )
		return -1;

#if defined( FFSQ_VIDEO2 )

	int gpp = 0;
	int used = avcodec_decode_video2( m_pCodecContext, m_pFrame, &gpp, &m_pkt );
	if ( 0 >= used )
	{	oexEcho( "!used" );
		return -1;
	} // end if

#else

	int gpp = 0;
	int used = avcodec_decode_video( m_pCodecContext, m_pFrame, &gpp, m_pkt.data, m_pkt.size );
	if ( 0 >= used )
	{	oexEcho( "!used" );
		return -1;
	} // end if

#endif

/ *
	// Left over data?
	if ( used < m_pkt.size )
	{
		if ( m_buf.getUsed() )
			m_buf.LShift( used );
		else
			m_buf.AppendBuffer( &m_pkt.data[ used ], m_pkt.size - used );

	} // end if
	else
		m_buf.setUsed( 0 );
* /

	if ( !gpp )
	{	oexEcho( "!gpp" );
		return -1;
	} // end if

	// Is it already the right format?
	if ( fmt == (int)m_pCodecContext->pix_fmt )
	{	int nSize = CFfConvert::CalcImageSize( fmt, m_pCodecContext->width, m_pCodecContext->height );
		dat->setBuffer( (sqbind::CSqBinary::t_byte*)m_pFrame->data[ 0 ], nSize, 0 );
		m_nFrames++;
		return m_pkt.stream_index;
	} // end if

	// Do colorspace conversion
	if ( !CFfConvert::ConvertColorFB( m_pFrame, m_pCodecContext->pix_fmt,
									  m_pCodecContext->width, m_pCodecContext->height,
									  fmt, dat, SWS_FAST_BILINEAR ) )
		return -1;

	// Frame
	m_nFrames++;

	return m_pkt.stream_index;
}
*/

