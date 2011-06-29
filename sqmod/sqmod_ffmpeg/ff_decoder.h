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

	/** \addtogroup CFfDecoder
		@{
	*/

	/// Releases all resources
	void Destroy();

	/// Creates a decoder
	int Create( int x_nCodec, int fmt, int width, int height, int fps, int brate, sqbind::CSqMulti *m );

	/// Reads info from a stream
	int FindStreamInfo( sqbind::CSqBinary *in );

	/// Encodes the specified image
	int Decode( sqbind::CSqBinary *in, int fmt, sqbind::CSqBinary *out, sqbind::CSqMulti *m, int flip );

	/// Encodes the specified image
	int DecodeImage( sqbind::CSqBinary *in, sqbind::CSqImage *img, sqbind::CSqMulti *m, int flip );

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

	/// Lookup a codec based on name
	static int LookupCodecId( const sqbind::stdString &sCodec );

	/// Lookup a codec based on id
	static sqbind::stdString LookupCodecName( int nId );

	/// Returns extra codec data
	sqbind::CSqBinary getExtraData() { return m_extra; }

	/// Sets extra codec data
	void setExtraData( sqbind::CSqBinary *p ) { if ( p ) m_extra = *p; }

	/// Appends data to temporary buffer and initializes padding
	int BufferData( sqbind::CSqBinary *in, sqbind::CSqMulti *m );

	/// Dequeues data from buffer
	int UnBufferData( int uUsed );

	/// Returns the amount of data buffered
	int getBufferSize();

	/// Returns the frame type
	/**
		@param [in] p	-	Pointer to frame buffer
		@param [in] len	-	Length of buffer in p
		
		@return		< 0 = error
					0 = I-Frame
					1 = P-Frame
					2 = B-Frame
					3 = S-Frame
	*/
	int GetH264FrameType( const void *p, int len );
	
	/// Returns the frame type
	/**
		@param [in] p	-	Pointer to frame buffer
		@param [in] len	-	Length of buffer in p
		
		@return		< 0 = error
					0 = I-Frame
					1 = P-Frame
					2 = B-Frame
					3 = S-Frame
	*/
	int getH264FrameType( sqbind::CSqBinary *in )
	{	if ( !in )
			return -1;
		return GetH264FrameType( in->Ptr(), in->getUsed() );
	}
	
	/** @} */

private:

	/// Image format
	int						m_nFmt;
	
	/// Non-zero if we're waiting for a key frame
	int						m_nWaitKeyFrame;

	/// Pointer to codec object
    AVCodec 				*m_pCodec;

    /// Pointer to codec context
    AVCodecContext 			*m_pCodecContext;

	/// Output container context
	AVFormatContext			*m_pFormatContext;

	/// Decode frame buffer
	AVFrame 				*m_pFrame;

	/// Packet structure
	AVPacket 				m_pkt;

	/// Temp buffer
	sqbind::CSqBinary		m_tmp;

	/// Extra codec data
	sqbind::CSqBinary		m_extra;

};
