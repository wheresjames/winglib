/*------------------------------------------------------------------
// script_thread.h
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

class CScriptThread :
		public oex::CThread,
		public CSqMsgQueue
{
public:

	struct STimerInfo
	{
		/// Timeout time in seconds
		oex::oexUINT	uTimeout;

		/// Timeout in milli-seconds
		oex::oexUINT	uMsTimeout;

		/// Timeout value
		oex::oexUINT	uTo;

		/// callback function
		stdString		sCallback;
	};

public:

	/// Property bag type
//	typedef oexStdMap( stdString, stdString )			t_PropertyBag;
	typedef oex::CPropertyBag							t_PropertyBag;

    /// Map type for child script  thread objects
	typedef oexStdMap( stdString, CScriptThread* )		t_ScriptList;

	/// Map type for timer callbacks
	typedef oexStdMap( oex::oexUINT, STimerInfo )		t_TimerList;

public:

    /// Default constructor
    CScriptThread();

    /// Destructor
	virtual ~CScriptThread();

    /// Releases resources
    void Destroy();

    /// Sets the module manager pointer
	void SetModuleManager( sqbind::CModuleManager *pMm );

    /// Sets the script information
	void SetScript( const sqbind::stdString &sScript, oex::oexBOOL bFile );

    /// Sets the parent script pointer
    void SetParentScript( CSqMsgQueue *pParent );

protected:

    /// Initialize Squirrel engine
	virtual oex::oexBOOL InitThread( oex::oexPVOID x_pData );

    /// Run the thing
	virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData );

    /// Shutdown Squirrel engine
	virtual oex::oexINT EndThread( oex::oexPVOID x_pData );

    /// Process a single message from the queue
	virtual oex::oexBOOL ProcessMsg( const stdString &x_sPath, stdString &sMsg, CSqMap &mapParams, stdString *pReply, oexEvent *pReplyEvent );

	/// Executes a single message
	oex::oexBOOL ExecuteMsg( stdString &sMsg, CSqMap &mapParams, stdString *pReply, oexEvent *pReplyEvent );

    /// Releases all child scripts
    void DestroyChildScripts();

	/// Frees stopped child scripts
    void CleanupChildScripts();

	/// Returns non-zero when messages will no longer be processed
	virtual oex::oexBOOL Running() { return CThread::IsRunning(); }

protected:

    /// Spawns a new child script thread
	void OnSpawn( CSqMap &mapParams, stdString *pReply );

    /// Routes a message to the proper script
	void OnMsg( CSqMap &mapParams, stdString *pReply );

    /// Processes a script message
//	void OnOnMsg( CSqMap &mapParams, stdString *pReply );

public:

	/// Call to quit the execution
	void Quit() { m_bQuit = oex::oexTRUE; }

	/// Sets the script name
	void SetName( stdString sName )
	{	m_sName = sName; }

	/// Returns the script name
	stdString GetName()
	{	return m_sName; }

	/// Call to set a function that is called when thread starts
	/// to export custom symbols
	void SetExportFunction( PFN_SQBIND_Export_Symbols fn, sqbind::SSqAllocator *pa );

	/// Set custom include function
	void SetIncludeScriptFunction( sqbind::CSqEngine::t_IncludeScript f )
	{	m_cSqEngine.SetIncludeScriptFunction( f ); }

	/// Sets the script name
	void SetScriptName( sqbind::stdString sName )
	{	m_cSqEngine.SetScriptName( sName.c_str() ); }

	/// Return a pointer to the engine
	virtual CSqEngine* GetEngine() { return &m_cSqEngine; }

	/// Returns the queue for the specified path
	virtual CSqMsgQueue* GetQueue( const stdString &x_sPath );

	/// Sets a timer
	oex::oexUINT SetTimer( oex::oexUINT x_uTo, const stdString &x_sCallback );

	/// Sets a timeout
	oex::oexUINT SetTimeout( oex::oexUINT x_uTo, const stdString &x_sCallback );

	/// Kills the specified timer
	oex::oexBOOL KillTimer( oex::oexUINT x_uId );

	/// Executes timer callbacks
	oex::oexINT RunTimers();

private:

	/// Set to non-zero when the thread should exit
	oex::oexBOOL						m_bQuit;

    /// Squirrel engine
	sqbind::CSqEngine					m_cSqEngine;

    /// Pointer to the module manager
    sqbind::CModuleManager  			*m_pModuleManager;

    /// List of child scripts
    t_ScriptList					 	m_lstScript;

    /// Script to load
    stdString							m_sName;

    /// Script to load
    stdString							m_sScript;

    /// If non-zero, m_sScript contains a file name
	oex::oexBOOL						m_bFile;

    /// Parent script
    CSqMsgQueue							*m_pParentScript;

	/// Property bag lock
	oexLock								m_lockPb;

	/// Generic property bag
	t_PropertyBag						m_pb;

	/// Timers
	t_TimerList							m_to;

	/// Next timer id
	oex::oexUINT						m_tid;

	/// Timeout frequency
	oex::oexUINT						m_uToFreq;

	/// Elapsed time
	oex::oexUINT						m_uToElapsed;

	oex::oexDOUBLE						m_dToValue;
};
