// ff_fmt.h

class CFfFmt
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CFfFmt )
	_SQBIND_CLASS_CTOR_END( CFfFmt )

	/// Default constructor
	CFfFmt();

	/// Registers the class
	static void Register( sqbind::VM vm );

	/** \addtogroup CFfFmt
		@{
	*/

	/// Patches broken formats
	static int FmtEquiv( int fmt );

	/// Calculate image buffer size for specified format
	static int CalcImageSize( int fmt, int width, int height );

	/// Fills in frame data for specified format
	static int FillAVFrame( AVFrame *pAv, int fmt, int width, int height, void *buf );

	/// Fills in the picture structure for the specified format
	static int FillAVPicture( AVPicture *pPic, int fmt, int width, int height, void *buf );
	
	/** @} */

public:


private:


};

DECLARE_INSTANCE_TYPE( CFfFmt );

