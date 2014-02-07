// ff_container.h

class CFfContainer
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CFfContainer )
	_SQBIND_CLASS_CTOR_END( CFfContainer )

	/// Default constructor
	CFfContainer();

	/// Destructor
	virtual ~CFfContainer() { Destroy(); }

	/// Registers the class
	static void Register( sqbind::VM vm );

	/** \addtogroup CFfContainer
		@{
	*/

	/// Frees all resources
	void Destroy();

	/// Closes the stream without releasing codecs
	int CloseStream();

	/// Open link
	int Open( const sqbind::stdString &sUrl, sqbind::CSqMulti *m );

	/// Reads frame data
	int ReadFrame( sqbind::CSqBinary *dat, sqbind::CSqMulti *m );

	/// Reads and decodes a frame
	int DecodeFrame( int stream, int fmt, sqbind::CSqBinary *dat, sqbind::CSqMulti *m, int flip );

	/// Decodes a frame from the specified buffer
	int DecodeFrameBin( sqbind::CSqBinary *in, int fmt, sqbind::CSqBinary *out, sqbind::CSqMulti *m, int flip );

	/// Decodes an audio frame from the specified buffer
	int DecodeAudioFrameBin( sqbind::CSqBinary *in, sqbind::CSqBinary *out, sqbind::CSqMulti *m );

	/// Creates a container
	int Create( const sqbind::stdString &sUrl, const sqbind::stdString &sType, sqbind::CSqMulti *m );

	/// Initializes file writing
	int InitWrite();

	/// Adds a video stream to the file
	int AddVideoStream( int fmt, int width, int height, int fps );

	/// Writes the specified image to the file
	int WriteFrame( sqbind::CSqBinary *dat, sqbind::CSqMulti *m );

	/// Writes the specified image to the file
	int WriteVideoFrame( sqbind::CSqBinary *dat, SQInteger nPts, SQInteger nDts, sqbind::CSqMulti *m );

	/// Adds an audio stream to the avi
	int AddAudioStream( int codec_id, int fmt, int channels, int sample_rate, int bps );

	/// Writes the specified audio to the file
	int WriteAudioFrame( sqbind::CSqBinary *dat, SQInteger nPts, SQInteger nDts, sqbind::CSqMulti *m );

	/// Returns the current byte offset into the file
	SQInteger getBytePos();

	/// Sets the current byte offset into the file, returns the final position
	SQInteger setBytePos( SQInteger pos );
   
	/// resets video stream frame rates
	void fixVideoFrameRate();

	/// Returns video frame rate
	double getFps()
	{	if ( !m_pFormatContext || 0 > m_nVideoStream
			 || !m_pFormatContext->streams[ m_nVideoStream ]->codec )
			return 0;
		long den = m_pFormatContext->streams[ m_nVideoStream ]->codec->time_base.den;
		long num = m_pFormatContext->streams[ m_nVideoStream ]->codec->time_base.num;
		if ( !num ) num = 1;

		return (double)den / (double)num;
	}

	/// Returns video width
	int getWidth()
	{	if ( !m_pFormatContext || 0 > m_nVideoStream
			 || !m_pFormatContext->streams[ m_nVideoStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nVideoStream ]->codec->width;
	}

	/// Returns video height
	int getHeight()
	{	if ( !m_pFormatContext || 0 > m_nVideoStream
			 || !m_pFormatContext->streams[ m_nVideoStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nVideoStream ]->codec->height;
	}

	/// Returns the bit rate
	int getVideoBitRate() 
	{	if ( !m_pFormatContext || 0 > m_nVideoStream
			 || !m_pFormatContext->streams[ m_nVideoStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nVideoStream ]->codec->bit_rate;
	}

	/// Returns video codec id
	int getVideoCodecId()
	{	if ( !m_pFormatContext || 0 > m_nVideoStream
			 || !m_pFormatContext->streams[ m_nVideoStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nVideoStream ]->codec->codec_id;
	}

	/// Returns video format
	int getVideoFormat()
	{	if ( !m_pFormatContext || 0 > m_nVideoStream
			 || !m_pFormatContext->streams[ m_nVideoStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nVideoStream ]->codec->pix_fmt;
	}

	/// Returns audio codec id
	int getAudioCodecId()
	{	if ( !m_pFormatContext || 0 > m_nAudioStream
			 || !m_pFormatContext->streams[ m_nAudioStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nAudioStream ]->codec->codec_id;
	}

	/// Returns the number of audio channels
	int getAudioChannels() 
	{	if ( !m_pFormatContext || 0 > m_nAudioStream
			 || !m_pFormatContext->streams[ m_nAudioStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nAudioStream ]->codec->channels;
	}

	/// Returns the sample rate
	int getAudioSampleRate() 
	{	if ( !m_pFormatContext || 0 > m_nAudioStream
			 || !m_pFormatContext->streams[ m_nAudioStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nAudioStream ]->codec->sample_rate;
	}

	/// Returns the audio sample format
	int getAudioSampleFmt();

	/// Returns the bit rate
	int getAudioBitRate() 
	{	if ( !m_pFormatContext || 0 > m_nAudioStream
			 || !m_pFormatContext->streams[ m_nAudioStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nAudioStream ]->codec->bit_rate;
	}

	/// Returns the bits per sample
	int getAudioBps() 
	{	if ( !m_pFormatContext || 0 > m_nAudioStream
			 || !m_pFormatContext->streams[ m_nAudioStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nAudioStream ]->codec->bits_per_coded_sample;
	}

	/// Returns the frame size
	int getAudioFrameSize() 
	{	if ( !m_pFormatContext || 0 > m_nAudioStream
			 || !m_pFormatContext->streams[ m_nAudioStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nAudioStream ]->codec->frame_size;
	}

	/// Returns a pointer to the audio decoder
	CFfAudioDecoder* getAudioDec() { return &m_audio_dec; }

	/// Returns video stream index
	int getVideoStream()
	{	return m_nVideoStream;
	}

	/// Returns video stream index
	int getAudioStream()
	{	return m_nAudioStream;
	}

	/// Returns non-zero if a container is open
	int isOpen()
	{	return m_pFormatContext ? 1 : 0;
	}

	/// Return the frame count
	int getFrameCount()
	{	return m_nFrames;
	}

	/// Returns non zero if there is a valid audio codec
	int isAudioCodec()
	{	return m_audio_dec.isValid();
	}

	/// Returns non zero if there is a valid video codec
	int isVideoCodec()
	{	return m_pCodecContext ? 1 : 0; }

	/// Seeks to the specified time
	/**
		\param [in] nStreamId		-	Stream Id to use as a reference
		\param [in] nOffset			-	Seek offset
		\param [in] nFlags			-	1 = Seek backward
										2 = Seek in bytes
										4 = Seek to any frame (not just key frames)
										8 = Seek based on frame number
		\param [in] nType			-	0 = Leave nOffset as is
										1 = Convert nOffset from time to frame number
	*/
	int Seek( int nStreamId, int nOffset, int nFlags, int nType );

	/// Seeks and decodes the specified frame
	int SeekFrame( int nStreamId, int nOffset, int nFlags, int nType, int fmt, sqbind::CSqBinary *in, sqbind::CSqBinary *out, sqbind::CSqMulti *m, int flip );

	/// Returns extra video codec data
	sqbind::CSqBinary getVideoExtraData() { return m_video_extra; }

	/// Sets extra video codec data
	void setVideoExtraData( sqbind::CSqBinary *p ) { if ( p ) m_video_extra = *p; }

	/// Returns extra audio codec data
	sqbind::CSqBinary getAudioExtraData() { return m_audio_extra; }

	/// Sets extra audio codec data
	void setAudioExtraData( sqbind::CSqBinary *p ) { if ( p ) m_audio_extra = *p; }

	/// Flushes all decoder buffers
	int FlushBuffers();

	/// Returns the number of reads on the fifo share
	int getFifoReads()
	{	sqbind::CSqFifoShare *pFs = getFifoShare(); return pFs ? pFs->getReads() : 0; }

	/// Sets the number of reads on the fifo share
	void setFifoReads( int n )
	{	sqbind::CSqFifoShare *pFs = getFifoShare(); pFs->setReads( n ); }

	/// Returns the number of writes on the fifo share
	int getFifoWrites()
	{	sqbind::CSqFifoShare *pFs = getFifoShare(); return pFs ? pFs->getWrites() : 0; }

	/// Sets the number of writes on the fifo share
	void setFifoWrites( int n )
	{	sqbind::CSqFifoShare *pFs = getFifoShare(); pFs->setWrites( n ); }

	/// Returns a pointer to the fifo share if any
	sqbind::CSqFifoShare* getFifoShare();

	/// Returns the curent url
	sqbind::stdString getUrl() { return m_sUrl; }

	/// Returns the default key frame interval
	int getKeyFrameInterval() { return m_nKeyFrameInterval; }

	/// Sets the default key frame interval
	void setKeyFrameInterval( int n ) { m_nKeyFrameInterval = n; }

	/// Returns last frame flags
	int getLastFrameFlags() { return m_nLastFrameFlags; }

	/// Returns the last frames encoded size
	int getLastFrameEncodedSize() { return m_nLastFrameEncodedSize; }

	/// Sets the time scale parameters
	void setTimeScale( SQInteger tb, SQInteger vs, SQInteger as )
	{	m_time_base = tb; m_video_scale = vs; m_audio_scale = as; }

	/// Sets the time base
	void setTimeBase( SQInteger tb ) { m_time_base = tb; }

	/// Returns the time base
	SQInteger getTimeBase() { return m_time_base; }

	/// Returns the video scale
	SQInteger getVideoScale() { return m_video_scale; }

	/// Returns the audio scale
	SQInteger getAudioScale() { return m_audio_scale; }

	/// Returns the video time stamp offset
	void setVideoTsOffset( SQInteger ts ) { m_vts_offset = ts; }

	/// Returns the video time stamp offset
	SQInteger getVideoTsOffset() { return m_vts_offset; }
	
	/// Returns the audio time stamp offset
	void setAudioTsOffset( SQInteger ts ) { m_ats_offset = ts; }

	/// Capture clock time when an .avi file is opened for recording
	void setVideoStartTime( SQInteger t ) { m_videoStartTime = t; }

	/// Capture clock time when an .avi recording file is closed
	void setVideoEndTime( SQInteger t ) { m_videoEndTime = t; }

	/// Set to non-zero to enable frame rate fixup
	void enableFramerateFixup( int b ) { m_bFixupFrameRate = b; }
	
	/// Returns diagnostics about the frame rate fixer
#if !defined( oexUNICODE )
	sqbind::stdString getFixVideoFrameRateDiag();
#endif

	/// Returns the audio time stamp offset
	SQInteger getAudioTsOffset() { return m_ats_offset; }

	/** @} */

private:

	/// The current url
	sqbind::stdString m_sUrl;

	/// Diagnostic info about the frame rate fixer
	sqbind::stdString m_sFfrDiag;

	/// Write ability?
	int m_nWrite;

	/// Read ability?
	int m_nRead;

	/// Output container context
	AVFormatContext *m_pFormatContext;

	/// Codec context
	AVCodecContext *m_pCodecContext;

	/// AV Packet
	AVPacket m_pkt;

	/// Decode frame
	AVFrame *m_pFrame;

	/// Non-zero if key frame has been received
	int m_bKeyRxd;

	/// Default key frame interval in frames
	int m_nKeyFrameInterval;

	/// Video stream index
	int m_nVideoStream;

	/// Audio stream index
	int m_nAudioStream;

	/// Left over packet data
	sqbind::CSqBinary m_buf;

	/// Number of frames that have been processed
	int m_nFrames;

	/// Last frame flags
	int m_nLastFrameFlags;

	/// Last frames encoded size
	int m_nLastFrameEncodedSize;

	/// Time when the video file is opened, used to correct frame rate upon close
	int m_videoStartTime;

	/// Time when the video file is closed, used to correct frame rate
	int m_videoEndTime;
	
	/// Fixup framerate
	int m_bFixupFrameRate;

    /// Audio decoder
	CFfAudioDecoder			m_audio_dec;

	/// Extra codec data
	sqbind::CSqBinary		m_audio_extra;

	/// Extra codec data
	sqbind::CSqBinary		m_video_extra;

	/// Video timestamp offset
	oex::oexINT64			m_vts_offset;

	/// Audio timestamp offset
	oex::oexINT64			m_ats_offset;

	/// Custom time base
	oex::oexINT64			m_time_base;

	/// Video time base scale
	oex::oexINT64			m_video_scale;

	/// Audio time base scale
	oex::oexINT64			m_audio_scale;
};

DECLARE_INSTANCE_TYPE( CFfContainer );
