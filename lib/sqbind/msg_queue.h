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

	/// Default constructor
	CSqMsgQueue();

	/// Destructor
	virtual ~CSqMsgQueue();

	/// Releases resources
	virtual void Destroy();

	/// Sends a command to the thread
	/**
		\param [in]		sPath		-	Destination for the message
		\param [in]     sMsg        -   Command
		\param [in]     mapParams   -   Parameters
		\param [out]    pmapReply   -   Receives reply
		\param [in]		uTimeout	-	Maximum amount of time to wait
										for a reply.

		If pmapReply is not NULL, the function waits for a reply
		from the thread.
	*/
	oex::oexBOOL Msg( stdString sPath, stdString sMsg, CSqMap *pmapParams = oexNULL, stdString *pReply = oexNULL, oex::oexUINT uTimeout = eLockTimeout );

public:

	/// Thread message structure
	struct SMsg
	{
		/// Default Constructor
		SMsg();

		/// Initializer
		SMsg( const stdString x_sPath, const stdString x_sMsg, CSqMap *x_pmapParams, oexEvent x_evReply, stdString *x_pReply );

		// Copy constructor
		SMsg( const SMsg &x_rMsg );

		/// Command
		stdString									sMsg;

		/// Destination
		stdString									sPath;

		/// Params
		stdString									sParams;

		/// Reply event
		oex::os::CResource							evReply;

		/// Reply object
		stdString 									*pReply;

	};

	/// Process a single message from the queue
	virtual oex::oexBOOL ProcessMsg( stdString &sPath, stdString &sMsg, CSqMap &mapParams, stdString *pReply );

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

	/// Sets the owning thread id
	void SetOwnerThreadId( oex::oexUINT x_uOwnerThreadId )
	{	m_uOwnerThreadId = x_uOwnerThreadId; }

	/// Returns the owning thread id
	oex::oexUINT GetOwnerThreadId()
	{	return m_uOwnerThreadId; }

private:

	/// Owner thread id
	oex::oexUINT								m_uOwnerThreadId;

	/// Message queue lock
	oexLock		                                m_cLock;

	/// Message list type
	typedef oexStdList( SMsg )					t_MsgQueue;

	/// The message queue
	t_MsgQueue                                  m_lstMsgQueue;
};

SQBIND_DECLARE_INSTANCE( CSqMsgQueue );

