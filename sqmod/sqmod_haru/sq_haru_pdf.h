
// sq_haru_pdf.h

#pragma once

class CHaruPdf
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CHaruPdf )
	_SQBIND_CLASS_CTOR_END( CHaruPdf )

	/// Default constructor
	CHaruPdf();

	/// Destructor
	virtual ~CHaruPdf();
	
	/** \addtogroup CHaruPdf
		@{
	*/

	/// Release document resources
	void Destroy();

	/// Static error handler
	static void _error_handler( HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data );
	
	/// Called on error by haru
	void error_handler( HPDF_STATUS error_no, HPDF_STATUS detail_no );
	
	/// Returns a string describing the last error
	sqbind::stdString getLastError() { return m_sLastError; }
	
	/// Starts a new pdf
	int New();
	
	/// Saves to a file
	int Save( const sqbind::stdString &sFile );
	
	/// Saves to a binary object
	int getBin( sqbind::CSqBinary *pBin );
	
	/** @} */

public:
	
	/// Adds a page to the document
	int AddPage();
	
	/// Returns the page width
	int getPageWidth() { if ( !m_page ) return 0; return HPDF_Page_GetWidth( m_page ); }
	
	/// Returns the page height
	int getPageHeight() { if ( !m_page ) return 0; return HPDF_Page_GetHeight( m_page ); }

	/// Sets line width
	int setLineWidth( int w );
	
	/// Draws a rectangle
	int Rectangle( int x, int y, int w, int h );

	/// Commit drawing
	int Stroke();
	
public:

	/// Sets the current font
	int SetFont( const sqbind::stdString &sName, int nSize );

	/// Sets the font from a TTF file
	int SetTtfFont( const sqbind::stdString &sFile, int nSize );

	/// Starts page text
	int BeginText();
	
	/// Ends page text
	int EndText();
	
	/// Returns the width of the specified text
	int getTextWidth( const sqbind::stdString &sText );
	
	/// Returns the height of the specified text
	int getTextHeight( const sqbind::stdString &sText );
	
	/// Prints text at the specified position using the currently selected font
	int TextOut( int x, int y, const sqbind::stdString &sText );

	/// Sets the text position
	int MoveTextPos( int x, int y );
	
	/// Outputs text at the current position
	int ShowText( const sqbind::stdString &sText );
	
private:

	/// PDF File handle
	HPDF_Doc				m_pdf;
	
	/// Current page
	HPDF_Page				m_page;
	
	/// Current font
	HPDF_Font				m_font;
	
	/// Description of the last error
	sqbind::stdString		m_sLastError;

};

SQBIND_DECLARE_INSTANCE( CHaruPdf, CHaruPdf );

