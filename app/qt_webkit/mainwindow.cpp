
#include "stdafx.h"
#include "network.h"
#include "mainwindow.h"

CMainWindow::CMainWindow()
{
	// Create web view
	m_pView = new QWebView( this );
	if ( m_pView.isNull() )
		throw;

	// Create custom network manager
	m_pNet = new CNetworkMgr( this, m_pView->page()->networkAccessManager() );
	if ( m_pNet.isNull() )
		throw;

	// Set window title
	setWindowTitle( OEX_PROJECT_DESC );

	// Set our custom network manager
	m_pView->page()->setNetworkAccessManager( m_pNet );

	// Set the window size
	setFixedSize( 800, 440 );

	// No scrollbars
	m_pView->page()->mainFrame()->setScrollBarPolicy( Qt::Vertical,Qt::ScrollBarAlwaysOff );
	m_pView->page()->mainFrame()->setScrollBarPolicy( Qt::Horizontal,Qt::ScrollBarAlwaysOff );

	// No context menu
	m_pView->setContextMenuPolicy( Qt::PreventContextMenu );

	// Enable cross scripting
	m_pView->settings()->setAttribute( QWebSettings::XSSAuditingEnabled, 0 );
	m_pView->settings()->setAttribute( QWebSettings::LocalContentCanAccessRemoteUrls, 1 );

	// Make the keyboard easier to use
	m_pView->settings()->setAttribute( QWebSettings::SpatialNavigationEnabled, 1 );

	// Start the web view
	setCentralWidget( m_pView );

	// Load the home page
	m_pView->load( QUrl( "http://embedded/index.htm" ) );

}

