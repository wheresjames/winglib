// ff_transcode.h

class CFfTranscode
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CFfTranscode )
	_SQBIND_CLASS_CTOR_END( CFfTranscode )

	/// Default constructor
	CFfTranscode();

	/// Destructor
	virtual ~CFfTranscode() { Destroy(); }

	/// Release resources
	void Destroy();

	/// Initializes the transcoder
	int Init( int width, int height, int fps, int brate, int src_codec, int dst_codec );

	/// Encode binary buffer
	int Transcode( sqbind::CSqBinary *src, sqbind::CSqBinary *dst, sqbind::CSqMulti *fi );

	/// Get raw intermediate image
	int GetRaw( sqbind::CSqBinary *img );

	/// Get intermediate image
	int GetImage( sqbind::CSqImage *img );

	/// Returns the decoder id
	int getDecoderCodecId() { return m_dec_id; }

	/// Returns the encoder id
	int getEncoderCodecId() { return m_enc_id; }

	/// Returns non-zero if valid encoder / decoder
	int isValid() { return m_dec.isValid() && m_enc.isValid(); }

	/// Returns the image width
	int getWidth() { return m_enc.getWidth(); }

	/// Returns the image height
	int getHeight() { return m_enc.getWidth(); }

	/// Returns the frame rate
	int getFps() { return m_enc.getFps(); }

	/// Returns the bit rate
	int getBitRate() { return m_enc.getBitRate(); }

private:

	/// Decoder codec
	int					m_dec_id;

	/// Encoder codec
	int					m_enc_id;

	/// The video decoder
	CFfDecoder			m_dec;

	/// The video encoder
	CFfEncoder			m_enc;

	/// Intermediate image used in transcode
	sqbind::CSqBinary	m_tmp;

};
