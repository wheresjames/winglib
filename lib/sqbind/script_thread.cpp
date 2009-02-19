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
{	m_pModuleManager = oexNULL;
	m_bFile = oex::oexFALSE;
	m_pParentScript = oexNULL;
}

CScriptThread::~CScriptThread()
{	Destroy(); }

void CScriptThread::Destroy()
{	m_pModuleManager = oexNULL;
	m_bFile = oex::oexFALSE;
	m_pParentScript = oexNULL;
}

void CScriptThread::SetModuleManager( sqbind::CModuleManager *pMm )
{   m_pModuleManager = pMm; }

void CScriptThread::SetScript( oex::oexCSTR pScript, oex::oexBOOL bFile )
{   m_sScript = pScript; m_bFile = bFile; }

void CScriptThread::SetParentScript( CSqMsgQueue *pParent )
{   m_pParentScript = pParent; }

oex::oexBOOL CScriptThread::InitThread( oex::oexPVOID x_pData )
{
	// +++ Ensure script
	if ( !m_sScript.length() )
		return oex::oexFALSE;

	// Set module manager
	m_cSqEngine.SetModuleManager( m_pModuleManager );

	// Pointer to our message queue
	m_cSqEngine.SetMessageQueue( this );

	// Start the script
	if ( !m_cSqEngine.Load( m_sScript.c_str(), m_bFile, FALSE ) )
	{
		stdString sErr = oexT( "File : " );
		sErr += m_sScript + oexT( "\r\n\r\n" );

		if ( m_cSqEngine.GetLastError().length() )
			sErr += m_cSqEngine.GetLastError();
		else
			sErr += oexT( "Error loading script" );

		oexERROR( 0, oexMks( oexT( "Script Error" ), sErr.c_str() ) );

		return oex::oexFALSE;

	} // end if

	return oex::oexTRUE;
}

oex::oexBOOL CScriptThread::DoThread( oex::oexPVOID x_pData )
{
	int nRet = 0;

	// Process script messages
	ProcessMsgs();

	// Idle processing
	if ( !m_cSqEngine.Execute( &nRet, _T( "_idle" ) ) )
		return oex::oexFALSE;

	return 0 == nRet;
}

oex::oexINT CScriptThread::EndThread( oex::oexPVOID x_pData )
{
	// Lose child scripts
	DestroyChildScripts();

	// Lose the squirrel engine
	m_cSqEngine.Destroy();

	return 0;
}

oex::oexBOOL CScriptThread::ProcessMsg( stdString &sMsg, CSqMap &mapParams, stdString *pReply )
{
	if ( sMsg == oexT( "spawn" ) )
		OnSpawn( mapParams, pReply );

	// Call to send message to child script
	else if ( sMsg == oexT( "msg" ) )
		OnMsg( mapParams, pReply );

	// Pass this on to the script
	else if ( sMsg == oexT( "onmsg" ) )
		OnOnMsg( mapParams, pReply );

	return TRUE;
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
	stdString sPath = mapParams[ oexT( "name" ) ];

	// Is it bound for another computer
	int pos = sPath.find_first_of( oexT( ":" ), -1 );
	if ( 0 <= pos )
	{
		stdString sAddress = sPath.substr( 0, pos );
		if ( sAddress.length() )
		{
			// +++ Route to remote computer
			oexASSERT( 0 );

			return;

		} // end if

	} // end if

	// Lose current script engine at this tag if any
	t_ScriptList::iterator it = m_lstScript.find( sPath );
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
		m_lstScript[ sPath ] = pSt;

		// Set us as the parent
		pSt->SetParentScript( this );

		// Allow child to have access to the module manager
		pSt->SetModuleManager( m_pModuleManager );

		// Load script information
		pSt->SetScript( mapParams[ oexT( "script" ) ].c_str(),
						0 != oex::CStr( mapParams[ oexT( "file" ) ].c_str() ).ToULong() );

		// Create the thread
		pSt->Start();

	} // end if
}

void CScriptThread::OnMsg( CSqMap &mapParams, stdString *pReply )
{
	// Grab the path
	stdString sPath = mapParams[ oexT( "name" ) ];

	// Is it bound for another computer
	int pos = sPath.find_first_of( oexT( ":" ), -1 );
	if ( 0 <= pos )
	{
		stdString sAddress = sPath.substr( 0, pos );
		if ( sAddress.length() )
		{
			// +++ Route to remote computer
			oexASSERT( 0 );

			return;

		} // end if

	} // end if

	stdString sOnMsg( oexT( "onmsg" ) );

	// Message to self
	if ( !sPath.length() || sPath == _T( "." ) )
		ProcessMsg( sOnMsg, mapParams, pReply );

	// All the way to the top?
	else if ( sPath == oexT( "/" ) || sPath == oexT( "\\" ) )
	{
		// Route to parent if any
		if ( m_pParentScript )
			m_pParentScript->Msg( oexT( "msg" ), &mapParams, pReply );

		// I guess it's ours
		else
			ProcessMsg( sOnMsg, mapParams, pReply );

		return;

	} // end if

	// Find path separator
	stdString sToken = sPath.substr( 0, sPath.find_first_of( oexT( '\\' ) ) );
	if ( !sToken.length() ) sToken = sPath.substr( 0, sPath.find_first_of( oexT( '/' ) ) );

	// Did we get a token
	if ( sPath.length() > sToken.length() )
	{
		// Skip the token
		sPath = sPath.substr( sToken.length() + 1 );

	} // end if
	else
	{   sToken = sPath;
		sPath = oexT( "" );
	} // end if

	// Is it going up to the parent?
	if ( sToken == oexT( ".." ) )
	{
		if ( m_pParentScript )
		{
			// End of the line?
			if ( !sPath.length() )
				m_pParentScript->Msg( oexT( "onmsg" ), &mapParams, pReply );

			// Keep routing
			else
			{   mapParams[ oexT( "name" ) ] = sPath;
				m_pParentScript->Msg( oexT( "msg" ), &mapParams, pReply );
			} // end if

		} // end if

	} // end if

	// Route to child
	else
	{
		// Lose current script engine at this tag if any
		t_ScriptList::iterator it = m_lstScript.find( sToken );
		if ( m_lstScript.end() != it && it->second )
			it->second->Msg( oexT( "onmsg" ), &mapParams, pReply );

	} // end else
}

void CScriptThread::OnOnMsg( CSqMap &mapParams, stdString *pReply )
{
	// Execute function?
	if ( mapParams[ oexT( "execute" ) ].length() )
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

}

