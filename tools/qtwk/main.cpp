
#include "stdafx.h"
#include "network.h"
#include "mainwindow.h"


// +++ Fix keyboard focus
// +++ Chat window

int main(int argc, char ** argv)
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
