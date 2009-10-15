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
	~CFfEncoder() { Destroy(); }

	/// Releases all resources
	void Destroy();

	/// Creates a decoder
	int Create( int x_nCodec, int width, int height );

	/// Encode image
	int Encode( int fmt, int width, int height, sqbind::CSqBinary *in, sqbind::CSqBinary *out );

	/// Encode image
	int EncodeImage( sqbind::CSqImage *img, sqbind::CSqBinary *buf );

private:

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
