// sq_irrlicht.h

#pragma once

class CSqIrrlicht
{
public:

	/// Default constructor
	CSqIrrlicht();

	~CSqIrrlicht();

	// Copy semantics
	CSqIrrlicht( const CSqIrrlicht &r ) {}
	CSqIrrlicht& operator =( const CSqIrrlicht &r ) { return *this; }

	/// Initializes irrlicht engine
	int Init( const sqbind::stdString &sName, int width, int height );

	/// Draws the scene
	int Draw();

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

