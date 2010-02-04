
#include "stdafx.h"
#include "main_app.h"
#include "frame_wnd.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_APP( CMainApp )

CMainApp::CMainApp()
{_STT();

}

CMainApp::~CMainApp()
{_STT();

}

bool CMainApp::OnInit()
{_STT();
	// Turn off color mapping
//	wxSystemOptions::SetOption( _T( "msw.remap" ), 0 );

    CFrameWnd *frame = new CFrameWnd( _T( "Sync" ), wxPoint( 50, 50 ), wxSize( 450, 340 ) );

    frame->Show( TRUE );

    SetTopWindow( frame );

    return TRUE;
} 
 
