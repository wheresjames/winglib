// sq_irrlicht.cpp

#include "stdafx.h"

#if defined( _MSC_VER ) || defined( _IRR_COMPILE_WITH_OPENGL_ ) || defined( _IRR_LINUX_X11_VIDMODE_ )
#	include <GL/gl.h>
#	define SQ_USE_OPENGL
#endif

CSqIrrlicht::CSqIrrlicht()
{
	m_pDevice = oexNULL;
	m_pSmgr = oexNULL;
	m_pDriver = oexNULL;
	m_pGui = oexNULL;
	m_pCamera = oexNULL;
	m_txtFps = oexNULL;
	m_llFrames = 0;
	m_fLastTime = 0;

	m_nDriverType = -1;
	m_colBackground = irr::video::SColor( 255, 0, 50, 100 );

	m_bStereo = 0;
	m_ulREyeKey = 0x0000ffff;
	m_ulLEyeKey = 0x00ff0000;
	m_fStereoWidth = 3.f;
	m_fStereoFocus = 1000.f;
}

CSqIrrlicht::~CSqIrrlicht()
{
	m_pSmgr = oexNULL;
	m_pDriver = oexNULL;
	m_pGui = oexNULL;
	m_nDriverType = -1;
	m_txtFps = oexNULL;
	m_llFrames = 0;
	m_fLastTime = 0;

	// Lose animators
    m_lstMeshAnimators.clear();

	// Lose scene camera reference
	if ( m_pCamera )
	{	m_pCamera->drop();
		m_pCamera = oexNULL;
	} // end if

	// Lose device
	if ( m_pDevice )
	{	m_pDevice->closeDevice();
		m_pDevice->drop();
		m_pDevice = oexNULL;
	} // end if
}

int CSqIrrlicht::Init( const sqbind::stdString &sName, int width, int height, int fps )
{
	irr::SIrrlichtCreationParameters param;

#if defined( _WIN32_WCE )
	param.Bits = 16;
	param.Fullscreen = true;
	param.DriverType = irr::video::EDT_BURNINGSVIDEO;
	param.WindowSize = irr::core::dimension2d<irr::u32>( width, height );
#elif defined( OEX_IPHONE )
	param.Bits = 16;
	param.Fullscreen = true;
	param.DriverType = irr::video::EDT_OGLES1;
	param.WindowSize = irr::core::dimension2d<irr::u32>( width, height );
#else
	param.Bits = 32;
	param.AntiAlias = true;
	param.DriverType = irr::video::EDT_OPENGL;
	param.WindowSize = irr::core::dimension2d<irr::s32>( width, height );
#endif

	m_pDevice = irr::createDeviceEx( param );

	if ( !m_pDevice )
		oexERROR( 0, oexT( "Failed to create irrlicht device" ) );

	else
	{
		// Save away driver type
		m_nDriverType = param.DriverType;

		// Set default event receiver
		m_er.SetDevice( m_pDevice );
		m_pDevice->setEventReceiver( &m_er );

		if ( sName.length() )
			m_pDevice->setWindowCaption( oexStrToStrWPtr( sName.c_str() ) );

		// Grab components
		m_pSmgr = m_pDevice->getSceneManager();
		m_pDriver = m_pDevice->getVideoDriver();
		m_pGui = m_pDevice->getGUIEnvironment();

		if ( !oexCHECK_PTR( m_pSmgr ) )
			oexERROR( 0, oexT( "Scene manager is invalid" ) );

		if ( !oexCHECK_PTR( m_pDriver ) )
			oexERROR( 0, oexT( "Driver pointer is invalid" ) );

		if ( !oexCHECK_PTR( m_pGui ) )
			oexERROR( 0, oexT( "GUI environment is invalid" ) );

#if defined( _WIN32_WCE )

		// Set the filesystem relative to the executable
		m_pDevice->getFileSystem()->addFolderFileArchive ( oexStrToStr8( oexGetModulePath() ).Ptr() );

#endif

		if ( fps )
		{
			m_txtFps =
				m_pGui->addStaticText( L"FPS: xx", irr::core::rect< irr::s32 >( 60, 5, 200, 20 ), false );

			m_pGui->addButton( irr::core::rect< int >( 10, 5, 50, 20 ), 0, 2, L"Quit" );

		} // end if

		// Initialize physics engine
		m_cPhysics.Init();

	} // end else

	return 1;
}

/// Sets the ambient color lighting
int CSqIrrlicht::SetAmbientLight( CSqirrColorf &x_col )
{
	if ( !m_pDriver )
		return 0;

	m_pDriver->setAmbientLight( x_col.Obj() );

	return 1;
}

CSqirrNode CSqIrrlicht::AddLight( CSqirrVector3d &pos, CSqirrColorf &col, float rad )
{
	if ( !m_pSmgr )
		return CSqirrNode();

	return m_pSmgr->addLightSceneNode( 0, pos.Obj(), col.Obj(), rad );
}

CSqirrCamera CSqIrrlicht::AddCamera( CSqirrVector3d &pos, CSqirrVector3d &look )
{
	if ( !m_pSmgr )
		return CSqirrCamera();

	irr::scene::ICameraSceneNode *pCamera =
		m_pSmgr->addCameraSceneNode( 0, pos.Obj(), look.Obj() );

	if ( !pCamera )
		return CSqirrCamera();

	if ( !m_pCamera )
	{	m_pCamera = pCamera;
		m_pCamera->grab();
	} // end if

	return pCamera;
}

CSqirrNode CSqIrrlicht::AddCube( float size )
{
	if ( !m_pSmgr )
		return CSqirrNode();

	return m_pSmgr->addCubeSceneNode( size );
}

CSqirrAnimator CSqIrrlicht::AddRotateAnimator( CSqirrVector3d &speed )
{
	CSqirrAnimator rotate( m_pSmgr->createRotationAnimator( speed.Obj() ) );
	if ( rotate.Ptr() ) rotate.Ptr()->drop();
	return rotate;
}

int CSqIrrlicht::Draw( CSqirrColor &bg )
{
	if ( !m_pDevice || !m_pDevice->run() )
		return -1;

	// Time values
	float fBootSeconds = oexGetBootSeconds();
	float fElapsed = m_fLastTime ? fBootSeconds - m_fLastTime : 0;

	// Update fps
	if ( m_txtFps && !( m_llFrames % 10 ) )
		m_txtFps->setText ( oexMksW( m_pDriver->getFPS() ).Ptr() );

	// Animate meshes
	AnimateMeshes();

	// Update physics engine
	m_cPhysics.Update( fElapsed );

    if ( m_bStereo && m_pCamera )
		DrawAnaglyph( m_pDriver, m_pSmgr, m_pGui, m_pCamera, bg.Obj(),
		              m_fStereoWidth, m_fStereoFocus, m_nDriverType,
					  m_ulREyeKey, m_ulLEyeKey );

	else
	{
		m_pDriver->beginScene( true, true, bg.Obj() );
		m_pSmgr->drawAll();
		m_pGui->drawAll();
		m_pDriver->endScene();

	} // end else

	// Count a frame
	m_llFrames++;
	m_fLastTime = fBootSeconds;

	return 0;
}

int CSqIrrlicht::DrawAnaglyph( irr::video::IVideoDriver *pDriver,
							   irr::scene::ISceneManager *pSm,
							   irr::gui::IGUIEnvironment *pGui,
							   irr::scene::ICameraSceneNode *pCamera,
							   irr::video::SColor colBackground,
							   float fWidth, float fFocus, int nDriverType,
							   unsigned long ulREyeKey, unsigned long ulLEyeKey )
{
	// Right eye
	irr::core::vector3df reye = pCamera->getPosition();

	// Left eye
	irr::core::vector3df v( fWidth, 0, 0 );
	irr::core::matrix4 m;
	m.setRotationDegrees( pCamera->getRotation() + irr::core::vector3df( 90.f, 0, 0 ) );
	m.transformVect( v );
	irr::core::vector3df leye = reye + v;

	// Eye target
	irr::core::vector3df oldt = pCamera->getTarget();
	irr::core::vector3df eyet = ( oldt - reye ).normalize() * fFocus;
	pCamera->setTarget( eyet );

	IDirect3DDevice9 *pDdx9 = NULL;
	if ( nDriverType == irr::video::EDT_DIRECT3D9
			  && pDriver->getExposedVideoData().D3D9.D3DDev9 )
	  pDdx9 = pDriver->getExposedVideoData().D3D9.D3DDev9;

	pDriver->beginScene( true, true, colBackground );

	if ( 0 )
		;

	// Setup right eye
#if defined( SQ_USE_OPENGL )

	else if ( nDriverType == irr::video::EDT_OPENGL )
	{   glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
	  	glColorMask( 0 != ( ulREyeKey & 0x00ff0000 ),
					 0 != ( ulREyeKey & 0x0000ff00 ),
					 0 != ( ulREyeKey & 0x000000ff ),
					 0 != ( ulREyeKey & 0xff000000 ) );
	} // end if

#endif

#if defined( _IRR_COMPILE_WITH_DIRECT3D_9_ ) && defined( OEX_ENABLE_DIRECTX )

	else if ( pDdx9 )
		pDdx9->SetRenderState( D3DRS_COLORWRITEENABLE,
							   ( ulREyeKey & 0x00ff0000 ? D3DCOLORWRITEENABLE_RED : 0 )
							   | ( ulREyeKey & 0x0000ff00 ? D3DCOLORWRITEENABLE_GREEN : 0 )
							   | ( ulREyeKey & 0x000000ff ? D3DCOLORWRITEENABLE_BLUE : 0 )
							   | ( ulREyeKey & 0xff000000 ? D3DCOLORWRITEENABLE_ALPHA : 0 ) );
#endif

	pSm->drawAll();

	if ( 0 )
		;

#if defined( SQ_USE_OPENGL )

	// Left Eye
	if ( nDriverType == irr::video::EDT_OPENGL )
	{   glClear( GL_DEPTH_BUFFER_BIT );
		glMatrixMode( GL_MODELVIEW );
	  	glLoadIdentity();
	  	glColorMask( 0 != ( ulLEyeKey & 0x00ff0000 ),
					 0 != ( ulLEyeKey & 0x0000ff00 ),
					 0 != ( ulLEyeKey & 0x000000ff ),
					 0 != ( ulLEyeKey & 0xff000000 ) );
	} // end if

#endif

#if defined( _IRR_COMPILE_WITH_DIRECT3D_9_ ) && defined( OEX_ENABLE_DIRECTX )

	else if ( pDdx9 )
	{   pDdx9->Clear(0 , 0 , D3DCLEAR_ZBUFFER, 0x000000 , 1.0f , 0);
	  	pDdx9->SetRenderState( D3DRS_COLORWRITEENABLE,
							   ( ulLEyeKey & 0x00ff0000 ? D3DCOLORWRITEENABLE_RED : 0 )
							   | ( ulLEyeKey & 0x0000ff00 ? D3DCOLORWRITEENABLE_GREEN : 0 )
							   | ( ulLEyeKey & 0x000000ff ? D3DCOLORWRITEENABLE_BLUE : 0 )
							   | ( ulLEyeKey & 0xff000000 ? D3DCOLORWRITEENABLE_ALPHA : 0 ) );
	} // end else if

#endif

	pCamera->setPosition( leye );
	pCamera->OnRegisterSceneNode();

	pSm->drawAll();

	if ( 0 )
		;

#if defined( SQ_USE_OPENGL )

	if ( nDriverType == irr::video::EDT_OPENGL )
		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

#endif

#if defined( _IRR_COMPILE_WITH_DIRECT3D_9_ ) && defined( OEX_ENABLE_DIRECTX )

	else if ( pDdx9 )
		pDdx9->SetRenderState( D3DRS_COLORWRITEENABLE,
							   D3DCOLORWRITEENABLE_BLUE
							   | D3DCOLORWRITEENABLE_GREEN
							   | D3DCOLORWRITEENABLE_RED
							   | D3DCOLORWRITEENABLE_ALPHA );
#endif

	if ( pGui )
		pGui->drawAll();

	pDriver->endScene();

	// Restore original position
	pCamera->setPosition( reye );
	pCamera->setTarget( oldt );

	return 1;
}

int CSqIrrlicht::AddSkyDome( const sqbind::stdString &sFile,
							 long lHorzRes, long lVertRes,
							 float fTxtPercent, float fSpherePercent )
{
	if ( !sFile.length() )
		return 0;

    irr::scene::ISceneNode *pNode =
        m_pSmgr->addSkyDomeSceneNode( m_pDriver->getTexture( sFile.c_str() ),
                                      lHorzRes, lVertRes, fTxtPercent, fSpherePercent );

    return 1;
}

CSqirrVector3d CSqIrrlicht::ScreenToPlane( CSqirrVector2d &ptScreen, float fDist )
{
    // Must have object
    if ( !m_pCamera || !m_pSmgr )
        return CSqirrVector3d();

    // Is mouse in the window?
    if ( 0 > ptScreen.Obj().X || 0 > ptScreen.Obj().Y )
        return CSqirrVector3d();

    // Update the camera view
    m_pCamera->OnRegisterSceneNode();

    // Get mouse line
    irr::core::line3df ml =
        m_pSmgr->getSceneCollisionManager()->
                    getRayFromScreenCoordinates( irr::core::position2di( (unsigned int)ptScreen.Obj().X, (unsigned int)ptScreen.Obj().Y ),
                                                 m_pCamera );

    // Get the camera target line
    irr::core::line3df cl( m_pCamera->getPosition(), m_pCamera->getTarget() );

    // Calculate the orthogonal plane position
    irr::core::vector3df pos = cl.start + ( cl.getVector().normalize() * fDist );

    // Calculate camera line plane intersection
	irr::core::vector3df ptWorld;
    if ( !irr::core::plane3df( pos, cl.getVector().normalize() ).
                    getIntersectionWithLine( ml.getMiddle(), ml.getVector(), ptWorld ) )
        return CSqirrVector3d();

    return ptWorld;
}

CSqirrNode CSqIrrlicht::NodeAtScreen( CSqirrVector2d &ptScreen, long lMask )
{
    if ( !m_pSmgr || !m_pCamera || 0 > ptScreen.x() || 0 > ptScreen.y() )
        return CSqirrNode();

    irr::core::position2d< irr::s32 > pos;
    pos.X = (int)ptScreen.x(); pos.Y = (int)ptScreen.y();

    irr::core::line3d< irr::f32 > line =
        m_pSmgr->getSceneCollisionManager()->getRayFromScreenCoordinates( pos, m_pCamera );

    // Which node was clicked
    irr::scene::ISceneNode *pNode =
        m_pSmgr->getSceneCollisionManager()->getSceneNodeFromRayBB( line, lMask );

    if ( !pNode )
        return CSqirrNode();

    // Drop if not visible
    if ( !pNode->isVisible() )
        pNode = 0;

    const irr::core::list< irr::scene::ISceneNode* >& children = m_pSmgr->getRootSceneNode()->getChildren();
    for ( irr::core::list< irr::scene::ISceneNode* >::ConstIterator it = children.begin();
          it != children.end(); ++it )
        if ( *it && *it != pNode && (*it)->isVisible() && 0 < (*it)->getID() && 0 != ( lMask & (*it)->getID() ) )
        {
            irr::core::aabbox3df bb = (*it)->getTransformedBoundingBox();
            if ( bb.intersectsWithLine( line ) )
                if ( !pNode || bb.intersectsWithBox( pNode->getTransformedBoundingBox() ) )
                    pNode = *it;

        } // end for

    // Ensure mask is correct
    if ( 0 > pNode->getID() || !( lMask & pNode->getID() ) )
        pNode = NULL;

	return pNode;
}

int CSqIrrlicht::InsertPoints(   irr::scene::IMeshSceneNode *pNode,
                                 irr::scene::SMeshBuffer **pMb,
                                 unsigned int lNewVertices, unsigned int lNewIndices,
                                 unsigned int &vi, unsigned int &ii )
{
    // Sanity check
    if ( !pNode || !pMb )
        return 0;

    // New mesh buffer
    *pMb = new irr::scene::SMeshBuffer();
    if ( !*pMb )
        return 0;

    // Existing mesh?
    irr::scene::IMeshBuffer *pOldMb = 0;
    if ( pNode->getMesh() && pNode->getMesh()->getMeshBufferCount() )
        pOldMb = pNode->getMesh()->getMeshBuffer( 0 );

    // Old buffer?
    if ( !pOldMb || 2 > pOldMb->getVertexCount() )
    {
        // Just add the two points
        (*pMb)->Vertices.set_used( lNewVertices );
        (*pMb)->Indices.set_used( lNewIndices );

        vi = 0;
        ii = 0;

        return 1;

    } // end else

    // Get current counts
    vi = pOldMb->getVertexCount();
    ii = pOldMb->getIndexCount();

    // Allocate new buffers
    (*pMb)->Vertices.set_used( vi + lNewVertices );
    (*pMb)->Indices.set_used( ii + lNewIndices );

    // Copy old data
    if ( vi )
//        memcpy( (*pMb)->getVertices(), pOldMb->getVertices(), pOldMb->getVertexPitch() * vi );
        memcpy( (*pMb)->getVertices(), pOldMb->getVertices(), irr::video::getVertexPitchFromType( pOldMb->getVertexType() ) * vi );
    if ( ii )
        memcpy( (*pMb)->getIndices(), pOldMb->getIndices(), sizeof( irr::u16 ) * ii );

    return 1;
}

int CSqIrrlicht::InsertPlane( irr::scene::IMeshSceneNode *pNode,
                               const irr::core::vector3df &tl, const irr::core::vector3df &tr,
                               const irr::core::vector3df &bl, const irr::core::vector3df &br,
                               irr::video::SColor &color, long lSides,
                               float minU, float maxU, float minV, float maxV )
{
    // Add new points
    unsigned int ovi = 0, oii = 0;
    irr::scene::SMeshBuffer *pMb = NULL;
    if ( !InsertPoints( pNode, &pMb, ( 1 < lSides ) ? 8 : 4, ( 1 < lSides ) ? 12 : 6, ovi, oii ) )
        return FALSE;

    unsigned int vi = ovi, ii = oii;

    pMb->Vertices[ vi++ ] = irr::video::S3DVertex( tl, irr::core::vector3df( 0, 0, 1 ), color, irr::core::vector2df( 1.f - minU, 1.f - maxV ) );
    pMb->Vertices[ vi++ ] = irr::video::S3DVertex( bl, irr::core::vector3df( 0, 0, 1 ), color, irr::core::vector2df( 1.f - minU, 1.f - minV ) );
    pMb->Vertices[ vi++ ] = irr::video::S3DVertex( br, irr::core::vector3df( 0, 0, 1 ), color, irr::core::vector2df( 1.f - maxU, 1.f - minV ) );
    pMb->Vertices[ vi++ ] = irr::video::S3DVertex( tr, irr::core::vector3df( 0, 0, 1 ), color, irr::core::vector2df( 1.f - maxU, 1.f - maxV ) );

    pMb->Indices[ ii++ ] = ovi + 1;
    pMb->Indices[ ii++ ] = ovi + 2;
    pMb->Indices[ ii++ ] = ovi + 3;
    pMb->Indices[ ii++ ] = ovi + 3;
    pMb->Indices[ ii++ ] = ovi + 0;
    pMb->Indices[ ii++ ] = ovi + 1;

    // Back side?
    if ( 1 < lSides )
    {
    pMb->Vertices[ vi++ ] = irr::video::S3DVertex( tl, irr::core::vector3df( 0, 0, -1 ), color, irr::core::vector2df( 1.f - minU, 1.f - maxV ) );
    pMb->Vertices[ vi++ ] = irr::video::S3DVertex( bl, irr::core::vector3df( 0, 0, -1 ), color, irr::core::vector2df( 1.f - minU, 1.f - minV ) );
    pMb->Vertices[ vi++ ] = irr::video::S3DVertex( br, irr::core::vector3df( 0, 0, -1 ), color, irr::core::vector2df( 1.f - maxU, 1.f - minV ) );
    pMb->Vertices[ vi++ ] = irr::video::S3DVertex( tr, irr::core::vector3df( 0, 0, -1 ), color, irr::core::vector2df( 1.f - maxU, 1.f - maxV ) );

        pMb->Indices[ ii++ ] = ovi + 4 + 1;
        pMb->Indices[ ii++ ] = ovi + 4 + 3;
        pMb->Indices[ ii++ ] = ovi + 4 + 2;
        pMb->Indices[ ii++ ] = ovi + 4 + 3;
        pMb->Indices[ ii++ ] = ovi + 4 + 1;
        pMb->Indices[ ii++ ] = ovi + 4 + 0;

    } // end if

    pMb->recalculateBoundingBox();

    irr::scene::SMesh *pMesh = new irr::scene::SMesh();
    if ( !pMesh ) return 0;

    pMesh->addMeshBuffer( pMb );
    pMb->drop();

    pMesh->recalculateBoundingBox();

    pNode->setMesh( pMesh );
    pMesh->drop();

    return 1;
}

CSqirrNode CSqIrrlicht::AddGrid( float fWidth, float fHeight,
                                 long lXPanels, long lYPanels,
                                 float fSpace, long lSides,
                                 CSqirrColor &rColor )
{
    if ( !m_pSmgr )
        return CSqirrNode();

    irr::scene::SMesh *pMesh = new irr::scene::SMesh();
    if ( !pMesh )
        return CSqirrNode();

    // Add empty mesh
    irr::scene::IMeshSceneNode *pNode =
        m_pSmgr->addMeshSceneNode( pMesh );
    pMesh->drop();

    if ( !pNode )
        return CSqirrNode();

    // Put a bunch of planes in the mesh
    float psize = 8;
    long sy = lXPanels;
    long sx = lYPanels;
    float xp, yp;
    float xs = fWidth - fSpace;
    float ys = fHeight - fSpace;

    for ( long y = 0; y < sy; y++ )
    {
        yp = ( y - ( sy / 2 ) ) * ys + ( y - ( sy / 2 ) ) * fSpace;

        for ( long x = 0; x < sx; x++ )
        {
            xp = ( x - ( sx / 2 ) ) * xs + ( x - ( sx / 2 ) ) * fSpace;

            InsertPlane( pNode, irr::core::vector3df( xp, yp + ys, 0 ),
                                irr::core::vector3df( xp + xs, yp + ys, 0 ),
                                irr::core::vector3df( xp, yp, 0 ),
                                irr::core::vector3df( xp + xs, yp, 0 ),
                                rColor.Obj(), lSides,
                                (float)x / (float)sx, (float)( x + 1 ) / (float)sx,
                                (float)y / (float)sy, (float)( y + 1 ) / (float)sy );
        } // end for

    } // end for

	return pNode;
}

/*

// Main entry point
function _init() : ( _g )
{
    local node = _irr.AddGrid( 10., 10., 10, 10, 0.2, 2
                               CSiColor( 255, 255, 255, 255 ), 2 );

    _irr.AddMeshAnimator( node, OnAnimate, 0 );

}

function _mod_fp( _x, _y )
{	local n = ( _x / _y ).tointeger();
	return _x - n * _y;
}

function OnAnimate( n, o, c )
{
//    _self.alert( n.tostring() );

    local pi = 3.141592654;
    local pi2 = pi * 2.;

    local attn = 6;
    local m = pi2 / 100;

    local clk = _irr.clock();

	for ( local i = 0; i < n; i++ )
	{
        // Normalize space
        local u = o.x( i ) * m;
//        local u = _mod_fp( o.x( i ) * m + clk, pi );
        local v = o.y( i ) * m;
//        local v = _mod_fp( o.y( i ) * m + clk, pi );
        local x = 0, y = 0, z = 0;

		// Cylinder
// 		x = sin( u );
//		y = ty;
//		z = cos( u );

		// Cone
//		x = sin( u ) * ( pi - v ) * 0.2;
//		y = v;
//		z = cos( u ) * ( pi - v ) * 0.2;

		// Sphere
//		x = sin( u ) * cos( v / 2 );
//		y = sin( v / 2 );
//		z = cos( u ) * cos( v / 2 );

		// Torus
//		x = ( 2 + cos( v ) ) * cos( u );
//		y = sin( v );
//		z = ( 2 + cos( v ) ) * sin( u );

		// Toroid
//		u += pi; u *= 1.5;
//		x = cos( u ) * ( u / ( 3 * pi ) * cos( v ) + 2 );
//		y = u * sin( v ) / ( 3 * pi );
//		z = sin( u ) * ( u / ( 3 * pi ) * cos( v ) + 2 );

		// Coil
//		x = cos( u * 1.5 ) * ( cos( v ) + 2 );
//		y = sin( u * 1.5 ) * ( cos( v ) + 2 );
//		z = sin( v ) + u;

		// Trefoil Knot
		local a = 0.5;
		u *= 2;
		x = a * ( cos( u ) * cos( v ) + 3 * cos( u ) * ( 1.5 + sin( 1.5 * u ) / 2 ) );
		y = a * ( sin( v ) + 2 * cos( 1.5 * u ) );
		z = a * ( sin( u ) * cos( v ) + 3 * sin( u ) * ( 1.5 + sin( 1.5 * u ) / 2 ) );

		// Nautilus
//		u += pi;
//		x = 0.5 * u * cos( u ) * ( cos( v ) + 1 );
//		y = 0.5 * u * sin( v );
//		z = 0.5 * u * sin( u ) * ( cos( v ) + 1 );

		// Mobius Strip
//		u += pi; // 0 <= u < 2pi
//		v *= 0.2; // -t <= v < t
//		x = cos( u ) + v * cos( u / 2 ) * cos( u );
//		y = sin( u ) + v * cos( u / 2 ) * sin( u );
//		z = v * sin( u / 2 );

		// Klein Bottle
//		local a = 2.0;
//		v += pi; // 0 <= v < 2pi
//		x = ( a + cos( u / 2 ) * sin( v ) - sin( u / 2 ) * sin( 2 * v ) ) * cos( u );
//		y = sin( u / 2 ) * sin( v ) + cos( u / 2 ) * sin( 2 * v );
//		z = ( a + cos( u / 2 ) * sin( v ) - sin( u / 2 ) * sin( 2 * v ) ) * sin( u );

		// Dini's surface
//		u += pi; u *= 2; // 0 <= u < 4pi
//		x = cos( u ) * sin( v );
//		y = -2 - ( cos( v ) + log( tan( v / 2 ) ) + 0.2 * u - 4 );
//		z = sin( u ) * sin( v );

		// Flag
//		 x = u;
//		 y = v;
//		 z = 0;

		// Waving
//        z += sin( u * 2. + clk / 2 ) + sin( u + ( clk * 2.2 ) );
//        z += sin( v * 1. + clk ) + sin( v * 1.1 + ( clk * 1.2 ) );
//        z *= ( ( u - pi ) / pi2 ) / attn;

        c.set( i, x / m, y / m, z / m );

    } // end for

}

*/
int CSqIrrlicht::AddMeshAnimator( sqbind::CSqEngineExport *e, CSqirrNode &n, SquirrelObject soF, long lFreq )
{
    if ( !m_pSmgr || !n.IsValid() || !e || !e->GetVmPtr() )
        return 0;

    if ( !lFreq )
    {
        CSqirrMeshAnimator ma;
        if ( !ma.Init( (irr::scene::IMeshSceneNode*)n.Ptr(), soF, e->GetVmPtr()->GetVMHandle(), 0 ) )
            return 0;

        // Let Squirrel manipulate the mesh
        ma.Run( m_pSmgr, oexGetBootSeconds(), e->GetEnginePtr() );

        // Just leave the mesh as is
        ma.Detach();

    } // end if

    else
    {
		CSqirrMeshAnimator &rMa = m_lstMeshAnimators[ &n ];

        if ( !rMa.Init( (irr::scene::IMeshSceneNode*)n.Ptr(), soF, e->GetVmPtr()->GetVMHandle(), lFreq ) )
        {   m_lstMeshAnimators.erase( &n );
            return 0;
        } // end if

        // Initialize the mesh
        rMa.Run( m_pSmgr, oexGetBootSeconds(), e->GetEnginePtr() );

    } // end else

    return 1;
}

void CSqIrrlicht::AnimateMeshes()
{
	if ( !m_pSmgr )
		return;

	// For each mesh animator
	for ( t_MeshAnimators::iterator it = m_lstMeshAnimators.begin();
			m_lstMeshAnimators.end() != it; it++ )
		it->second.Run( m_pSmgr, oexGetBootSeconds(), oexNULL );
}
