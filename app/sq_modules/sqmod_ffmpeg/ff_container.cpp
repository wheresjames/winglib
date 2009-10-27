// ff_container.cpp

#include "stdafx.h"

CFfContainer::CFfContainer()
{
	m_pFormatContext = oexNULL;
	m_nVideoStream = -1;
	m_nAudioStream = -1;
	m_nWrite = 0;
	m_nRead = 0;
}

void CFfContainer::Destroy()
{
	if ( m_nRead )
	{
		if ( m_pFormatContext )
			av_close_input_file( m_pFormatContext );
	} // end if

	else if ( m_nWrite )
	{
		if ( m_pFormatContext )
		{
			// Write the rest of the file
			av_write_trailer( m_pFormatContext );

			// Free streams
			for ( unsigned int i = 0; i < m_pFormatContext->nb_streams; i++ )
				if (  m_pFormatContext->streams[ i ] )
					av_freep( m_pFormatContext->streams[ i ] );

			// Close file / socket resources
			if ( m_pFormatContext->oformat
				 && !( m_pFormatContext->oformat->flags & AVFMT_NOFILE )
				 && m_pFormatContext->pb )
				url_fclose( m_pFormatContext->pb );

			// Free the stream
			av_free( m_pFormatContext );

		} // end if

	} // end else if

	m_pFormatContext = oexNULL;
	m_nVideoStream = -1;
	m_nAudioStream = -1;
	m_nWrite = 0;
	m_nRead = 0;
}

int CFfContainer::Open( const sqbind::stdString &sUrl )
{
	// Lose old container
	Destroy();

	// Did we get a valid string?
	if ( !sUrl.length() )
		return 0;

	// Attempt to open it
	int res = av_open_input_file( &m_pFormatContext, oexStrToMbPtr( sUrl.c_str() ),
								  oexNULL, 0, oexNULL );
	if ( res )
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

	// Find audio and video stream indices
	for ( unsigned int i = 0; i < m_pFormatContext->nb_streams; i++ )
		if ( CODEC_TYPE_VIDEO == m_pFormatContext->streams[i]->codec->codec_type )
			m_nVideoStream = i;
		else if ( CODEC_TYPE_AUDIO == m_pFormatContext->streams[i]->codec->codec_type )
			m_nAudioStream = i;

	// Did we find a stream?
	if ( 0 > m_nVideoStream && 0 > m_nAudioStream )
	{	oexERROR( 0, oexT( "file contains no video or audio streams" ) );
		Destroy();
		return 0;
	} // end if

	// Reading
	m_nRead = 1;

//	dump_format( m_pFormatContext, 0, 0, 0 );

	return 1;
}

int CFfContainer::ReadFrame( sqbind::CSqBinary *dat, CFfFrameInfo *fi )
{
	if ( !m_pFormatContext )
		return -1;

	if ( !m_nRead )
		return -1;

	AVPacket pkt;

	int res = av_read_frame( m_pFormatContext, &pkt );
	if ( res )
		return -1;

	if ( fi )
	{
		fi->setKeyFrame( 0 != ( pkt.flags & PKT_FLAG_KEY ) );

	} // end if

	dat->setBuffer( pkt.data, pkt.size );

	return pkt.stream_index;
}

int CFfContainer::Create( const sqbind::stdString &sUrl, const sqbind::stdString &sType )
{
	Destroy();

	// Writing
	m_nWrite = 1;

	if ( !sUrl.length() )
		return 0;

	// What type of file?
	AVOutputFormat *pOut = 0;

	if ( sType.length() )
		pOut = guess_format( sType.c_str(), 0, 0 );
	else
		pOut = guess_format( 0, sUrl.c_str(), 0 );

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
	oexStrCpySz( m_pFormatContext->filename, sUrl.c_str() );

	return 1;
}

int CFfContainer::InitWrite()
{
	if ( !m_pFormatContext || !m_pFormatContext->oformat )
		return 0;

	if ( !m_nWrite )
		return 0;

	int res = 0;
	if ( 0 > ( res = av_set_parameters( m_pFormatContext, oexNULL ) ) )
	{	oexERROR( res, oexT( "av_set_parameters() failed" ) );
		m_nWrite = 0;
		Destroy();
		return 0;
	} // end if

	if ( !( m_pFormatContext->oformat->flags & AVFMT_NOFILE ) )
		if ( 0 > ( res = url_fopen( &m_pFormatContext->pb, m_pFormatContext->filename, URL_WRONLY ) ) )
		{	oexERROR( res, oexT( "url_fopen() failed" ) );
			m_nWrite = 0;
			Destroy();
			return 0;
		} // end if

    if ( 0 > ( res = av_write_header( m_pFormatContext ) ) )
	{	oexERROR( res, oexT( "av_write_header() failed" ) );
		Destroy();
		return 0;
	} // end if

	return 1;
}

int CFfContainer::AddVideoStream( int codec_id, int width, int height, int fps )
{
	if ( !m_pFormatContext )
		return -1;

	if ( !m_nWrite )
		return -1;

	if ( 0 <= m_nVideoStream )
		return m_nVideoStream;

	AVStream *pst = av_new_stream( m_pFormatContext, 0 );
	if ( !pst )
		return -1;

	m_nVideoStream = 0;

    AVCodecContext *pcc = pst->codec;
	if ( !pcc )
		return -1;

	// Fill in codec info
	pcc->codec_id = (CodecID)codec_id;
	pcc->codec_type = CODEC_TYPE_VIDEO;
	pcc->bit_rate = 400000;
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

int CFfContainer::WriteFrame( sqbind::CSqBinary *dat, CFfFrameInfo *fi )
{
	if ( !m_pFormatContext )
		return 0;

	if ( !m_nWrite )
		return 0;

	if ( 0 > m_nVideoStream )
		return 0;

	AVStream *pStream = m_pFormatContext->streams[ m_nVideoStream ];
	if ( !pStream )
		return 0;

	AVCodecContext *pCodec = pStream->codec;
	if ( !pCodec )
		return 0;

	AVPacket pkt;
	av_init_packet( &pkt );

	if ( pCodec->coded_frame )
		pkt.pts = pCodec->coded_frame->pts;
//	pkt.flags |= PKT_FLAG_KEY;

	if ( fi && fi->getKeyFrame() )
		pkt.flags |= PKT_FLAG_KEY;

	pkt.stream_index = pStream->index;
	pkt.data = dat->_Ptr();
	pkt.size = dat->getUsed();

	if ( av_write_frame( m_pFormatContext, &pkt ) )
		return 0;

	return 1;
}

