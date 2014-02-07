// sq_face.h

class CFtFace
{
public:

	// Glyph map type
	typedef oexStdMap( oex::oexTCHAR, FT_BitmapGlyph ) t_GlyphMap;

public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CFtFace )
	_SQBIND_CLASS_CTOR_END( CFtFace )

	/// Default constructor
	CFtFace();

	/// Destructor
	virtual ~CFtFace();

	/// Registers the class
	static void Register( sqbind::VM vm );

	/** \addtogroup CFtFace
		@{
	*/

	/// Destroy connection
	void Destroy();

	/// Loads a character image
	int LoadChar( const sqbind::stdString &c );

	/// Returns the last error
	int getLastError() { return m_last_error; }

	/// Returns the bitmap for the specified character
	FT_Bitmap* getCharBmp( oex::oexTCHAR c );

	/// Returns the glyph for the specified character
	FT_BitmapGlyph getCharGlyph( oex::oexTCHAR c );

	/// Set font char params
	int setCharSize( int width, int height, int hrez, int vrez );

	/// Calculate string width
	int CalcSize( const sqbind::stdString &s, sqbind::CSqSize *pSize );

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

	/// Set font color
	void setColor( int r, int g, int b )
	{	m_red = r; m_green = g; m_blue = b; }

	/// Draws the string into the specified image
	int DrawImg( const sqbind::stdString &sStr, sqbind::CSqPos *pPos, sqbind::CSqImage *pImg, int flip );

	/// Draws the string into a binary buffer
	int DrawBin( const sqbind::stdString &sStr, sqbind::CSqPos *pPos, sqbind::CSqBinary *pBin, int fmt, sqbind::CSqSize *pSize, int sw, int flip );

	/// Draws the character into a binary buffer
	int DrawCharBin( FT_BitmapGlyph g, int x, int y, sqbind::CSqBinary *pBin, int fmt, int w, int h, int sw, int flip );

	/// Returns an ascii map of the first character
	int Char2Ascii( long *ox, long *oy, long tw, FT_Bitmap *pBmp, sqbind::stdString *sOut, const sqbind::stdString &sChars, const sqbind::stdString &sEol );

	/// Returns a scaled ascii map of the string
	sqbind::stdString Str2Ascii( int width, int height, const sqbind::stdString &sStr, const sqbind::stdString &sChars, const sqbind::stdString &sEol );

	/** @} */

	/// Returns the native object
	FT_Face& Obj() { return m_face; }

private:

	/// Last error
	FT_Error				m_last_error;

	/// Free type font
	FT_Face					m_face;

	/// Matrix transformation
	FT_Matrix				m_matrix;

	/// Font pen
	FT_Vector				m_pen;

	/// Font angle
	double					m_dAngle;

	/// Glyph map
	t_GlyphMap				m_mapGlyph;

	/// Color red
	int						m_red;

	/// Color green
	int						m_green;

	/// Color blue
	int						m_blue;
};

DECLARE_INSTANCE_TYPE( CFtFace );
