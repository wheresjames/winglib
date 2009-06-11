
#include "stdafx.h"
#include "frame_wnd.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Event table
BEGIN_EVENT_TABLE( CFrameWnd, wxFrame )

	EVT_MENU( CFrameWnd::WXIDM_Exit, CFrameWnd::OnExit )

	EVT_MENU( CFrameWnd::WXIDM_Open, CFrameWnd::OnOpen )

END_EVENT_TABLE()

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
		pcMenuBar->Append( pcMenuFile, _T( "&File" ) ),
		SetMenuBar( pcMenuBar );

	m_pWebView = new wxWebView( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 ); //wxHW_SCROLLBAR_AUTO );


	CreateStatusBar();

//	SetStatusText( _T( "This is a status text" ) );

//	wxMessageBox( oex::os::CTrace::GetBacktrace().Ptr(), _T( "Stack Trace" ), wxOK, this );

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
}


