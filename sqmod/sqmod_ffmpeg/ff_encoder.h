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

	/// Registers the class
	static void Register( sqbind::VM vm );

	/** \addtogroup CFfEncoder
		@{
	*/

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

	/// Returns the Decode Time Stamp
	SQInteger getPktDts()
	{	if ( !m_pCodecContext && m_pCodecContext->coded_frame )
			return 0;
		return m_pCodecContext->coded_frame->pkt_dts;
	}

	/// Returns the Presentation Time Stamp
	SQInteger getPktPts()
	{	if ( !m_pCodecContext && m_pCodecContext->coded_frame )
			return 0;
		return m_pCodecContext->coded_frame->pkt_pts;
	}

	/// Returns the Presentation Time Stamp
	SQInteger getPts()
	{	if ( !m_pCodecContext && m_pCodecContext->coded_frame )
			return 0;
		return m_pCodecContext->coded_frame->pts;
	}

	/// Calculates the PTS based on the current frame index
	SQInteger calcPts()
	{
		// Sanity checks
		if ( !m_pCodecContext || 0 >= m_nFps )
			return -1;

		// Calculate pts
		return m_nFrame * m_pCodecContext->time_base.den / ( m_pCodecContext->time_base.num * m_nFps );
	}

	/// Returns extra codec data
	sqbind::CSqBinary getExtraData() 
	{ 	if ( !m_pCodecContext || !m_pCodecContext->extradata || 0 >= m_pCodecContext->extradata_size )
			return sqbind::CSqBinary();
		return sqbind::CSqBinary( (sqbind::CSqBinary::t_byte*)m_pCodecContext->extradata, 
								  m_pCodecContext->extradata_size );
	}

	/// Sets the packet header
	void setHeader( sqbind::CSqBinary *header ) { if ( header ) m_header = *header; }

	/// Returns the packet header
	sqbind::CSqBinary getHeader() { return m_header; }

	/// Returns the current frame number
	SQInteger getFrame() { return m_nFrame; }

	/// Sets the current frame number
	void setFrame( SQInteger n ) { m_nFrame = n; }

	/// Sets the time base, if zero, defaults to frame rate
	void setTimeBase( SQInteger n ) { m_nTimeBase = n; }

	/// Returns the current time base
	SQInteger getTimeBase() { return m_nTimeBase; }

	/** @} */

private:

	/// Format type
	int						m_nFmt;

	/// Frame rate
	int						m_nFps;

	/// Time base, if zero, defaults to m_nFps
	oex::oexINT64			m_nTimeBase;

	/// Frame number
	oex::oexINT64			m_nFrame;

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

	/// Temp buffers
	sqbind::CSqBinary		m_tmp, m_tmp2;

	/// Optional packet header
	sqbind::CSqBinary		m_header;

};
