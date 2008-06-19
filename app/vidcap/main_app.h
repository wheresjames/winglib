#pragma once

//==================================================================
// CMainApp
//
/// Main application class
/**
	
*/
//==================================================================
class CMainApp : public wxApp
{

public:

	/// Constructor
	CMainApp();

	/// Destructor
	virtual ~CMainApp();

	//==============================================================
	// OnInit()
	//==============================================================
	/// WxWidgets initialization overload
    virtual bool OnInit();


};

DECLARE_APP( CMainApp )


