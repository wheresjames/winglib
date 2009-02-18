/*------------------------------------------------------------------
// msg_queue.h
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

#pragma once

class CSqMsgQueue : public oexEvent
{
public:

	enum
	{
		/// Thread lock timeout value
		eLockTimeout    = 60000
	};

	/// Parameter object type
	typedef std::map< std::tstring, std::tstring >    t_Params;

public:

	/// Default constructor
	CSqMsgQueue()
	{
	}

	/// Destructor
	virtual ~CSqMsgQueue()
	{
		// Acquire lock
		oexAutoLock ll( m_cLock );
		if ( !ll.IsLocked() )
			return;

		Destroy();
	}

	/// Releases resources
	virtual void Destroy()
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
	oex::oexBOOL Msg( std::tstring sMsg, t_Params *pmapParams = oexNULL, t_Params *pmapReply = oexNULL, oex::oexUINT uTimeout = eLockTimeout )
	{
		oexEvent evReply;

		// If it's our own thread calling
		if ( m_uCurrentThreadId && oex::os::CSys::GetCurrentThreadId() == m_uCurrentThreadId )
		{
			// Just process the message directly
			if ( pmapParams )
				return ProcessMsg( sMsg, *pmapParams, pmapReply );

			else
			{   t_Params params;
				return ProcessMsg( sMsg, params, pmapReply );
			} // end else

		} // end if

		{ // Stuff message into buffer

			// Acquire lock
			oexAutoLock ll( m_cLock );
			if ( !ll.IsLocked() )
				return FALSE;

			// Reply event handle needed?
			if ( !pmapReply )
				evReply.Destroy();

			// Add a message
			m_lstMsgQueue.push_back( SMsg( sMsg, pmapParams, evReply, pmapReply ) );

			// Signal that a message is waiting
			Signal();

		} // end message stuffing

		// Wait for relpy if needed
		if ( pmapReply )
		{
			// Wait for reply
			oex::oexBOOL bSuccess = evReply.Wait( uTimeout );

			// Punt if we got the reply
			if ( bSuccess )
				return TRUE;

			// Acquire lock
			oexAutoLock ll( &m_cLock );
			if ( !ll.IsLocked() )
			{
				// You're screwed here because pmapReply is dangling
				// But don't worry, this won't ever happen ;)
	//            ASSERT( 0 );

				// Only safe thing to do would be kill the thread...
				KillThread();

				return FALSE;

			} // end if

			// Clear the list so pmapReply is not dangling
			m_lstMsgQueue.clear();

		} // end if

		return TRUE;
	}

public:


	/// Thread message structure
	struct SMsg
	{
		/// Default Constructor
		SMsg() { pmapReply = NULL; }

		/// Initializer
		SMsg( const std::tstring x_sMsg, t_Params *x_pmapParams, oex::os::CResource x_hReply, t_Params *x_pmapReply )
		{   sMsg = x_sMsg;
			if ( x_pmapParams )
				mapParams = *x_pmapParams;
			hReply = x_hReply;
			pmapReply = x_pmapReply;
		}

		// Copy constructor
		SMsg( const SMsg &x_rMsg )
		{   sMsg = x_rMsg.sMsg;
			mapParams = x_rMsg.mapParams;
			hReply = x_rMsg.hReply;
			pmapReply = x_rMsg.pmapReply;
		}

		/// Command
		std::tstring                                sMsg;

		/// Params
		t_Params                                    mapParams;

		/// Reply event
		oex::os::CResource							hReply;

		/// Reply object
		t_Params                                    *pmapReply;

	};

protected:

	/// Process messages
	oex::oexBOOL ProcessMsgs()
	{
		// Acquire lock
		oexAutoLock ll( &m_cLock );
		if ( !ll.IsLocked() )
			return FALSE;

		// Any messages waiting?
		if ( m_lstMsgQueue.begin() == m_lstMsgQueue.end() )
			return TRUE;

		// Process messages
		t_MsgQueue::iterator it;
		while ( m_lstMsgQueue.end() != ( it = m_lstMsgQueue.begin() ) )
		{
			// Process the message
			ProcessMsg( it->sMsg, it->mapParams, it->pmapReply );

			// We must stop processing if someone is waiting for a reply
			if ( it->pmapReply )
			{
				// Signal that reply is ready
				//::SetEvent( it->hReply );
				it->hReply.Signal();
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
	virtual oex::oexBOOL ProcessMsg( std::tstring &sMsg, t_Params &mapParams, t_Params *pmapReply )
	{
		return TRUE;
	}

	/// Returns a handle to the message wait function
//	oex::os::CResource GetMsgWaitHandle() { return m_evtMsgWaiting; }

	/// Over-ride for thread killing function
	virtual void KillThread() {}

private:

public:

	virtual oex::oexBOOL execute( t_Params *pReply, const std::tstring &sName, const std::tstring &sFunction )
	{	t_Params params;
		params[ oexT( "name" ) ] = sName;
		params[ oexT( "execute" ) ] = sFunction;
		return Msg( oexT( "msg" ), &params, pReply );
	}

	virtual oex::oexBOOL execute( t_Params *pReply, const std::tstring &sName, const std::tstring &sFunction, const std::tstring &sP1 )
	{	t_Params params;
		params[ oexT( "name" ) ] = sName;
		params[ oexT( "execute1" ) ] = sFunction;
		params[ oexT( "p1" ) ] = sP1;
		return Msg( oexT( "msg" ), &params, pReply );
	}


private:

	/// Our thread id
	oex::oexUINT								m_uCurrentThreadId;

	/// Set when a message is waiting in the queue
//	oexEvent									m_evMsgWaiting;

	/// Message queue lock
	oexLock		                                m_cLock;

	/// Message list type
	typedef std::list< SMsg >                   t_MsgQueue;

	/// The message queue
	t_MsgQueue                                  m_lstMsgQueue;
};

SQBIND_DECLARE_INSTANCE( CSqMsgQueue );

/*
class SQ_CSqMsgQueue
{
public:

	SQ_CSqMsgQueue()
	{
		m_pMsgQueue = oexNULL;
	}

	SQ_CSqMsgQueue( const SQ_CSqMsgQueue &r )
	{	m_pMsgQueue = r.m_pMsgQueue; }

	void Set( CSqMsgQueue *x_pMsgQueue )
	{	m_pMsgQueue = x_pMsgQueue; }

	CSqMsgQueue* Get()
	{	return m_pMsgQueue; }

public:
	static void Register( SquirrelVM &vm )
	{
		SqPlus::SQClassDef< SQ_CSqMsgQueue >( vm, oexT( "CSqMsgQueue" ) )
			;
	}

private:

	CSqMsgQueue		*m_pMsgQueue;
};

SQBIND_DECLARE_INSTANCE( SQ_CSqMsgQueue );
*/
