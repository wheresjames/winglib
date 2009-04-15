// sq_irrlicht.h

#pragma once

class CSqIrrlicht
{
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
	int Init( const sqbind::stdString &sName, int width, int height );

	/// Draws the scene
	int Draw( CSqirrColor &bg );

	/// Draws the sceen in anaglyphic stereo
	static int DrawAnaglyph( irr::video::IVideoDriver *pDriver,
							 irr::scene::ISceneManager *pSm,
							 irr::scene::ICameraSceneNode *pCamera,
							 irr::video::SColor colBackground,
							 float fWidth, float fFocus, int nDriverType,
							 unsigned long ulREyeKey, unsigned long ulLEyeKey );

	/// Adds a sky dome to the scene
	int AddSkyDome( const sqbind::stdString &sFile,
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

public:

	/// Convert screen coords into world coords
	CSqirrVector3d ScreenToPlane( CSqirrVector2d &ptScreen, float fDist );

	CSqirrNode NodeAtScreen( CSqirrVector2d &ptScreen, long lMask );

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

	CSqirrNode AddGrid( float fWidth, float fHeight,
						long lXPanels, long lYPanels,
						float fSpace, long lSides,
						CSqirrColor &rColor );

	int AddMeshAnimator( sqbind::CSqEngineExport *e, CSqirrNode &n, SquirrelObject soF, long lFreq );

public:

	void SetStereo( int b ) { m_bStereo = b; }
	int GetStereo() { return m_bStereo; }

	void SetREyeKey( unsigned long ul ) { m_ulREyeKey = ul; }
	int GetREyeKey() { return m_ulREyeKey; }

	void SetLEyeKey( unsigned long ul ) { m_ulLEyeKey = ul; }
	int GetLEyeKey() { return m_ulLEyeKey; }

private:

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

	/// Type of driver in use
	int								m_nDriverType;

	/// Background color
	irr::video::SColor 				m_colBackground;

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

};

_SQBIND_DECLARE_INSTANCE( CSqIrrlicht, CSqIrrlicht );

