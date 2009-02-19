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
CSqMsgQueue::SMsg::SMsg( const stdString x_sMsg, CSqMap *x_pmapParams, oexEvent x_evReply, stdString *x_pReply )
{   sMsg = x_sMsg;
	if ( x_pmapParams )
//		mapParams = *x_pmapParams;
	sParams = x_pmapParams->serialize();
	evReply = x_evReply;
	pReply = x_pReply;
}

/*
// Copy constructor
CSqMsgQueue::SMsg::SMsg( const CSqMsgQueue::SMsg &x_rMsg )
{   sMsg = x_rMsg.sMsg;
	mapParams.list() = x_rMsg.mapParams.list();
	evReply = x_rMsg.evReply;
	pReply = x_rMsg.pReply;
}
*/

/// Default constructor
CSqMsgQueue::CSqMsgQueue()
{
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
oex::oexBOOL CSqMsgQueue::Msg( stdString sMsg, CSqMap *pmapParams, stdString *pReply, oex::oexUINT uTimeout )
{
	oexEvent evReply;

	// If it's our own thread calling
	if ( m_uCurrentThreadId && oex::os::CSys::GetCurrentThreadId() == m_uCurrentThreadId )
	{
		// Just process the message directly
		if ( pmapParams )
			return ProcessMsg( sMsg, *pmapParams, pReply );

		else
		{   CSqMap params;
			return ProcessMsg( sMsg, params, pReply );
		} // end else

	} // end if

	{ // Stuff message into buffer

		// Acquire lock
		oexAutoLock ll( m_cLock );
		if ( !ll.IsLocked() )
			return FALSE;

		// Reply event handle needed?
		if ( !pReply )
			evReply.Destroy();

		// Add a message
		m_lstMsgQueue.push_back( SMsg( sMsg, pmapParams, evReply, pReply ) );

		// Signal that a message is waiting
		Signal();

	} // end message stuffing

	// Wait for relpy if needed
	if ( pReply )
	{
		// Wait for reply
		oex::oexBOOL bSuccess = !evReply.Wait( uTimeout );

		// Punt if we got the reply
		if ( bSuccess )
			return TRUE;

		// Acquire lock
		oexAutoLock ll( &m_cLock );
		if ( !ll.IsLocked() )
		{
			// You're screwed here because pmapReply is dangling
			// But don't worry, this won't ever happen ;)
            oexASSERT( 0 );

			// Only safe thing to do would be kill the thread...
			KillThread();

			return FALSE;

		} // end if

		// Clear the list so pmapReply is not dangling
		m_lstMsgQueue.clear();

	} // end if

	return TRUE;
}

/// Process messages
oex::oexBOOL CSqMsgQueue::ProcessMsgs()
{
	// Acquire lock
	oexAutoLock ll( &m_cLock );
	if ( !ll.IsLocked() )
		return FALSE;

	// Remember who owns us
	m_uCurrentThreadId = oex::os::CSys::GetCurrentThreadId();

	// Any messages waiting?
	if ( m_lstMsgQueue.begin() == m_lstMsgQueue.end() )
		return TRUE;

	// Process messages
	t_MsgQueue::iterator it;
	while ( m_lstMsgQueue.end() != ( it = m_lstMsgQueue.begin() ) )
	{
		// Deserialize params
		CSqMap mapParams( it->sParams );

		// Process the message
		ProcessMsg( it->sMsg, mapParams, it->pReply );

		// We must stop processing if someone is waiting for a reply
		if ( it->pReply )
		{
			// Signal that reply is ready
			it->evReply.Signal();
			m_lstMsgQueue.erase( it );

			// Reset signal if queue is empty
			if ( !m_lstMsgQueue.size() )
				Reset();

			return TRUE;

		} // end if

		// Ditch the message
		m_lstMsgQueue.erase( it );

	} // end while

	// Reset signal
	Reset();

	return TRUE;
}

/// Process a single message from the queue
oex::oexBOOL CSqMsgQueue::ProcessMsg( stdString &sMsg, CSqMap &mapParams, stdString *pReply )
{
	return TRUE;
}

oex::oexBOOL CSqMsgQueue::execute( stdString *pReply, const stdString &sName, const stdString &sFunction )
{	CSqMap params;
	params[ oexT( "name" ) ] = sName;
	params[ oexT( "execute" ) ] = sFunction;
	return Msg( oexT( "msg" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::execute( stdString *pReply, const stdString &sName, const stdString &sFunction, 
								   const stdString &sP1 )
{	CSqMap params;
	params[ oexT( "name" ) ] = sName;
	params[ oexT( "execute1" ) ] = sFunction;
	params[ oexT( "p1" ) ] = sP1;
	return Msg( oexT( "msg" ), &params, pReply );
}
