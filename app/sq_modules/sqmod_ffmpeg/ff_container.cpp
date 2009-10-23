// ff_container.cpp

#include "stdafx.h"

CFfContainer::CFfContainer()
{
	m_pFormatContext = oexNULL;
	m_nVideoStream = -1;
	m_nAudioStream = -1;
}

void CFfContainer::Destroy()
{
	if ( m_pFormatContext )
	{	av_close_input_file( m_pFormatContext );
		m_pFormatContext = oexNULL;
	} // end if

	m_nVideoStream = -1;
	m_nAudioStream = -1;
}

int CFfContainer::Open( const sqbind::stdString &sUrl )
{
	// Lose old container
	Destroy();

	// Did we get a valid string?
	if ( !sUrl.length() )
		return 0;
/*
	if ( !oexExists( sUrl.c_str() ) )
	{	oexERROR( 0, oexMks( oexT( "File not found : " ), sUrl.c_str() ) );
		Destroy();
		return 0;
	} // end if
*/
	// Attempt to open it
	int res = av_open_input_file( &m_pFormatContext, oexStrToMbPtr( sUrl.c_str() ),
								  oexNULL, 0, oexNULL );
	if ( res )
	{	oexERROR( 0, oexT( "av_open_input_file() failed" ) );
		Destroy();
		return 0;
	} // end if

	res = av_find_stream_info( m_pFormatContext );
	if ( 0 > res )
	{	oexERROR( 0, oexT( "av_find_stream_info() failed" ) );
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

//	dump_format( m_pFormatContext, 0, 0, 0 );

	return 1;
}

int CFfContainer::ReadFrame( sqbind::CSqBinary *dat )
{
	if ( !m_pFormatContext )
		return -1;

	AVPacket pkt;

	int res = av_read_frame( m_pFormatContext, &pkt );
	if ( res )
		return -1;

	dat->setBuffer( pkt.data, pkt.size );

	return pkt.stream_index;
}
