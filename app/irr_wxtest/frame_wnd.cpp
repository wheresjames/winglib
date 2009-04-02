
#include "stdafx.h"
#include "frame_wnd.h"

/*
	/    \
	|~~~~|
	'-..-'
      ||
	 _||_
	`""""`
*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Event table
BEGIN_EVENT_TABLE( CFrameWnd, wxFrame )

	EVT_MENU( CFrameWnd::WXIDM_Exit, CFrameWnd::OnExit )

	EVT_MENU( CFrameWnd::WXIDM_Open, CFrameWnd::OnOpen )

	EVT_PAINT( CFrameWnd::OnPaint )

	EVT_ERASE_BACKGROUND( CFrameWnd::OnEraseBackground )

	EVT_TIMER( wxID_ANY, CFrameWnd::OnTimer )

END_EVENT_TABLE()

CFrameWnd::~CFrameWnd()
{
	if ( m_pTimer )
	{	delete m_pTimer;
		m_pTimer = NULL;
	} // end if

}

CFrameWnd::CFrameWnd( const wxString& x_sTitle, const wxPoint& x_ptWin, const wxSize& x_sizeWin )
	: wxFrame( (wxFrame*)NULL, -1, x_sTitle, x_ptWin, x_sizeWin,
				wxFULL_REPAINT_ON_RESIZE | wxDEFAULT_FRAME_STYLE )
{
	m_pTimer = NULL;

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

//	SetStatusText( _T( "This is a status text" ) );

	int x = 320, y = 240;
	wxDisplaySize(&x,&y);

	irr::SIrrlichtCreationParameters param;
	param.Bits = 16;
	param.AntiAlias = true;
	param.WindowSize = irr::core::dimension2d<irr::s32>(x-200,y);
	param.DriverType = irr::video::EDT_OPENGL;
	param.WindowId = (void*)reinterpret_cast<irr::s32>( GetHandle() );
	m_pDevice = irr::createDeviceEx( param );
//	m_pDevice = NULL;

	if ( !m_pDevice )
		SetStatusText( oexLocalTimeStr( oexT( "Failed to create device" ) ).Ptr() );

	else
	{
		SetStatusText( oexLocalTimeStr( oexT( "%W, %B %D, %Y - %h:%m:%s %A" ) ).Ptr() );

		irr::scene::ISceneManager *smgr = m_pDevice->getSceneManager();
		irr::video::IVideoDriver *driver = m_pDevice->getVideoDriver();

	    driver->setAmbientLight( irr::video::SColorf( .5f, .5f, .5f ) );
		smgr->addLightSceneNode( 0, irr::core::vector3df( 0, 100, 0 ),
								 irr::video::SColorf( 0.5f, 0.5f, 0.5f ), 100 );
		smgr->addLightSceneNode( 0, irr::core::vector3df( 0, 100, -50 ),
								 irr::video::SColorf( 0.5f, 0.5f, 0.5f ), 100 );


		irr::scene::ICameraSceneNode *camera = 
			smgr->addCameraSceneNode( 0, irr::core::vector3df( 0, 30, -40 ), 
									  irr::core::vector3df( 0, 5, 0 ) );
		
		irr::scene::ISceneNode *node = smgr->addCubeSceneNode( 10.f );
		if ( node )
		{
//			node->setMaterialFlag( irr::video::EMF_LIGHTING, false );


			irr::scene::ISceneNodeAnimator* rotate = 
				smgr->createRotationAnimator( irr::core::vector3df( 0, 0.4f, 0 ) );

			node->addAnimator( rotate );

			rotate->drop();

		} // end if

	/*
		//Set up test scene
		scene::IAnimatedMesh* mesh = smgr->getMesh("../media/sydney.md2");
			scene::IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );

		if (node)
		{
			node->setMaterialFlag(EMF_LIGHTING, false);
			node->setFrameLoop(0, 310);
			node->setMaterialTexture( 0, device->getVideoDriver()->getTexture("../media/sydney.bmp") );
		}
		smgr->addCameraSceneNode(0, core::vector3df(0,30,-40), core::vector3df(0,5,0));
	*/

		// Render timer
		m_pTimer = new wxTimer( this );
		m_pTimer->Start( 1000 / 15 );

	} // end else

/*
	if ( m_cCapture.IsOpen() )
	{
		// +++ Just a hack till we get the callbacks going
		m_pTimer = new wxTimer( this );
		m_pTimer->Start( 1000 / 15 );

	} // end if
*/

//	wxMessageBox( oex::os::CTrace::GetBacktrace().Ptr(), _T( "Stack Trace" ), wxOK, this );

//	SetStatusText( m_v4lCap.GetLastErrorStr().Ptr() );
}

void CFrameWnd::OnExit( wxCommandEvent& x_wxCe )
{
	if ( m_pDevice )
	{
		m_pDevice->closeDevice();
		m_pDevice->drop();

		m_pDevice = NULL;

	} // end if

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

void CFrameWnd::OnPaint( wxPaintEvent& x_wxPe )
{
	wxPaintDC dc( this );

	wxSize sizeClient = GetClientSize();

	wxRect rect;
	rect.x = 0; rect.y = 0;
	rect.width = sizeClient.GetWidth();
	rect.height = sizeClient.GetHeight();

	if ( !m_pDevice )
	{
		dc.SetBrush( wxBrush( wxColor( 255, 0, 0 ) ) );
		dc.DrawRectangle( rect.x, rect.y, rect.width, rect.height );
		return;
	} // end if

	m_pDevice->run();
	m_pDevice->getVideoDriver()->beginScene(true,true,0);
	m_pDevice->getSceneManager()->drawAll();
	m_pDevice->getVideoDriver()->endScene();

}

void CFrameWnd::OnTimer( wxTimerEvent &x_wxTe )
{
	Refresh();

}


void CFrameWnd::OnEraseBackground( wxEraseEvent& x_wxEe )
{

}
