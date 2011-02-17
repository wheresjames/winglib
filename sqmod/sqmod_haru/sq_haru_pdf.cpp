
// sq_haru_pdf.cpp

#include "stdafx.h"

CHaruPdf::CHaruPdf()
{_STT();

	m_pdf = oexNULL;
	
	m_page = oexNULL;
	m_page = oexNULL;
	m_font = oexNULL;
}

CHaruPdf::~CHaruPdf()
{_STT();
	Destroy();
}

void CHaruPdf::Destroy()
{_STT();

	if ( m_pdf )
		HPDF_Free( m_pdf );
		
	m_sLastError = oexT( "" );
	m_pdf = oexNULL;
	m_page = oexNULL;
	m_font = oexNULL;
}

void CHaruPdf::_error_handler( HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data )
{_STT();
	CHaruPdf *pThis = (CHaruPdf*)user_data;
	if ( pThis )
		pThis->error_handler( error_no, detail_no );
}

void CHaruPdf::error_handler( HPDF_STATUS error_no, HPDF_STATUS detail_no )
{_STT();

	m_sLastError = sqbind::oex2std( oexMks( oexT( "error_no = " ), error_no, oexT( ", detail_no = " ), detail_no ) );

}

int CHaruPdf::New()
{_STT();

	Destroy();
	
	// Attempt to allocate a new document
	m_pdf = HPDF_New( &_error_handler, this );
	if ( !m_pdf )
		return 0;

	return 1;
}

int CHaruPdf::Save( const sqbind::stdString &sFile )
{_STT();

	// Sanity checks
	if ( !m_pdf || !sFile.length() )
		return 0;

	// Save to file
	if ( HPDF_OK != HPDF_SaveToFile( m_pdf, sFile.c_str() ) )
		return 0;

	return 1;
}

int CHaruPdf::getBin( sqbind::CSqBinary *pBin )
{_STT();

	// Sanity checks
	if ( !m_pdf || !pBin )
		return 0;

	// Reset stream
//	HPDF_ResetStream( m_pdf );
		
	// Save to file
	if ( HPDF_OK != HPDF_SaveToStream( m_pdf ) )
		return 0;

	// How large is the document?
	HPDF_UINT32 uSize = HPDF_GetStreamSize( m_pdf );
	if ( !uSize )
		return 0;

	// Allocate memory for document
	if ( uSize > pBin->Size() )
		if ( !pBin->Allocate( uSize ) )
			return 0;

	pBin->setUsed( 0 );

	// Copy the document
	if ( HPDF_OK != HPDF_ReadFromStream( m_pdf, (HPDF_BYTE*)pBin->_Ptr(), &uSize ) )
		return 0;

	pBin->setUsed( uSize );

	return 1;
}

int CHaruPdf::AddPage()
{_STT();

	// Sanity checks
	if ( !m_pdf )
		return 0;

	// Add a page
	m_page = HPDF_AddPage( m_pdf );
	if ( !m_page )
		return 0;

	return 1;
}

int CHaruPdf::setLineWidth( int w )
{_STT();

	// Sanity checks
	if ( !m_page )
		return 0;

	// Set line width
	if ( HPDF_OK != HPDF_Page_SetLineWidth( m_page, w ) )
		return 0;

	return 1;
}

int CHaruPdf::Rectangle( int x, int y, int w, int h )
{_STT();

	// Sanity checks
	if ( !m_page )
		return 0;

	// Draw rectangle
	if ( HPDF_OK != HPDF_Page_Rectangle( m_page, x, y, w, h ) )
		return 0;

	return 1;
}

int CHaruPdf::Stroke()
{_STT();

	// Sanity checks
	if ( !m_page )
		return 0;

	// Commit drawing
	if ( HPDF_OK != HPDF_Page_Stroke( m_page ) )
		return 0;

	return 1;
}

int CHaruPdf::SetFont( const sqbind::stdString &sName, int nSize )
{_STT();

	// Sanity checks
	if ( !m_pdf || !m_page )
		return 0;
		
	// Attempt to get the specified font
	m_font = HPDF_GetFont( m_pdf, sName.c_str(), oexNULL );
	if ( !m_font )
		return 0;
		
	// Set the font
	if ( HPDF_OK != HPDF_Page_SetFontAndSize( m_page, m_font, nSize ) )
		return 0;

	return 1;
}

int CHaruPdf::SetTtfFont( const sqbind::stdString &sName, int nSize )
{_STT();

	// Sanity checks
	if ( !m_pdf || !m_page )
		return 0;
		
	// Attempt to load font from file
	const char *pFont = HPDF_LoadTTFontFromFile( m_pdf, sName.c_str(), HPDF_TRUE );
	if ( !pFont )
	{	sqbind::stdString sErr = oexT( "Loading font " );
		sErr += sName; sErr += oexT( " : " ); m_sLastError = sErr + m_sLastError;
		return 0;
	} // end if

	return SetFont( pFont, nSize );
}

int CHaruPdf::BeginText()
{_STT();

	// Sanity checks
	if ( !m_page )
		return 0;

	// Begin text
	if ( HPDF_OK != HPDF_Page_BeginText( m_page ) )
		return 0;

	return 1;
}

int CHaruPdf::EndText()
{_STT();

	// Sanity checks
	if ( !m_page )
		return 0;

	// End text
	if ( HPDF_OK != HPDF_Page_EndText( m_page ) )
		return 0;

	return 1;
}

int CHaruPdf::getTextWidth( const sqbind::stdString &sText )
{_STT();

	// Sanity checks
	if ( !m_page )
		return 0;

	// Get text width
	return HPDF_Page_TextWidth( m_page, sText.c_str() );
}

int CHaruPdf::getTextHeight( const sqbind::stdString &sText )
{_STT();

	// Sanity checks
	if ( !m_page )
		return 0;

	// Get text height
//	return HPDF_Page_TextHeight( m_page, sText.c_str() );

}

int CHaruPdf::TextOut( int x, int y, const sqbind::stdString &sText )
{_STT();

	// Sanity checks
	if ( !m_page )
		return 0;

	// Commit drawing
	if ( HPDF_OK != HPDF_Page_TextOut( m_page, x, y, sText.c_str() ) )
		return 0;

	return 1;
}

int CHaruPdf::MoveTextPos( int x, int y )
{_STT();

	// Sanity checks
	if ( !m_page )
		return 0;

	// Commit drawing
	if ( HPDF_OK != HPDF_Page_MoveTextPos( m_page, x, y ) )
		return 0;

	return 1;
}

int CHaruPdf::ShowText( const sqbind::stdString &sText )
{_STT();

	// Sanity checks
	if ( !m_page )
		return 0;

	// Commit drawing
	if ( HPDF_OK != HPDF_Page_ShowText( m_page, sText.c_str() ) )
		return 0;

	return 1;
}

