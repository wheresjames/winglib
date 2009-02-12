/*------------------------------------------------------------------
// resource.h
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
// CResource
//
/// Encapsulates a system resource handle
/**
	Ok, the idea here, right or wrong, is to encapsulate all the
	different types of handles like files, threads, mutexes, etc
	into a single structure so that they can be released, and most
	importantly to me at the moment, 'waited on' in a unified
	manner.  Windows already does this to a large extent,
	unfortunately, Posix does not.

	This class acts as a true handle, there is no memory management.
	You must call Destroy() manually when your done with this resource.

	This may be better implemented if CResource were a super class
	and subing out the specifics.  Not sure, I have my reasons for
	doing it this way atm. ;)

*/
//==================================================================
struct SResourceInfo;
class CResource
{
public:

	enum E_RESOURCE_TYPE
	{
		/// Invalid resource type
		eRtInvalid 			= 0,

		/// System file
		eRtFile				= 1,

		/// Network socket
		eRtSocket			= 2,

		/// Process thread
		eRtThread			= 3,

		/// Thread mutex
		eRtMutex			= 4,

		/// Event
		eRtEvent			= 5,

		/// Lock
		eRtLock				= 6
	};

	enum etWait
	{
		/// Value indicating wait succeeded
		waitSuccess 		= 0,

		/// Value indicating wait timed out
		waitTimeout 		= -1,

		/// Value indicating wait expired
		waitFailed 			= -2
	};

	/// The wait resolution in micro-seconds
	enum { eWaitResolution = oexWAIT_RESOLUTION };

	/// Thread callback function
	typedef oexPVOID (*PFN_ThreadProc)( oexPVOID x_pData );

public:

	/// Handle type
	typedef oexPVOID		t_HANDLE;

public:

	/// Constructor
	CResource( oexBOOL x_bRelease = oexFALSE )
	{	m_bRelease = x_bRelease;
		m_hHandle = cInvalid();
		m_eType = eRtInvalid;
	}

	/// Destructor
	~CResource()
	{
		if ( m_bRelease )
			Destroy();
	}

	/// Copy constructor
	CResource( oexCONST CResource &x_r )
	{	m_bRelease = oexFALSE;
		m_hHandle = x_r.m_hHandle;
		m_eType = x_r.m_eType;
	}

	// Assignment operator
	CResource& operator =( oexCONST CResource &x_r )
	{	m_bRelease = oexFALSE;
		m_hHandle = x_r.m_hHandle;
		m_eType = x_r.m_eType;
		return *this;
	}

	/// Enables auto release
	CResource& EnableAutoRelease()
	{	m_bRelease = oexTRUE;
		return *this;
	}

	// Disables auto release
	CResource* Detach( CResource *pRes = oexNULL )
	{
		// Pass on traits if needed
		if ( oexCHECK_PTR( pRes ) )
		{	pRes->m_bRelease = m_bRelease;
			pRes->m_hHandle = m_hHandle;
			pRes->m_eType = m_eType;
		} // end if

		// Detach from resource
		m_bRelease = oexFALSE;
		m_hHandle = oexNULL;
		m_eType = eRtInvalid;

		return pRes;
	}

	//==============================================================
	// Destroy()
	//==============================================================
	/// Releases the handle
	/**
		\param [in] x_uTimeout	-	Maximum amount of time to wait for
									the resource to be released
									gracefully.
		\param [in] x_bForce	-	If x_uTimeout expires, and x_bForce
									is non-zero, the resource will be
									forcefully removed.  If x_uTimeout
									expires and x_bForce is zero, the
									resource will be abandoned.

		return	error code for the release operation.   Zero if
				success, -1 if the operation could not be performed.
	*/
	oexRESULT Destroy( oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT, oexBOOL x_bForce = oexTRUE );

	/// Creates a mutex object
	/**
		\param [in] x_sName 		-	Name for the mutex
		\param [in] x_bInitialOwner	-	Non-zero if the calling thread
										should take ownership before
										the function returns.
	*/
	oexRESULT CreateMutex( oexCSTR x_sName = oexNULL, oexBOOL x_bInitialOwner = oexFALSE );

	/// Creates an event object
	/**
		\param [in] x_sName			-	Event name, NULL for unamed event.
		\param [in] x_bManualReset	-	TRUE if the event must be manually
										reset
		\param [in] x_bInitialState	-	The initial state of the event,
										non-zero for signaled.
	*/
	oexRESULT CreateEvent( oexCSTR x_sName = oexNULL, oexBOOL x_bManualReset = oexTRUE, oexBOOL x_bInitialState = oexFALSE );

	/// Creates a thread
	/**
		\param [in] x_fnCallback	-	Thread callback function
		\param [in] x_pData			-	User data passed to thread
										callback function
	*/
	oexRESULT CreateThread( PFN_ThreadProc x_fnCallback, oexPVOID x_pData );

	/// Creates a thread lock
	/**
		\param [in] x_sName			-	Event name, NULL for unamed event.
		\param [in] x_bInitialOwner	-	Non-zero if the calling thread
										should take ownership before
										the function returns.
	*/
	oexRESULT CreateLock( oexCSTR x_sName = oexNULL, oexBOOL x_bInitialOwner = oexFALSE );


private:

    /// The thread procedure
    static oexPVOID ThreadProc( oexPVOID x_pData );

    // Protect access to ThreadProc()
    class CThreadProcImpl;
    friend class CThreadProcImpl;

public:

	//==============================================================
	// Wait()
	//==============================================================
	/// Waits for the handle to signal
	/**
		\param [in] x_uTimeout 		-	Maximum time to wait for signal

		This is a consolidated wait function.  It's exact characteristics
		depend on the type of object being waited on.

		\return Returns zero if success, otherwise an error code is
				returned.
	*/
	oexRESULT Wait( oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT );

	//==============================================================
	// Signal()
	//==============================================================
	/// Signals the handle if the resource type supports it
	/**
		\param [in] x_uTimeout	- Maximum amount of time to wait for
								  the signal to become available for
								  signaling.

		\return Returns zero if success, otherwise an error code is
				returned.
	*/
	oexRESULT Signal( oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT );

	//==============================================================
	// Reset()
	//==============================================================
	/// Resets the signaled state if the resource type supports it
	/**
		\param [in] x_uTimeout	- Maximum amount of time to wait for
								  the signal to become available for
								  resetting.

		\return Returns zero if success, otherwise an error code is
				returned.
	*/
	oexRESULT Reset( oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT );

	//==============================================================
	// vInfinite()
	//==============================================================
	/// Value representing infinite timeout.
	/**
		Use with wait functions
	*/
	static oexCONST oexUINT cInfinite()
    {
		return c_Infinite;
    }

	//==============================================================
	// vInvalid()
	//==============================================================
	/// Value corresponding to an invalid handle value
	static oexCONST t_HANDLE cInvalid()
    {
    	return c_Invalid;
    }

	//==============================================================
	// IsValid()
	//==============================================================
	/// Returns the os dependent handle for the resource
    oexBOOL IsValid()
    {
    	return ( cInvalid() != m_hHandle );
	}

	//==============================================================
	// GetHandle()
	//==============================================================
	/// Returns the os dependent handle for the resource
    t_HANDLE GetHandle()
    {	return m_hHandle;
	}

	//==============================================================
	// GetOwner()
	//==============================================================
	/// Returns the thread id of the resource owner
	/**
		For threads this is the creator.

		For Mutexes and Locks, it is the thread that has it locked.

		For Events, it is the thread that last signaled it

	*/
    oexUINT GetOwner();

	//==============================================================
	// GetRi()
	//==============================================================
	/// Returns m_hHandle as SResourceInfo pointer
	SResourceInfo* GetRi()
	{	return (SResourceInfo*)m_hHandle; }

	//==============================================================
	// SetHandle()
	//==============================================================
	/// Sets the os dependent handle for the resource
    void SetHandle( t_HANDLE x_hHandle, E_RESOURCE_TYPE x_eType )
    {
    	m_hHandle = x_hHandle;
		m_eType = x_eType;
	}

	//==============================================================
	// GetType()
	//==============================================================
	/// Returns the type of the os dependent resource
    E_RESOURCE_TYPE GetType()
    {
    	return m_eType;
	}

	//==============================================================
	// SetType()
	//==============================================================
	/// Sets the type of the os dependent resource
    void SetType( E_RESOURCE_TYPE x_eType )
    {
    	m_eType = x_eType;
	}

private:

	/// Invalid event handle value
	static oexCONST t_HANDLE	c_Invalid;

	/// Infinite timeout value
	static oexCONST oexUINT		c_Infinite;

private:

	/// The resource handle
	t_HANDLE					m_hHandle;

	/// Resource type
	E_RESOURCE_TYPE				m_eType;

	/// Auto Release
	oexBOOL						m_bRelease;
};


//==================================================================
// CLock
//
/// Generic thread lock
//==================================================================
class CLock
{
public:

	/// Default constructor
	CLock( oexCSTR x_pName = oexNULL )
	{	m_lock.CreateLock( x_pName ); }

	/// Casts to CResource object
	operator CResource&() { return m_lock; }

	/// Returns a reference to the CResource object
	CResource& Obj() { return m_lock; }

	/// Returns a pointer to the CResource object
	CResource* Ptr() { return &m_lock; }

private:

	/// The actual lock
	CResource	m_lock;
};


//==================================================================
// CAutoLock
//
/// Use this to lock and automatically unlock CResource objects
/**
	Use this to lock and automatically unlock CResource objects
*/
//==================================================================
class CAutoLock
{

public:

	/// Default constructor
	CAutoLock()
    {
        m_ptr = oexNULL;
    }

	/// Destructor - Unlocks the underlying CResource object
	virtual ~CAutoLock()
    {
        Unlock();
    }

	//==============================================================
	// CAutoLock()
	//==============================================================
	/// Constructor - Takes a CResource pointer
	/**
		\param [in] x_ptr		-	Pointer to CResource object
		\param [in] x_uTimeout	-	Maximum time in milli-seconds to
									wait for lock.
	*/
	CAutoLock( CResource *x_ptr, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{   m_ptr = oexNULL;
		if ( oexCHECK_PTR( x_ptr ) )
            if ( 0 == x_ptr->Wait( x_uTimeout ) )
                m_ptr = x_ptr;
    }

	//==============================================================
	// CAutoLock()
	//==============================================================
	/// Constructor - Takes a CResource reference
	/**
		\param [in] x_lock		-	Reference to CResource object
		\param [in] x_uTimeout	-	Maximum time in milli-seconds to
									wait for lock.
	*/
	CAutoLock( CResource &x_lock, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{   m_ptr = oexNULL;
        if ( 0 == x_lock.Wait( x_uTimeout ) )
            m_ptr = &x_lock;
    }

	//==============================================================
	// CAutoLock()
	//==============================================================
	/// Constructor - Takes a CResource reference
	/**
		\param [in] x_ptr		-	Pointer to CResource object
		\param [in] x_uTimeout	-	Maximum time in milli-seconds to
									wait for lock.
	*/
	CAutoLock( CLock *x_ptr, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{   m_ptr = oexNULL;
		if ( oexCHECK_PTR( x_ptr ) )
            if ( 0 == x_ptr->Obj().Wait( x_uTimeout ) )
                m_ptr = x_ptr->Ptr();
    }

	//==============================================================
	// IsLocked()
	//==============================================================
	/// Returns true if the local object is locked
	oexBOOL IsLocked( oexUINT x_uWho = oexGetCurrentThreadId() )
    {
		if ( !oexCHECK_PTR( m_ptr ) )
			return oexFALSE;

		return m_ptr->GetOwner() == x_uWho;
    }

	//==============================================================
	// Attach()
	//==============================================================
	/// Attaches to an existing CResource without locking
	void Attach( CResource *x_ptr )
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
	oexBOOL Lock( CResource *x_ptr, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
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

		oexM();

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
	CResource		*m_ptr;

};

