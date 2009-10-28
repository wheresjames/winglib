// ff_convert.h

class CFfConvert
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CFfConvert )
	_SQBIND_CLASS_CTOR_END( CFfConvert )

	/// Default constructor
	CFfConvert();

	/// Fills in frame data for specified format
	static int FillAVFrame( AVFrame *pAv, int fmt, int width, int height, void *buf );

	/// Fills in the picture structure for the specified format
	static int FillAVPicture( AVPicture *pPic, int fmt, int width, int height, void *buf );

	/// Calculate image buffer size for specified format
	static int CalcImageSize( int fmt, int width, int height );

	/// Converts image colorspace
	static int ConvertColorBB( int width, int height, sqbind::CSqBinary *src, int src_fmt, sqbind::CSqBinary *dst, int dst_fmt, int alg );

	/// Converts image colorspace
	static int ConvertColorIB( sqbind::CSqImage *img, sqbind::CSqBinary *dst, int dst_fmt, int alg, int flip );

	/// Converts image colorspace
	static int ConvertColorBI( sqbind::CSqBinary *src, int src_fmt, int width, int height, sqbind::CSqImage *img, int alg, int flip );

	/// Convert raw buffer to image
	static int ConvertColorRI( void *buf, int src_fmt, int width, int height, sqbind::CSqImage *img, int alg, int flip );

	/// Convert AVFrame to image
	static int ConvertColorFI( AVFrame* pAf, int src_fmt, int width, int height, sqbind::CSqImage *img, int alg, int flip );

	/// Convert AVFrame
	static int ConvertColorFB( AVFrame* pAf, int src_fmt, int width, int height, int dst_fmt, sqbind::CSqBinary *dst, int alg );

private:


};
