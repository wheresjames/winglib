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
	typedef oexStdMap( stdString, stdString )	t_Params;

public:

	SQBIND_CLASS_CTOR_BEGIN( CSqMsgQueue )
	SQBIND_CLASS_CTOR_END( CSqMsgQueue )

	/// Default constructor
	CSqMsgQueue();

	/// Destructor
	virtual ~CSqMsgQueue();

	// Copy semantics
	CSqMsgQueue( const CSqMsgQueue &r ) {}
	CSqMsgQueue& operator =( const CSqMsgQueue &r ) { return *this; }

	/// Releases resources
	virtual void Destroy();

	/// Sends a command to the thread
	/**
		\param [in]		sPath		-	Destination for the message
		\param [in]     sMsg        -   Command
		\param [in]     mapParams   -   Parameters
		\param [out]    pmapReply   -   Receives reply
		\param [out]	pReplyEvent	-	Signaled when reply is processed
		\param [in]		uTimeout	-	Maximum amount of time to wait
										for a reply.

		If pmapReply is not NULL, the function waits for a reply
		from the thread.
	*/
	oex::oexBOOL Msg( stdString sPath, stdString sMsg, CSqMap *pmapParams = oexNULL, stdString *pReply = oexNULL, oexEvent *pReplyEvent = oexNULL, oex::oexUINT uTimeout = eLockTimeout );

public:

	/// Thread message structure
	struct SMsg
	{
		/// Default Constructor
		SMsg();

		/// Initializer
		SMsg( const stdString x_sPath, const stdString x_sMsg, CSqMap *x_pmapParams, oexEvent *x_evReply, stdString *x_pReply );

		// Copy constructor
		SMsg( const SMsg &x_rMsg );

		/// Command
		stdString									sMsg;

		/// Destination
		stdString									sPath;

		/// Params
		stdString									sParams;

		/// Reply event
		oexEvent									evReply;

		/// Reply object
		stdString 									*pReply;

	};

	/// Process a single message from the queue
	virtual oex::oexBOOL ProcessMsg( const stdString &sPath, stdString &sMsg, CSqMap &mapParams, stdString *pReply, oexEvent *pReplyEvent );

protected:

	/// Process messages
	oex::oexBOOL ProcessMsgs();

	/// Over-ride for thread killing function
	virtual void KillThread() {}

private:

public:

	/// Spawns a script in another thread
	oex::oexBOOL spawn( stdString *pReply, const stdString &sPath, const stdString &sName, const stdString &sScript, int bFile );
	oex::oexBOOL kill( stdString *pReply, const stdString &sPath );
	oex::oexBOOL is_path( const stdString &sPath );

	/// Runs the specified script
	oex::oexBOOL run( stdString *pReply, const stdString &sPath, const stdString &sScript );

	/// Executes the specified function with params
	oex::oexBOOL execute( stdString *pReply, const stdString &sPath, const stdString &sFunction );
	oex::oexBOOL execute( stdString *pReply, const stdString &sPath, const stdString &sFunction,
						  const stdString &sP1 );
	oex::oexBOOL execute( stdString *pReply, const stdString &sPath, const stdString &sFunction,
						  const stdString &sP1, const stdString &sP2 );
	oex::oexBOOL execute( stdString *pReply, const stdString &sPath, const stdString &sFunction,
						  const stdString &sP1, const stdString &sP2, const stdString &sP3 );
	oex::oexBOOL execute( stdString *pReply, const stdString &sPath, const stdString &sFunction,
						  const stdString &sP1, const stdString &sP2, const stdString &sP3,
						  const stdString &sP4 );

	/// Returns a list of child nodes
	oex::oexBOOL get_children( stdString *pReply, const stdString &sPath );

	/// Sets the owning thread id
	void SetOwnerThreadId( oex::oexUINT x_uOwnerThreadId )
	{	m_uOwnerThreadId = x_uOwnerThreadId; }

	/// Returns the owning thread id
	oex::oexUINT GetOwnerThreadId()
	{	return m_uOwnerThreadId; }

	/// Call to request thread exit
	void RequestQuit() { m_bWantQuit = oex::oexTRUE; }

	/// Returns quit request status
	oex::oexBOOL WantQuit() { return m_bWantQuit; }

	/// Returns non-zero when messages will no longer be processed
	virtual oex::oexBOOL Running() { return oex::oexFALSE; }

	/// Sets the specified property
	void set( const stdString &sPath, const stdString &sKey, const stdString &sVal );

	/// Gets the specified property
	stdString get( const stdString &sPath, const stdString &sKey );

	/// Returns non-zero if the specified value exists
	stdString isset( const stdString &sPath, const stdString &sKey );

	/// Gets the property bag
	stdString pb( const stdString &sPath );

private:

	/// Owner thread id
	oex::oexUINT								m_uOwnerThreadId;

	/// Message queue lock
	oexLock		                                m_cLock;

	/// Message list type
	typedef oexStdList( SMsg )					t_MsgQueue;

	/// The message queue
	t_MsgQueue                                  m_lstMsgQueue;

	/// Non-zero if script has requested exit
	oex::oexBOOL								m_bWantQuit;
};

}; SQBIND_DECLARE_INSTANCE( sqbind::CSqMsgQueue, CSqMsgQueue ); namespace sqbind {
