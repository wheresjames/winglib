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
		pcMenuBar->Append( pcMenuFile, _T( "&File" ) ),
		SetMenuBar( pcMenuBar );

	CreateStatusBar();

	SetStatusText( _T( "This is a status text" ) );
/*
	oex::oexUINT		uCopy = 100;
	oex::vid::CAviFile	in, out;
	
	// Open the input avi file
	if ( !in.Open( _T( "c:/TestClip.avi" ), oex::oexFALSE ) )
		return;
	
	// Output avi
	if ( !out.Create( _T( "c:/TestOut.avi" ) ) )
		return;

	// Set avi frame rate
	out.SetFrameRate( 15.f );

	// Fill in main header
	out.Amh()->dwWidth					= oexLittleEndian( 320 );
	out.Amh()->dwHeight					= oexLittleEndian( 240 );
	out.Amh()->dwStreams				= oexLittleEndian( 1 );
	out.Amh()->dwTotalFrames			= oexLittleEndian( uCopy );
	out.Amh()->dwFlags					= oexLittleEndian( oex::vid::CAviFile::SAviMainHeader::eAmhTrustCkType );

	// Stream header
	out.Ash()->fccType					= oex::vid::CAviFile::SAviStreamHeader::eAshStreamTypeVideo;
//	out.Ash()->fccHandler				= oexLittleEndian( MAKE_FOURCC( 'D', 'I', 'B', ' ' ) );
	out.Ash()->fccHandler				= oexLittleEndian( MAKE_FOURCC( 'M', 'P', '4', '2' ) );
	out.Ash()->dwLength					= oexLittleEndian( uCopy );
	out.Ash()->dwQuality				= oexLittleEndian( 0xffffffff );
	out.Ash()->rcFrame.left				= 0;
	out.Ash()->rcFrame.top				= 0;
	out.Ash()->rcFrame.right			= oexLittleEndian( 320 );
	out.Ash()->rcFrame.bottom			= oexLittleEndian( 240 );

	// Bitmap info
	out.Bi()->bmiHeader.biWidth			= oexLittleEndian( 320 );
	out.Bi()->bmiHeader.biHeight		= oexLittleEndian( 240 );
//	out.Bi()->bmiHeader.biCompression	= oexLittleEndian( MAKE_FOURCC( 'M', 'J', 'P', 'G' ) );
	out.Bi()->bmiHeader.biCompression	= oexLittleEndian( MAKE_FOURCC( 'M', 'P', '4', '2' ) );
	out.Bi()->bmiHeader.biPlanes		= oexLittleEndian( 1 );
	out.Bi()->bmiHeader.biBitCount		= oexLittleEndian( 24 );
	out.Bi()->bmiHeader.biSizeImage		= oexLittleEndian( oex::cmn::Align4( out.Bi()->bmiHeader.biWidth )
														   * oex::cmn::Abs( out.Bi()->bmiHeader.biHeight )
														   * oex::cmn::FitTo( out.Bi()->bmiHeader.biBitCount, 8 ) );

	// Copy frames
	oex::oexPVOID pData = NULL;
	oex::oexINT64 llSize = 0;
	for ( DWORD i = 0; i < uCopy; i++ )
	{
		if ( in.GetFrameData( i, &pData, &llSize ) )
			out.AddFrame( oex::vid::CAviFile::eFtUncompressedVideo, 0, pData, llSize );

	} // end for

	in.Destroy();
	out.Destroy();
	
	m_cAviFile.Open( _T( "c:/TestClip.avi" ), FALSE );
*/
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
	// +++ Check this cast
//	m_cAviFile.Open( (oex::oexCSTR)fd.GetPath().c_str(), TRUE );
}

void CFrameWnd::OnPaint( wxPaintEvent& x_wxPe )
{
	wxPaintDC dc( this );

	wxSize sizeClient = GetClientSize();
	wxRect rect;

	rect.x = 0; rect.y = 0;
	rect.width = sizeClient.GetWidth();
	rect.height = sizeClient.GetHeight();

	oex::oexPVOID pData = NULL;
	oex::oexINT64 llSize = 0;

	// Open the input avi file
//	if ( !m_cAviFile.IsOpen() || !m_cAviFile.GetFrameData( 0, &pData, &llSize ) )
	{	dc.SetBrush( wxBrush( wxColor( 255, 0, 0 ) ) );
		dc.DrawRectangle( rect.x, rect.y, rect.width, rect.height );
		return;
	} // end if

	wxImage		imgMem( 100, 100, (unsigned char*)pData, true );
	wxBitmap	bmMem( imgMem );

	// Create a memory DC
	wxMemoryDC dcMem;
	dcMem.SelectObject( bmMem );
	dc.Blit( 0, 0, 100, 100, &dcMem, 0, 0 );

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
