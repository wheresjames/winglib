// gs_capture.h

class CGsCapture
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CGsCapture )
	_SQBIND_CLASS_CTOR_END( CGsCapture )

	/// Default constructor
	CGsCapture();

	/// Destructor
	~CGsCapture() { Destroy(); }

	/// Releases all resources
	void Destroy();

	/// Initializes capture stream
	int Init();

private:

};
