// ff_audio_decoder.h

class CFfAudioDecoder
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CFfAudioDecoder )
	_SQBIND_CLASS_CTOR_END( CFfAudioDecoder )

	/// Default constructor
	CFfAudioDecoder();

	/// Destructor
	virtual ~CFfAudioDecoder() { Destroy(); }

	/** \addtogroup CFfAudioDecoder
		@{
	*/

	/// Releases all resources
	void Destroy();

	/// Creates a decoder
	int Create( int x_nCodec, int x_nChannels, int x_nSampleRate, int x_nBps );

	/// Reads info from a stream
	int FindStreamInfo( sqbind::CSqBinary *in );

	/// Encodes the specified image
	int Decode( sqbind::CSqBinary *in, sqbind::CSqBinary *out, sqbind::CSqMulti *m );

	/// Lookup a codec based on name
	static int LookupCodecId( const sqbind::stdString &sCodec );

	/// Lookup a codec based on id
	static sqbind::stdString LookupCodecName( int nId );

	/// Returns extra codec data
	sqbind::CSqBinary getExtraData() { return m_extra; }

	/// Sets extra codec data
	void setExtraData( sqbind::CSqBinary *p ) { if ( p ) m_extra = *p; }

	/// Returns sync
	sqbind::CSqBinary getSync() { return m_sync; }

	/// Sets sync
	void setSync( sqbind::CSqBinary *p ) { if ( p ) m_sync = *p; }

	/// Appends data to temporary buffer and initializes padding
	int BufferData( sqbind::CSqBinary *in, sqbind::CSqMulti *m );

	/// Dequeues data from buffer
	int UnBufferData( int uUsed );

	/// Returns the amount of data buffered
	int getBufferSize();

	/// Returns the number of audio channels
	int getChannels() { return m_pCodecContext ? m_pCodecContext->channels : 0; }
	
	/// Returns the sample rate
	int getSampleRate() { return m_pCodecContext ? m_pCodecContext->sample_rate : 0; }

	/// Returns the bits per sample
	int getBps() { return m_pCodecContext ? m_pCodecContext->bits_per_coded_sample : 0; }

	/** @} */

private:

	/// Number of frames decoded
	long					m_lFrames;

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

	/// Extra codec data
	sqbind::CSqBinary		m_extra;
	
	/// Sync header
	sqbind::CSqBinary		m_sync;
	
};
