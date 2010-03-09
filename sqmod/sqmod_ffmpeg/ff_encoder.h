// ff_encoder.h

class CFfEncoder
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CFfEncoder )
	_SQBIND_CLASS_CTOR_END( CFfEncoder )

	/// Default constructor
	CFfEncoder();

	/// Destructor
	virtual ~CFfEncoder() { Destroy(); }

	/// Releases all resources
	void Destroy();

	/// Creates an encoder
	int Create( int x_nCodec, int fmt, int width, int height, int fps, int brate, sqbind::CSqMulti *m );

	/// Encode raw buffer
	int EncodeRaw( int fmt, int width, int height, const void *in, int sz_in, sqbind::CSqBinary *out, sqbind::CSqMulti *m);

	/// Encode binary buffer
	int Encode( int fmt, int width, int height, sqbind::CSqBinary *in, sqbind::CSqBinary *out, sqbind::CSqMulti *m );

	/// Encode image
	int EncodeImage( sqbind::CSqImage *img, sqbind::CSqBinary *out, sqbind::CSqMulti *m );

	/// Returns non-zero if valid codec
	int isValid() { return 0 != m_pCodecContext; }

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

	/// Returns the frame rate
	int getFps()
	{	if ( !m_pCodecContext )
			return 0;
		if ( m_pCodecContext->time_base.num )
			return m_pCodecContext->time_base.den / m_pCodecContext->time_base.num;
		return m_pCodecContext->time_base.den;
	}

	/// Returns the bit rate
	int getBitRate()
	{	if ( !m_pCodecContext )
			return 0;
		return m_pCodecContext->bit_rate;
	}

private:

	/// Format type
	int						m_nFmt;

	/// Pointer to codec object
    AVCodec 				*m_pCodec;

    /// Pointer to codec context
    AVCodecContext 			*m_pCodecContext;

	/// Stream object
	AVStream 				*m_pStream;

	/// Output container
	AVOutputFormat			*m_pOutput;

	/// Output container context
	AVFormatContext			*m_pFormatContext;

	/// Temp buffer
	sqbind::CSqBinary		m_tmp;

};
