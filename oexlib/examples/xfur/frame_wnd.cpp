/*------------------------------------------------------------------
// frame_wnd.cpp
// Copyright (c) 2006 
// Robert Umbehant
// winglib@wheresjames.com
// http://www.wheresjames.com
//
// Redistribution and use in source and binary forms, with or 
// without modification, are permitted for commercial and 
// non-commercial purposes, provided that the following 
// conditions are met:
//
// * Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// * The names of the developers or contributors may not be used to 
//   endorse or promote products derived from this software without 
//   specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
//   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "frame_wnd.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


// Event table
BEGIN_EVENT_TABLE( CFrameWnd, wxFrame )

	EVT_MENU( CFrameWnd::WXIDM_Exit, CFrameWnd::OnExit )

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

	wxMenu *pcMenuFile = new wxMenu;
	if ( pcMenuFile )
		pcMenuFile->Append( WXIDM_Exit, _T( "E&xit" ) );

	wxMenuBar *pcMenuBar = new wxMenuBar;
	if ( pcMenuBar ) 
	{
		pcMenuBar->Append( pcMenuFile, _T( "&File" ) );

		SetMenuBar( pcMenuBar );

	} // end if

	CreateStatusBar();

	SetStatusText( _T( "This is a status text" ) );

//	m_cWxImg.LoadFile( _T( "windsurfing.bmp" ) );

}

void CFrameWnd::OnExit( wxCommandEvent& x_wxCe )
{	Close( TRUE );
}

void CFrameWnd::OnPaint( wxPaintEvent& x_wxPe )
{
	wxPaintDC dc( this );

	wxSize sizeClient = GetClientSize();
	wxRect rect;

	rect.x = 0; rect.y = 0;
	rect.width = sizeClient.GetWidth();
	rect.height = sizeClient.GetHeight();

	StretchDraw( dc, m_cWxImg, rect );
}

void CFrameWnd::OnEraseBackground( wxEraseEvent& x_wxEe )
{
}

BOOL CFrameWnd::StretchDraw(wxDC &x_dc, wxImage &x_img, wxRect &x_rect)
{
    if ( !x_img.Ok() ) return FALSE;

	// The slow but portable way...
	wxImage cWxImage = x_img.Copy();
	cWxImage.Rescale( x_rect.GetWidth(), x_rect.GetHeight() );
	x_dc.DrawBitmap( cWxImage, x_rect.x, x_rect.y );

	return TRUE;
}
