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
{_STT();
	pReply = NULL;
}

/// Initializer
CSqMsgQueue::SMsg::SMsg( const stdString x_sPath, const stdString x_sMsg, CSqMulti *x_pmapParams, oexEvent *x_evReply, stdString *x_pReply )
{_STT();
	sPath = x_sPath;
	sMsg = x_sMsg;

	if ( x_pmapParams )
//		mapParams = *x_pmapParams;
		sParams = x_pmapParams->serialize();
	if ( x_evReply )
		evReply = *x_evReply;
	pReply = x_pReply;
}

// Copy constructor
CSqMsgQueue::SMsg::SMsg( const CSqMsgQueue::SMsg &x_rMsg )
{_STT();
	sMsg = x_rMsg.sMsg;
	sPath = x_rMsg.sPath;
	sParams = x_rMsg.sParams;
	evReply = x_rMsg.evReply;
	pReply = x_rMsg.pReply;
}

// assignment
CSqMsgQueue::SMsg& CSqMsgQueue::SMsg::operator = ( const CSqMsgQueue::SMsg &x_rMsg )
{_STT();
	sMsg = x_rMsg.sMsg;
	sPath = x_rMsg.sPath;
	sParams = x_rMsg.sParams;
	evReply = x_rMsg.evReply;
	pReply = x_rMsg.pReply;
	return *this;
}

// Default constructor
CSqMsgQueue::CSqMsgQueue()
{_STT();
	m_uOwnerThreadId = 0;
	m_bWantQuit = oex::oexFALSE;
}

// Destructor
CSqMsgQueue::~CSqMsgQueue()
{_STT();
	// Acquire lock
	oexAutoLock ll( m_cLock );
	if ( !ll.IsLocked() )
		return;

	Destroy();
}

// Releases resources
void CSqMsgQueue::Destroy()
{_STT();
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

// Sends a command to the thread
/**
	\param [in]     sMsg        -   Command
	\param [in]     mapParams   -   Parameters
	\param [out]    pmapReply   -   Receives reply
	\param [in]		uTimeout	-	Maximum amount of time to wait
									for a reply.

	If pmapReply is not NULL, the function waits for a reply
	from the thread.
*/
oex::oexBOOL CSqMsgQueue::Msg( stdString sPath, stdString sMsg, CSqMulti *pmapParams, stdString *pReply, oexEvent *pReplyEvent, oex::oexUINT uTimeout )
{_STT();

	if ( WantQuit() )
		return oex::oexFALSE;

	oex::TMem< oexEvent > mevReply;
	oex::oexBOOL bRet = oex::oexFALSE;

	// Do we want replies?
	if ( !pReply )
		pReplyEvent = oexNULL;

	// Ensure event if replies are needed
	else if ( !pReplyEvent )
		if ( !( pReplyEvent = mevReply.OexConstruct().Ptr() ) )
			return oex::oexFALSE;

//	if ( sMsg == oexT( "kill" ) )
//		oexPrintf( oexT( "Msg(): %s, To: %s, Caller: 0x%08x, Owner: 0x%08x\n" ), sMsg.c_str(), sPath.c_str(), (unsigned int)oexGetCurThreadId(), m_uOwnerThreadId );

	// If it's our own thread calling
	if ( m_uOwnerThreadId && oexGetCurThreadId() == m_uOwnerThreadId )
	{
		// Just process the message directly
		if ( pmapParams )
			bRet = ProcessMsg( sPath, sMsg, *pmapParams, pReply, pReplyEvent );

		else
		{   CSqMulti params;
			bRet = ProcessMsg( sPath, sMsg, params, pReply, pReplyEvent );
		} // end else

		// How'd it go?
		if ( !bRet )
			return oex::oexFALSE;

	} // end if

	else
	{ // Stuff message into buffer

		// Acquire lock
		oexAutoLock ll( m_cLock );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		// Add a message
		m_lstMsgQueue.push_back( SMsg( sPath, sMsg, pmapParams, pReplyEvent, pReply ) );

		// Signal that a message is waiting
		Signal();

		bRet = oex::oexTRUE;

	} // end message stuffing

	// Wait for reply if needed
	if ( mevReply.Size() )
	{
		// Wait for reply
		bRet = !mevReply->Wait( uTimeout );

	} // end if

	return bRet;
}

// Process messages
oex::oexBOOL CSqMsgQueue::ProcessMsgs()
{_STT();

	// forever
	for( ; ; ) 
	{
		SMsg msg;
		{ // Check for a message

			oexAutoLock ll( &m_cLock );
			if ( !ll.IsLocked() )
				return oex::oexFALSE;

			// Any messages waiting?
			if ( m_lstMsgQueue.begin() == m_lstMsgQueue.end() )
				return oex::oexTRUE;

			// Get message info
			msg = *m_lstMsgQueue.begin();
			m_lstMsgQueue.erase( m_lstMsgQueue.begin() );

			// Is it empty now?
			if ( m_lstMsgQueue.begin() == m_lstMsgQueue.end() )
				Reset();

		} // end scope

		// Deserialize params
		CSqMulti mapParams( msg.sParams );

		// Process the message
		ProcessMsg( msg.sPath, msg.sMsg, mapParams, msg.pReply, &msg.evReply );

		// We must stop processing if someone is waiting for a reply
		if ( msg.pReply )
			return oex::oexTRUE;

	} // end for

}

// Process a single message from the queue
oex::oexBOOL CSqMsgQueue::ProcessMsg( const stdString &sPath, stdString &sMsg, CSqMulti &mapParams, stdString *pReply, oexEvent *pReplyEvent )
{_STT();
	return oex::oexFALSE;
}

oex::oexBOOL CSqMsgQueue::is_path( const stdString &sPath )
{_STT();
	CSqMulti params; stdString sMsg = oexT( "is_path" );
	return ProcessMsg( sPath, sMsg, params, oexNULL, oexNULL );
}

oex::oexBOOL CSqMsgQueue::run( stdString *pReply, const stdString &sPath, const stdString &sName, const stdString &sScript )
{_STT();
	CSqMulti params;
	params[ oexT( "run" ) ] = sScript;
	params[ oexT( "name" ) ] = sName;
	return Msg( sPath, oexT( "msg" ), &params, pReply );
}

int CSqMsgQueue::shell( stdString *pReply, const stdString &sPath, const stdString &sFile, const stdString &sParams, const stdString &sDirectory )
{_STT();
	CSqMulti params;
	params[ oexT( "shell" ) ] = sFile;
	params[ oexT( "params" ) ] = sParams;
	params[ oexT( "dir" ) ] = sDirectory;
	return Msg( sPath, oexT( "msg" ), &params, pReply );
}

int CSqMsgQueue::exec( stdString *pReply, const stdString &sPath, const stdString &sFile, const stdString &sParams, const stdString &sDirectory )
{_STT();
	CSqMulti params;
	params[ oexT( "exec" ) ] = sFile;
	params[ oexT( "params" ) ] = sParams;
	params[ oexT( "dir" ) ] = sDirectory;
	return Msg( sPath, oexT( "msg" ), &params, pReply );
}

int CSqMsgQueue::sqexe( stdString *pReply, const stdString &sPath, const stdString &sParams, const stdString &sDir )
{_STT();
	CSqMulti params;
	params[ oexT( "sqexe" ) ] = sParams;
	params[ oexT( "dir" ) ] = sDir;
	return Msg( sPath, oexT( "msg" ), &params, pReply );
}

int CSqMsgQueue::sqexe_script( stdString *pReply, const stdString &sPath, const stdString &sScript, const stdString &sParams, const stdString &sDir )
{_STT();
	CSqMulti params;
	params[ oexT( "sqexe_script" ) ] = sScript;
	params[ oexT( "params" ) ] = sParams;
	params[ oexT( "dir" ) ] = sDir;
	return Msg( sPath, oexT( "msg" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::execute( stdString *pReply, const stdString &sPath, const stdString &sFunction )
{_STT();
	CSqMulti params;
	params[ oexT( "execute" ) ] = sFunction;
	return Msg( sPath, oexT( "msg" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::execute( stdString *pReply, const stdString &sPath, const stdString &sFunction,
								   const stdString &sP1 )
{_STT();
	CSqMulti params;
	params[ oexT( "execute1" ) ] = sFunction;
	params[ oexT( "p1" ) ] = sP1;
	return Msg( sPath, oexT( "msg" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::execute( stdString *pReply, const stdString &sPath, const stdString &sFunction,
								   const stdString &sP1, const stdString &sP2 )
{_STT();
	CSqMulti params;
	params[ oexT( "execute2" ) ] = sFunction;
	params[ oexT( "p1" ) ] = sP1;
	params[ oexT( "p2" ) ] = sP2;
	return Msg( sPath, oexT( "msg" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::execute( stdString *pReply, const stdString &sPath, const stdString &sFunction,
								   const stdString &sP1, const stdString &sP2, const stdString &sP3 )
{_STT();
	CSqMulti params;
	params[ oexT( "execute3" ) ] = sFunction;
	params[ oexT( "p1" ) ] = sP1;
	params[ oexT( "p2" ) ] = sP2;
	params[ oexT( "p3" ) ] = sP3;
	return Msg( sPath, oexT( "msg" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::execute( stdString *pReply, const stdString &sPath, const stdString &sFunction,
								   const stdString &sP1, const stdString &sP2, const stdString &sP3,
								   const stdString &sP4 )
{_STT();
	CSqMulti params;
	params[ oexT( "execute4" ) ] = sFunction;
	params[ oexT( "p1" ) ] = sP1;
	params[ oexT( "p2" ) ] = sP2;
	params[ oexT( "p3" ) ] = sP3;
	params[ oexT( "p4" ) ] = sP4;
	return Msg( sPath, oexT( "msg" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::spawn( stdString *pReply, const stdString &sPath, const stdString &sName, const stdString &sScript, int bFile )
{_STT();
	CSqMulti params;
	params[ oexT( "name" ) ] = sName;
	params[ oexT( "script" ) ] = sScript;
	params[ oexT( "file" ) ] = bFile ? oexT( "1" ) : oexT( "0" );
	return Msg( sPath, oexT( "spawn" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::spawn2( stdString *pReply, const stdString &sPath, const stdString &sName, const stdString &sScriptName, const stdString &sScript, int bFile )
{_STT();
	CSqMulti params;
	params[ oexT( "name" ) ] = sName;
	params[ oexT( "script" ) ] = sScript;
	params[ oexT( "script_name" ) ] = sScriptName;
	params[ oexT( "file" ) ] = bFile ? oexT( "1" ) : oexT( "0" );
	return Msg( sPath, oexT( "spawn" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::kill( stdString *pReply, const stdString &sPath )
{_STT();
	CSqMulti params;
	return Msg( sPath, oexT( "kill" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::terminate_thread( stdString *pReply, const stdString &sPath )
{_STT();
	CSqMulti params;
	return Msg( sPath, oexT( "terminate_thread" ), &params, pReply );
}

oex::oexBOOL CSqMsgQueue::get_children( stdString *pReply, const stdString &sPath )
{_STT();
	CSqMulti params;
	return Msg( sPath, oexT( "get_children" ), &params, pReply );
}

void CSqMsgQueue::tset( const stdString &sPath, const stdString &sKey, unsigned int uTo )
{_STT();
	CSqMulti params;
	params[ oexT( "key" ) ] = sKey;
	params[ oexT( "to" ) ] = oex2std( oexMks( uTo ) );
	Msg( sPath, oexT( "pb_tset" ), &params, oexNULL );
}


void CSqMsgQueue::set( const stdString &sPath, const stdString &sKey, const stdString &sVal )
{_STT();
	CSqMulti params;
	params[ oexT( "key" ) ] = sKey;
	params[ oexT( "val" ) ] = sVal;
	Msg( sPath, oexT( "pb_set" ), &params, oexNULL );
}

void CSqMsgQueue::aset( const stdString &sPath, const stdString &sKey, const stdString &sVal )
{_STT();
	CSqMulti params;
	params[ oexT( "key" ) ] = sKey;
	params[ oexT( "val" ) ] = sVal;
	Msg( sPath, oexT( "pb_aset" ), &params, oexNULL );
}

void CSqMsgQueue::mset( const stdString &sPath, const stdString &sKey, const stdString &sVal )
{_STT();
	CSqMulti params;
	params[ oexT( "key" ) ] = sKey;
	params[ oexT( "val" ) ] = sVal;
	Msg( sPath, oexT( "pb_mset" ), &params, oexNULL );
}

stdString CSqMsgQueue::get( const stdString &sPath, const stdString &sKey )
{_STT();
	stdString sRet;
	CSqMulti params;
	params[ oexT( "key" ) ] = sKey;
	Msg( sPath, oexT( "pb_get" ), &params, &sRet );
	return sRet;
}

stdString CSqMsgQueue::kget( const stdString &sPath, const stdString &sKey )
{_STT();
	stdString sRet;
	CSqMulti params;
	params[ oexT( "key" ) ] = sKey;
	Msg( sPath, oexT( "pb_kget" ), &params, &sRet );
	return sRet;
}

stdString CSqMsgQueue::isset( const stdString &sPath, const stdString &sKey )
{_STT();
	stdString sRet;
	CSqMulti params;
	params[ oexT( "key" ) ] = sKey;
	Msg( sPath, oexT( "pb_isset" ), &params, &sRet );
	return sRet;
}

stdString CSqMsgQueue::pb( const stdString &sPath )
{_STT();
	stdString sRet;
	CSqMulti params;
	Msg( sPath, oexT( "pb_all" ), &params, &sRet );
	return sRet;
}

stdString CSqMsgQueue::set_timer( const stdString &sPath, const stdString &sTo, const stdString &sCallback )
{_STT();
	stdString sRet;
	CSqMulti params;
	params[ oexT( "to" ) ] = sTo;
	params[ oexT( "cb" ) ] = sCallback;
	Msg( sPath, oexT( "set_timer" ), &params, &sRet );
	return sRet;
}

stdString CSqMsgQueue::set_timeout( const stdString &sPath, const stdString &sTo, const stdString &sCallback )
{_STT();
	stdString sRet;
	CSqMulti params;
	params[ oexT( "to" ) ] = sTo;
	params[ oexT( "cb" ) ] = sCallback;
	Msg( sPath, oexT( "set_timeout" ), &params, &sRet );
	return sRet;
}

stdString CSqMsgQueue::kill_timer( const stdString &sPath, const stdString &sId )
{_STT();
	stdString sRet;
	CSqMulti params;
	params[ oexT( "id" ) ] = sId;
	Msg( sPath, oexT( "kill_timer" ), &params, &sRet );
	return sRet;
}

stdString CSqMsgQueue::jget( const stdString &sPath, const stdString &sKey )
{_STT();
	stdString sRet;
	CSqMulti params;
	params[ oexT( "key" ) ] = sKey;
	Msg( sPath, oexT( "pb_jget" ), &params, &sRet );
	return sRet;
}

void CSqMsgQueue::jset( const stdString &sPath, const stdString &sKey, const stdString &sVal )
{_STT();
	CSqMulti params;
	params[ oexT( "key" ) ] = sKey;
	params[ oexT( "val" ) ] = sVal;
	Msg( sPath, oexT( "pb_jset" ), &params, oexNULL );
}

int CSqMsgQueue::asize( const stdString &sPath, const stdString &sKey )
{_STT();
	stdString sRet;
	CSqMulti params;
	params[ oexT( "key" ) ] = sKey;
	Msg( sPath, oexT( "pb_asize" ), &params, &sRet );
	return oexStrToLong( sRet.c_str() );
}

int CSqMsgQueue::addlog( const stdString &sPath, const stdString &sKey, int uType )
{_STT();
	stdString sRet;
	CSqMulti params;
	params[ oexT( "key" ) ] = sKey;
	params[ oexT( "type" ) ] = oex2std( oexMks( uType ) );
	Msg( sPath, oexT( "pb_addlog" ), &params, &sRet );
	return oexStrToLong( sRet.c_str() );
}

int CSqMsgQueue::removelog( const stdString &sPath, const stdString &sKey )
{_STT();
	stdString sRet;
	CSqMulti params;
	params[ oexT( "key" ) ] = sKey;
	Msg( sPath, oexT( "pb_removelog" ), &params, &sRet );
	return oexStrToLong( sRet.c_str() );
}

int CSqMsgQueue::setlogroot( const stdString &sPath, const stdString &sRoot )
{_STT();
	stdString sRet;
	CSqMulti params;
	params[ oexT( "root" ) ] = sRoot;
	Msg( sPath, oexT( "pb_setlogroot" ), &params, &sRet );
	return oexStrToLong( sRet.c_str() );
}

stdString CSqMsgQueue::getlogroot( const stdString &sPath )
{_STT();
	stdString sRet;
	CSqMulti params;
	Msg( sPath, oexT( "pb_getlogroot" ), &params, &sRet );
	return sRet;
}

int CSqMsgQueue::setlogfreq( const stdString &sPath, int nFreq )
{_STT();
	stdString sRet;
	CSqMulti params;
	params[ oexT( "freq" ) ] = oex2std( oexMks( nFreq ) );
	Msg( sPath, oexT( "pb_setlogfreq" ), &params, &sRet );
	return oexStrToLong( sRet.c_str() );
}

int CSqMsgQueue::getlogfreq( const stdString &sPath )
{_STT();
	stdString sRet;
	CSqMulti params;
	Msg( sPath, oexT( "pb_getlogfreq" ), &params, &sRet );
	return oexStrToLong( sRet.c_str() );
}

stdString CSqMsgQueue::getlogkeys( const stdString &sPath, int nTime )
{_STT();
	stdString sRet;
	CSqMulti params;
	params[ oexT( "freq" ) ] = oex2std( oexMks( nTime ) );
	Msg( sPath, oexT( "pb_getlogkeys" ), &params, &sRet );
	return sRet;
}

stdString CSqMsgQueue::getlog( const stdString &sPath, const stdString &sKey, int nStart, int nStop, int nInterval, int nDataType, int nMethod )
{_STT();
	stdString sRet;
	CSqMulti params;
	params[ oexT( "key" ) ] = sKey;
	params[ oexT( "start" ) ] = oex2std( oexMks( nStart ) );
	params[ oexT( "stop" ) ] = oex2std( oexMks( nStop ) );
	params[ oexT( "interval" ) ] = oex2std( oexMks( nInterval ) );
	params[ oexT( "type" ) ] = oex2std( oexMks( nDataType ) );
	params[ oexT( "method" ) ] = oex2std( oexMks( nMethod ) );
	Msg( sPath, oexT( "pb_getlog" ), &params, &sRet );
	return sRet;
}

stdString CSqMsgQueue::getlogbin( const stdString &sPath, const stdString &sKey, int nStart, int nStop, int nInterval, const stdString &sExt )
{_STT();
	stdString sRet;
	CSqMulti params;
	params[ oexT( "key" ) ] = sKey;
	params[ oexT( "start" ) ] = oex2std( oexMks( nStart ) );
	params[ oexT( "stop" ) ] = oex2std( oexMks( nStop ) );
	params[ oexT( "interval" ) ] = oex2std( oexMks( nInterval ) );
	params[ oexT( "ext" ) ] = sExt;
	Msg( sPath, oexT( "pb_getlogbin" ), &params, &sRet );
	return sRet;
}

void CSqMsgQueue::logbin( const stdString &sPath, const stdString &sKey, const stdString &sBin, int nType, int nTime, int nTimeMs, int nBuffering )
{_STT();
	CSqMulti params;
	params[ oexT( "key" ) ] = sKey;
	params[ oexT( "bin" ) ] = sBin;
	params[ oexT( "type" ) ] = oex2std( oexMks( nType ) );
	params[ oexT( "time" ) ] = oex2std( oexMks( nTime ) );
	params[ oexT( "timems" ) ] = oex2std( oexMks( nTimeMs ) );
	params[ oexT( "buffering" ) ] = oex2std( oexMks( nBuffering ) );
	Msg( sPath, oexT( "pb_logbin" ), &params, oexNULL );
}


int CSqMsgQueue::resetlog( const stdString &sPath )
{_STT();
	stdString sRet;
	CSqMulti params;
	Msg( sPath, oexT( "pb_resetlog" ), &params, &sRet );
	return oexStrToLong( sRet.c_str() );
}

