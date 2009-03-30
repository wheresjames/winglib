// sq_irrlicht.h

#pragma once

class CSqIrrlicht
{
public:

	/// Default constructor
	CSqIrrlicht()
	{	m_pDevice = oexNULL;		
	}
	
	~CSqIrrlicht();

	// Initializes irrlicht engine
	int Init( int width, int height );
	
	// Draws the scene
	int Draw();

	// Copy semantics
	CSqIrrlicht( const CSqIrrlicht &r ) {}
	CSqIrrlicht& operator =( const CSqIrrlicht &r ) { return *this; }

private:

	/// Irrlicht device
	irr::IrrlichtDevice		*m_pDevice;

};

