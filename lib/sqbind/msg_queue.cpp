/*------------------------------------------------------------------
// msg_queue.cpp
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

/// Default Constructor
CSqMsgQueue::SMsg::SMsg()
{
	pReply = NULL;
}

/// Initializer
CSqMsgQueue::SMsg::SMsg( const stdString x_sPath, const stdString x_sMsg, CSqMap *x_pmapParams, oexEvent x_evReply, stdString *x_pReply )
{
	sPath = x_sPath;
	sMsg = x_sMsg;

	if ( x_pmapParams )
//		mapParams = *x_pmapParams;
		sParams = x_pmapParams->serialize();
	evReply = x_evReply;
	pReply = x_pReply;
}

// Copy constructor
CSqMsgQueue::SMsg::SMsg( const CSqMsgQueue::SMsg &x_rMsg )
{   sMsg = x_rMsg.sMsg;
	sPath = x_rMsg.sPath;
	sParams = x_rMsg.sParams;
	evReply = x_rMsg.evReply;
	pReply = x_rMsg.pReply;
}

/// Default constructor
CSqMsgQueue::CSqMsgQueue()
{
	m_uOwnerThreadId = 0;
	m_bWantQuit = oex::oexFALSE;
}

/// Destructor
CSqMsgQueue::~CSqMsgQueue()
{
	// Acquire lock
	oexAutoLock ll( m_cLock );
	if ( !ll.IsLocked() )
		return;

	Destroy();
}

/// Releases resources
void CSqMsgQueue::Destroy()
{
	// Acquire lock
	oexAutoLock ll( m_cLock );
	if ( !ll.IsLocked() )
		return;

	// Dump the queue
	m_lstMsgQueue.clear();

	// Reset event
	Reset();

	m_bWantQuit = oex::oexFALSE;
}

/// Sends a command to the thread
/**
	\param [in]     sMsg        -   Command
	\param [in]     mapParams   -   Parameters
	\param [out]    pmapReply   -   Receives reply
	\param [in]		uTimeout	-	Maximum amount of time to wait
									for a reply.

	If pmapReply is not NULL, the function waits for a reply
	from the thread.
*/
oex::oexBOOL CSqMsgQueue::Msg( stdString sPath, stdString sMsg, CSqMap *pmapParams, stdString *pReply, oex::oexUINT uTimeout )
{
	oexEvent evReply;

//	oexPrintf( oexT( "MSG: %s, To: %s, Caller: 0x%08x, Owner: 0x%08x\n" ), sMsg.c_str(), sPath.c_str(), (unsigned int)oexGetCurThreadId(), m_uOwnerThreadId );

	// If it's our own thread calling
	if ( m_uOwnerThreadId && oexGetCurThreadId() == m_uOwnerThreadId )
	{
		// Just process the message directly
		if ( pmapParams )
			return ProcessMsg( sPath, sMsg, *pmapParams, pReply );

		else
		{   CSqMap params;
			return ProcessMsg( sPath, sMsg, params, pReply );
		} // end else

	} // end if

	{ // Stuff message into buffer

		// Acquire lock
		oexAutoLock ll( m_cLock );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		// Reply event handle needed?
		if ( !pReply )
			evReply.Destroy();

		// Add a message
		m_lstMsgQueue.push_back( SMsg( sPath, sMsg, pmapParams, evReply, pReply ) );

		// Signal that a message is waiting
		Signal();

	} // end message stuffing

	// Wait for reply if needed
	if ( pReply )
	{
		// Don't wait if messages will not be processed
		if ( !Running() )
			return oex::oexFALSE;

		// Wait for reply
		oex::oexBOOL bSuccess = !evReply.Wait( uTimeout );

		// Punt if we got the reply
		if ( bSuccess )
			return oex::oexTRUE;

		// Acquire lock
		oexAutoLock ll( &m_cLock );
		if ( !ll.IsLocked() )
		{
			// You're screwed here because pmapReply is dangling
			// But don't worry, this won't ever happen ;)
            oexASSERT( 0 );

			// Only safe thing to do would be kill the thread...
			KillThread();

			return oex::oexFALSE;

		} // end if

		// Clear the list so pmapReply is not dangling
		m_lstMsgQueue.clear();

	} // end if

	return oex::oexTRUE;
}

/// Process messages
oex::oexBOOL CSqMsgQueue::ProcessMsgs()
{
	// Acquire lock
	oexAutoLock ll( &m_cLock );
	if ( !ll.IsLocked() )
		return oex::oexFALSE;

	// Any messages waiting?
	if ( m_lstMsgQueue.begin() == m_lstMsgQueue.end() )
		return oex::oexTRUE;

	// Process messages
	t_MsgQueue::iterator it;
	while ( m_lstMsgQueue.end() != ( it = m_lstMsgQueue.begin() ) )
	{
		// Deserialize params
		CSqMap mapParams( it->sParams );

		// Process the message
		ProcessMsg( it->sPath, it->sMsg, mapParams, it->pReply );

		// We must stop processing if someone is waiting for a reply
		if ( it->pReply )
		{
			// Signal that reply is ready
			it->evReply.Signal();
			m_lstMsgQueue.erase( it );

			// Reset signal if queue is empty
			if ( !m_lstMsgQueue.size() )
				Reset();

			return oex::oexTRUE;

		} // end if

		// Ditch the message
		m_lstMsgQueue.erase( it );

	} // end while

	// Reset signal
	Reset();

	return oex::oexTRUE;
}

/// Process a single message from the queue
oex::oexBOOL CSqMsgQueue::ProcessMsg( const stdString &sPath, stdString &sMsg, CSqMap &mapParams, stdString *pReply )
{
	return oex::oexFALSE;
}

oex::oexBOOL CSqMsgQueue::is_path( const stdString &sPath )
{	CSqMap params; stdString sMsg = oexT( "is_path" );
	return ProcessMsg( sPath, sMsg, params, oexNULL );
}

oex::oexBOOL CSqMsgQueue::run( stdString *pReply, const stdString &sPath, const stdString &sScript )
{	CSqMap params;
	params[ oexT( "run" ) ] = sScript;
	return Msg( sPath, oexT( "msg" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::execute( stdString *pReply, const stdString &sPath, const stdString &sFunction )
{	CSqMap params;
	params[ oexT( "execute" ) ] = sFunction;
	return Msg( sPath, oexT( "msg" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::execute( stdString *pReply, const stdString &sPath, const stdString &sFunction,
								   const stdString &sP1 )
{	CSqMap params;
	params[ oexT( "execute1" ) ] = sFunction;
	params[ oexT( "p1" ) ] = sP1;
	return Msg( sPath, oexT( "msg" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::execute( stdString *pReply, const stdString &sPath, const stdString &sFunction,
								   const stdString &sP1, const stdString &sP2 )
{	CSqMap params;
	params[ oexT( "execute2" ) ] = sFunction;
	params[ oexT( "p1" ) ] = sP1;
	params[ oexT( "p2" ) ] = sP2;
	return Msg( sPath, oexT( "msg" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::execute( stdString *pReply, const stdString &sPath, const stdString &sFunction,
								   const stdString &sP1, const stdString &sP2, const stdString &sP3 )
{	CSqMap params;
	params[ oexT( "execute3" ) ] = sFunction;
	params[ oexT( "p1" ) ] = sP1;
	params[ oexT( "p2" ) ] = sP2;
	params[ oexT( "p3" ) ] = sP3;
	return Msg( sPath, oexT( "msg" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::execute( stdString *pReply, const stdString &sPath, const stdString &sFunction,
								   const stdString &sP1, const stdString &sP2, const stdString &sP3,
								   const stdString &sP4 )
{	CSqMap params;
	params[ oexT( "execute4" ) ] = sFunction;
	params[ oexT( "p1" ) ] = sP1;
	params[ oexT( "p2" ) ] = sP2;
	params[ oexT( "p3" ) ] = sP3;
	params[ oexT( "p4" ) ] = sP4;
	return Msg( sPath, oexT( "msg" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::spawn( stdString *pReply, const stdString &sPath, const stdString &sName, const stdString &sScript, int bFile )
{
	CSqMap params;
	params[ oexT( "name" ) ] = sName;
	params[ oexT( "script" ) ] = sScript;
	params[ oexT( "file" ) ] = bFile ? oexT( "1" ) : oexT( "0" );
	return Msg( sPath, oexT( "spawn" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::kill( stdString *pReply, const stdString &sPath )
{	CSqMap params;
	params[ oexT( "quit" ) ] = 1;
	return Msg( sPath, oexT( "kill" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::get_children( stdString *pReply, const stdString &sPath )
{	CSqMap params;
	return Msg( sPath, oexT( "get_children" ), &params, pReply );
}

void CSqMsgQueue::set( const stdString &sPath, const stdString &sKey, const stdString &sVal )
{	CSqMap params;
	params[ oexT( "key" ) ] = sKey;
	params[ oexT( "val" ) ] = sVal;
	Msg( sPath, oexT( "pb_set" ), &params, oexNULL );
}

stdString CSqMsgQueue::get( const stdString &sPath, const stdString &sKey )
{	stdString sRet;
	CSqMap params;
	params[ oexT( "key" ) ] = sKey;
	Msg( sPath, oexT( "pb_get" ), &params, &sRet );
	return sRet;
}
