
#if defined( _WIN32_WCE )

#include "stdafx.h"
#include <windows.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class MyEventReceiver : public IEventReceiver
{
private:
	IrrlichtDevice *Device;
public:
	MyEventReceiver ( IrrlichtDevice *device ): Device ( device ) {}

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();

			switch(event.GUIEvent.EventType)
			{
				case EGET_BUTTON_CLICKED:
				if (id == 2)
				{
					Device->closeDevice();
					return true;
				} break;
			}
		}

		return false;
	}
};

#if defined(_WIN32_WCE)
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine,
                        int nCmdShow )
#else
int main( int argc, char* argv[] )
#endif
{

	IrrlichtDevice *device = 0;

#if defined( _WIN32_WCE )
	device = createDevice( EDT_BURNINGSVIDEO, dimension2d<u32>(240, 320), 16, true );
#elif defined( OEX_IPHONE )
	device = createDevice( EDT_OGLES1, dimension2d<u32>(240, 320), 16, true );
#else
	device = createDevice( EDT_OPENGL, dimension2d<s32>(240, 320), 16, true );
#endif

	// create device
	if (device == 0)
		return 1; // could not create selected driver.

	// create engine and camera
	MyEventReceiver receiver(device);
	device->setEventReceiver(&receiver);
	device->setWindowCaption(L"Irrlicht CE Demo");

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

#if defined( _WIN32_WCE )
	// set the filesystem relative to the executable
	{
		wchar_t buf[255];
		GetModuleFileNameW ( 0, buf, 255 );

		stringc base = buf;
		base = base.subString ( 0, base.findLast ( '\\' ) );
		device->getFileSystem()->addFolderFileArchive ( base.c_str() );
	}
#endif

	IGUIStaticText *text =
	guienv->addStaticText(L"FPS: 25",
		rect<s32>(60,5,200,20), false );

	guienv->addButton(core::rect<int>(10,5,50,20), 0, 2, L"Quit");

	irr::scene::ICameraSceneNode *camera;
	{ // Create scene

	    driver->setAmbientLight( irr::video::SColorf( .5f, .5f, .5f ) );
		smgr->addLightSceneNode( 0, irr::core::vector3df( 0, 100, 0 ),
								 irr::video::SColorf( 0.5f, 0.5f, 0.5f ), 100 );
		smgr->addLightSceneNode( 0, irr::core::vector3df( 0, 100, -50 ),
								 irr::video::SColorf( 0.5f, 0.5f, 0.5f ), 100 );

		camera =
			smgr->addCameraSceneNode( 0, irr::core::vector3df( 0, 30, -40 ),
									  irr::core::vector3df( 0, 5, 0 ) );

		irr::scene::ISceneNode *node = smgr->addCubeSceneNode( 10.f );
		if ( node )
		{
			irr::scene::ISceneNodeAnimator* rotate =
				smgr->createRotationAnimator( irr::core::vector3df( 0, 0.4f, 0 ) );

			node->addAnimator( rotate );

			rotate->drop();

		} // end if

	} // end Create scene

	u32 frames=0;
	while(device->run())
	{
		driver->beginScene(true, true, SColor(0,100,100,100));
		smgr->drawAll();
		guienv->drawAll();
		driver->endScene();

		if (++frames==10)
		{
			stringw str = L"FPS: ";
			str += (s32)driver->getFPS();

			text->setText ( str.c_str() );
			frames=0;
		}
	}

	device->drop();

	return 0;
}

#else

#include "stdafx.h"

#if defined( _WIN32_WCE )
#include <windows.h>
#endif

class MyEventReceiver : public irr::IEventReceiver
{
private:
	irr::IrrlichtDevice *Device;
public:
	MyEventReceiver ( irr::IrrlichtDevice *device ): Device ( device ) {}

	virtual bool OnEvent(const irr::SEvent& event)
	{
		if (event.EventType == irr::EET_GUI_EVENT)
		{
			irr::s32 id = event.GUIEvent.Caller->getID();

			switch(event.GUIEvent.EventType)
			{
				case irr::gui::EGET_BUTTON_CLICKED:
				if (id == 2)
				{
					Device->closeDevice();
					return true;
				} break;
			}
		}

		return false;
	}
};


/// Irrlicht device
irr::IrrlichtDevice			*g_pDevice = oexNULL;
irr::gui::IGUIStaticText 	*g_text;
int 						g_frames = 0;
MyEventReceiver				*g_receiver = oexNULL;

void Close()
{
	if ( g_pDevice )
	{	g_pDevice->closeDevice();
		g_pDevice->drop();
		g_pDevice = oexNULL;
	} // end if
}

int Init( int width, int height )
{

#if defined( _WIN32_WCE )

	g_pDevice = irr::createDevice( irr::video::EDT_BURNINGSVIDEO,
								   irr::core::dimension2d< irr::u32 >( 320, 240 ),
								   16, true );

//	g_pDevice = irr::createDevice( irr::video::EDT_OGLES1, irr::core::dimension2d<irr::u32>(width, height),
//	     						   16, true, false, false, 0 );


//	irr::SIrrlichtCreationParameters param;
//	oexZeroMemory( &param, sizeof( param ) );
//	param.Bits = 16;
//	param.WindowSize = irr::core::dimension2d<irr::u32>( width, height );
//	param.DriverType = irr::video::EDT_OGLES1;
//	param.DriverType = irr::video::EDT_SOFTWARE;
//	g_pDevice = irr::createDeviceEx( param );

#elif defined( OEX_IPHONE )

	irr::SIrrlichtCreationParameters param;
	param.Bits = 16;
	param.AntiAlias = true;
	param.WindowSize = irr::core::dimension2d<irr::u32>( width, height );
	param.DriverType = irr::video::EDT_OGLES1;
	g_pDevice = irr::createDeviceEx( param );

#else

	irr::SIrrlichtCreationParameters param;
	param.Bits = 16;
	param.AntiAlias = true;
	param.WindowSize = irr::core::dimension2d<irr::s32>( width, height );
	param.DriverType = irr::video::EDT_OPENGL;
	g_pDevice = irr::createDeviceEx( param );

#endif



//	m_pDevice = irr::createDevice( irr::video::EDT_OPENGL, irr::core::dimension2d<irr::s32>( 640, 480 ), 16,
//				   			  false, false, false, 0 );

	if ( !g_pDevice )
		oexERROR( 0, oexT( "Failed to create irrlicht device" ) );

	else
	{
//		m_pDevice->setWindowCaption( oexT( "Irrlicht Engine" ) );

		irr::scene::ISceneManager *smgr = g_pDevice->getSceneManager();
		irr::video::IVideoDriver *driver = g_pDevice->getVideoDriver();
		irr::gui::IGUIEnvironment* guienv = g_pDevice->getGUIEnvironment();

		g_receiver = new MyEventReceiver(g_pDevice);
		g_pDevice->setEventReceiver(g_receiver);
		g_pDevice->setWindowCaption(L"Irrlicht Demo");


		g_text = guienv->addStaticText(L"FPS: xx",irr::core::rect<irr::s32>(60,5,200,20), false );
		guienv->addButton(irr::core::rect<int>(10,5,50,20), 0, 2, L"Quit");


		{ // Create scene

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
			irr::scene::ISceneNodeAnimator* rotate =
				smgr->createRotationAnimator( irr::core::vector3df( 0, 0.4f, 0 ) );

			node->addAnimator( rotate );

			rotate->drop();

		} // end if

		} // end Create Scene

	} // end else

	return 1;
}

int Draw()
{
	if ( !g_pDevice || !g_pDevice->run() )
		return -1;

	if (++g_frames==10)
	{
		irr::core::stringw str = L"FPS: ";
		str += (int)g_pDevice->getVideoDriver()->getFPS();

		g_text->setText ( str.c_str() );
		g_frames=0;
	}

	g_pDevice->getVideoDriver()->beginScene( true, true, irr::video::SColor( 255, 100, 101, 140 ) );
	g_pDevice->getSceneManager()->drawAll();
	g_pDevice->getGUIEnvironment()->drawAll();
	g_pDevice->getVideoDriver()->endScene();


	return 0;
}

#if defined(_WIN32_WCE)
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
#else
int main( int argc, char* argv[] )
#endif
{
    // Initialize the oex library
	oexINIT();

	// Init irrilcht device
	Init( 240, 320 );

	// Run the scene
	while ( !Draw() )
		;
//		oexSleep( 30 );

	if ( g_receiver )
		delete g_receiver;
	g_receiver = oexNULL;

	// Uninitialize the oex library
    oexUNINIT();

	return 0;
}

#endif

