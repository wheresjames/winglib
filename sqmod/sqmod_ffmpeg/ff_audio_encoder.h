// ff_audio_encoder.h

class CFfAudioEncoder
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CFfAudioEncoder )
	_SQBIND_CLASS_CTOR_END( CFfAudioEncoder )

	/// Default constructor
	CFfAudioEncoder();

	/// Destructor
	virtual ~CFfAudioEncoder() { Destroy(); }

	/// Registers the class
	static void Register( sqbind::VM vm );

	/** \addtogroup CFfAudioEncoder
		@{
	*/

	/// Releases all resources
	void Destroy();

	/// Creates an encoder
	int Create( int x_nCodec, int x_nFmt, int x_nChannels, int x_nSampleRate, int x_nBps, sqbind::CSqMulti *m );

	/// Encode binary buffer
	int Encode( sqbind::CSqBinary *in, sqbind::CSqBinary *out, sqbind::CSqMulti *m );

	/// Returns non-zero if valid codec
	int isValid() { return 0 != m_pCodecContext; }

	/// Returns the number of audio channels
	int getChannels()
	{	if ( !m_pCodecContext )
			return 0;
		return m_pCodecContext->channels;
	}

	/// Returns the audio sample format
	int getFormat() { return m_nFmt; }

	/// Returns the audio codec id
	int getCodecId() { return m_nCodecId; }

	/// Returns the sample rate
	int getSampleRate()
	{	if ( !m_pCodecContext )
			return 0;
		return m_pCodecContext->sample_rate;
	}

	/// Returns the bits per sample
	int getBps()
	{	if ( !m_pCodecContext )
			return 0;
		return m_pCodecContext->bits_per_coded_sample;
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
		if ( !m_pCodecContext )
			return -1;

		// +++ So this is probably just 'return m_nFrame'
		oex::oexINT64 nFps = m_pCodecContext->time_base.den;
		if ( 0 >= nFps )
			return -1;

		// Calculate pts
		return m_nFrame * m_pCodecContext->time_base.den / ( m_pCodecContext->time_base.num * nFps );
	}

	/// Returns the Frame size
	SQInteger getFrameSize()
	{	if ( !m_pCodecContext && m_pCodecContext )
			return 0;
		return m_pCodecContext->frame_size;
	}

	/// Buffers incomming data
	int BufferData( sqbind::CSqBinary *in );

	/// Unbuffers the specified number of bytes
	int UnbufferData( int uUsed );

	/// Get input data format
	int getFmtCnv() { return m_nCnv; }

	/// Set input data format
	void setFmtCnv( int n ) { m_nCnv = n; }

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

	/// Input data format
	int						m_nCnv;

	/// Codec ID
	int						m_nCodecId;

	/// Time base, if zero, defaults to m_nFps
	oex::oexINT64			m_nTimeBase;

	/// Frame number
	oex::oexINT64			m_nFrame;

	/// Audio data buffer
	sqbind::CSqBinary		m_buf;

	/// Packet structure
	AVPacket 				m_pkt;

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

};
