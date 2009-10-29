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

	void Destroy();

	/// Open link
	int Open( const sqbind::stdString &sUrl, sqbind::CSqMulti *m );

	/// Reads frame data
	int ReadFrame( sqbind::CSqBinary *dat, sqbind::CSqMulti *m );

	/// Reads and decodes a frame
	int DecodeFrame( int stream, int fmt, sqbind::CSqBinary *dat, sqbind::CSqMulti *m );

	/// Creates a container
	int Create( const sqbind::stdString &sUrl, const sqbind::stdString &sType, sqbind::CSqMulti *m );

	/// Initializes file writing
	int InitWrite();

	/// Adds a video stream to the file
	int AddVideoStream( int fmt, int width, int height, int fps );

	/// Writes the specified image to the file
	int WriteFrame( sqbind::CSqBinary *dat, sqbind::CSqMulti *m );

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

	/// Returns video stream index
	int getVideoStream()
	{	return m_nVideoStream;
	}

	/// Returns video stream index
	int getAudioStream()
	{	return m_nVideoStream;
	}

private:

	/// Write ability?
	int 					m_nWrite;

	/// Write ability?
	int 					m_nRead;

	/// Output container context
	AVFormatContext			*m_pFormatContext;

	/// Codec context
	AVCodecContext			*m_pCodecContext;

	/// AV Packet
	AVPacket				m_pkt;

	/// Decode frame
	AVFrame					*m_pFrame;

	/// Video stream index
	int						m_nVideoStream;

	/// Audio stream index
	int						m_nAudioStream;

};
