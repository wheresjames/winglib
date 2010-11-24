// sq_quickfix.cpp

#include "stdafx.h"


CSqQuickfix::CSqQuickfix()
{_STT();

	m_pSs = oexNULL;
	m_pApp = oexNULL;
	m_pFsf = oexNULL;
	m_pLog = oexNULL;
	m_pSa = oexNULL;

}

CSqQuickfix::~CSqQuickfix()
{_STT();
	Destroy();
}

void CSqQuickfix::Destroy()
{_STT();

	if ( m_pSa )
		OexAllocDelete( m_pSa );

	if ( m_pLog )
		OexAllocDelete( m_pLog );

	if ( m_pFsf )
		OexAllocDelete( m_pFsf );

	// +++
//	if ( m_pApp )
//		OexAllocDelete( m_pApp );

	if ( m_pSs )
		OexAllocDelete( m_pSs );

	m_pSa = oexNULL;
	m_pLog = oexNULL;
	m_pFsf = oexNULL;
	m_pApp = oexNULL;
	m_pSs = oexNULL;
}

int CSqQuickfix::Init( const sqbind::stdString &sSettings )
{_STT();

	// Out with the old
	Destroy();

	try
	{

		// Create settings object
		m_pSs = OexAllocConstruct< FIX::SessionSettings >();
		if ( !m_pSs ) 
		{	m_sLastError = oexT( "Failed to allocate memory for FIX::SessionSettings" );
			Destroy();
			return 0;
		} // end if

		m_pFsf = OexAllocConstruct< FIX::FileStoreFactory >( *m_pSs );
		if ( !m_pFsf ) 
		{	m_sLastError = oexT( "Failed to allocate memory for FIX::FileStoreFactory" );
			Destroy();
			return 0;
		} // end if

		m_pLog = OexAllocConstruct< FIX::ScreenLogFactory >( *m_pSs );
		if ( !m_pLog ) 
		{	m_sLastError = oexT( "Failed to allocate memory for FIX::ScreenLogFactory" );
			Destroy();
			return 0;
		} // end if

		// +++ Need to create this class
		if ( !m_pApp ) 
			return 1;
//		{	m_sLastError = oexT( "FIX::Application object is invalid" );
//			Destroy();
//			return 0;
//		} // end if

//		m_pSa = OexAllocConstruct< FIX::SocketAcceptor >( *m_pApp, *m_pFsf, *m_pSs, *m_pLog );
//		if ( !m_pSa ) 
//		{	m_sLastError = oexT( "Failed to allocate memory for FIX::SocketAcceptor" );
//			Destroy();
//			return 0;
//		} // end if

	} // end try
	catch ( std::exception & e )
	{	
		m_sLastError = e.what();
		
		return 0;
	}

	return 1;
}

