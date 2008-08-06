/*------------------------------------------------------------------
// thread_safe.h
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
	CTlLock()
    {
        m_bLocked = oexFALSE;
        m_uRef = 0;
        m_hMutex = os::CMutex::osCreateMutex( oexNULL );
    }

	//==============================================================
	// CTlLock()
	//==============================================================
	/// Creates named lock object
	/**
		\param [in] x_pName	-	Name for mutex
	*/
	CTlLock( oexCSTR x_pName )
    {
        m_bLocked = oexFALSE;
        m_uRef = 0;
        m_hMutex = os::CMutex::osCreateMutex( x_pName );
    }

	/// Destructor
	virtual ~CTlLock()
    {
        Destroy();
    }

    void Destroy()
    {
        if ( m_hMutex )
        {   os::CMutex::osDestroyMutex( m_hMutex );
            m_hMutex = oexNULL;
        } // end if
    }

	//==============================================================
	// Create()
	//==============================================================
	/// Creates named lock object
	/**
		\param [in] x_pName	-	Name for mutex
	*/
	oexBOOL Create( oexCSTR x_pName = oexNULL )
    {
        // Lose the old lock
        Destroy();

        m_bLocked = oexFALSE;
        m_uRef = 0;
        m_hMutex = os::CMutex::osCreateMutex( x_pName );
        return os::CMutex::vInvalid() != m_hMutex;
    }

	//==============================================================
	// Lock()
	//==============================================================
	/// Locks the object
	/**
		\param [in] x_uTimeout	-	Maximum time in milli-seconds to
									wait for lock

		\return Non-zero if success
	*/
	oexBOOL Lock( oexULONG x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{
		oexTRACE( oexT( "Lock not implemented!\n" ) );
		return oexTRUE;

        m_bLocked = ( os::CSys::waitSuccess == os::CSys::WaitForSingleObject( m_hMutex, x_uTimeout ) );

		if ( m_bLocked )
			m_uRef++;
		else
			oexTRACE( oexT( "Lock Failed!\n" ) );

		return m_bLocked;
	}

	//==============================================================
	// Unlock()
	//==============================================================
	/// Unlocks the object
	oexBOOL Unlock()
	{
        os::CMutex::osReleaseMutex( m_hMutex );

		if ( m_uRef )
		{
            if ( !( --m_uRef ) )
                m_bLocked = oexFALSE;

        } // end if

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
	oexBOOL IsLocked() { return m_bLocked; }

private:

	/// Number of times locked
	oexULONG					m_uRef;

	/// Handle to mutex object
	os::CMutex::t_MUTEX			m_hMutex;

	/// Non-zero if locked
	oexBOOL					m_bLocked;
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
	CTlLocalLock()
    {
        m_ptr = oexNULL;
    }

	//==============================================================
	// CTlLocalLock()
	//==============================================================
	/// Constructor - Takes a CTlLock pointer
	/**
		\param [in] x_ptr		-	Pointer to CTlLock object
		\param [in] x_uTimeout	-	Maximum time in milli-seconds to
									wait for lock.
	*/
	CTlLocalLock( CTlLock *x_ptr, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{
        m_ptr = oexNULL;
        if ( x_ptr != oexNULL )
            if ( x_ptr->Lock( x_uTimeout ) )
                m_ptr = x_ptr;
    }

	//==============================================================
	// CTlLocalLock()
	//==============================================================
	/// Constructor - Takes a CTlLock reference
	/**
		\param [in] x_lock		-	Reference to CTlLock object
		\param [in] x_uTimeout	-	Maximum time in milli-seconds to
									wait for lock.
	*/
	CTlLocalLock( CTlLock &x_lock, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{
        m_ptr = oexNULL;
        if ( x_lock.Lock( x_uTimeout ) )
            m_ptr = &x_lock;
    }

	/// Destructor - Unlocks the underlying lock
	virtual ~CTlLocalLock()
    {
        Unlock();
    }

	//==============================================================
	// IsLocked()
	//==============================================================
	/// Returns true if the local object is locked
	oexBOOL IsLocked()
    {
        return ( m_ptr != oexNULL );
    }

	//==============================================================
	// Attach()
	//==============================================================
	/// Attaches to an existing CTlLock without locking
	void Attach( CTlLock *x_ptr )
    {
        Unlock();
        m_ptr = x_ptr;
    }

	//==============================================================
	// Detach()
	//==============================================================
	/// Detaches from CTlLock without unlocking
	void Detach()
    {
        m_ptr = oexNULL;
    }

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
	oexBOOL Lock( CTlLock *x_ptr, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{
        if ( x_ptr == m_ptr )
            return oexTRUE;

		if ( x_ptr )
        {
            Unlock();
            m_ptr = oexNULL;
        } // end if

		if ( !x_ptr )
            return oexFALSE;

		if ( x_ptr->Lock( x_uTimeout ) )
            m_ptr = x_ptr;

		return IsLocked();
	}

	//==============================================================
	// Unlock()
	//==============================================================
	/// Unlocks attached CTlLock object
	/**
		\return Always returns non-zero
	*/
	oexBOOL Unlock()
	{
        if ( m_ptr == oexNULL )
            return oexTRUE;

		m_ptr->Unlock();
        m_ptr = oexNULL;

		return oexTRUE;
	}

private:

	/// Pointer to CTlLock object
	CTlLock		*m_ptr;

};

class CTlEvent
{
public:

	/// Constructor
	CTlEvent( oexBOOL x_bNoCreate )
	{
        m_hEvent = oexNULL;
	}

	/// Constructor
	CTlEvent( oexCSTR x_pName = oexNULL, oexBOOL x_bManualReset = oexTRUE )
	{
        m_hEvent = os::CEvent::osCreateEvent( x_pName, x_bManualReset );
	}

	/// Destructor
	virtual ~CTlEvent()
	{
        Destroy();
    }

    void Destroy()
    {
		// Close event handle
		void* hTemp = m_hEvent;
        m_hEvent = os::CResource::cInvalid();
		if ( hTemp )
            os::CEvent::osDestroyEvent( hTemp );
    }

    /// Creates an event
    os::CEvent::t_EVENT Create( oexCSTR x_pName = oexNULL, oexBOOL x_bManualReset = oexTRUE )
    {	return ( m_hEvent = os::CEvent::osCreateEvent( x_pName, x_bManualReset ) ); }

	//==============================================================
	// Set()
	//==============================================================
	/// Sets the event
	oexBOOL Set()
	{   return os::CEvent::osSetEvent( m_hEvent ); }

	//==============================================================
	// Reset()
	//==============================================================
	/// Resets the event
	oexBOOL Reset()
    { 	return os::CEvent::osResetEvent( m_hEvent ); }

	//==============================================================
	// Wait()
	//==============================================================
	/// Waits for the event
	/**
		\param [in] x_uTimeout	-	Maximum amount of time to wait

		\return Non-zero if start event was set

		\see
	*/
	oexBOOL Wait( oexUINT x_uTimeout )
	{	return os::CEvent::waitSuccess == os::CEvent::osWaitForSingleObject( m_hEvent, x_uTimeout ); }

	//==============================================================
	// Wait()
	//==============================================================
	/// Waits for the event
	/**
		\param [in] x_uTimeout		-	Maximum amount of time to wait
		\param [in] x_hOtherEvent	-	Handle to another event to wait for

		\return Index of the event that was set, less than zero if error.

		\see
	*/
	oexUINT Wait( oexUINT x_uTimeout, CTlEvent &x_rOtherEvent )
    {	os::CEvent::t_EVENT hEvents[ 2 ] = { m_hEvent, x_rOtherEvent.m_hEvent };
		return os::CEvent::waitSuccess == os::CEvent::osWaitForMultipleObjects( 2, hEvents, oexFALSE, x_uTimeout );
	}

    /// Returns a handle to the event
    os::CSys::t_WAITABLE GetHandle()
    {   return m_hEvent; }

    /// Cast operator
    operator os::CSys::t_WAITABLE ()
    {   return m_hEvent; }

    /// Attach to event
    os::CEvent::t_EVENT Attach( os::CEvent::t_EVENT hEvent )
    {
        Destroy();
        m_hEvent = hEvent;
        return m_hEvent;
    }

    /// Detaches from event handle
    os::CEvent::t_EVENT Detach()
    {   os::CEvent::t_EVENT hEvent = m_hEvent;
        m_hEvent = os::CResource::cInvalid();
        return hEvent;
    }

private:

	/// Event handle
    os::CEvent::t_EVENT				m_hEvent;

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
	{	m_hSignal = os::CEvent::osCreateEvent( oexNULL, oexTRUE );
		m_hUnsignal = os::CEvent::osCreateEvent( oexNULL, oexTRUE );
	}

	/// Destructor
	virtual ~CTlSignal()
	{
		// Close start handle
		void* hTemp = m_hSignal;
		m_hSignal = oexNULL;
		if ( hTemp )
            os::CEvent::osDestroyEvent( hTemp );

		// Close done handle
		hTemp = m_hUnsignal;
		m_hUnsignal = oexNULL;
		if ( hTemp )
            os::CEvent::osDestroyEvent( hTemp );
	}

	//==============================================================
	// GetSignalEvent()
	//==============================================================
	/// Returns the handle to the signal event
	os::CEvent::t_EVENT GetSignalEvent()
    {   return m_hSignal; }

	//==============================================================
	// GetUnsignalEvent()
	//==============================================================
	/// Returns the handle to the unsignal event
	os::CEvent::t_EVENT GetUnsignalEvent()
    {   return m_hUnsignal; }

	//==============================================================
	// Signal()
	//==============================================================
	/// Sets the signal
	oexBOOL Signal()
	{ 	os::CEvent::osResetEvent( m_hUnsignal );
		return os::CEvent::osSetEvent( m_hSignal );
	}

	//==============================================================
	// Unsignal()
	//==============================================================
	/// Sets the done event signal
	oexBOOL Unsignal()
	{ 	os::CEvent::osResetEvent( m_hSignal );
		return os::CEvent::osSetEvent( m_hUnsignal );
	}

	//==============================================================
	// ResetSignal()
	//==============================================================
	/// Resets the signal event signal
	oexBOOL ResetSignal()
    {   return os::CEvent::osResetEvent( m_hSignal ); }

	//==============================================================
	// ResetUnsignal()
	//==============================================================
	/// Resets the unsignal event signal
	oexBOOL ResetUnsignal()
    {   return os::CEvent::osResetEvent( m_hUnsignal ); }

	//==============================================================
	// WaitSignal()
	//==============================================================
	/// Waits for the signal event
	/**
		\param [in] x_uTimeout	-	Maximum amount of time to wait

		\return Non-zero if start event was set

		\see
	*/
	oexBOOL WaitSignal( oexUINT x_uTimeout )
	{	return os::CEvent::waitSuccess == os::CEvent::osWaitForSingleObject( m_hSignal, x_uTimeout ); }

	//==============================================================
	// WaitUnsignal()
	//==============================================================
	/// Waits for the unsignal event
	/**
		\param [in] x_uTimeout	-	Maximum amount of time to wait

		\return Non-zero if done event was set

		\see
	*/
	oexBOOL WaitUnsignal( oexUINT x_uTimeout )
	{	return os::CEvent::waitSuccess == os::CEvent::osWaitForSingleObject( m_hUnsignal, x_uTimeout ); }

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
	oexUINT WaitSignal( oexUINT x_uTimeout, os::CEvent::t_EVENT x_hOtherEvent )
	{	os::CEvent::t_EVENT hEvents[ 2 ] = { m_hSignal, x_hOtherEvent };
		return os::CEvent::waitSuccess == os::CEvent::osWaitForMultipleObjects( 2, hEvents, oexFALSE, x_uTimeout );
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
    oexUINT WaitUnsignal( oexUINT x_uTimeout, os::CEvent::t_EVENT x_hOtherEvent )
	{	os::CEvent::t_EVENT hEvents[ 2 ] = { m_hUnsignal, x_hOtherEvent };
        return os::CEvent::waitSuccess == os::CEvent::osWaitForMultipleObjects( 2, hEvents, oexFALSE, x_uTimeout );
	}

private:

	/// Signal event
    os::CEvent::t_EVENT				m_hSignal;

	/// Unsignal event
	os::CEvent::t_EVENT				m_hUnsignal;
};

