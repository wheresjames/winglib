/*------------------------------------------------------------------
// thread_safe.h
// Copyright (c) 2006 
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
// CTlLock
//
/// Thread locking mechanism
/**
	Encapsulates a Windows Mutex object to provide thread locking.	
*/
//==================================================================
class CTlLock
{
public:


	//==============================================================
	// CTlLock()
	//==============================================================
	/// Creates default lock object
	CTlLock() { m_bLocked = stFALSE; m_uRef = 0; m_hMutex = CMutex::osCreateMutex( stNULL ); }

	//==============================================================
	// CTlLock()
	//==============================================================
	/// Creates named lock object
	/**
		\param [in] x_pName	-	Name for mutex
	*/
	CTlLock( stCSTR x_pName ) { m_bLocked = stFALSE; m_uRef = 0; m_hMutex = CMutex::osCreateMutex( x_pName ); }

	/// Destructor
	~CTlLock() { if ( m_hMutex ) { CMutex::osDestroyMutex( m_hMutex ); m_hMutex = stNULL; } }

	//==============================================================
	// Lock()
	//==============================================================
	/// Locks the object
	/**
		\param [in] x_uTimeout	-	Maximum time in milli-seconds to
									wait for lock

		\return Non-zero if success
	*/
	stBOOL Lock( stULONG x_uTimeout = 3000 ) 
	{	m_bLocked = ( CMutex::waitSuccess == CMutex::osWaitForSingleObject( m_hMutex, x_uTimeout ) );
		if ( m_bLocked ) 
			m_uRef++; 
		else 
			stTRACE( stT( "Lock Failed!\n" ) );
		return m_bLocked; 
	}

	//==============================================================
	// Unlock()
	//==============================================================
	/// Unlocks the object
	stBOOL Unlock() 
	{	CMutex::osReleaseMutex( m_hMutex );
		if ( m_uRef ) 
		{	if ( !( --m_uRef ) ) m_bLocked = stFALSE; }
		return !m_bLocked;
	}

	//==============================================================
	// IsLocked()
	//==============================================================
	/// Returns non-zero if locked
	/**
		Checks to see if the object is locked.  Does not mean the 
		caller owns the lock
	*/
	stBOOL IsLocked() { return m_bLocked; }
	
private:

	/// Number of times locked
	stULONG					m_uRef;

	/// Handle to mutex object
	CMutex::t_MUTEX			m_hMutex;

	/// Non-zero if locked
	stBOOL					m_bLocked;
};

//==================================================================
// CTlLocalLock
//
/// Use this to lock and automatically unlock CTlLock objects
/**
	Use this to lock and automatically unlock CTlLock objects
*/
//==================================================================
class CTlLocalLock
{

public:

	/// Default constructor
	CTlLocalLock() { m_ptr = stNULL; }

	//==============================================================
	// CTlLocalLock()
	//==============================================================
	/// Constructor - Takes a CTlLock pointer
	/**
		\param [in] x_ptr		-	Pointer to CTlLock object
		\param [in] x_uTimeout	-	Maximum time in milli-seconds to
									wait for lock.
	*/
	CTlLocalLock( CTlLock *x_ptr, stUINT x_uTimeout = 3000 ) 
	{	m_ptr = stNULL; if ( x_ptr != stNULL ) if ( x_ptr->Lock( x_uTimeout ) ) m_ptr = x_ptr; }

	//==============================================================
	// CTlLocalLock()
	//==============================================================
	/// Constructor - Takes a CTlLock reference
	/**
		\param [in] x_lock		-	Reference to CTlLock object
		\param [in] x_uTimeout	-	Maximum time in milli-seconds to
									wait for lock.
	*/
	CTlLocalLock( CTlLock &x_lock, stUINT x_uTimeout = 3000 ) 
	{	m_ptr = stNULL; if ( x_lock.Lock( x_uTimeout ) ) m_ptr = &x_lock; }

	/// Destructor - Unlocks the underlying lock
	~CTlLocalLock() { Unlock(); }

	//==============================================================
	// IsLocked()
	//==============================================================
	/// Returns true if the local object is locked
	stBOOL IsLocked() { return ( m_ptr != stNULL ); }

	//==============================================================
	// Attach()
	//==============================================================
	/// Attaches to an existing CTlLock without locking
	void Attach( CTlLock *x_ptr ) { Unlock(); m_ptr = x_ptr; }

	//==============================================================
	// Detach()
	//==============================================================
	/// Detaches from CTlLock without unlocking
	void Detach() { m_ptr = stNULL; }

	//==============================================================
	// Lock()
	//==============================================================
	/// Locks a CTlLock object.  Returns true only if lock was achieved
	/**
		\param [in] x_ptr			-	Pointer to CTlLock object
		\param [in] x_uTimeout	-	Maximum time in milli-seconds to
									wait for lock.
		
		\return Non-zero if lock was acquired.
	
		\see 
	*/
	stBOOL Lock( CTlLock *x_ptr, stUINT x_uTimeout = 3000 )
	{	if ( x_ptr == m_ptr ) return stTRUE;
		if ( x_ptr ) { Unlock(); m_ptr = stNULL; }
		if ( !x_ptr ) return stFALSE;
		if ( x_ptr->Lock( x_uTimeout ) ) m_ptr = x_ptr;
		return IsLocked();
	}

	//==============================================================
	// Unlock()
	//==============================================================
	/// Unlocks attached CTlLock object
	/**
		\return Always returns non-zero
	*/
	stBOOL Unlock()
	{	if ( m_ptr == stNULL ) return stTRUE;
		m_ptr->Unlock(); m_ptr = stNULL;
		return stTRUE;
	}

private:

	/// Pointer to CTlLock object
	CTlLock		*m_ptr;

};


//==================================================================
// CTlSignal
//
/// Encapsulates two events for syncronizing threads
/**

  Typical use

  Worker thread...
  \code

	// Ready for work
	g_cSignal.SignalDone();

	// Worker thread loop
	while ( g_cSignal.WaitStart( INFINITE ) )
	{
		// Acknowledge start
		g_cSignal.ResetStart();

		// Want exit?
		if ( g_bExit ) break;

		// Do the work
		DoWork();

		// Signal that we're done
		g_cSignal.SignalDone();

	} // end while

  \endcode

  User Thread...
  \code

	// Is thread done with the last task?
	if ( g_cSignal.WaitDone( 0 ) )
	{
		// Acknowledge done
		g_cSignal.ResetDone();

		/// Build new task for thread
		CreateWork();

		// Start the worker thread
		g_cSignal.SignalStart();

	} // end if

	// Go do something else

  \endcode

	
*/
//==================================================================
class CTlSignal  
{
public:

	/// Default constructor
	CTlSignal()
	{	m_hSignal = CMutex::osCreateMutex( stNULL );
		m_hUnsignal = CMutex::osCreateMutex( stNULL );
	}

	/// Destructor
	virtual ~CTlSignal()
	{
		// Close start handle
		void* hTemp = m_hSignal;
		m_hSignal = stNULL;
		if ( hTemp ) CMutex::osDestroyMutex( hTemp );

		// Close done handle
		hTemp = m_hUnsignal;
		m_hUnsignal = stNULL;
		if ( hTemp ) CMutex::osDestroyMutex( hTemp );
	}

	//==============================================================
	// GetSignalEvent()
	//==============================================================
	/// Returns the handle to the signal event
	CMutex::t_MUTEX GetSignalEvent() { return m_hSignal; }

	//==============================================================
	// GetUnsignalEvent()
	//==============================================================
	/// Returns the handle to the unsignal event
	CMutex::t_MUTEX GetUnsignalEvent() { return m_hUnsignal; }

	//==============================================================
	// Signal()
	//==============================================================
	/// Sets the signal
	stBOOL Signal() 
	{ 	CMutex::osResetEvent( m_hUnsignal ); 
		return CMutex::osSetEvent( m_hSignal ); 
	}

	//==============================================================
	// Unsignal()
	//==============================================================
	/// Sets the done event signal
	stBOOL Unsignal()
	{ 	CMutex::osResetEvent( m_hSignal ); 
		return CMutex::osSetEvent( m_hUnsignal ); 
	}

	//==============================================================
	// ResetSignal()
	//==============================================================
	/// Resets the signal event signal
	stBOOL ResetSignal() { return CMutex::osResetEvent( m_hSignal ); }

	//==============================================================
	// ResetUnsignal()
	//==============================================================
	/// Resets the unsignal event signal
	stBOOL ResetUnsignal() { return CMutex::osResetEvent( m_hUnsignal ); }

	//==============================================================
	// WaitSignal()
	//==============================================================
	/// Waits for the signal event
	/**
		\param [in] x_uTimeout	-	Maximum amount of time to wait
		
		\return Non-zero if start event was set
	
		\see 
	*/
	stBOOL WaitSignal( stUINT x_uTimeout ) 
	{	return CMutex::waitSuccess == CMutex::osWaitForSingleObject( m_hSignal, x_uTimeout ); }

	//==============================================================
	// WaitUnsignal()
	//==============================================================
	/// Waits for the unsignal event
	/**
		\param [in] x_uTimeout	-	Maximum amount of time to wait
		
		\return Non-zero if done event was set
	
		\see 
	*/
	stBOOL WaitUnsignal( stUINT x_uTimeout ) 
	{	return CMutex::waitSuccess == CMutex::osWaitForSingleObject( m_hUnsignal, x_uTimeout ); }

	//==============================================================
	// WaitSignal()
	//==============================================================
	/// Waits for the signal event
	/**
		\param [in] x_uTimeout		-	Maximum amount of time to wait
		\param [in] x_hOtherEvent	-	Handle to another event to wait for
		
		\return Index of the event that was set, less than zero if error.
	
		\see 
	*/
	stUINT WaitSignal( stUINT x_uTimeout, CMutex::t_MUTEX x_hOtherEvent )
	{	CMutex::t_MUTEX hEvents[ 2 ] = { m_hSignal, x_hOtherEvent };
		return CMutex::osWaitForMultipleObjects( 2, hEvents, stFALSE, x_uTimeout );
	}

	//==============================================================
	// WaitUnsignal()
	//==============================================================
	/// Waits for the unsignal event
	/**
		\param [in] x_uTimeout		-	Maximum amount of time to wait
		\param [in] x_hOtherEvent	-	Handle to another event to wait for
		
		\return Index of the event that was set, less than zero if error.
	
		\see 
	*/
	stUINT WaitUnsignal( stUINT x_uTimeout, CMutex::t_MUTEX x_hOtherEvent )
	{	CMutex::t_MUTEX hEvents[ 2 ] = { m_hUnsignal, x_hOtherEvent };
		return CMutex::osWaitForMultipleObjects( 2, hEvents, stFALSE, x_uTimeout );
	}

private:

	/// Signal event
	CMutex::t_MUTEX				m_hSignal;

	/// Unsignal event
	CMutex::t_MUTEX				m_hUnsignal;
};

