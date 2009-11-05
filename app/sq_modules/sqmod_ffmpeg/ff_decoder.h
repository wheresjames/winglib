// ff_decoder.h

class CFfDecoder
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CFfDecoder )
	_SQBIND_CLASS_CTOR_END( CFfDecoder )

	/// Default constructor
	CFfDecoder();

	/// Destructor
	virtual ~CFfDecoder() { Destroy(); }

	/// Releases all resources
	void Destroy();

	/// Creates a decoder
	int Create( int x_nCodec, int fmt, int width, int height, int cmp );

	/// Reads info from a stream
	int FindStreamInfo( sqbind::CSqBinary *in );

	/// Encodes the specified image
	int Decode( sqbind::CSqBinary *in, int fmt, sqbind::CSqBinary *out, sqbind::CSqMulti *m );

	/// Encodes the specified image
	int DecodeImage( sqbind::CSqBinary *in, sqbind::CSqImage *img, sqbind::CSqMulti *m );

	/// Returns the width of the decoded video
	int getWidth()
	{	if ( !m_pCodecContext )
			return 0;
		return m_pCodecContext->width; 
	}

	/// Returns the width of the decoded video
	int getHeight()
	{	if ( !m_pCodecContext )
			return 0;
		return m_pCodecContext->height; 
	}

	/// Lookup a codec based on name
	static int LookupCodecId( const sqbind::stdString &sCodec );

	/// Lookup a codec based on id
	static sqbind::stdString LookupCodecName( int nId );

private:

	/// Image format
	int						m_nFmt;

	/// Pointer to codec object
    AVCodec 				*m_pCodec;

    /// Pointer to codec context
    AVCodecContext 			*m_pCodecContext;

	/// Output container context
	AVFormatContext			*m_pFormatContext;

	/// Decode frame buffer
	AVFrame 				*m_pFrame;

	/// Frame packet
	AVPacket				m_pkt;

	/// Temp buffer
	sqbind::CSqBinary		m_tmp;

};
