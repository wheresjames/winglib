// sq_face.h

class CFtFace
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CFtFace )
	_SQBIND_CLASS_CTOR_END( CFtFace )

	/// Default constructor
	CFtFace();

	/// Destructor
	virtual ~CFtFace();

	/// Destroy connection
	void Destroy();

	/// Returns the last error
	int getLastError() { return m_last_error; }

	/// Set font char params
	int setCharSize( int width, int height, int hrez, int vrez );

	/// Set font angle
	void setAngle( double d );

	/// Return font angle
	double getAngle() { return m_dAngle; }

	/// Returns the native object
	FT_Face& Obj() { return m_face; }

private:

	// Last error
	FT_Error				m_last_error;

	// Free type font
	FT_Face					m_face;

	// Matrix transformation
	FT_Matrix				m_matrix;

	// Font angle
	double					m_dAngle;

};
