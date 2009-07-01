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

private:

	/// Pointer to codec object
    AVCodec 				*m_pCodec;
    
    /// Pointer to codec context
    AVCodecContext 			*m_pCodecContext;


};
