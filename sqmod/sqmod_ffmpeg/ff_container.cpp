// ff_container.cpp

// http://dranger.com/ffmpeg/functions.html

#include "stdafx.h"

#ifndef OEX_NEWFF

extern "C"
{
	#include "libavformat/url.h"
};

#endif

// typedefs from avienc.c http://ffmpeg.org/doxygen/1.2/avienc_8c_source.html#l00039
// note that AVIStream is defined completely differently in avidec.c
typedef struct AVIIentry {
  unsigned int flags, pos, len;
} AVIIentry;

typedef struct AVIIndex {
  int64_t     indx_start;
  int         entry;
  int         ents_allocated;
  AVIIentry** cluster;
} AVIIndex;

typedef struct {
  int64_t riff_start, movi_list, odml_list;
  int64_t frames_hdr_all;
  int riff_id;
} AVIContext;

typedef struct  {
  int64_t frames_hdr_strm;
  int audio_strm_length;
  int packet_count;
  int entry;

  AVIIndex indexes;
} AVIStream ;


#define FFF_KEY_FRAME	AV_PKT_FLAG_KEY

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CFfContainer, CFfContainer )

	SQBIND_MEMBER_FUNCTION( CFfContainer, Open )
	SQBIND_MEMBER_FUNCTION( CFfContainer, isOpen )
	SQBIND_MEMBER_FUNCTION( CFfContainer, ReadFrame )
	SQBIND_MEMBER_FUNCTION( CFfContainer, DecodeFrame )
	SQBIND_MEMBER_FUNCTION( CFfContainer, DecodeFrameBin )
	SQBIND_MEMBER_FUNCTION( CFfContainer, DecodeAudioFrameBin )

	SQBIND_MEMBER_FUNCTION( CFfContainer, Create )
	SQBIND_MEMBER_FUNCTION( CFfContainer, InitWrite )
	SQBIND_MEMBER_FUNCTION( CFfContainer, AddVideoStream )
	SQBIND_MEMBER_FUNCTION( CFfContainer, WriteFrame )
	SQBIND_MEMBER_FUNCTION( CFfContainer, WriteVideoFrame )
	SQBIND_MEMBER_FUNCTION( CFfContainer, AddAudioStream )
	SQBIND_MEMBER_FUNCTION( CFfContainer, WriteAudioFrame )
	SQBIND_MEMBER_FUNCTION( CFfContainer, Seek )
	SQBIND_MEMBER_FUNCTION( CFfContainer, SeekFrame )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getBytePos )
	SQBIND_MEMBER_FUNCTION( CFfContainer, setBytePos )
	SQBIND_MEMBER_FUNCTION( CFfContainer, FlushBuffers )

	SQBIND_MEMBER_FUNCTION( CFfContainer, Destroy )
	SQBIND_MEMBER_FUNCTION( CFfContainer, CloseStream )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getFps )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getWidth )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getHeight )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getVideoFormat )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getVideoCodecId )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getVideoBitRate )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getVideoStream )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioStream )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getFrameCount )
	SQBIND_MEMBER_FUNCTION( CFfContainer, isVideoCodec )
	SQBIND_MEMBER_FUNCTION( CFfContainer, setVideoExtraData )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getVideoExtraData )

	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioFrameSize )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioBps )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioBitRate )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioSampleRate )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioSampleFmt )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioChannels )
	SQBIND_MEMBER_FUNCTION( CFfContainer, setAudioExtraData )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioExtraData )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioDec )
	SQBIND_MEMBER_FUNCTION( CFfContainer, isAudioCodec )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioCodecId )

	SQBIND_MEMBER_FUNCTION( CFfContainer, getFifoShare )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getFifoReads )
	SQBIND_MEMBER_FUNCTION( CFfContainer, setFifoReads )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getFifoWrites )
	SQBIND_MEMBER_FUNCTION( CFfContainer, setFifoWrites )

	SQBIND_MEMBER_FUNCTION( CFfContainer, getUrl )

	SQBIND_MEMBER_FUNCTION( CFfContainer, getKeyFrameInterval )
	SQBIND_MEMBER_FUNCTION( CFfContainer, setKeyFrameInterval )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getLastFrameFlags )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getLastFrameEncodedSize )

	SQBIND_MEMBER_FUNCTION( CFfContainer, setTimeScale )
	SQBIND_MEMBER_FUNCTION( CFfContainer, setTimeBase )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getTimeBase )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getVideoScale )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioScale )

	SQBIND_MEMBER_FUNCTION( CFfContainer, setVideoTsOffset )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getVideoTsOffset )
	SQBIND_MEMBER_FUNCTION( CFfContainer, setAudioTsOffset )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getAudioTsOffset )

    SQBIND_MEMBER_FUNCTION( CFfContainer, enableFramerateFixup )
    SQBIND_MEMBER_FUNCTION( CFfContainer, setVideoStartTime )
    SQBIND_MEMBER_FUNCTION( CFfContainer, setVideoEndTime )

#if !defined( oexUNICODE )
	SQBIND_MEMBER_FUNCTION( CFfContainer, getFixVideoFrameRateDiag )
#endif

//	SQBIND_MEMBER_FUNCTION( CFfContainer,  )

	SQBIND_GLOBALCONST( FFF_KEY_FRAME )

	/// Seek backward
	SQBIND_GLOBALCONST( AVSEEK_FLAG_BACKWARD )

	/// Seek to byte position
	SQBIND_GLOBALCONST( AVSEEK_FLAG_BYTE )

	/// Seek to any frame, i.e. non-keyframes
	SQBIND_GLOBALCONST( AVSEEK_FLAG_ANY )

	/// Seek based on frame number
	SQBIND_GLOBALCONST( AVSEEK_FLAG_FRAME )

SQBIND_REGISTER_CLASS_END()

void CFfContainer::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CFfContainer );
}

CFfContainer::CFfContainer()
{_STT();

	m_pFormatContext = oexNULL;
	m_pCodecContext = oexNULL;
	m_pFrame = oexNULL;
	m_nVideoStream = -1;

	m_nAudioStream = -1;

	m_nWrite = 0;
	m_nRead = 0;
	m_bKeyRxd = 0;
	m_nFrames = 0;
	m_nLastFrameFlags = 0;
	m_nLastFrameEncodedSize = 0;
	m_nKeyFrameInterval = 0;
	oexZero( m_pkt );

	m_time_base = 0;
	m_video_scale = 0;
	m_audio_scale = 0;

	m_vts_offset = 0;
	m_ats_offset = 0;
     
	m_bFixupFrameRate = 0;
	m_videoStartTime = -1;
	m_videoEndTime   = -1;
}

void CFfContainer::Destroy()
{_STT();

	oexAutoLock ll( _g_ffmpeg_lock );
	if ( !ll.IsLocked() ) return;

	// Detach audio
	m_audio_dec.Detach();

	// Close stream
	CloseStream();

//	m_video_extra.Free();
//	m_audio_extra.Free();

	if ( m_pkt.data )
		av_free_packet( &m_pkt );

	if ( m_pFrame ) 
		av_free( m_pFrame );
	m_pFrame = oexNULL;

	m_pCodecContext = oexNULL;
	m_buf.Free();
	oexZero( m_pkt );
	m_nFrames = 0;
	m_nLastFrameFlags = 0;
	m_nLastFrameEncodedSize = 0;
	m_nVideoStream = -1;
	m_nAudioStream = -1;

//	m_pAudioCodecContext = oexNULL;
//	m_nAudioFrames = 0;

	m_nWrite = 0;
	m_nRead = 0;
	m_bKeyRxd = 0;

	m_vts_offset = 0;
	m_ats_offset = 0;

	m_bFixupFrameRate = 0;
	m_videoStartTime = -1;
	m_videoEndTime   = -1;
	
	m_sUrl.clear();
}

int CFfContainer::getAudioSampleFmt()
{	
	if ( !m_pFormatContext || 0 > m_nAudioStream
		 || !m_pFormatContext->streams[ m_nAudioStream ]->codec )
		return 0;

#	define CNVFMT( t, v ) case v : return oex::obj::t;
	switch( m_pFormatContext->streams[ m_nAudioStream ]->codec->sample_fmt )
        {	CNVFMT( tUInt8 , AV_SAMPLE_FMT_U8  );
                CNVFMT( tInt16 , AV_SAMPLE_FMT_S16 );
                CNVFMT( tInt32 , AV_SAMPLE_FMT_S32 );
                CNVFMT( tFloat , AV_SAMPLE_FMT_FLT );
		CNVFMT( tDouble, AV_SAMPLE_FMT_DBL );
		default : return 0;
	} // end switch
	return 0;
}

/// void CFfContainer::fixVideoFrameRate( AVFormatContext* s, int nSeconds, int nFrames )
/// Modeled from avi_write_trailer() http://ffmpeg.org/doxygen/1.2/avienc_8c_source.html#l00594
/// and especially avi_write_counters() http://ffmpeg.org/doxygen/1.2/avienc_8c_source.html#l00113
/// Note that the AVIStream also seems to track nFrames in avist->packet_count
void CFfContainer::fixVideoFrameRate()
{
	// Is the fixup enabled?
	if ( !m_bFixupFrameRate )
		return;

	// Make sure framerate will be sensible
  if ( 0 >= (m_videoEndTime - m_videoStartTime) )
    return;
  // Is the video stream number defined?
  if ( 0 > m_nVideoStream )
    return;
  // check if this is an .avi container before proceeding
  AVFormatContext* s = m_pFormatContext;
  if ( NULL == s )
    return;
  if ( NULL == s->oformat )
    return;
  if ( NULL == s->oformat->name )
    return;
  if ( strcmp( s->oformat->name, "avi" ) )
    return;
  
  AVIOContext* pb = s->pb;
  if ( !(pb->seekable) )
    return;

  int64_t file_size = avio_tell(pb);
  AVIStream *avist= (AVIStream *)(s->streams[m_nVideoStream]->priv_data);
  if ( 12 > avist->frames_hdr_strm )
    return;
  if ( file_size <= avist->frames_hdr_strm )
    return;
  if ( 0 >= avist->packet_count )
    return;

  avio_seek(pb, avist->frames_hdr_strm - 12, SEEK_SET);
  avio_wl32(pb, m_videoEndTime - m_videoStartTime );
  avio_wl32(pb, avist->packet_count );
  avio_seek(pb, file_size, SEEK_SET);
}

#if !defined( oexUNICODE )
sqbind::stdString CFfContainer::getFixVideoFrameRateDiag()
{
#define LINE_SZ 64
#define MSG_SZ (LINE_SZ * 16)
  char msg[MSG_SZ];
  char format[LINE_SZ];
  char seek[LINE_SZ];
  char pktc[LINE_SZ * 2];

  if ( NULL == m_pFormatContext )
    _snprintf( format, LINE_SZ, "m_pFormatContext = NULL\n");
   else if ( NULL == m_pFormatContext->oformat )
    _snprintf( format, LINE_SZ, "m_pFormatContext->oformat = NULL\n");
   else if ( NULL == m_pFormatContext->oformat->name )
    _snprintf( format, LINE_SZ, "m_pFormatContext->oformat->name = NULL\n");
   else
    _snprintf( format, LINE_SZ, "m_pFormatContext->oformat->name = '%s'\n", m_pFormatContext->oformat->name );

  if ( m_pFormatContext )
    { AVIOContext* pb = m_pFormatContext->pb;
      if ( pb->seekable )
        _snprintf( seek, LINE_SZ, "AVIOContext is seekable\n" );
       else
        _snprintf( seek, LINE_SZ, "AVIOContext is NOT seekable\n" );
    }
   else
    seek[0] = NULL;

  if ( 0 <= m_nVideoStream )
    { AVIStream *avist= (AVIStream *)(m_pFormatContext->streams[m_nVideoStream]->priv_data);
      _snprintf( pktc, LINE_SZ * 2, "avist->packet_count    : %d\n"
                                    "avist->frames_hdr_strm : %d\n",
                                     avist->packet_count,
                                     avist->frames_hdr_strm );
    }
   else
    pktc[0] = NULL;

  _snprintf( msg, MSG_SZ,
            "fixVideoFrameRate:\n"
            "m_videoEndTime   : %d\n"
            "m_videoStartTime : %d\n"
            "m_nVideoStream   : %d\n"
            "%s%s%s\n",
             m_videoEndTime,
             m_videoStartTime,
             m_nVideoStream,
             format, seek, pktc
          );

  m_sFfrDiag.assign( msg );
  return m_sFfrDiag;
}
#endif

int CFfContainer::CloseStream()
{_STT();
    int i;

	oexAutoLock ll( _g_ffmpeg_lock );
	if ( !ll.IsLocked() ) return 0;

	if ( !m_pFormatContext )
		return 0;

	// Do we want to do the frame rate fixup?
	if ( m_bFixupFrameRate )
		fixVideoFrameRate();

	// Close all open streams
	for ( i = 0; i < m_pFormatContext->nb_streams; i++ )
		if ( m_pFormatContext->streams[ i ] && m_pFormatContext->streams[ i ]->codec )
			avcodec_close( m_pFormatContext->streams[ i ]->codec );

	// No more streams
	m_nVideoStream = -1;
	m_nAudioStream = -1;

	if ( m_nRead )
//		av_close_input_file( m_pFormatContext ),
		avformat_close_input( &m_pFormatContext );          // calls avformat_free_context(), avio_close()
//		avformat_free_context( m_pFormatContext );

	else if ( m_nWrite )
	{
		if ( 1 < m_nWrite ) 
		{
			// Write the rest of the file
			av_write_trailer( m_pFormatContext );

			// Close file / socket resources
			if ( m_pFormatContext->oformat
				 && !( m_pFormatContext->oformat->flags & AVFMT_NOFILE )
				 && m_pFormatContext->pb )
				avio_close( m_pFormatContext->pb );
		} // end if

		avformat_free_context(m_pFormatContext);

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

	m_pFormatContext = avformat_alloc_context();
	int res = avformat_open_input( &m_pFormatContext, oexStrToMbPtr( sUrl.c_str() ), 0, 0 );
	if ( res )
	{	oexERROR( res, oexT( "avio_open() failed" ) );
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
		(*m)[ oexT( "ctx_flags" ) ].set( oexMks( m_pFormatContext->ctx_flags ).Ptr() );
		(*m)[ oexT( "start_time" ) ].set( oexMks( m_pFormatContext->start_time ).Ptr() );
		(*m)[ oexT( "duration" ) ].set( oexMks( m_pFormatContext->duration ).Ptr() );
		(*m)[ oexT( "bit_rate" ) ].set( oexMks( m_pFormatContext->bit_rate ).Ptr() );
		(*m)[ oexT( "packet_size" ) ].set( oexMks( m_pFormatContext->packet_size ).Ptr() );
		(*m)[ oexT( "max_delay" ) ].set( oexMks( m_pFormatContext->max_delay ).Ptr() );
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
//			m_pCodecContext->error_recognition = FF_ER_CAREFUL;
			if( pCodec->capabilities & CODEC_CAP_TRUNCATED )
				m_pCodecContext->flags |= CODEC_FLAG_TRUNCATED;

			if ( 0 > avcodec_open( m_pCodecContext, pCodec ) )
				m_pCodecContext = oexNULL;

			if ( m_pCodecContext->extradata && m_pCodecContext->extradata_size )
				m_video_extra.setBuffer( (sqbind::CSqBinary::t_byte*)m_pCodecContext->extradata,
										 m_pCodecContext->extradata_size, 0, oex::oexFALSE );

		} // end else

	} // end if

	// Audio stream
	if ( 0 <= m_nAudioStream
		 && m_pFormatContext->streams[ m_nAudioStream ]
		 &&  m_pFormatContext->streams[ m_nAudioStream ]->codec )
	{
		AVCodecContext *pcc = m_pFormatContext->streams[ m_nAudioStream ]->codec;
		AVCodec *pCodec = avcodec_find_decoder( pcc->codec_id );
		if ( !pCodec )
			pcc = oexNULL;

		else
		{
			pcc->workaround_bugs |= FF_BUG_AUTODETECT;
			pcc->error_concealment = FF_EC_GUESS_MVS;
//			pcc->error_recognition = FF_ER_CAREFUL;
			if( pCodec->capabilities & CODEC_CAP_TRUNCATED )
				pcc->flags |= CODEC_FLAG_TRUNCATED;

			if ( 0 > avcodec_open( pcc, pCodec ) )
				pcc = oexNULL;

			if ( pcc->extradata && pcc->extradata_size )
				m_audio_extra.setBuffer( (sqbind::CSqBinary::t_byte*)pcc->extradata,
										 pcc->extradata_size, 0, oex::oexFALSE );

		} // end if

		m_audio_dec.Attach( pcc, oexNULL );

	} // end if

	// Reading
	m_nRead = 1;

	// Save url
	m_sUrl = sUrl;

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

	} // end if

	// Save flags
	m_nLastFrameFlags = m_pkt.flags;
	m_nLastFrameEncodedSize = m_pkt.size;

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

	// Save flags
	m_nLastFrameFlags = m_pkt.flags;
	m_nLastFrameEncodedSize = m_pkt.size;

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

	// Save flags
	m_nLastFrameFlags = m_pkt.flags;
	m_nLastFrameEncodedSize = m_pkt.size;

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

	return m_pkt.stream_index;
}

int CFfContainer::DecodeAudioFrameBin( sqbind::CSqBinary *in, sqbind::CSqBinary *out, sqbind::CSqMulti *m )
{_STT();
	return m_audio_dec.Decode( in, out, m );
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
	{	oexERROR( 0, oexMks( oexT( "guess_format() failed : " ), sUrl.c_str(), oexT( " : " ), sType.c_str() ) );
		Destroy();
		return 0;
	} // end if

	// Allocate format context
	m_pFormatContext = avformat_alloc_context();
	if ( !m_pFormatContext )
	{	oexERROR( 0, oexMks( oexT( "avformat_alloc_context() failed : " ), sUrl.c_str(), oexT( " : " ), sType.c_str() ) );
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

	} // end if

	m_nWrite = 1;

	m_sUrl = sUrl;

	return 1;
}

int CFfContainer::InitWrite()
{_STT();
	if ( !m_pFormatContext || !m_pFormatContext->oformat )
		return 0;

	if ( !m_nWrite )
		return 0;

	int res = 0;

	// +++ Below Seems to memleak when avio_open fails
	if ( !( m_pFormatContext->oformat->flags & AVFMT_NOFILE ) )
		if ( 0 > ( res = avio_open( &m_pFormatContext->pb, m_pFormatContext->filename, AVIO_FLAG_WRITE ) ) )
		{	oexERROR( res, oexT( "avio_open() failed" ) );
			Destroy();
			return 0;
		} // end if

	// Write out header
    if ( 0 > ( res = avformat_write_header( m_pFormatContext, 0 ) ) )
	{	oexERROR( res, oexT( "av_write_header() failed" ) );
		Destroy();
		return 0;
	} // end if

	// Writing
	m_nWrite = 2;

//	dump_format( m_pFormatContext, 0, 0, 1 );

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

	// Sanity check on video parameters
	if ( 0 >= width || 0 >= height || 0 >= fps )
		return -1;

	AVStream *pst = avformat_new_stream( m_pFormatContext, avcodec_find_encoder( (CodecID)codec_id ) );
//	AVStream *pst = av_new_stream( m_pFormatContext, 0 );
	if ( !pst )
		return -1;

	AVCodecContext *pcc = pst->codec;
	if ( !pcc )
		return -1;

	// Save stream index
	m_nVideoStream = pst->index;

	// Get defaults
	avcodec_get_context_defaults2( pcc, AVMEDIA_TYPE_VIDEO );

	// Fill in codec info
	pcc->codec_id = (CodecID)codec_id;
	pcc->codec_type = AVMEDIA_TYPE_VIDEO;
	pcc->bit_rate = width * height * fps / 3;
	pcc->width = width;
	pcc->height = height;
//	pcc->pix_fmt = PIX_FMT_YUV420P;

	pcc->time_base.num = 1;
	pcc->time_base.den = ( 0 < m_time_base ) ? m_time_base : fps;

	pcc->gop_size = ( 0 < m_nKeyFrameInterval ) ? m_nKeyFrameInterval : fps;

	// Signal global headers if needed
	if ( 0 != ( m_pFormatContext->oformat->flags & AVFMT_GLOBALHEADER ) )
		pcc->flags |= CODEC_FLAG_GLOBAL_HEADER;

	// Set extra codec data
	if ( m_video_extra.getUsed() )
	{	m_video_extra.Resize( m_video_extra.getUsed() + FF_INPUT_BUFFER_PADDING_SIZE * 2 );
		pcc->flags |= CODEC_FLAG_GLOBAL_HEADER;
		pcc->extradata = (uint8_t*)m_video_extra._Ptr();
		pcc->extradata_size = m_video_extra.getUsed();
	} // end if

	else
	{	oex::CStr sFName( oexMbToStr( m_pFormatContext->oformat->name ) );
		if (  sFName == oexT( "3gp" ) || sFName == oexT( "mov" ) || sFName == oexT( "mp4" ) )
			pcc->flags |= CODEC_FLAG_GLOBAL_HEADER;
	} // end else

	return m_nVideoStream;
}

// http://www.rfc-editor.org/rfc/rfc3984.txt
// http://www.ietf.org/rfc/rfc1889.txt
// http://www.siptutorial.net/RTP/header.html

int getNalHeader( const void *p, int len )
{
	if ( 4 > len )
		return 0;

	unsigned char *b = (unsigned char*)p;

	// Verify NAL marker
	if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
		return 0;

	b += 3;

//	int f					= ( *b & 0x80 ) >> 7;
//	int nri					= ( *b & 0x60 ) >> 5;
//	int ntype				= *b & 0x1f;

	return 1;
}


int getRtpHeader( const void *p, int len )
{
	if ( 4 > len )
		return 0;

	unsigned char *b = (unsigned char*)p;

	int version				= ( *b & 0xc0 ) >> 6;
//	int padding 			= ( *b & 0x20 ) >> 5;
//	int extension			= ( *b & 0x10 ) >> 4;
//	int cc					= *b & 0x0f;

//oexSHOW( version );
//oexSHOW( padding );
//oexSHOW( extension );
//oexSHOW( cc );

	if ( 2 != version )
		return 0;

	b++;
//	int marker				= *b & 0x01;
//	int type				= ( *b & 0xfe ) >> 1;
//oexSHOW( marker );
//oexSHOW( type );

	return 1;
}

// - = Error
// 0 = I-Frame
// 1 = P-Frame
// 2 = B-Frame
// 3 = S-Frame
int getVopType( const void *p, int len )
{
    if ( !p || 6 >= len )
        return -1;

    unsigned char *b = (unsigned char*)p;

    // Verify NAL marker
    if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
    {   b++;
        if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
            return -1;
    } // end if

    b += 3;

    // Verify VOP id
    if ( 0xa0 == ( *b & 0xe0 ) )
    {   b++;
        return ( *b & 0xc0 ) >> 6;
    } // end if

    switch( *b )
    {   case 0x65 : return 0;
        case 0x61 : return 1;
        case 0x01 : return 2;
    } // end switch

    return -1;
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

	if ( m && m->isset( oexT( "flags" ) ) )
		pkt.flags = (*m)[ oexT( "flags" ) ].toint();

	else if ( pStream && pStream->codec
			  &&
				( CODEC_ID_H264 == pStream->codec->codec_id
				  || CODEC_ID_MPEG4 == pStream->codec->codec_id
				)
			)
		pkt.flags |= ( !getVopType( dat->Ptr(), dat->getUsed() ) ? AV_PKT_FLAG_KEY : 0 );

	else
		pkt.flags |= AV_PKT_FLAG_KEY;

	// Waiting for key frame?
	if ( !m_bKeyRxd )
	{	if ( 0 == ( pkt.flags & AV_PKT_FLAG_KEY ) )
			return 0;
		m_bKeyRxd = 1;
	} // end if

	// Save time
	pkt.pts = ( 0 <= nPts ) ? nPts : AV_NOPTS_VALUE;
	pkt.dts = ( 0 <= nDts ) ? nDts : AV_NOPTS_VALUE;

//oexSHOW( pkt.pts );
//oexSHOW( pkt.dts );
/*
	// Offset
	if ( 0 < pkt.pts )
	{
		if ( !m_vts_offset )
			m_vts_offset = pkt.pts;

		if ( !m_ats_offset )
			m_ats_offset = m_vts_offset;

		if ( pkt.pts >= m_vts_offset )
			pkt.pts -= m_vts_offset;
		else
			return 0;

		if ( pkt.dts >= m_vts_offset )
			pkt.dts -= m_vts_offset;
		else if ( 0 < pkt.dts )
			return 0;

	} // end if
*/
	// +++ is this causing the audio issue???
//	AVCodecContext *pcc = pStream->codec;
//	if ( pcc && pcc->coded_frame && pcc->coded_frame->pts != AV_NOPTS_VALUE )
//		pkt.pts = av_rescale_q( pcc->coded_frame->pts, pcc->time_base, pStream->time_base );

	pkt.stream_index = pStream->index;
	pkt.data = (uint8_t*)dat->_Ptr();
	pkt.size = dat->getUsed();

	// Save flags
	m_nLastFrameFlags = pkt.flags;
	m_nLastFrameEncodedSize = pkt.size;


//	if ( 0 > m_nAudioStream )
//	{
		if ( av_write_frame( m_pFormatContext, &pkt ) )
			return 0;
/*
	} // end if

	else

		if ( av_interleaved_write_frame( m_pFormatContext, &pkt ) )
			return 0;
*/
	return 1;
}


int CFfContainer::AddAudioStream( int codec_id, int fmt, int channels, int sample_rate, int bps )
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
    pcc->bit_rate = pcc->sample_rate * ( fmt & 0xf ) * pcc->channels * 8;

	// Custom time base?
	if ( 0 < m_time_base )
		pcc->time_base.num = 1, pcc->time_base.den = m_time_base;

#	define CNVTYPE( t, v ) case oex::obj::t : pcc->sample_fmt = v; break;
	switch( fmt )
	{	CNVTYPE( tUInt8, AV_SAMPLE_FMT_U8 );
		CNVTYPE( tInt16, AV_SAMPLE_FMT_S16 );
		CNVTYPE( tInt32, AV_SAMPLE_FMT_S32 );
		CNVTYPE( tFloat, AV_SAMPLE_FMT_FLT );
		CNVTYPE( tDouble, AV_SAMPLE_FMT_DBL );
		default : pcc->sample_fmt = AV_SAMPLE_FMT_NONE; break;
	} // end switch

//	pcc->bits_per_coded_sample = bps ? bps : av_get_bits_per_sample( pcc->codec_id );
//	pcc->frame_size = ( fmt & 0xf );// * channels * 2;
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

	// Offset
	if ( 0 < pkt.pts )
	{
		if ( !m_ats_offset )
			m_ats_offset = pkt.pts;

		if ( !m_vts_offset )
			m_vts_offset = m_ats_offset;

		if ( pkt.pts >= m_ats_offset )
			pkt.pts -= m_ats_offset;
		else
			return 0;

		if ( pkt.dts >= m_ats_offset )
			pkt.dts -= m_ats_offset;
		else if ( 0 < pkt.dts )
			return 0;

	} // end if

	// Calculate frame size
	long fsz = ( pStream->codec && 0 < pStream->codec->frame_size ) ? pStream->codec->frame_size : 1;
	pkt.size = dat->getUsed() / fsz;

	// +++ is this causing the audio issue???
//	AVCodecContext *pcc = pStream->codec;
//	if ( pcc && pcc->coded_frame && pcc->coded_frame->pts != AV_NOPTS_VALUE )
//		pkt.pts = av_rescale_q( pcc->coded_frame->pts, pcc->time_base, pStream->time_base );


//	if ( 0 > m_nVideoStream )
//	{	
		if ( av_write_frame( m_pFormatContext, &pkt ) )
			return 0;
/*
	} // end if

	else

		if ( av_interleaved_write_frame( m_pFormatContext, &pkt ) )
			return 0;
*/
	return 1;
}


int CFfContainer::WriteFrame( sqbind::CSqBinary *dat, sqbind::CSqMulti *m )
{_STT();
	return WriteVideoFrame( dat, 0, 0, m );
}

// http://dranger.com/ffmpeg/tutorial07.html

/// Seeks to the specified time
int CFfContainer::Seek( int nStreamId, int nOffset, int nFlags, int nType )
{
	if ( !m_pFormatContext )
		return -1;

	// Default to video stream
	if ( 0 > nStreamId )
		nStreamId = m_nVideoStream;

	// Do we have a valid stream?
	if ( 0 > nStreamId || !m_pFormatContext->streams[ nStreamId ] )
		return -1;

	// Calculate time offset if needed
	oex::oexINT64 t = nOffset;
	if ( 0 != ( 0x01 & nType ) )
		t = av_rescale( nOffset,
						m_pFormatContext->streams[ nStreamId ]->time_base.den,
						m_pFormatContext->streams[ nStreamId ]->time_base.num ) / 1000;

	// Let's try and seek to that position
	if ( 0 > avformat_seek_file( m_pFormatContext, nStreamId, 0, t, t, nFlags ) )
		return -1;

	return 0;
}

int CFfContainer::SeekFrame( int nStreamId, int nOffset, int nFlags, int nType,
							 int fmt, sqbind::CSqBinary *in, sqbind::CSqBinary *out, sqbind::CSqMulti *m, int flip )
{
	// Ensure we have a container open
	if ( !m_pFormatContext )
		return -1;

	// Default to video stream
	if ( 0 > nStreamId )
		nStreamId = m_nVideoStream;

	// Do we have a valid stream?
	if ( 0 > nStreamId || !m_pFormatContext->streams[ nStreamId ] )
		return -1;

	// See if we can seek this frame
	if ( 0 > Seek( nStreamId, nOffset, nFlags, nType ) )
		return -1;

	// Calculate the pts for the frame we want
	oex::oexINT64 pts = nOffset;
	if ( 0 != ( 0x01 & nType ) )
		pts = av_rescale( nOffset,
						m_pFormatContext->streams[ nStreamId ]->time_base.den,
						m_pFormatContext->streams[ nStreamId ]->time_base.num ) / 1000;

	long fps = (long)getFps();
	if ( 0 >= fps )
		return -1;

	// Set reasonable search limits
	long max = getFps();
	if ( 30 > max )
		max = 30;
	else if ( 1000 < max )
		max = 1000;

	// Seek forward until we find the frame we're looking for
	long si = 0;
	while ( max && 0 <= ( si = ReadFrame( in, m ) ) )
		if ( si == nStreamId )

			// Do we have the frame we want?
			if ( 100 > max && (*m)[ oexT( "pts" ) ].toint() > pts )
				return 1;

			else
				max--, DecodeFrameBin( in, fmt, out, m, flip );

	return 0;
}

SQInteger CFfContainer::getBytePos()
{
	if ( !m_pFormatContext || !m_pFormatContext->pb )
		return -1;

	return m_pFormatContext->pb->pos;

}

SQInteger CFfContainer::setBytePos( SQInteger pos )
{
	if ( !m_pFormatContext || !m_pFormatContext->pb )
		return -1;

	// Seek to the position
	if ( 0 > avio_seek( m_pFormatContext->pb, pos, SEEK_SET ) )
		return -1;

	// Return the byte position
	return m_pFormatContext->pb->pos;
}

int CFfContainer::FlushBuffers()
{
	if ( !m_pFormatContext )
		return 0;

	// Flush write buffers
	if ( m_nWrite )
	{

#if defined( AVFMT_ALLOW_FLUSH )

		// Flush muxer if possible
		if ( 0 != ( AVFMT_ALLOW_FLUSH & m_pFormatContext->flags ) )
			av_write_frame( m_pFormatContext, 0 );

#endif

		// Flush io
		avio_flush( m_pFormatContext->pb );

		return 0;

	} // end if

	// Flush codec buffers
	int n = 0;
	for ( unsigned int i = 0; i < m_pFormatContext->nb_streams; i++ )
		if ( m_pFormatContext->streams[ i ]->codec )
			avcodec_flush_buffers( m_pFormatContext->streams[ i ]->codec ), n++;

	return n;
}

sqbind::CSqFifoShare* CFfContainer::getFifoShare()
{
	// Make sure we have a format context open
	if ( !m_pFormatContext || !m_pFormatContext->pb )
		return 0;

	// Verify that the protocol is as we expect
	if ( 11 > m_sUrl.length() || oex::zstr::CompareLen( oexT( "memshare://" ), 11, m_sUrl.c_str(), 11 ) )
		return 0;

	// Get url context
	URLContext *uc = (URLContext*)m_pFormatContext->pb->opaque;
	if ( !uc || !uc->is_streamed || !uc->filename )
		return 0;

	// Verify that the protocol is as we expect
	if ( oex::zstr::CompareLen( "memshare://", 11, uc->filename, 11 ) )
		return 0;

	// This should be a fifo share
	return (sqbind::CSqFifoShare*)uc->priv_data;
}

