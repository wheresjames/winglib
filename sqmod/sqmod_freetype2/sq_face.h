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

	/// Loads a character image
	int LoadChar( int nChar );

	/// Returns the last error
	int getLastError() { return m_last_error; }

	/// Set font char params
	int setCharSize( int width, int height, int hrez, int vrez );

	/// Set font angle
	void setAngle( double d );

	/// Return font angle
	double getAngle() { return m_dAngle; }

	/// Sets the pen pos
	void setPen( int x, int y ) { m_pen.x = x; m_pen.y = y; }

	/// Sets the pen x
	void setPenX( int x ) { m_pen.x = x; }

	/// Gets the pen x
	int getPenX() { return m_pen.x; }

	/// Sets the pen y
	void setPenY( int y ) { m_pen.y = y; }

	/// Gets the pen y
	int getPenY() { return m_pen.y; }

	/// Returns the native object
	FT_Face& Obj() { return m_face; }

private:

	// Last error
	FT_Error				m_last_error;

	// Free type font
	FT_Face					m_face;

	// Matrix transformation
	FT_Matrix				m_matrix;

	// Font pen
	FT_Vector				m_pen;

	// Font angle
	double					m_dAngle;

};
