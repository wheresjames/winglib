

#if 0 // defined( _WIN32_WCE )

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

class CSampleSceneNode : public ISceneNode
{
	aabbox3d<f32> Box;
	S3DVertex Vertices[4];
	SMaterial Material;
public:

	CSampleSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
		: ISceneNode(parent, mgr, id)
	{
		Material.Wireframe = false;
		Material.Lighting = false;

		Vertices[0] = S3DVertex(0,0,10, 1,1,0, SColor(255,0,255,255), 0, 1);
		Vertices[1] = S3DVertex(10,0,-10, 1,0,0, SColor(255,255,0,255), 1, 1);
		Vertices[2] = S3DVertex(0,20,0, 0,1,1, SColor(255,255,255,0), 1, 0);
		Vertices[3] = S3DVertex(-10,0,-10, 0,0,1, SColor(255,0,255,0), 0, 0);
		Box.reset(Vertices[0].Pos);
		for (s32 i=1; i<4; ++i)
			Box.addInternalPoint(Vertices[i].Pos);
	}
	virtual void OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}

	virtual void render()
	{
		u16 indices[] = {	0,2,3, 2,1,3, 1,0,3, 2,0,1	};
		IVideoDriver* driver = SceneManager->getVideoDriver();

		driver->setMaterial(Material);
		driver->setTransform(ETS_WORLD, AbsoluteTransformation);
		driver->drawIndexedTriangleList(&Vertices[0], 4, &indices[0], 4);
	}

	virtual const aabbox3d<f32>& getBoundingBox() const
	{
		return Box;
	}

	virtual u32 getMaterialCount()
	{
		return 1;
	}

	virtual SMaterial& getMaterial(u32 i)
	{
		return Material;
	}	
};


#if defined(_WIN32_WCE)
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine,
                        int nCmdShow )
#else
int main()
#endif
{
	//E_DRIVER_TYPE driverType = EDT_BURNINGSVIDEO;
	E_DRIVER_TYPE driverType = EDT_OPENGL;

	// create device

	IrrlichtDevice *device =
		createDevice(driverType, dimension2d<u32>(240, 320), 16, true );
		
	if (device == 0)
		return 1; // could not create selected driver.

	// create engine and camera
	MyEventReceiver receiver(device);
	device->setEventReceiver(&receiver);
	device->setWindowCaption(L"Irrlicht CE Demo");

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

	// set the filesystem relative to the executable
	{
		wchar_t buf[255];
		GetModuleFileNameW ( 0, buf, 255 );

		stringc base = buf;
		base = base.subString ( 0, base.findLast ( '\\' ) );
		device->getFileSystem()->addFolderFileArchive ( base.c_str() );
	}


	IGUIStaticText *text = 
	guienv->addStaticText(L"FPS: 25",
		rect<s32>(60,5,200,20), false );

	guienv->addButton(core::rect<int>(10,5,50,20), 0, 2, L"Quit");

	smgr->addCameraSceneNode(0, vector3df(0,-40,0), vector3df(0,0,0));

	CSampleSceneNode *myNode = 
		new CSampleSceneNode(smgr->getRootSceneNode(), smgr, 666);

	ISceneNodeAnimator* anim = 
		smgr->createRotationAnimator(vector3df(0.8f, 0, 0.8f));

	if(anim)
	{
		myNode->addAnimator(anim);
		anim->drop();
		anim = 0; // As I shouldn't refer to it again, ensure that I can't
	}

	myNode->drop();
	myNode = 0; // As I shouldn't refer to it again, ensure that I can't

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

/// Irrlicht device
irr::IrrlichtDevice		*g_pDevice = oexNULL;

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

	g_pDevice = irr::createDevice( irr::video::EDT_OPENGL, 
								   irr::core::dimension2d< irr::u32 >( 120, 160 ), 
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

#else

	irr::SIrrlichtCreationParameters param;
	oexZeroMemory( &param, sizeof( param ) );	
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

	} // end else

	return 1;
}

int Draw()
{
	if ( !g_pDevice || !g_pDevice->run() )
		return -1;

	g_pDevice->getVideoDriver()->beginScene( true, true, irr::video::SColor( 255, 100, 101, 140 ) );
	g_pDevice->getSceneManager()->drawAll();
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
	Init( 320, 240 );

	// Run the scene	
	while ( !Draw() )
		oexSleep( 30 );

	// Uninitialize the oex library
    oexUNINIT();

	return 0;
}

#endif

