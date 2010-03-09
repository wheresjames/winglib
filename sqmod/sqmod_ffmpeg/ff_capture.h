// ff_capture.h

class CFfCapture
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CFfCapture )
	_SQBIND_CLASS_CTOR_END( CFfCapture )

	/// Default constructor
	CFfCapture();

	/// Destructor
	virtual ~CFfCapture() { Destroy(); }

	void Destroy();

	/// Open link
	int Open( const sqbind::stdString &sFormat, const sqbind::stdString &sDevice, sqbind::CSqMulti *m );

	/// Reads a captured frame
	int ReadFrame( sqbind::CSqBinary *dat, sqbind::CSqMulti *m );

	/// Decodes a captured frame
//	int DecodeFrame( int stream, int fmt, sqbind::CSqBinary *dat, sqbind::CSqMulti *m );

	/// Returns video width
	int getWidth()
	{	if ( !m_pFormatContext || 0 > m_nVideoStream
			 || !m_pFormatContext->streams[ m_nVideoStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nVideoStream ]->codec->width;
	}

	/// Returns video width
	int getHeight()
	{	if ( !m_pFormatContext || 0 > m_nVideoStream
			 || !m_pFormatContext->streams[ m_nVideoStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nVideoStream ]->codec->height;
	}

	/// Returns video format
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

	/// Returns non-zero if a container is open
	int isOpen()
	{	return m_pFormatContext ? 1 : 0;
	}

	/// Return the frame count
	int getFrameCount()
	{	return m_nFrames;
	}

private:

	/// Output container context
	AVFormatContext			*m_pFormatContext;

	/// Codec context
	AVCodecContext			*m_pCodecContext;

	/// AV Packet
	AVPacket				m_pkt;

	/// Decode frame
	AVFrame					*m_pFrame;

	/// Number of frames that have been processed
	int						m_nFrames;

	/// Index of the video stream
	int						m_nVideoStream;

	/// Non-zero if key frame has been received
	int						m_bKeyRxd;

};
