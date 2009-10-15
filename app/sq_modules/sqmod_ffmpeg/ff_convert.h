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

	/// Calculate image buffer size for specified format
	static int CalcImageSize( int fmt, int width, int height );

	/// Converts image colorspace
	static int ConvertColorBB( int width, int height, sqbind::CSqBinary *src, int src_fmt, sqbind::CSqBinary *dst, int dst_fmt, int alg );

	/// Converts image colorspace
	static int ConvertColorIB( sqbind::CSqImage *img, sqbind::CSqBinary *dst, int dst_fmt, int alg );

private:


};
