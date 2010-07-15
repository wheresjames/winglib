// sq_library.h

class CFtLibrary
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CFtLibrary )
	_SQBIND_CLASS_CTOR_END( CFtLibrary )

	/// Default constructor
	CFtLibrary();

	/// Destructor
	virtual ~CFtLibrary();

	/// Destroy connection
	void Destroy();

	/// Returns the last error
	int getLastError() { return m_last_error; }

	/// Load font from file
	int LoadFile( CFtFace *pFace, const sqbind::stdString &sFile, int nIndex );

private:

	// Last error
	FT_Error				m_last_error;

	// Freetype library
	FT_Library				m_library;

};
