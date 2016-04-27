// ff_convert.h

class CFfConvert
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CFfConvert )
	_SQBIND_CLASS_CTOR_END( CFfConvert )

	/// Default constructor
	CFfConvert();
	
	/// Destructor
	~CFfConvert();

	/// Registers the class
	static void Register( sqbind::VM vm );

	/** \addtogroup CFfConvert
		@{
	*/

	/// Release resources and ready object for reuse
	void Destroy();
	
	int getSrcWidth() { return m_src_width; }
	
	int getSrcHeight() { return m_src_height; }
	
	int getSrcSize() { return m_src_size; }
	
	int getSrcFmt() { return m_src_fmt; } 
	
	int getDstWidth() { return m_dst_width; }
	
	int getDstHeight() { return m_dst_height; }
	       
	int getDstSize() { return m_dst_size; }
	       
	int getDstFmt() { return m_dst_fmt; } 
	
	/// Creates the image transformation context
	int Create( int src_width, int src_height, int src_fmt, int dst_width, int dst_height, int dst_fmt, int alg, int flip );
	
	/// Converts image colorspace and size
	int ConvertBB( sqbind::CSqBinary *src, sqbind::CSqBinary *dst );

	/// Converts image colorspace and size
	int ConvertIB( sqbind::CSqImage *img, sqbind::CSqBinary *dst );

	/// Converts image colorspace and size
	int ConvertBI( sqbind::CSqBinary *src, sqbind::CSqImage *dst );

	/// Converts image colorspace and size
	int ConvertII( sqbind::CSqImage *src, sqbind::CSqImage *dst );

	/// Rotates an image
	static int Rotate( int deg, sqbind::CSqBinary *src, int src_fmt, int width, int height, sqbind::CSqBinary *dst, int dst_fmt );

	/// Flips image verticaly
	static int FlipVert( sqbind::CSqBinary *src, int src_fmt, int width, int height, sqbind::CSqBinary *dst, int dst_fmt );

	/** @} */

private:

	SwsContext 			*m_psc;
	
	/// Source Width
	int					m_src_width;
	
	/// Source height
	int					m_src_height;
	
	/// Source image size
	int					m_src_size;
	
	/// Source image format
	int					m_src_fmt;
	
	/// Destination width
	int					m_dst_width;
	
	/// Destination height
	int					m_dst_height;
	
	/// Destination image size
	int					m_dst_size;
	
	/// Destination image format
	int					m_dst_fmt;
	
	/// Non zero if image should be flipped
	int					m_flip;
	
public:

	/// Convert AVFrame to image
	int ConvertFI( AVFrame* src, sqbind::CSqImage *dst );

	/// Convert AVFrame
	int ConvertFB( AVFrame* src, sqbind::CSqBinary *dst );
	
	/// Convert raw buffer to image
	int ConvertRI( void *src, sqbind::CSqImage *dst );


	// Flips the information in an AVPicture structure
	static int Flip( int fmt, int h, AVPicture *p );
	
	/// Rotates an image 90 degrees
	static int Rotate90_3( void *src, void *dst, int width, int height );

	/// Rotates an image 180 degrees
	static int Rotate180_3( void *src, void *dst, int width, int height );

	/// Rotates an image 270 degrees
	static int Rotate270_3( void *src, void *dst, int width, int height );

	/// Rotates an image 90 degrees
	static int Rotate90_4( void *src, void *dst, int width, int height );

	/// Rotates an image 180 degrees
	static int Rotate180_4( void *src, void *dst, int width, int height );

	/// Rotates an image 270 degrees
	static int Rotate270_4( void *src, void *dst, int width, int height );

	/// Flips image vertically
	static int FlipVert_3( void *src, void *dst, int width, int height );

	/// Flips image vertically
	static int FlipVert_4( void *src, void *dst, int width, int height );

private:


};

DECLARE_INSTANCE_TYPE( CFfConvert );

