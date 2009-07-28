/*------------------------------------------------------------------
// script_thread.cpp
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

#include "stdafx.h"

using namespace sqbind;

CScriptThread::CScriptThread()
{	m_bQuit = oex::oexFALSE;
	m_pModuleManager = oexNULL;
	m_bFile = oex::oexFALSE;
	m_pParentScript = oexNULL;
}

CScriptThread::~CScriptThread()
{	Destroy(); }

void CScriptThread::Destroy()
{	m_bQuit = oex::oexTRUE;
	m_pModuleManager = oexNULL;
	m_pParentScript = oexNULL;
}

void CScriptThread::SetModuleManager( sqbind::CModuleManager *pMm )
{   m_pModuleManager = pMm; }

void CScriptThread::SetScript( oex::oexCSTR pScript, oex::oexBOOL bFile )
{   m_sScript = pScript; m_bFile = bFile; }

void CScriptThread::SetParentScript( CSqMsgQueue *pParent )
{   m_pParentScript = pParent; }

void CScriptThread::SetExportFunction( PFN_SQBIND_Export_Symbols fn, sqbind::SSqAllocator *pa )
{	m_cSqEngine.SetExportFunction( fn, pa ); }

oex::oexBOOL CScriptThread::InitThread( oex::oexPVOID x_pData )
{
	// +++ Ensure script
	if ( !m_sScript.length() )
		return oex::oexFALSE;

	// Save thread id
	SetOwnerThreadId( oexGetCurThreadId() );

	// Set module manager
	m_cSqEngine.SetModuleManager( m_pModuleManager );

	// Pointer to our message queue
	m_cSqEngine.SetMessageQueue( this );

	// Let the user know we're starting a thread
//	oexPrintf( oexT( "Spawning : 0x%08x : %s : %s\n" ),
//			  (unsigned int)oexGetCurThreadId(), m_sName.c_str(), oexGetFileName( m_sScript.c_str() ).Ptr() );

	// Start the script
	if ( !m_cSqEngine.Load( m_sScript.c_str(), m_bFile, FALSE ) )
	{
		stdString sErr = oexT( "File : " );

		if ( m_bFile )
			sErr += m_sScript + oexT( "\r\n\r\n" );
		else
			sErr += oexT( "(buffer)\r\n\r\n" );

		if ( m_cSqEngine.GetLastError().length() )
			sErr += m_cSqEngine.GetLastError();
		else
			sErr += oexT( "Error loading script" );

		oexERROR( 0, oexMks( oexT( "Script Error : " ), sErr.c_str() ) );

		return oex::oexFALSE;

	} // end if

	return oex::oexTRUE;
}

oex::oexBOOL CScriptThread::DoThread( oex::oexPVOID x_pData )
{
	// Have we gotten a quit flag?
	if ( m_bQuit )
		return oex::oexFALSE;

	// Process script messages
	ProcessMsgs();

	// Punt if quit has been requested
	if ( WantQuit() )
		return oex::oexFALSE;

	// Idle processing
	int nRet = 0;
	if ( !m_cSqEngine.Execute( &nRet, _T( "_idle" ) ) )
		return oex::oexFALSE;

	// Cleanup child scripts
	CleanupChildScripts();

	return 0 == nRet;
}

oex::oexINT CScriptThread::EndThread( oex::oexPVOID x_pData )
{
	// No more owner thread
	SetOwnerThreadId( 0 );

	// Lose child scripts
	DestroyChildScripts();

	// Lose the squirrel engine
	m_cSqEngine.Destroy();

	// Let the user know we're starting a thread
//	oexPrintf( oexT( "Exiting : 0x%08x : %s\n" ),
//			   (unsigned int)oexGetCurThreadId(), m_sName.c_str() );

	return 0;
}

oex::oexBOOL CScriptThread::ExecuteMsg( stdString &sMsg, CSqMap &mapParams, stdString *pReply )
{
	// Just a path check?
	if ( sMsg == oexT( "is_path" ) )
		return oex::oexTRUE;

	// Just post it to the queue if not our thread
	if ( GetOwnerThreadId() != oexGetCurThreadId() )
		return Msg( stdString(), sMsg, &mapParams, pReply );

	if ( sMsg == oexT( "spawn" ) )
		OnSpawn( mapParams, pReply );

	// Call to send message to child script
	else if ( sMsg == oexT( "msg" ) )
		OnMsg( mapParams, pReply );

	// Return comma separated list of child scripts command
	else if ( sMsg == oexT( "get_children" ) )
	{	if ( oexCHECK_PTR( pReply ) )
		{	oex::TList< oex::CStr > strlst;
			for (	t_ScriptList::iterator it = m_lstScript.begin();
					m_lstScript.end() != it; it++ )
				if ( oexCHECK_PTR( it->second ) && it->second->IsRunning() && !it->second->WantQuit() )
					strlst << oex::CParser::UrlEncode( it->second->GetName().c_str() );
			*pReply = oex::CParser::Implode( strlst, oexT( "," ) ).Ptr();
		} // end if
	} // end else if

	// Quit command
	else if ( sMsg == oexT( "kill" ) )
		RequestQuit();

	else
		return oex::oexFALSE;

	return oex::oexTRUE;
}

oex::oexBOOL CScriptThread::ProcessMsg( const stdString &x_sPath, stdString &sMsg, CSqMap &mapParams, stdString *pReply )
{
	stdString sPath = x_sPath;

	// Is it bound for another computer
	int pos = sPath.find_first_of( oexT( ":" ), -1 );
	if ( 0 <= pos )
	{
		stdString sAddress = sPath.substr( 0, pos );
		if ( sAddress.length() )
		{
			// +++ Route to remote computer
			oexASSERT( 0 );

			return oex::oexFALSE;

		} // end if

	} // end if

	// Is it for us?
	if ( !sPath.length() || sPath == _T( "." ) )
		return ExecuteMsg( sMsg, mapParams, pReply );

	// All the way to the top?
	else if ( sPath[ 0 ] == oexT( '/' ) || sPath[ 0 ] == oexT( '\\' ) )
	{
		// Route to parent if any
		if ( m_pParentScript )
//			return m_pParentScript->Msg( sPath, sMsg, &mapParams, pReply );
			return m_pParentScript->ProcessMsg( sPath, sMsg, mapParams, pReply );

		// Shave off parent sep
		while ( sPath[ 0 ] == oexT( '/' )
				|| sPath[ 0 ] == oexT( '\\' )
				|| sPath[ 0 ] == oexT( '.' ) )
			sPath = sPath.substr( 1 );

		// Is it ours?
		if ( !sPath.length() )
			return ExecuteMsg( sMsg, mapParams, pReply );

	} // end if

	// Find path separator
	stdString sToken;
	stdString::size_type nPos = sPath.find_first_of( oexT( '\\' ) );
	if ( stdString::npos == nPos ) nPos = sPath.find_first_of( oexT( '/' ) );
	if ( stdString::npos != nPos )
	{	sToken = sPath.substr( 0, nPos );
		sPath = sPath.substr( nPos + 1 );
	} // end if
	else
	{   sToken = sPath;
		sPath = oexT( "" );
	} // end if

	// Is it going up to the parent?
	if ( sToken == oexT( ".." ) )
	{
		if ( !m_pParentScript )
			return oex::oexFALSE;

//		return m_pParentScript->Msg( sPath, sMsg, &mapParams, pReply );
		return m_pParentScript->ProcessMsg( sPath, sMsg, mapParams, pReply );

	} // end if

	// Route to child
	t_ScriptList::iterator it = m_lstScript.find( sToken );
	if ( m_lstScript.end() != it && it->second )
//		return it->second->Msg( sPath, sMsg, &mapParams, pReply );
		return it->second->ProcessMsg( sPath, sMsg, mapParams, pReply );

	return oex::oexFALSE;
}

void CScriptThread::CleanupChildScripts()
{

	// Kill our script
	t_ScriptList::iterator it = m_lstScript.begin();
	while ( m_lstScript.end() != it )
	{
		// Delete node if child has stopped
		if ( it->second && !it->second->GetInitEvent().Wait( 0 ) && !it->second->IsRunning() )
		{	it->second->Stop();
			OexAllocDestruct( it->second );
			it->second = oexNULL;
		} // end if

		if ( !it->second )
		{	t_ScriptList::iterator nx = it; nx++;
			m_lstScript.erase( it );
			it = nx;
		} // end if

		else
			it++;

		// ???
//			it = m_lstScript.erase( it );

	} // end while

}

void CScriptThread::DestroyChildScripts()
{
	// Kill our script
	t_ScriptList::iterator it = m_lstScript.begin();
	while ( m_lstScript.end() != it )
	{
		if ( it->second )
		{	it->second->Stop();
			OexAllocDestruct( it->second );
			it->second = oexNULL;
		} // end if

		t_ScriptList::iterator nx = it; nx++;
		m_lstScript.erase( it );
		it = nx;

		// ???
//			it = m_lstScript.erase( it );

	} // end while
}

void CScriptThread::OnSpawn( CSqMap &mapParams, stdString *pReply )
{
	// Grab the path
	stdString sName = mapParams[ oexT( "name" ) ];

	// Lose current script engine at this tag if any
	t_ScriptList::iterator it = m_lstScript.find( sName );
	if ( m_lstScript.end() != it )
	{	if ( it->second )
		{	OexAllocDestruct( it->second );
			it->second = oexNULL;
		} // end if
		m_lstScript.erase( it );
	} // end if

	// Create new object
	CScriptThread *pSt = OexAllocConstruct< CScriptThread >();
	if ( pSt )
	{
		// Save away pointer for later
		m_lstScript[ sName ] = pSt;

		// Set include function
		pSt->SetIncludeScriptFunction( m_cSqEngine.GetIncludeScriptFunction() );

		// Let the script know it's name
		pSt->SetName( sName );

		// Set us as the parent
		pSt->SetParentScript( this );

		// Allow child to have access to the module manager
		pSt->SetModuleManager( m_pModuleManager );

		// Import environment
		pSt->SetExportFunction( m_cSqEngine.GetExportFunction(), m_cSqEngine.GetAllocator() );

		// Is it a file?
		if ( 0 != oex::CStr( mapParams[ oexT( "file" ) ].c_str() ).ToULong() )
		{
			if ( m_cSqEngine.GetIncludeScriptFunction() )
			{
				stdString sData;
				if ( m_cSqEngine.GetIncludeScriptFunction()( mapParams[ oexT( "script" ) ], sData ) )
					*pReply = oexT( "FAILED" );
				else
					pSt->SetScript( sData.c_str(), 0 );

			} // end if

			else
				pSt->SetScript( m_cSqEngine.path( mapParams[ oexT( "script" ) ] ).c_str(), 1 );
		} // end if

		else
			// Load script information
			pSt->SetScript( mapParams[ oexT( "script" ) ].c_str(), 0 );

		// Create the thread
		pSt->Start();

		// Wait for thread to start if reply buffer
		if ( pReply )
			if ( pSt->GetInitEvent().Wait() )
				*pReply = oexT( "FAILED" );

	} // end if
}

void CScriptThread::OnMsg( CSqMap &mapParams, stdString *pReply )
{
	// Run script?
	if ( mapParams[ oexT( "run" ) ].length() )
		m_cSqEngine.Run( mapParams[ oexT( "run" ) ].c_str() );

	// Execute function?
	else if ( mapParams[ oexT( "execute" ) ].length() )
		m_cSqEngine.Execute( pReply, mapParams[ oexT( "execute" ) ].c_str() );

	// Execute one param
	else if ( mapParams[ oexT( "execute1" ) ].length() )
		m_cSqEngine.Execute( pReply, mapParams[ oexT( "execute1" ) ].c_str(), mapParams[ oexT( "p1" ) ] );

	// Execute 2 params
	else if ( mapParams[ oexT( "execute2" ) ].length() )
		m_cSqEngine.Execute( pReply, mapParams[ oexT( "execute2" ) ].c_str(), mapParams[ oexT( "p1" ) ], mapParams[ oexT( "p2" ) ] );

	// Execute 3 params
	else if ( mapParams[ oexT( "execute3" ) ].length() )
		m_cSqEngine.Execute( pReply, mapParams[ oexT( "execute3" ) ].c_str(), mapParams[ oexT( "p1" ) ], mapParams[ oexT( "p2" ) ], mapParams[ oexT( "p3" ) ] );

	// Execute 4 params
	else if ( mapParams[ oexT( "execute4" ) ].length() )
		m_cSqEngine.Execute( pReply, mapParams[ oexT( "execute4" ) ].c_str(), mapParams[ oexT( "p1" ) ], mapParams[ oexT( "p2" ) ], mapParams[ oexT( "p3" ) ], mapParams[ oexT( "p4" ) ] );
}

