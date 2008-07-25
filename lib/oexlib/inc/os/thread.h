/*------------------------------------------------------------------
// thread.h
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

//==================================================================
// CThread
//
/// Thread class
/**
*/
//==================================================================
class CThread
{
public:

    /// Thread handle
    typedef oexPVOID t_HTHREAD;

	/// Invalid socket value
	static oexCONST t_HTHREAD c_InvalidThread;

public:

	struct SThreadData
	{
		/// Thread context, posix only so far
		oexPVOID				pContext;
	};

public:

	/// Default Constructor
	CThread();

	/// Destrucotr
	virtual ~CThread();

	//==============================================================
	// Start()
	//==============================================================
	/// Starts the thread
	/**
        \param [in] x_uSleep        -   Sleep time in mSec.  If not
                                        zero, a sleep of this length
                                        will be injected after every
                                        call to DoThread().
		\param [in] x_pData			-	Custom data passed on to thread

		\return Non-zero if thread was started.

		\see
	*/
	virtual oexBOOL Start( oexUINT x_uSleep = 30, oexPVOID x_pData = oexNULL );

	//==============================================================
	// Stop()
	//==============================================================
	/// Stops any running thread
	/**
		\param [in] x_bKill	    -	Set to non-zero to kill thread if
                                    it fails to stop gracefully.
		\param [in] x_dwWait    -	Time in milli-seconds to wait for
								    thread to stop before killing it.

		\return Non-zero if success

		\see
	*/
	virtual oexBOOL Stop( oexBOOL x_bKill = oexTRUE, oexUINT x_uWait = oexDEFAULT_WAIT_TIMEOUT );

protected:

	//==============================================================
	// InitThread()
	//==============================================================
	/// Do thread initialization here
	/**
		\param [in] x_pData - User defined data

		The pData value passed to these functions represent the original value
		in m_pvoidData when the thread started.  Changing m_pvoidData in these
		functions does not affect the passed value to the next function.
		This -hopefully- garentees that x_pData is always equal to the value
		passed to Start()

		Overide this function to provide custom thread Initialization
		If you don't need initialization you can just use DoThread()

		x_pData - whatever was passed to StartThread()

		return non-zero if you want to continue the thread ( calling DoThread() )
		return zero to end the thread now

		\return Non-zero to continue thread, zero to terminate thread execution.

		\see DoThread(), EndThread(), GhostThread()
	*/
	virtual oexBOOL InitThread( oexPVOID x_pData ) { return oexTRUE; }

	//==============================================================
	// DoThread()
	//==============================================================
	/// Normal thread work
	/**
		\param [in] x_pData	-	User defined value

		Overide this function to provide custom thread work

		This function is called over and over until it returns zero
		or Stop() is called

		x_pData - whatever was passed to Start()

        return non-zero if you want more processing
        (i.e. DoThread() will be called again )

		return zero to end the thread

		\return Non-zero to continue thread, zero to terminate thread execution.

		\see InitThread(), EndThread(), GhostThread()
	*/
	virtual oexBOOL DoThread( oexPVOID x_pData ) { return oexFALSE; }

	//==============================================================
	// EndThread()
	//==============================================================
	/// Do thread cleanup here
	/**
		\param [in] x_pData	-	User defined value

		Overide this function to provide custom cleanup

		x_pData - whatever was passed to StartThread()

		The return value is ignored
		if you want to return a thread value then set m_dwThreadReturn

		\return Return value is the return value for the thread

		\see InitThread(), DoThread(), GhostThread()
	*/
	virtual oexINT EndThread( oexPVOID x_pData ) { return 0; }

public:

    /// Returns the running state of the thread
    oexBOOL IsRunning()
    {   return m_hThread != vInvalidThread(); }

	/// Invalid socket handle value
    oexCONST t_HTHREAD vInvalidThread()
    {   return c_InvalidThread; }

    /// Returns the user value passed to Start()
    oexPVOID GetUserData() { return m_pData; }

    /// Returns the injected thread sleep time
    oexUINT GetSleepTime() { return m_uSleep; }

	//==============================================================
	// GetThreadCount()
	//==============================================================
	/// Returns the total number of threads started using this class
	/**
		This is retrieved from a static variable.  Indicates
		the number of threads started using this class.

		\return Number of threads started using this class

		\see
	*/
	static oexUINT GetThreadCount();

	//==============================================================
	// GetRunningThreadCount()
	//==============================================================
	/// Returns the total number of threads running using this class
	/**
		This is retrieved from a static variable.  Indicates
		the number of threads currently running using this class.

		\return Number of threads running using this class

		\see
	*/
	static oexUINT GetRunningThreadCount();

    /// Returns the encapsulated thread handle
    t_HTHREAD GetThreadHandle() { return m_hThread; }

    /// Waits for thread to initialize
    oexBOOL WaitInit( oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
    {   return m_evInit.Wait( x_uTimeout ); }

    /// Waits for the thread to quit
    oexBOOL WaitThreadStop( oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
    {   return os::CSys::waitSuccess == os::CSys::WaitForSingleObject( m_hThread, x_uTimeout ); }

protected:

	//==============================================================
	// IncThreadCount()
	//==============================================================
	/// Increments the total thread count
	static void IncThreadCount();

	//==============================================================
	// IncRunningThreadCount()
	//==============================================================
	/// Increments the running thread count
	static void IncRunningThreadCount();

	//==============================================================
	// DecRunningThreadCount()
	//==============================================================
	/// Decrements the running thread count
	static void DecRunningThreadCount();

private:

    /// The thread procedure
    static oexPVOID ThreadProc( oexPVOID x_pData );

    // Protect access to ThreadProc()
    class CThreadProcImpl;

    friend class CThreadProcImpl;

private:

    /// Handle to the thread
    t_HTHREAD                               m_hThread;

	/// Thread data
	SThreadData								m_td;

    /// The thread id
    oexUINT                                 m_uThreadId;

    /// Users data
    oexPVOID                                m_pData;

    /// Inject sleep
    oexUINT                                 m_uSleep;

    /// Count of all threads that were created
    static oexLONG                          m_lThreadCount;

    /// Number of threads still running
    static oexLONG                          m_lRunningThreadCount;

protected:

    /// Quit signal
    CTlEvent                                m_evQuit;

    /// Signals when the thread has initialized
    CTlEvent                                m_evInit;

};

