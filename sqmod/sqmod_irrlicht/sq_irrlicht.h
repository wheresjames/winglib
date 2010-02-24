// sq_irrlicht.h

#pragma once

class CSqIrrlicht
{
public:

	/// Mesh animator list type
	typedef oexStdMap( CSqirrNode*, CSqirrMeshAnimator ) t_MeshAnimators;

public:

	class CEventReceiver : public irr::IEventReceiver
	{
	public:

		CEventReceiver() { m_pIrr = oexNULL; }
		CEventReceiver ( CSqIrrlicht *device, int *pQuit )
			: m_pIrr( device ) {}

		void SetDevice( CSqIrrlicht *device, int *pQuit )
		{	m_pIrr = device; }

		virtual bool OnEvent( const irr::SEvent& rEvent )
		{
			// Let irrlicht in on it
			if ( m_pIrr )
				m_pIrr->OnEvent( rEvent );

			return false;
		}

	private:

		/// Pointer to irrlicht device
		CSqIrrlicht			*m_pIrr;

	};

public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqIrrlicht )
	_SQBIND_CLASS_CTOR_END( CSqIrrlicht )

	/// Default constructor
	CSqIrrlicht();

	~CSqIrrlicht();

	// Copy semantics
	CSqIrrlicht( const CSqIrrlicht &r ) {}
	CSqIrrlicht& operator =( const CSqIrrlicht &r ) { return *this; }

	/// Initializes irrlicht engine
	int Init( const sqbind::stdString &sName, int width, int height, int fps );

	/// Draws the scene
	int Draw( CSqirrColor &bg );

	/// Draws the scene to a texture
	int RenderToTexture( CSqirrTexture *txt, CSqirrNode *pCamera );

	/// Captures the current frame buffer data
	int Capture( sqbind::CSqBinary *pBin );

	/// Draws the sceen in anaglyphic stereo
	static int DrawAnaglyph( irr::video::IVideoDriver *pDriver,
							 irr::scene::ISceneManager *pSm,
							 irr::gui::IGUIEnvironment *pGui,
							 irr::scene::ICameraSceneNode *pCamera,
							 irr::video::SColor colBackground,
							 float fWidth, float fFocus, int nDriverType,
							 unsigned long ulREyeKey, unsigned long ulLEyeKey );

	/// Adds a sky dome to the scene
	CSqirrNode AddSkyDome( const sqbind::stdString &sFile,
						   long lHorzRes, long lVertRes,
						   float fTxtPercent, float fSpherePercent );

	/// Sets the ambient color lighting
	int SetAmbientLight( CSqirrColorf &x_col );

	/// Adds a light to the scene
	CSqirrNode AddLight( CSqirrVector3d &pos, CSqirrColorf &col, float rad );

	/// Adds a camera to the scene
	CSqirrCamera AddCamera( CSqirrVector3d &pos, CSqirrVector3d &look );

	/// Adds a cube
	CSqirrNode AddCube( float size );

	/// Creates a rotation animator
	CSqirrAnimator AddRotateAnimator( CSqirrVector3d &speed );

	/// Adds a mesh
	CSqirrNode AddMesh( const sqbind::stdString &sFile, float x_fScale, int x_bClearFromCache );

	/// Sets the vertex colors on a mesh
	int SetVertexColors( CSqirrNode &x_node, CSqirrColor &x_col );

	/// Sets the vertex alpha
	int SetVertexColorAlpha( CSqirrNode &x_node, float x_fAlpha );

	/// Inserts a sphere mesh
	CSqirrNode AddSphereMesh( float fWidth, float fHeight, long lPoints );

	/// Inserts a cylinder mesh
	CSqirrNode AddCylinderMesh( float fWidth, float fHeight, long lPoints );

	/// Inserts a cone mesh
	CSqirrNode AddConeMesh( float fWidth, float fHeight, long lPoints );

	/// Inserts an arrow mesh
	CSqirrNode AddArrowMesh( float fWidth, float fConeHeight, float fShaftHeight, long lPoints );

public:

	/// Convert screen coords into world coords
	CSqirrVector3d ScreenToPlane( CSqirrVector2d &ptScreen, float fDist );

	/// Returns the first node at the given screen position
	CSqirrNode NodeAtScreen( CSqirrVector2d &ptScreen, long lMask );

	/// Returns the distance from the camera to the specified node
	float getNodeDist( CSqirrNode &v );

public:

	/// Appends specified number of vertices and indices to the end
	/// of a mesh.
	static int InsertPoints( irr::scene::IMeshSceneNode *pNode,
							 irr::scene::SMeshBuffer **pMb,
							 unsigned int lNewVertices, unsigned int lNewIndices,
							 unsigned int &vi, unsigned int &ii );

	/// Inserts a plane into the mesh
	static int InsertPlane( irr::scene::IMeshSceneNode *pNode,
							const irr::core::vector3df &tl, const irr::core::vector3df &tr,
							const irr::core::vector3df &bl, const irr::core::vector3df &br,
							irr::video::SColor &color, long lSides,
							float minU = 0, float maxU = 1, float minV = 0, float maxV = 1 );

	/// Inserts a cylinder intot the mesh
	static int InsertCylinder( irr::scene::SMeshBuffer *pMb, const irr::core::vector3df &center,
							   float fWidth, float fHeight, long lPoints, const irr::video::SColor &color );


	/// Inserts a sphere into a mesh
	static int InsertSphere( irr::scene::SMeshBuffer *pMb, const irr::core::vector3df &center,
							 float fWidth, float fHeight, long lPoints, const irr::video::SColor &color );

	/// Inserts a cone into a mesh
	static int InsertCone( irr::scene::SMeshBuffer *pMb, irr::core::vector3df center,
						   float fWidth, float fHeight, long lPoints, irr::video::SColor &color );


	/// Adds a sphere
	CSqirrNode AddSphere( float fRadius, long lPolyCount );


	/// Adds a grid node
	CSqirrNode AddGrid( float fWidth, float fHeight,
						long lXPanels, long lYPanels,
						float fSpace, long lSides,
						CSqirrColor &rColor );

	/// Adds a plane
	CSqirrNode AddPlane( float fWidth, float fHeight, long lSides, CSqirrColor &rColor );

	/// Creates a mesh animator
	int AddMeshAnimator( sqbind::CSqEngineExport *e, CSqirrNode &n, SquirrelObject soF, long lFreq );

	/// Runs mesh animators
	void AnimateMeshes();

	/// Loads a texture from a disk image
	CSqirrTexture LoadTexture( const sqbind::stdString &sFile, int bMipMapping );

	/// Create texture
	CSqirrTexture CreateTexture( long lWidth, long lHeight, int bMipMapping );

	/// Creates a texture suitable for rendering
	CSqirrTexture CreateRenderTexture( long lWidth, long lHeight, int bMipMapping );

	/// Called when an event occurs
	int OnEvent( const irr::SEvent& rEvent );

public:

	/// Gets the intersect between a line and a plane
	int getLineIntersect( CSqirrLine &line, CSqirrVector3d &pt, CSqirrVector3d &axis, CSqirrVector3d *intersect );

	/// Get screen intersect point
	int getIntersect( CSqirrVector2d &ptScreen, CSqirrVector3d &pt, CSqirrVector3d &axis, CSqirrVector3d *intersect );

	/// Returns the intersection between the mouse and the specified plane
	int getMouseIntersect( CSqirrVector2d &ptScreen, CSqirrVector3d &pt, CSqirrVector3d &axis, CSqirrVector3d *intersect );

	/// Returns the specified pick point
	int getPickPoint( CSqirrVector2d &ptScreen, CSqirrVector3d &pt, CSqirrVector3d *intersect );

	/// Gets a nodes screen position
	CSqirrVector2d getNodeScreenPos( CSqirrNode &rNode );

	/// Gets a screen position
	CSqirrVector2d getScreenPos( CSqirrVector3d &v );

	/// Converts a 2d screen point into a world position
	int screenToWorld( CSqirrVector2d &ptScreen, CSqirrVector3d *ptWorld, float fDist );

	/// Converts a 2d rectangle into a world position
	int screenToWorldBox( CSqirrRect2d &rcScreen, CSqirrBoundingBox3d *bbWorld, float fDist );

	/// Converts a 2d screen point into a plane orthogonal to the camera
	int screenToPlane( CSqirrVector2d &ptScreen, CSqirrVector3d *ptWorld, float fDist );

	/// Scales the node to fill the specified volume
	int FillVolume( CSqirrNode &rNode, CSqirrBoundingBox3d &rBb );

	/// Return default camera
	CSqirrCamera getCamera() 
	{	if ( !m_pCamera ) return CSqirrCamera(); return m_pCamera; }

public:

	void SetStereo( int b ) { m_bStereo = b; }
	int GetStereo() { return m_bStereo; }

	void SetREyeKey( unsigned long ul ) { m_ulREyeKey = ul; }
	int GetREyeKey() { return m_ulREyeKey; }

	void SetLEyeKey( unsigned long ul ) { m_ulLEyeKey = ul; }
	int GetLEyeKey() { return m_ulLEyeKey; }

	// Sets the callback queue
	void SetCallback( sqbind::CSqMsgQueue *x_pMsgQueue, const sqbind::stdString &x_sFunction )
	{	m_pCallbackQueue = x_pMsgQueue; m_sCallbackFunction = x_sFunction; }

	int getWidth() { return m_nWidth; }
	int getHeight() { return m_nHeight; }

private:

	/// Default event receiver
	CEventReceiver					m_er;

	/// Irrlicht device
	irr::IrrlichtDevice				*m_pDevice;

	/// Scene manager
	irr::scene::ISceneManager		*m_pSmgr;

	/// Scene driver
	irr::video::IVideoDriver		*m_pDriver;

	/// GUI
	irr::gui::IGUIEnvironment 		*m_pGui;

	/// Main scene camera
	irr::scene::ICameraSceneNode	*m_pCamera;

	/// Display width
	int								m_nWidth;

	/// Display height
	int								m_nHeight;

	/// Type of driver in use
	int								m_nDriverType;

	/// Number of frames that have been processed
	oex::oexINT64					m_llFrames;

	/// Time of last run
	float							m_fLastTime;

	/// Background color
	irr::video::SColor 				m_colBackground;

	/// Fps count
	irr::gui::IGUIStaticText 		*m_txtFps;

	/// Non-zero to enable anaglyphic stereo mode
	int								m_bStereo;

	/// Right eye key for a3d
	unsigned long					m_ulREyeKey;

	/// Left eye key for a3d
	unsigned long					m_ulLEyeKey;

	/// Stereo eye width
	float							m_fStereoWidth;

	/// Stereo focal distance
	float							m_fStereoFocus;

	/// Mesh animators
	t_MeshAnimators                 m_lstMeshAnimators;

	/// World physics
	CSqirrPhysics					m_cPhysics;

	/// Non-zero if engine should quit
	int								m_bQuit;

	/// Callback queue
	sqbind::CSqMsgQueue				*m_pCallbackQueue;

	/// Callback path
	sqbind::stdString				m_sCallbackFunction;

};

_SQBIND_DECLARE_INSTANCE( CSqIrrlicht, CSqIrrlicht );

