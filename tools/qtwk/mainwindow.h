
#pragma once

//#ifdef Q_MOC_RUN
#	include "stdafx.h"
#	include "network.h"
//#endif

class CMainWindow : public QMainWindow
{
	Q_OBJECT

public:

	/// Constructor
	CMainWindow();

private:

	/// Web view
	QPointer< QWebView > m_pView;

	/// Custom network object
	QPointer< CNetworkMgr > m_pNet;

};
