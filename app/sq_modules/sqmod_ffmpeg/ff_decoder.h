// ff_decoder.h

class CFfDecoder
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CFfDecoder )
	_SQBIND_CLASS_CTOR_END( CFfDecoder )

	/// Default constructor
	CFfDecoder();

	/// Releases all resources
	void Destroy();

	/// Creates a decoder
	int Create( int x_nCodec );

	/// Reads info from a stream
	int FindStreamInfo( sqbind::CSqBinary *in );

	/// Encodes the specified image
	int DecodeImage( sqbind::CSqBinary *in, sqbind::CSqImage *img );

private:

	/// Pointer to codec object
    AVCodec 				*m_pCodec;
    
    /// Pointer to codec context
    AVCodecContext 			*m_pCodecContext;

	/// Output container context
	AVFormatContext			*m_pFormatContext;

	/// Temp buffer
	sqbind::CSqBinary		m_tmp;

};
