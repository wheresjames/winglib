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

/*
	The Raven
	- Edgar Allan Poe

Once upon a midnight dreary, while I pondered weak and weary,
Over many a quaint and curious volume of forgotten lore,
While I nodded, nearly napping, suddenly there came a tapping,
As of some one gently rapping, rapping at my chamber door.
`'Tis some visitor,' I muttered, `tapping at my chamber door -
Only this, and nothing more.'

Ah, distinctly I remember it was in the bleak December,
And each separate dying ember wrought its ghost upon the floor.
Eagerly I wished the morrow; - vainly I had sought to borrow
From my books surcease of sorrow - sorrow for the lost Lenore -
For the rare and radiant maiden whom the angels named Lenore -
Nameless here for evermore.

And the silken sad uncertain rustling of each purple curtain
Thrilled me - filled me with fantastic terrors never felt before;
So that now, to still the beating of my heart, I stood repeating
`'Tis some visitor entreating entrance at my chamber door -
Some late visitor entreating entrance at my chamber door; -
This it is, and nothing more,'

Presently my soul grew stronger; hesitating then no longer,
`Sir,' said I, `or Madam, truly your forgiveness I implore;
But the fact is I was napping, and so gently you came rapping,
And so faintly you came tapping, tapping at my chamber door,
That I scarce was sure I heard you' - here I opened wide the door; -
Darkness there, and nothing more.

Deep into that darkness peering, long I stood there wondering, fearing,
Doubting, dreaming dreams no mortal ever dared to dream before
But the silence was unbroken, and the darkness gave no token,
And the only word there spoken was the whispered word, `Lenore!'
This I whispered, and an echo murmured back the word, `Lenore!'
Merely this and nothing more.

Back into the chamber turning, all my soul within me burning,
Soon again I heard a tapping somewhat louder than before.
`Surely,' said I, `surely that is something at my window lattice;
Let me see then, what thereat is, and this mystery explore -
Let my heart be still a moment and this mystery explore; -
'Tis the wind and nothing more!'

Open here I flung the shutter, when, with many a flirt and flutter,
In there stepped a stately raven of the saintly days of yore.
Not the least obeisance made he; not a minute stopped or stayed he;
But, with mien of lord or lady, perched above my chamber door -
Perched upon a bust of Pallas just above my chamber door -
Perched, and sat, and nothing more.

Then this ebony bird beguiling my sad fancy into smiling,
By the grave and stern decorum of the countenance it wore,
`Though thy crest be shorn and shaven, thou,' I said, `art sure no craven.
Ghastly grim and ancient raven wandering from the nightly shore -
Tell me what thy lordly name is on the Night's Plutonian shore!'
Quoth the raven, `Nevermore.'

Much I marvelled this ungainly fowl to hear discourse so plainly,
Though its answer little meaning - little relevancy bore;
For we cannot help agreeing that no living human being
Ever yet was blessed with seeing bird above his chamber door -
Bird or beast above the sculptured bust above his chamber door,
With such name as `Nevermore.'

But the raven, sitting lonely on the placid bust, spoke only,
That one word, as if his soul in that one word he did outpour.
Nothing further then he uttered - not a feather then he fluttered -
Till I scarcely more than muttered `Other friends have flown before -
On the morrow he will leave me, as my hopes have flown before.'
Then the bird said, `Nevermore.'

Startled at the stillness broken by reply so aptly spoken,
`Doubtless,' said I, `what it utters is its only stock and store,
Caught from some unhappy master whom unmerciful disaster
Followed fast and followed faster till his songs one burden bore -
Till the dirges of his hope that melancholy burden bore
Of "Never-nevermore."'

But the raven still beguiling all my sad soul into smiling,
Straight I wheeled a cushioned seat in front of bird and bust and door;
Then, upon the velvet sinking, I betook myself to linking
Fancy unto fancy, thinking what this ominous bird of yore -
What this grim, ungainly, ghastly, gaunt, and ominous bird of yore
Meant in croaking `Nevermore.'

This I sat engaged in guessing, but no syllable expressing
To the fowl whose fiery eyes now burned into my bosom's core;
This and more I sat divining, with my head at ease reclining
On the cushion's velvet lining that the lamp-light gloated o'er,
But whose velvet violet lining with the lamp-light gloating o'er,
She shall press, ah, nevermore!

Then, methought, the air grew denser, perfumed from an unseen censer
Swung by Seraphim whose foot-falls tinkled on the tufted floor.
`Wretch,' I cried, `thy God hath lent thee - by these angels he has sent thee
Respite - respite and nepenthe from thy memories of Lenore!
Quaff, oh quaff this kind nepenthe, and forget this lost Lenore!'
Quoth the raven, `Nevermore.'

`Prophet!' said I, `thing of evil! - prophet still, if bird or devil! -
Whether tempter sent, or whether tempest tossed thee here ashore,
Desolate yet all undaunted, on this desert land enchanted -
On this home by horror haunted - tell me truly, I implore -
Is there - is there balm in Gilead? - tell me - tell me, I implore!'
Quoth the raven, `Nevermore.'

`Prophet!' said I, `thing of evil! - prophet still, if bird or devil!
By that Heaven that bends above us - by that God we both adore -
Tell this soul with sorrow laden if, within the distant Aidenn,
It shall clasp a sainted maiden whom the angels named Lenore -
Clasp a rare and radiant maiden, whom the angels named Lenore?'
Quoth the raven, `Nevermore.'

`Be that word our sign of parting, bird or fiend!' I shrieked upstarting -
`Get thee back into the tempest and the Night's Plutonian shore!
Leave no black plume as a token of that lie thy soul hath spoken!
Leave my loneliness unbroken! - quit the bust above my door!
Take thy beak from out my heart, and take thy form from off my door!'
Quoth the raven, `Nevermore.'

And the raven, never flitting, still is sitting, still is sitting
On the pallid bust of Pallas just above my chamber door;
And his eyes have all the seeming of a demon's that is dreaming,
And the lamp-light o'er him streaming throws his shadow on the floor;
And my soul from out that shadow that lies floating on the floor
Shall be lifted - nevermore!

*/

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

	// I assume we want to start off running
	// ( this is needed to ensure things work when running 'as exe')
	m_bRun = oexTRUE;

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

	return pInstance->OnRunService( argc, argv, pName, pDesc );
}

int CServiceImpl::OnRunService( int argc, char** argv, oexCSTR pName, oexCSTR pDesc  )
{
	// Set the command line
	SetCommandLine( argc, argv );

	// Run as plain old executable?
	if ( CommandLine().IsKey( oexT( "run" ) ) )
		return OnRun();

	if ( CommandLine().IsKey( oexT( "install" ) ) )
	{
		int nErr = InstallService( pName, pDesc, getAutoRestart() );
		if ( 0 > nErr )
		{	oexEcho( oexT( "Error installing service" ) );
			return nErr;
		} // end if

		oexEcho( oexT( "Installed service" ) );

		return 0;

	} // end if

	if ( CommandLine().IsKey( oexT( "remove" ) )
		 || CommandLine().IsKey( oexT( "uninstall" ) ) )
	{
		int nErr = RemoveService( pName );
		if ( 0 > nErr )
		{	oexEcho( oexT( "Error removing service" ) );
			return nErr;
		} // end if

		oexEcho( oexT( "Removed service" ) );

		return 0;

	} // end if

	// Service data
	SERVICE_TABLE_ENTRY ServiceTable[ 2 ];

	// Set service name
	ServiceTable[ 0 ].lpServiceName = (LPSTR)GetName().Ptr();

	// Set pointer to main service function
	ServiceTable[ 0 ].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)_ServiceMain;

	ServiceTable[ 1 ].lpServiceName = NULL;
	ServiceTable[ 1 ].lpServiceProc = NULL;

	// Start the control dispatcher thread for our service
	if ( !StartServiceCtrlDispatcher( ServiceTable ) )
	{
		// +++ This probably means we're running as exe
		//     Need to do more testing on this
		if ( ERROR_FAILED_SERVICE_CONTROLLER_CONNECT == GetLastError() )
			if ( !CommandLine().IsKey( oexT( "service" ) ) )
				return OnRun();

		// Log the error
		oexERROR( GetLastError(), oexT( "StartServiceCtrlDispatcher() failed" ) );

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
	m_hService = (oexPVOID)::RegisterServiceCtrlHandler( GetName().Ptr(), _ServiceHandler );

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
			oexERROR( GetLastError(), "Could not set service restart options, RegCreateKeyEx() failed" );
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
				oexERROR( GetLastError(), oexT( "Could not set service restart options, RegSetValueEx() failed" ) );

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
			oexERROR( GetLastError(), oexT( "Could not set service restart options, RegCreateKeyEx() failed" ) );
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

