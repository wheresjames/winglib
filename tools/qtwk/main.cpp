
#include "stdafx.h"
#include "network.h"
#include "mainwindow.h"

#if defined(_WIN32_WCE)
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
//#elif defined( OEX_GUI ) // Added qtmain.lib ref to windows.mk instead ;)
//int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, char* pCmdLine, int nCmdShow )
#else
int main( int argc, char* argv[] )
#endif
{
	// Init oexlib
	oexINIT();
	oexInitResources();

	// Initialize application object
	QApplication app( argc, argv );

	// Create main window
	CMainWindow *pWindow = new CMainWindow;
	if ( !pWindow )
		return -1;

	// Show the window
	pWindow->show();

	// Run the app
	int res = app.exec();

	oexUNINIT();

	return res;
}
