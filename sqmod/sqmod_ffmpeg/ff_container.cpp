// ff_container.cpp

// http://dranger.com/ffmpeg/functions.html

#include "stdafx.h"

CFfContainer::CFfContainer()
{_STT();

	m_pFormatContext = oexNULL;
	m_pCodecContext = oexNULL;
	m_pFrame = oexNULL;
	m_nVideoStream = -1;

	m_pAudioCodecContext = oexNULL;
	m_nAudioStream = -1;

	m_nWrite = 0;
	m_nRead = 0;
	m_bKeyRxd = 0;
	m_nFrames = 0;
	m_nAudioFrames = 0;
	oexZero( m_pkt );
}

void CFfContainer::Destroy()
{_STT();

	oexAutoLock ll( _g_ffmpeg_lock );
	if ( !ll.IsLocked() ) return;

	if ( m_pkt.data )
		av_free_packet( &m_pkt );

	if ( m_pCodecContext )
		avcodec_close( m_pCodecContext );

	if ( m_pAudioCodecContext )
		avcodec_close( m_pAudioCodecContext );

	CloseStream();

	m_pCodecContext = oexNULL;
	m_pFrame = oexNULL;
	m_nVideoStream = -1;
	m_buf.Free();
	oexZero( m_pkt );
	m_nFrames = 0;

	m_pAudioCodecContext = oexNULL;
	m_nAudioStream = -1;
	m_nAudioFrames = 0;

	m_nWrite = 0;
	m_nRead = 0;
	m_bKeyRxd = 0;
}

int CFfContainer::CloseStream()
{_STT();

	oexAutoLock ll( _g_ffmpeg_lock );
	if ( !ll.IsLocked() ) return 0;

	if ( !m_pFormatContext )
		return 0;

		if ( m_nRead )
		av_close_input_file( m_pFormatContext );

	else if ( 1 < m_nWrite )
	{
		// Write the rest of the file
		av_write_trailer( m_pFormatContext );

//	av_write_trailer() does this now
/*
		// Free streams
		for ( unsigned int i = 0; i < m_pFormatContext->nb_streams; i++ )
			if ( m_pFormatContext->streams[ i ] )
			{	if ( m_pFormatContext->streams[ i ]->codec )
					av_freep( m_pFormatContext->streams[ i ]->codec );
				av_freep( m_pFormatContext->streams[ i ] );
				m_pFormatContext->streams[ i ] = 0;
			} // end if
*/

		// Close file / socket resources
		if ( m_pFormatContext->oformat
			 && !( m_pFormatContext->oformat->flags & AVFMT_NOFILE )
			 && m_pFormatContext->pb )
			avio_close( m_pFormatContext->pb );
			
		// Free the stream
		av_free( m_pFormatContext );

	} // end else if

	m_pFormatContext = oexNULL;

	return 1;
}


int CFfContainer::Open( const sqbind::stdString &sUrl, sqbind::CSqMulti *m )
{_STT();

	// Lose old container
	Destroy();

	// Did we get a valid string?
	if ( !sUrl.length() )
		return 0;

	oexAutoLock ll( _g_ffmpeg_lock );
	if ( !ll.IsLocked() ) return 0;

//	AVFormatParameters	fp;
//	oexZero( fp );
//	fp.initial_pause = 1;

	// Attempt to open it
	int res = av_open_input_file( &m_pFormatContext, oexStrToMbPtr( sUrl.c_str() ),
								  oexNULL, 0, oexNULL );
	if ( res )
	{	oexERROR( res, oexT( "av_open_input_file() failed" ) );
		Destroy();
		return 0;
	} // end if

    // +++ Holy crap this function can be slow
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
//		(*m)[ oexT( "title" ) ].set( oexMbToStrPtr( m_pFormatContext->title ) );
//		(*m)[ oexT( "author" ) ].set( oexMbToStrPtr( m_pFormatContext->author ) );
//		(*m)[ oexT( "copyright" ) ].set( oexMbToStrPtr( m_pFormatContext->copyright ) );
//		(*m)[ oexT( "comment" ) ].set( oexMbToStrPtr( m_pFormatContext->comment ) );
//		(*m)[ oexT( "album" ) ].set( oexMbToStrPtr( m_pFormatContext->album ) );
//		(*m)[ oexT( "year" ) ].set( oexMks( m_pFormatContext->year ).Ptr() );
//		(*m)[ oexT( "track" ) ].set( oexMks( m_pFormatContext->track ).Ptr() );
//		(*m)[ oexT( "genre" ) ].set( oexMbToStrPtr( m_pFormatContext->genre ) );
		(*m)[ oexT( "ctx_flags" ) ].set( oexMks( m_pFormatContext->ctx_flags ).Ptr() );
		(*m)[ oexT( "start_time" ) ].set( oexMks( m_pFormatContext->start_time ).Ptr() );
		(*m)[ oexT( "duration" ) ].set( oexMks( m_pFormatContext->duration ).Ptr() );
		(*m)[ oexT( "file_size" ) ].set( oexMks( m_pFormatContext->file_size ).Ptr() );
		(*m)[ oexT( "bit_rate" ) ].set( oexMks( m_pFormatContext->bit_rate ).Ptr() );
//		(*m)[ oexT( "index_built" ) ].set( oexMks( m_pFormatContext->index_built ).Ptr() );
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
		if ( AVMEDIA_TYPE_VIDEO == m_pFormatContext->streams[ i ]->codec->codec_type )
			m_nVideoStream = i;
		else if ( AVMEDIA_TYPE_AUDIO == m_pFormatContext->streams[ i ]->codec->codec_type )
			m_nAudioStream = i;

	// Did we find a stream?
	if ( 0 > m_nVideoStream && 0 > m_nAudioStream )
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
		AVCodec *pCodec = avcodec_find_decoder( m_pCodecContext->codec_id );
		if ( !pCodec )
			m_pCodecContext = oexNULL;

		else
		{
			m_pCodecContext->workaround_bugs |= FF_BUG_AUTODETECT;
			m_pCodecContext->error_concealment = FF_EC_GUESS_MVS;
			m_pCodecContext->error_recognition = FF_ER_CAREFUL;
			if( pCodec->capabilities & CODEC_CAP_TRUNCATED )
				m_pCodecContext->flags |= CODEC_FLAG_TRUNCATED;

			if ( 0 > avcodec_open( m_pCodecContext, pCodec ) )
				m_pCodecContext = oexNULL;

		} // end else

	} // end if

	// Audio stream
	if ( 0 <= m_nAudioStream
	     && m_pFormatContext->streams[ m_nAudioStream ]
		 &&  m_pFormatContext->streams[ m_nAudioStream ]->codec )
	{
		m_pAudioCodecContext = m_pFormatContext->streams[ m_nAudioStream ]->codec;
		AVCodec *pCodec = avcodec_find_decoder( m_pAudioCodecContext->codec_id );
		if ( !pCodec )
			m_pAudioCodecContext = oexNULL;

		else
		{
			m_pAudioCodecContext->workaround_bugs |= FF_BUG_AUTODETECT;
			m_pAudioCodecContext->error_concealment = FF_EC_GUESS_MVS;
			m_pAudioCodecContext->error_recognition = FF_ER_CAREFUL;
			if( pCodec->capabilities & CODEC_CAP_TRUNCATED )
				m_pAudioCodecContext->flags |= CODEC_FLAG_TRUNCATED;

			if ( 0 > avcodec_open( m_pAudioCodecContext, pCodec ) )
				m_pAudioCodecContext = oexNULL;

		} // end if

	} // end if

	// Reading
	m_nRead = 1;

//	dump_format( m_pFormatContext, 0, 0, 0 );

	return 1;
}

int CFfContainer::ReadFrame( sqbind::CSqBinary *dat, sqbind::CSqMulti *m )
{_STT();

	if ( !m_pFormatContext )
		return -1;

	if ( !m_nRead )
		return -1;

	if ( m_pkt.data )
		av_free_packet( &m_pkt );
	oexZero( m_pkt );

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

	if ( dat )
		dat->setBuffer( (sqbind::CSqBinary::t_byte*)m_pkt.data, m_pkt.size, 0, 0 );

	return m_pkt.stream_index;
}

int CFfContainer::DecodeFrame( int stream, int fmt, sqbind::CSqBinary *dat, sqbind::CSqMulti *m, int flip )
{_STT();

	// Read a frame from the packet
	int res = -1;

	do
	{
		// Read frames from input stream
		res = ReadFrame( oexNULL, m );
		if ( 0 > res )
			return res;

	} while ( res != stream );

	// Waiting key frame?
	if ( !m_bKeyRxd )
	{	if ( 0 == ( m_pkt.flags & AV_PKT_FLAG_KEY ) )
			return -1;
		m_bKeyRxd = 1;
	} // end if

	// Video only atm
	if ( !dat || stream != m_nVideoStream || !m_pCodecContext )
		return -1;

	if ( !m_pFrame )
		m_pFrame = avcodec_alloc_frame();
	if ( !m_pFrame )
		return -1;

	int gpp = 0, used = 0;

#if defined( FFSQ_VIDEO2 )

	used = avcodec_decode_video2( m_pCodecContext, m_pFrame, &gpp, &m_pkt );
	if ( 0 >= used )
	{	oexSHOW( used );
		return -1;
	} // end if

#else

	used = avcodec_decode_video( m_pCodecContext, m_pFrame, &gpp, m_pkt.data, m_pkt.size );
	if ( 0 >= used )
	{	oexSHOW( used );
		return -1;
	} // end if

#endif

	if ( !gpp )
		return -1;

	// Is it already the right format?
	if ( fmt == (int)m_pCodecContext->pix_fmt )
	{	int nSize = CFfConvert::CalcImageSize( fmt, m_pCodecContext->width, m_pCodecContext->height );
		dat->setBuffer( (sqbind::CSqBinary::t_byte*)m_pFrame->data[ 0 ], nSize, 0, 0 );
		m_nFrames++;
		return m_pkt.stream_index;
	} // end if

	// Do colorspace conversion
	if ( !CFfConvert::ConvertColorFB( m_pFrame, m_pCodecContext->pix_fmt,
									  m_pCodecContext->width, m_pCodecContext->height,
									  fmt, dat, SWS_FAST_BILINEAR, flip ) )
		return -1;

	// Frame
	m_nFrames++;

	return m_pkt.stream_index;
}

int CFfContainer::DecodeFrameBin( sqbind::CSqBinary *in, int fmt, sqbind::CSqBinary *out, sqbind::CSqMulti *m, int flip )
{_STT();

	AVPacket pkt; oexZero( pkt );
	pkt.data = (uint8_t*)in->Ptr();
	pkt.size = in->getUsed();

/*
	m_buf.setUsed( 0 );

	// Ensure buffer size
	if ( ( m_buf.Size() - m_buf.getUsed() ) < (sqbind::CSqBinary::t_size)( pkt.size + FF_INPUT_BUFFER_PADDING_SIZE ) )
		m_buf.Allocate( 2 * ( m_buf.Size() + pkt.size + FF_INPUT_BUFFER_PADDING_SIZE ) );

	// Add new data
	m_buf.AppendBuffer( (sqbind::CSqBinary::t_byte*)pkt.data, pkt.size );

	// Create packet
	pkt.data = (uint8_t*)m_buf._Ptr();
	pkt.size = m_buf.getUsed();

	// Zero padding
	int nPadding = m_buf.Size() - m_buf.getUsed();
	if ( 0 < nPadding )
	{
		// Don't zero more than twice the padding size
		if ( nPadding > ( FF_INPUT_BUFFER_PADDING_SIZE * 2 ) )
			nPadding = FF_INPUT_BUFFER_PADDING_SIZE * 2;

		// Set end to zero to ensure no overreading on damaged blocks
		oexZeroMemory( &m_buf._Ptr()[ m_buf.getUsed() ], nPadding );

	} // end if
*/

	if ( !m_pFrame )
		m_pFrame = avcodec_alloc_frame();
	if ( !m_pFrame )
		return -1;

	int gpp = 0, used = 0;

#if defined( FFSQ_VIDEO2 )

	if ( m )
	{
		pkt.flags 					= (*m)[ oexT( "flags" ) ].toint();
		pkt.stream_index 			= (*m)[ oexT( "stream_index" ) ].toint();
		pkt.pos 					= (*m)[ oexT( "pos" ) ].toint64();
		pkt.dts 					= (*m)[ oexT( "dts" ) ].toint64();
		pkt.pts 					= (*m)[ oexT( "pts" ) ].toint64();
		pkt.duration 				= (*m)[ oexT( "duration" ) ].toint();
		pkt.convergence_duration 	= (*m)[ oexT( "convergence_duration" ) ].toint64();

	} // end if

	used = avcodec_decode_video2( m_pCodecContext, m_pFrame, &gpp, &pkt );
	if ( 0 >= used )
	{	oexSHOW( used );
		return -1;
	} // end if

#else

	used = avcodec_decode_video( m_pCodecContext, m_pFrame, &gpp, pkt.data, pkt.size );
	if ( 0 >= used )
	{	oexSHOW( used );
		return -1;
	} // end if

#endif

	if ( !gpp )
		return -1;

	// Is it already the right format?
	if ( fmt == (int)m_pCodecContext->pix_fmt )
	{	int nSize = CFfConvert::CalcImageSize( fmt, m_pCodecContext->width, m_pCodecContext->height );
		out->setBuffer( (sqbind::CSqBinary::t_byte*)m_pFrame->data[ 0 ], nSize, 0, 0 );
		m_nFrames++;
		return m_pkt.stream_index;
	} // end if

	// Do colorspace conversion
	if ( !CFfConvert::ConvertColorFB( m_pFrame, m_pCodecContext->pix_fmt,
									  m_pCodecContext->width, m_pCodecContext->height,
									  fmt, out, SWS_FAST_BILINEAR, flip ) )
		return -1;

	// Frame
	m_nFrames++;

	return 1;
}

int CFfContainer::DecodeAudioFrameBin( sqbind::CSqBinary *in, sqbind::CSqBinary *out, sqbind::CSqMulti *m )
{_STT();
/*
	if ( !in || !out )
		return 0;

	int in_size = in->getUsed();
	const uint8_t* in_ptr = (const uint8_t*)in->Ptr();
/ *
	m_audio_buf.setUsed( 0 );

	// Ensure buffer size
	if ( ( m_audio_buf.Size() - m_audio_buf.getUsed() ) < (sqbind::CSqBinary::t_size)( in_size + FF_INPUT_BUFFER_PADDING_SIZE ) )
		m_audio_buf.Allocate( 2 * ( m_audio_buf.Size() + in_size + FF_INPUT_BUFFER_PADDING_SIZE ) );

	// Add new data
	m_audio_buf.AppendBuffer( (sqbind::CSqBinary::t_byte*)in_ptr, in_size );
	in_ptr = (uint8_t*)m_audio_buf._Ptr();
	in_size = m_audio_buf.getUsed();

	// Zero padding
	int nPadding = m_audio_buf.Size() - m_audio_buf.getUsed();
	if ( 0 < nPadding )
	{
		// Don't zero more than twice the padding size
		if ( nPadding > ( FF_INPUT_BUFFER_PADDING_SIZE * 2 ) )
			nPadding = FF_INPUT_BUFFER_PADDING_SIZE * 2;

		// Set end to zero to ensure no overreading on damaged blocks
		oexZeroMemory( &m_audio_buf._Ptr()[ m_audio_buf.getUsed() ], nPadding );

	} // end if
*/
/*
	int out_size = oex::cmn::Max( (int)(in->getUsed() * 2), (int)AVCODEC_MAX_AUDIO_FRAME_SIZE );
	if ( (int)out->Size() < out_size )
		out->Allocate( out_size );

	int used = avcodec_decode_audio2( m_pAudioCodecContext,
									  (int16_t*)out->_Ptr(), &out_size,
									  (const uint8_t*)in_ptr, in_size );

	out->setUsed( out_size );

	if ( 0 >= used )
	{	oexSHOW( used );
		return -1;
	} // end if

//	if ( used != in->getUsed() )
//		oexSHOW( "!!! Left over data !!!" );

/ *
	// Left over data?
	if ( used < m_pkt.size )
	{
		if ( m_buf.getUsed() )
			m_buf.LShift( used );
		else
			m_buf.AppendBuffer( (sqbind::CSqBinary::t_byte*)&m_pkt.data[ used ], m_pkt.size - used );

	} // end if
	else
		m_buf.setUsed( 0 );
*/

	return 0;

	// Frame
	m_nAudioFrames++;

	return 1;
}


int CFfContainer::Create( const sqbind::stdString &sUrl, const sqbind::stdString &sType, sqbind::CSqMulti *m )
{_STT();
	Destroy();

	if ( !sUrl.length() )
		return 0;

	// What type of file?
	AVOutputFormat *pOut = 0;

	if ( sType.length() )
		pOut = av_guess_format( oexStrToMbPtr( sType.c_str() ), 0, 0 );
	else
		pOut = av_guess_format( 0, oexStrToMbPtr( sUrl.c_str() ), 0 );

	if ( !pOut )
	{	oexERROR( 0, oexT( "guess_format() failed" ) );
		Destroy();
		return 0;
	} // end if

	// Allocate format context
	m_pFormatContext = avformat_alloc_context();
	if ( !m_pFormatContext )
	{	oexERROR( 0, oexT( "avformat_alloc_context() failed" ) );
		Destroy();
		return 0;
	} // end if

	// Save format and file name
	m_pFormatContext->oformat = pOut;
	oexStrCpySz( m_pFormatContext->filename, oexStrToMbPtr( sUrl.c_str() ) );

	if ( m )
	{
		if ( m->isset( oexT( "flags" ) ) )
			m_pFormatContext->flags = (*m)[ oexT( "flags" ) ].toint();

		if ( m->isset( oexT( "max_analyze_duration" ) ) )
			m_pFormatContext->max_analyze_duration = (*m)[ oexT( "max_analyze_duration" ) ].toint();

		if ( m->isset( oexT( "probesize" ) ) )
			m_pFormatContext->probesize = (*m)[ oexT( "probesize" ) ].toint();

		if ( m->isset( oexT( "bit_rate" ) ) )
			m_pFormatContext->bit_rate = (*m)[ oexT( "bit_rate" ) ].toint();

		if ( m->isset( oexT( "loop_input" ) ) )
			m_pFormatContext->loop_input = (*m)[ oexT( "loop_input" ) ].toint();

		if ( m->isset( oexT( "loop_output" ) ) )
			m_pFormatContext->loop_output = (*m)[ oexT( "loop_output" ) ].toint();

	} // end if

	m_nWrite = 1;

/*
	int res = 0;
	if ( 0 > ( res = av_set_parameters( m_pFormatContext, oexNULL ) ) )
	{	oexERROR( res, oexT( "av_set_parameters() failed" ) );
		m_nWrite = 0;
		Destroy();
		return 0;
	} // end if
*/
	return 1;
}

int CFfContainer::InitWrite()
{_STT();
	if ( !m_pFormatContext || !m_pFormatContext->oformat )
		return 0;

	if ( !m_nWrite )
		return 0;

	int res = 0;
	if ( 0 > ( res = av_set_parameters( m_pFormatContext, oexNULL ) ) )
	{	oexERROR( res, oexT( "av_set_parameters() failed" ) );
		Destroy();
		return 0;
	} // end if

	// +++ Below Seems to memleak when avio_open fails

	if ( !( m_pFormatContext->oformat->flags & AVFMT_NOFILE ) )
		if ( 0 > ( res = avio_open( &m_pFormatContext->pb, m_pFormatContext->filename, URL_WRONLY ) ) )
		{	oexERROR( res, oexT( "avio_open() failed" ) );
			Destroy();
			return 0;
		} // end if

    if ( 0 > ( res = av_write_header( m_pFormatContext ) ) )
	{	oexERROR( res, oexT( "av_write_header() failed" ) );
		Destroy();
		return 0;
	} // end if

	// Writing
	m_nWrite = 2;

	dump_format( m_pFormatContext, 0, 0, 1 );
	
	return 1;
}

int CFfContainer::AddVideoStream( int codec_id, int width, int height, int fps )
{_STT();
	if ( !m_pFormatContext )
		return -1;

	if ( !m_nWrite )
		return -1;

	if ( 0 <= m_nVideoStream )
		return m_nVideoStream;

	AVStream *pst = av_new_stream( m_pFormatContext, 0 );
	if ( !pst )
		return -1;

	m_nVideoStream = pst->index;

    AVCodecContext *pcc = pst->codec;
	if ( !pcc )
		return -1;

	// Get defaults
	avcodec_get_context_defaults2( pcc, AVMEDIA_TYPE_VIDEO );

	// Fill in codec info
	pcc->codec_id = (CodecID)codec_id;
	pcc->codec_type = AVMEDIA_TYPE_VIDEO;
	pcc->bit_rate = 2000000;
	pcc->width = width;
	pcc->height = height;
    pcc->time_base.num = 1;
    pcc->time_base.den = fps;
	pcc->gop_size = 12;

	oex::CStr sFName( m_pFormatContext->oformat->name );
	if (  sFName == oexT( "3gp" ) || sFName == oexT( "mov" ) || sFName == oexT( "mp4" ) )
		pcc->flags |= CODEC_FLAG_GLOBAL_HEADER;

	return m_nVideoStream;
}

int CFfContainer::WriteVideoFrame( sqbind::CSqBinary *dat, SQInteger nPts, SQInteger nDts, sqbind::CSqMulti *m )
{_STT();
	if ( !m_pFormatContext )
		return 0;

	if ( !m_nWrite )
		return 0;

	if ( 0 > m_nVideoStream )
		return 0;

	AVStream *pStream = m_pFormatContext->streams[ m_nVideoStream ];
	if ( !pStream )
		return 0;

	AVPacket pkt;
	oexZero( pkt );
	av_init_packet( &pkt );

	if ( m )
	{	if ( m->isset( oexT( "flags" ) ) )
			pkt.flags = (*m)[ oexT( "flags" ) ].toint();
		else
			pkt.flags |= AV_PKT_FLAG_KEY;
	} // end if
	else
		pkt.flags |= AV_PKT_FLAG_KEY;

	pkt.pts = nPts;
	pkt.dts = nDts;
	
    AVCodecContext *pcc = pStream->codec;
    if ( pcc && pcc->coded_frame && pcc->coded_frame->pts != AV_NOPTS_VALUE )
        pkt.pts = av_rescale_q( pcc->coded_frame->pts, pcc->time_base, pStream->time_base );
	
	pkt.stream_index = pStream->index;
	pkt.data = (uint8_t*)dat->_Ptr();
	pkt.size = dat->getUsed();

	if ( 0 > m_nAudioStream )
	{	if ( av_write_frame( m_pFormatContext, &pkt ) )
			return 0;
	} // end if
	
	else if ( av_interleaved_write_frame( m_pFormatContext, &pkt ) )
		return 0;

	return 1;
}


int CFfContainer::AddAudioStream( int codec_id, int channels, int sample_rate, int bps )
{_STT();

	if ( !m_pFormatContext )
		return -1;

	if ( !m_nWrite )
		return -1;

	if ( 0 <= m_nAudioStream )
		return m_nAudioStream;

	AVStream *pst = av_new_stream( m_pFormatContext, 1 );
	if ( !pst )
		return -1;

	m_nAudioStream = pst->index;
	
    AVCodecContext *pcc = pst->codec;
	if ( !pcc )
		return -1;
	
	avcodec_get_context_defaults2( pcc, AVMEDIA_TYPE_AUDIO );
		
	// Fill in codec info
	pcc->codec_id = (CodecID)codec_id;
	pcc->codec_type = AVMEDIA_TYPE_AUDIO;
	
    pcc->channels = channels;
	pcc->sample_rate = sample_rate;
	pcc->bits_per_coded_sample = bps ? bps : av_get_bits_per_sample( pcc->codec_id );
    pcc->bit_rate = pcc->sample_rate * pcc->channels * 8;
//	pcc->sample_fmt = AV_SAMPLE_FMT_S16;	
//	pcc->block_align = pcc->bits_per_coded_sample * pcc->channels / 8;
//	pcc->frame_size = 4096;
//	pcc->time_base.num = 1;
//	pcc->time_base.den = sample_rate;
	
	// Set extra codec data
	if ( m_audio_extra.getUsed() )
	{	pcc->flags |= CODEC_FLAG_GLOBAL_HEADER;
		pcc->extradata = (uint8_t*)m_audio_extra._Ptr();
		pcc->extradata_size = m_audio_extra.getUsed();
	} // end if
	
    else if ( m_pFormatContext->oformat->flags & AVFMT_GLOBALHEADER )
        pcc->flags |= CODEC_FLAG_GLOBAL_HEADER;

	return m_nAudioStream;
}

int CFfContainer::WriteAudioFrame( sqbind::CSqBinary *dat, SQInteger nPts, SQInteger nDts, sqbind::CSqMulti *m )
{_STT();
	if ( !m_pFormatContext )
		return 0;

	if ( !m_nWrite )
		return 0;

	if ( 0 > m_nAudioStream )
		return 0;

	AVStream *pStream = m_pFormatContext->streams[ m_nAudioStream ];
	if ( !pStream )
		return 0;

	AVPacket pkt;
	oexZero( pkt );
	av_init_packet( &pkt );

	if ( m )
	{	if ( m->isset( oexT( "flags" ) ) )
			pkt.flags = (*m)[ oexT( "flags" ) ].toint();
		else
			pkt.flags |= AV_PKT_FLAG_KEY;
	} // end if
	else
		pkt.flags |= AV_PKT_FLAG_KEY;

	// av_rescale_q
	
	pkt.pts = nPts;
	pkt.dts = nDts;
	pkt.stream_index = pStream->index;
	pkt.data = (uint8_t*)dat->_Ptr();
	pkt.size = dat->getUsed();

	if ( 0 > m_nVideoStream )
	{	if ( av_write_frame( m_pFormatContext, &pkt ) )
			return 0;
	} // end if
	
	else if ( av_interleaved_write_frame( m_pFormatContext, &pkt ) )
		return 0;

	return 1;
}


int CFfContainer::WriteFrame( sqbind::CSqBinary *dat, sqbind::CSqMulti *m )
{_STT();
	return WriteVideoFrame( dat, 0, 0, m );
}

// http://dranger.com/ffmpeg/tutorial07.html

/// Seeks to the specified time
int CFfContainer::Seek( int nStreamId, int nOffset, int nFlags )
{
	if ( !m_pFormatContext )
		return 0;

	// Convert to uS offset
	oex::oexINT64 t;
	if ( 0 > nStreamId )
		t = nOffset * AV_TIME_BASE / 1000;

	else if ( !m_pFormatContext->streams[ nStreamId ] )
		return 0;

	else
	{	AVRational q = { 1, AV_TIME_BASE };
		t = av_rescale_q( nOffset * 1000, q, m_pFormatContext->streams[ m_nVideoStream ]->time_base );
	} // end else

	int res = av_seek_frame( m_pFormatContext, nStreamId, t, nFlags );

	return 0 <= res ? 1 : 0;
}
