// FrameWnd.cpp: implementation of the CFrameWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "frame_wnd.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


// Event table
BEGIN_EVENT_TABLE( CFrameWnd, wxFrame )

	EVT_MENU( CFrameWnd::WXIDM_Exit, CFrameWnd::OnExit )

	EVT_MENU( CFrameWnd::WXIDM_Open, CFrameWnd::OnOpen )

	EVT_PAINT( CFrameWnd::OnPaint )

	EVT_ERASE_BACKGROUND( CFrameWnd::OnEraseBackground )

END_EVENT_TABLE()

CFrameWnd::CFrameWnd()
{

}

CFrameWnd::~CFrameWnd()
{

}

CFrameWnd::CFrameWnd( const wxString& x_sTitle, const wxPoint& x_ptWin, const wxSize& x_sizeWin )
	: wxFrame( (wxFrame*)NULL, -1, x_sTitle, x_ptWin, x_sizeWin, 
				wxFULL_REPAINT_ON_RESIZE | wxDEFAULT_FRAME_STYLE )
{
//	SetIcon( wxIcon( mondrian_xpm ) ); 

	wxImage::AddHandler( new wxJPEGHandler ); 

	wxMenu *pcMenuFile = new wxMenu;
	if ( pcMenuFile )
		pcMenuFile->Append( WXIDM_Open, _T( "&Open" ) ),
		pcMenuFile->AppendSeparator(),
		pcMenuFile->Append( WXIDM_Exit, _T( "E&xit" ) );

	wxMenuBar *pcMenuBar = new wxMenuBar;
	if ( pcMenuBar ) 
	{
		pcMenuBar->Append( pcMenuFile, _T( "&File" ) );

		SetMenuBar( pcMenuBar );

	} // end if

	CreateStatusBar();

	SetStatusText( _T( "This is a status text" ) );

	// Open the avi file
//	m_cAviFile.Open( _T( "c:/TestClip.avi" ) );

	m_cAviFile.Create( _T( "c:/TestOut.avi" ) );

}

void CFrameWnd::OnExit( wxCommandEvent& x_wxCe )
{
	Close( TRUE );
}

void CFrameWnd::OnOpen( wxCommandEvent& x_wxCe )
{
	wxFileDialog fd( this, _T( "Choose AVI file" ),
					 wxEmptyString, wxEmptyString,
					 _T( "AVI Files (*.avi)|*.avi" )
					 _T( "|All Files (*.*)|*.*" ),
					 wxFD_OPEN, wxDefaultPosition );

	if ( wxID_OK != fd.ShowModal() )
		return;

	// Open the avi file
	m_cAviFile.Open( fd.GetPath() );

}

void CFrameWnd::OnPaint( wxPaintEvent& x_wxPe )
{
	wxPaintDC dc( this );

	wxSize sizeClient = GetClientSize();
	wxRect rect;

	rect.x = 0; rect.y = 0;
	rect.width = sizeClient.GetWidth();
	rect.height = sizeClient.GetHeight();

	dc.DrawRectangle( rect.x, rect.y, rect.width, rect.height );

//	StretchDraw( dc, m_cWxImg, rect );
}

void CFrameWnd::OnEraseBackground( wxEraseEvent& x_wxEe )
{
}

bool CFrameWnd::StretchDraw(wxDC &x_dc, wxImage &x_img, wxRect &x_rect)
{
    if ( !x_img.Ok() ) return FALSE;

	// The slow but portable way...
	wxImage cWxImage = x_img.Copy();
	cWxImage.Rescale( x_rect.GetWidth(), x_rect.GetHeight() );
	x_dc.DrawBitmap( cWxImage, x_rect.x, x_rect.y );

	return TRUE;
}
