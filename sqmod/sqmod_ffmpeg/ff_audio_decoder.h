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
	int Create( int x_nCodec );

	/// Reads info from a stream
	int FindStreamInfo( sqbind::CSqBinary *in );

	/// Encodes the specified image
	int Decode( sqbind::CSqBinary *in, sqbind::CSqBinary *out, sqbind::CSqMulti *m );

	/// Lookup a codec based on name
	static int LookupCodecId( const sqbind::stdString &sCodec );

	/// Lookup a codec based on id
	static sqbind::stdString LookupCodecName( int nId );

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

};
