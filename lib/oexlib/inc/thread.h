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
// CLock
//
/// Generic thread lock
//==================================================================
class CLock : public os::CResource
{
public:

	/// Default constructor
	CLock( oexCSTR x_pName = oexNULL )
	{	NewLock( x_pName ); }

	/// Create lock
	oexBOOL Create( oexCSTR x_pName = oexNULL )
	{	return 0 == NewLock( x_pName ); }

};


//==================================================================
// CScopeLock
//
/// Use this to lock and automatically unlock CResource objects
/**
	Use this to lock and automatically unlock CResource objects
*/
//==================================================================
class CScopeLock
{

public:

	/// Default constructor
	CScopeLock()
    {
        m_ptr = oexNULL;
    }

	/// Destructor - Unlocks the underlying CResource object
	virtual ~CScopeLock()
    {
        Unlock();
    }

	//==============================================================
	// CScopeLock()
	//==============================================================
	/// Constructor - Takes a CResource pointer
	/**
		\param [in] x_ptr		-	Pointer to CResource object
		\param [in] x_uTimeout	-	Maximum time in milli-seconds to
									wait for lock.
	*/
	CScopeLock( os::CResource *x_ptr, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{   m_ptr = oexNULL;
		if ( oexCHECK_PTR( x_ptr ) )
            if ( 0 == x_ptr->Wait( x_uTimeout ) )
                m_ptr = x_ptr;
    }

	//==============================================================
	// CScopeLock()
	//==============================================================
	/// Constructor - Takes a CResource reference
	/**
		\param [in] x_lock		-	Reference to CResource object
		\param [in] x_uTimeout	-	Maximum time in milli-seconds to
									wait for lock.
	*/
	CScopeLock( os::CResource &x_lock, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{   m_ptr = oexNULL;
        if ( 0 == x_lock.Wait( x_uTimeout ) )
            m_ptr = &x_lock;
    }

	//==============================================================
	// CScopeLock()
	//==============================================================
	/// Constructor - Takes a CResource reference
	/**
		\param [in] x_ptr		-	Pointer to CResource object
		\param [in] x_uTimeout	-	Maximum time in milli-seconds to
									wait for lock.
	*/
	CScopeLock( CLock *x_ptr, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{   m_ptr = oexNULL;
		if ( oexCHECK_PTR( x_ptr ) )
            if ( 0 == x_ptr->Wait( x_uTimeout ) )
                m_ptr = x_ptr;
    }

	//==============================================================
	// CScopeLock()
	//==============================================================
	/// Constructor - Takes a CResource reference
	/**
		\param [in] x_lock		-	Reference to CLock object
		\param [in] x_uTimeout	-	Maximum time in milli-seconds to
									wait for lock.
	*/
	CScopeLock( CLock &x_lock, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{   m_ptr = oexNULL;
        if ( 0 == x_lock.Wait( x_uTimeout ) )
            m_ptr = &x_lock;
    }

	//==============================================================
	// IsLocked()
	//==============================================================
	/// Returns true if the local object is locked
	oexBOOL IsLocked( oexUINT x_uWho = oexGetCurThreadId() )
    {
		if ( !oexCHECK_PTR( m_ptr ) )
			return oexFALSE;

		return m_ptr->GetOwner() == x_uWho;
    }

	//==============================================================
	// Attach()
	//==============================================================
	/// Attaches to an existing CResource without locking
	void Attach( os::CResource *x_ptr )
    {
        Unlock();
        m_ptr = x_ptr;
    }

	//==============================================================
	// Detach()
	//==============================================================
	/// Detaches from CResource without unlocking
	void Detach()
    {
        m_ptr = oexNULL;
    }

	//==============================================================
	// Lock()
	//==============================================================
	/// Locks a CResource object.  Returns true only if lock was achieved
	/**
		\param [in] x_ptr		-	Pointer to CResource object
		\param [in] x_uTimeout	-	Maximum time in milli-seconds to
									wait for lock.

		\return Non-zero if lock was acquired.

		\see
	*/
	oexBOOL Lock( os::CResource *x_ptr, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{
		// Do we already have the lock?
        if ( x_ptr == m_ptr )
            return oexTRUE;

		// Unlock existing
        Unlock();

		// Ensure valid lock pointer
		if ( !oexCHECK_PTR( x_ptr ) )
            return oexFALSE;

		// Attempt to acquire the lock
		if ( x_ptr->Wait( x_uTimeout ) )
			return oexFALSE;

        m_ptr = x_ptr;

		return oexTRUE;
	}

	//==============================================================
	// Unlock()
	//==============================================================
	/// Unlocks attached CResource object
	/**
		\return Always returns non-zero
	*/
	oexBOOL Unlock()
	{
		if ( !oexCHECK_PTR( m_ptr ) )
            return oexTRUE;

		m_ptr->Reset();
        m_ptr = oexNULL;

		return oexTRUE;
	}

private:

	/// Pointer to CResource object
	os::CResource		*m_ptr;

};

//==================================================================
// CEvent
//
/// Thread event
//==================================================================
class CEvent : public os::CResource
{
public:

	/// Default constructor
	CEvent( oexCSTR x_pName = oexNULL )
	{	NewEvent( x_pName ); }

	/// Create lock
	oexBOOL Create( oexCSTR x_pName = oexNULL )
	{	return 0 == NewEvent( x_pName ); }

};

//==================================================================
// CSignal
//
/// Thread signals
//==================================================================
class CSignal : public os::CResource
{
public:

	/// Default constructor
	CSignal( oexCSTR x_pName = oexNULL )
	{	Create( x_pName ); }

	/// Create lock
	oexBOOL Create( oexCSTR x_pName = oexNULL );

	/// Destroys the event
	virtual void Destroy() { os::CResource::Destroy(); m_nosig.Destroy(); }

	/// Signals the event
	virtual oexRESULT Signal( oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{	return m_nosig.Reset() | os::CResource::Signal(); }

	/// Resets the event
	virtual oexRESULT Reset( oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{	return os::CResource::Reset() | m_nosig.Signal(); }

	/// Returns reset signal
	os::CResource& GetResetEvent() { return m_nosig; }

private:

	/// Triggered when not signaled
	os::CResource	m_nosig;
};


//==================================================================
// CThread
//
/// Thread class
/**
*/
//==================================================================
class CThread : public os::CResource
{

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
		\param [in] x_pData			-	Custom data passed on to thread
        \param [in] x_uSleep        -   Sleep time in mSec.  If not
                                        zero, a sleep of this length
                                        will be injected after every
                                        call to DoThread().

		\return Non-zero if thread was started.

		\see
	*/
	virtual oexRESULT Start( oexPVOID x_pData = oexNULL, oexUINT x_uSleep = 30 );

	//==============================================================
	// Stop()
	//==============================================================
	/// Stops any running thread
	/**
		\param [in] x_uWait    	-	Time in milli-seconds to wait for
								    thread to stop before killing it.
		\param [in] x_bKill	    -	Set to non-zero to kill thread if
                                    it fails to stop gracefully.

		\return Non-zero if success

		\see
	*/
	virtual oexRESULT Stop( oexUINT x_uWait = oexDEFAULT_WAIT_TIMEOUT, oexBOOL x_bKill = oexTRUE );

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
    oexBOOL IsRunning();

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

    /// Waits for thread to initialize
    oexBOOL WaitThreadExit( oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT );

	/// Returns a reference to the stop event
	CResource& GetStopEvent() { return m_evStop; }

	/// Returns a reference to the init event
	CResource& GetInitEvent() { return m_evInit; }

	/// Return value of InitThread()
	/// call GetInitEvent().Wait() to ensure it's valid
	oexBOOL GetInitStatus() { return m_bInitStatus; }

private:

	//==============================================================
	// ThreadProc()
	//==============================================================
	/// Static thread callback function
	static oexPVOID ThreadProc( oexPVOID x_pData );

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

    /// Users data
    oexPVOID                                m_pData;

    /// Inject sleep
    oexUINT                                 m_uSleep;

	/// Return value of InitThread()
	oexBOOL									m_bInitStatus;

    /// Count of all threads that were created
    static oexLONG                          m_lThreadCount;

    /// Number of threads still running
    static oexLONG                          m_lRunningThreadCount;

protected:

    /// Quit signal
	CResource								m_evStop;

    /// Signals when the thread has initialized
	CResource								m_evInit;

};

