/*------------------------------------------------------------------
// service_impl.cpp
//
// Copyright (c) 1997
// Robert Umbehant
// winglib@wheresjames.com
// http://www.wheresjames.com
//
// Redistribution and use in source and binary forms, with or
// without modification, are permitted for commercial and
// non-commercial purposes, provided that the following
// conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * The names of the developers or contributors may not be used to
//   endorse or promote products derived from this software without
//   specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/

#include "../../../oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

// Assumptions
oexSTATIC_ASSERT( sizeof( unsigned int ) == sizeof( DWORD ) );
oexSTATIC_ASSERT( sizeof( oexPVOID ) == sizeof( SERVICE_STATUS_HANDLE ) );

// Singleton instance
CServiceImpl *CServiceImpl::m_pInstance = oexNULL;

// Service status
static SERVICE_STATUS g_ss;

/// ServiceMain() dispatch function
static void WINAPI _ServiceMain( int argc, char** argv )
{
	CServiceImpl *pInstance = CServiceImpl::GetInstance();
	if ( pInstance )
		pInstance->OnServiceMain( argc, argv );
}

/// ServiceHandler() dispatch function
static void WINAPI _ServiceHandler( DWORD fdwControl )
{
	CServiceImpl *pInstance = CServiceImpl::GetInstance();
	if ( pInstance )
		pInstance->OnServiceHandler( fdwControl );
}


CServiceImpl::CServiceImpl()
{
	// Initialize service structure
	oexZeroMemory( &g_ss, sizeof( g_ss ) );

	// Fill in service structure
	g_ss.dwServiceType = SERVICE_WIN32;
	g_ss.dwCurrentState = SERVICE_START_PENDING;
	g_ss.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;

	// Save instance pointer
	m_pInstance = this;

	// No command line yet
	m_argc = oexNULL;
	m_argv = oexNULL;

	// Set service handle to null
	m_hService = oexNULL;

	m_bRun = oexFALSE;

	// Auto restart service by default
	m_bAutoRestart = oexTRUE;
}

CServiceImpl::~CServiceImpl()
{
	m_pInstance = oexNULL;
	m_argc = oexNULL;
	m_argv = oexNULL;
	m_hService = oexNULL;
	m_bRun = oexFALSE;
}

CServiceImpl* CServiceImpl::GetInstance()
{
	return m_pInstance;
}

int CServiceImpl::RunService( int argc, char** argv, oexCSTR pName, oexCSTR pDesc  )
{
	// Get singleton instance
	OEX_NAMESPACE::os::CServiceImpl *pInstance = OEX_NAMESPACE::os::CServiceImpl::GetInstance();
	if ( !pInstance )
	{	oexERROR( 0, oexT( "Service not implemented" ) );
		return -1;
	} // end if

	// Set the command line
	pInstance->SetCommandLine( argc, argv );

	// Run as plain old executable?
	if ( pInstance->CommandLine().IsKey( oexT( "exe" ) ) )
		return pInstance->OnRun();

	if ( pInstance->CommandLine().IsKey( oexT( "RegService" ) ) )
	{
		int nErr = InstallService( pName, pDesc, pInstance->getAutoRestart() );
		if ( 0 > nErr )
		{	oexEcho( oexT( "Error registering service" ) );
			return nErr;
		} // end if

		oexEcho( oexT( "Registered service" ) );

		return 0;

	} // end if

	if ( pInstance->CommandLine().IsKey( oexT( "UnregService" ) ) )
	{		
		int nErr = RemoveService( pName );
		if ( 0 > nErr )
		{	oexEcho( oexT( "Error unregistering service" ) );
			return nErr;
		} // end if

		oexEcho( oexT( "Unregistered service" ) );

		return 0;

	} // end if

	// Service data
	SERVICE_TABLE_ENTRY ServiceTable[ 2 ];

	// Set service name
	ServiceTable[ 0 ].lpServiceName = (LPSTR)pInstance->GetName().Ptr();

	// Set pointer to main service function
	ServiceTable[ 0 ].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)_ServiceMain;

	ServiceTable[ 1 ].lpServiceName = NULL;
	ServiceTable[ 1 ].lpServiceProc = NULL;

	// Start the control dispatcher thread for our service
	if ( !StartServiceCtrlDispatcher( ServiceTable ) )
	{	oexERROR( GetLastError(), oexT( "StartServiceCtrlDispatcher() failed" ) );
		return -2;
	} // end if

	return g_ss.dwWin32ExitCode;
}

void CServiceImpl::SetCommandLine( int argc, char **argv )
{
	// Save command line pointers
	m_argc = argc;
	m_argv = argv;

	if ( argc && argv )
		// Get command line params
		m_pbCommandLine = oex::CParser::ParseCommandLine( argc, argv );
	else
		m_pbCommandLine.Destroy();
}

void CServiceImpl::OnServiceMain( int argc, char** argv )
{
	// Register our service control handler
	m_hService = ::RegisterServiceCtrlHandler( GetName().Ptr(), _ServiceHandler );

	// Initialize service
	int nErr = OnInitService();
	if ( 0 > nErr )
	{	g_ss.dwCurrentState = SERVICE_STOPPED;
		g_ss.dwWin32ExitCode = -1;
		SetServiceStatus ( (SERVICE_STATUS_HANDLE)m_hService, &g_ss );
		return;
	} // end if

	// Report that we're running
	m_bRun = oexTRUE;
	g_ss.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus ( (SERVICE_STATUS_HANDLE)m_hService, &g_ss );

	// Run the service
	g_ss.dwWin32ExitCode = OnRun(); 

	// Call exit service
	OnExitService();

	// Notify that we're stopping
	m_bRun = oexFALSE;
	g_ss.dwCurrentState = SERVICE_STOPPED;	
	SetServiceStatus ( (SERVICE_STATUS_HANDLE)m_hService, &g_ss );
}

void CServiceImpl::OnServiceHandler( unsigned int fdwControl )
{
	switch( fdwControl )
	{ 
		case SERVICE_CONTROL_STOP: 
			m_bRun = oexFALSE;
			g_ss.dwCurrentState = SERVICE_STOPPED; 
			break; 

		case SERVICE_CONTROL_SHUTDOWN: 
			m_bRun = oexFALSE;
			g_ss.dwCurrentState = SERVICE_STOPPED; 
			break; 

		default:
			break;

	} // end switch

	// Report current status
	SetServiceStatus ( (SERVICE_STATUS_HANDLE)m_hService, &g_ss );

	return; 
}

int CServiceImpl::InstallService( oexCSTR pName, oexCSTR pDesc, oexBOOL bAutoRestart )
{
	if ( !pName || !*pName )
	{	oexERROR( ERROR_INVALID_PARAMETER, oexT( "Invalid Parameter" ) );
		return -1;
	} // end if

	// Use name as description if not specified
	if ( !pDesc || !*pDesc )
		pDesc = pName;

	// Open the service manager
	SC_HANDLE hSCManager = OpenSCManager ( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( !hSCManager )
	{	oexERROR( GetLastError(), oexMks( oexT( "OpenSCManager() failed : " ), pName ) );
		return -2;
	} // end if

	// Create the service
	SC_HANDLE hService = CreateService( hSCManager, pName, pDesc, SERVICE_ALL_ACCESS,
									    SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
									    SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
									    oexGetModuleFileName().Ptr(),
									    NULL, NULL, NULL, NULL, NULL );
	if( !hService )
	{	oexERROR( GetLastError(), oexMks( oexT( "CreateService() failed : " ), pName ) );
		return -3;
	} // end if
								
	// Close handles
	CloseServiceHandle ( hService );
	CloseServiceHandle ( hSCManager );

	// Do we want auto restart?
	if ( bAutoRestart )
	{
		HKEY hKey = NULL;
		CStr sKey = CStr() << oexT( "SYSTEM\\CurrentControlSet\\Services\\" ) << pName;
		LONG lRes = RegCreateKeyEx( HKEY_LOCAL_MACHINE, sKey.Ptr(),
									0, NULL, 0, KEY_WRITE | KEY_SET_VALUE, NULL, &hKey, NULL );
		if ( ERROR_SUCCESS != lRes )
			oexERROR( GetLastError(), "Could not set service restart optinos, RegCreateKeyEx() failed" );
		else
		{
			// Set failure mode
			if ( ERROR_SUCCESS != RegSetValueEx( hKey, oexT( "FailureActions" ), 0, REG_BINARY,
													   (const BYTE*)
													   "\x00\x00\x00\x00\x00\x00\x00\x00"
													   "\x00\x00\x00\x00\x03\x00\x00\x00"
													   "\x44\x00\x55\x00\x01\x00\x00\x00"
													   "\x60\xEA\x00\x00\x01\x00\x00\x00"
													   "\x60\xEA\x00\x00\x01\x00\x00\x00"
													   "\x60\xEA\x00\x00", 44 ) )
				oexERROR( GetLastError(), oexT( "Could not set service restart optinos, RegSetValueEx() failed" ) );

			RegCloseKey( hKey );

		} // end else

	} // end if

	return 0;
}

int CServiceImpl::RemoveService( oexCSTR pName )
{
	if ( !pName )
	{	oexERROR( ERROR_INVALID_PARAMETER, oexT( "Invalid Parameter" ) );
		return -1;
	} // end if

	// Open the service manager
	SC_HANDLE hSCManager = OpenSCManager ( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( !hSCManager )
	{	oexERROR( GetLastError(), oexT( "OpenSCManager() failed" ) );
		return -2;
	} // end if

	SC_HANDLE hService = OpenService( hSCManager, pName, DELETE );
	if( !hService )
	{	oexERROR( GetLastError(), oexT( "Could not delete service : OpenService() failed" ) );
		return 0; // Probably, the service does not exist
	} // end if

	if ( !DeleteService( hService ) )
	{	oexERROR( GetLastError(), oexT( "Could not delete service : DeleteService() failed" ) );
		return -4;
	} // end if
							   
	// Close handles
	CloseServiceHandle ( hService );
	CloseServiceHandle ( hSCManager );

	{ // Delete sub keys

		HKEY hKey = NULL;
		CStr sKey = CStr() << oexT( "SYSTEM\\CurrentControlSet\\Services\\" ) << pName;
		LONG lRes = RegCreateKeyEx( HKEY_LOCAL_MACHINE, sKey.Ptr(),
									0, NULL, 0, KEY_WRITE | KEY_SET_VALUE | KEY_ENUMERATE_SUB_KEYS, NULL, &hKey, NULL );
		if ( ERROR_SUCCESS != lRes )
			oexERROR( GetLastError(), oexT( "Could not set service restart optinos, RegCreateKeyEx() failed" ) );
		else
		{
			TCHAR szName[ 1024 ] = { 0 };
			DWORD dwName = sizeof( szName ) / sizeof( TCHAR );

			// Enumerate all keys
			DWORD i = 0;
			while ( ERROR_SUCCESS == RegEnumKeyEx( hKey, i, szName, &dwName, 0, 0, 0, 0 ) )
			{
				// Attempt to delete this key
				if ( ERROR_SUCCESS != RegDeleteKey( hKey, szName ) )
				{	i++;
					oexERROR( GetLastError(), oexMks( oexT( "RegDeleteKey() failed to delete sub key : " ), szName ) );
				} // end if

				// Reset name variable
				*szName = 0;
				dwName = sizeof( szName ) / sizeof( TCHAR );

			} // end while

			RegCloseKey( hKey );

		} // end else

	} // end delete sub keys

	HKEY hKey = NULL;
	LONG lRes = RegCreateKeyEx( HKEY_LOCAL_MACHINE, oexT( "SYSTEM\\CurrentControlSet\\Services\\" ),								
								0, NULL, 0, KEY_WRITE | KEY_SET_VALUE, NULL, &hKey, NULL );
	if ( ERROR_SUCCESS != lRes )
		oexERROR( GetLastError(), oexT( "Failed to delete registry key : RegCreateKeyEx() failed" ) );
	else
	{
		if ( ERROR_SUCCESS != RegDeleteKey( hKey, pName ) )
			oexERROR( GetLastError(), oexT( "Failed to delete registry key, RegDeleteKey() failed" ) );

		RegCloseKey( hKey );

	} // end else

	return 0;
}

