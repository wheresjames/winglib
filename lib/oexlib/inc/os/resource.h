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
	CResource()
	{	m_hHandle = cInvalid();
		m_eType = eRtInvalid;
	}

	/// Destructor
	virtual ~CResource()
	{
		Destroy();
	}

	/// Copy constructor
	CResource( oexCONST CResource &x_r )
	{	if ( 1 < x_r.AddRef() )
		{	m_hHandle = x_r.m_hHandle;
			m_eType = x_r.m_eType;
		} // end if
		else
		{	m_hHandle = cInvalid();
			m_eType = eRtInvalid;
		} // end else
	}


	/// Assignment operator
	CResource& operator = ( oexCONST CResource &x_r )
	{	Destroy();
		if ( 1 < x_r.AddRef() )
		{	m_hHandle = x_r.m_hHandle;
			m_eType = x_r.m_eType;
		} // end if
		else
		{	m_hHandle = cInvalid();
			m_eType = eRtInvalid;
		} // end else
		return *this;
	}

	/// Adds a reference count to the objcet
	oexINT AddRef() const;

/*
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
*/

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
	virtual oexRESULT Destroy( oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT, oexBOOL x_bForce = oexTRUE );

	/// Detaches from the resource without freeing it
	void Detach()
	{	m_hHandle = cInvalid();
		m_eType = eRtInvalid;
	}

	/// Creates a mutex object
	/**
		\param [in] x_sName 		-	Name for the mutex
		\param [in] x_bInitialOwner	-	Non-zero if the calling thread
										should take ownership before
										the function returns.
	*/
	oexRESULT NewMutex( oexCSTR x_sName = oexNULL, oexBOOL x_bInitialOwner = oexFALSE );

	/// Creates an event object
	/**
		\param [in] x_sName			-	Event name, NULL for unamed event.
		\param [in] x_bManualReset	-	TRUE if the event must be manually
										reset
		\param [in] x_bInitialState	-	The initial state of the event,
										non-zero for signaled.
	*/
	oexRESULT NewEvent( oexCSTR x_sName = oexNULL, oexBOOL x_bManualReset = oexTRUE, oexBOOL x_bInitialState = oexFALSE );

	/// Creates a thread
	/**
		\param [in] x_fnCallback	-	Thread callback function
		\param [in] x_pData			-	User data passed to thread
										callback function
	*/
	oexRESULT NewThread( PFN_ThreadProc x_fnCallback, oexPVOID x_pData );

	/// Creates a thread lock
	/**
		\param [in] x_sName			-	Event name, NULL for unamed event.
		\param [in] x_bInitialOwner	-	Non-zero if the calling thread
										should take ownership before
										the function returns.
	*/
	oexRESULT NewLock( oexCSTR x_sName = oexNULL, oexBOOL x_bInitialOwner = oexFALSE );


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
	virtual oexRESULT Wait( oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT );

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
	virtual oexRESULT Signal( oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT );

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
	virtual oexRESULT Reset( oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT );

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

public:

	//==============================================================
	// WaitMultiple()
	//==============================================================
	/// Waits for multiple resources to become signaled
	/**
		\param [in] x_nCount		-	Number of CResource objects in
										x_pResources
		\param [in] x_pResources	-	Array of CResource object pointers
		\param [in] x_uTimeout 		-	Maximum time to wait for signal
		\param [in] x_nMin			-	Minimum number of events to become
										signaled.  This only works on posix
										systems.  On Windows, if x_nMin is
										zero, WaitMultiple() will return when
										ANY of the events become signaled,
										if x_nMin is non-zero, it will only
										return once ALL events become signaled.
										Note: To ensure the same result on all
										platforms, set x_nMin to zero for ANY
										and set to x_nCount for ALL.

		This is a consolidated wait function.  It's exact characteristics
		depend on the type of object being waited on.

		\return Returns negative number if failure, otherwise, returns the index of a
				signaled object.
	*/
	static oexINT WaitMultiple( oexINT x_nCount, CResource **x_pResources,
			   	  			    oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT, oexBOOL x_nMin = oexFALSE );


	/// Waits for the specified events or the stop signal
	oexINT Wait( CResource& rRes1, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{	CResource* p[] = { this, &rRes1 };
		return CResource::WaitMultiple( 2, p, x_uTimeout );
	}

	/// Waits for the specified events or the stop signal
	oexINT Wait( CResource& rRes1, CResource& rRes2, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{	CResource* p[] = { this, &rRes1, &rRes2 };
		return CResource::WaitMultiple( 3, p, x_uTimeout );
	}

	/// Waits for the specified events or the stop signal
	oexINT Wait( CResource& rRes1, CResource& rRes2, CResource& rRes3, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{	CResource* p[] = { this, &rRes1, &rRes2, &rRes3 };
		return CResource::WaitMultiple( 4, p, x_uTimeout );
	}

	/// Waits for the specified events or the stop signal
	oexINT Wait( CResource& rRes1, CResource& rRes2, CResource& rRes3, CResource& rRes4, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{	CResource* p[] = { this, &rRes1, &rRes2, &rRes3, &rRes4 };
		return CResource::WaitMultiple( 5, p, x_uTimeout );
	}

	/// Waits for the specified events or the stop signal
	oexINT Wait( CResource& rRes1, CResource& rRes2, CResource& rRes3, CResource& rRes4,
				 CResource& rRes5,
					oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{	CResource* p[] = { this, &rRes1, &rRes2, &rRes3, &rRes4, &rRes5 };
		return CResource::WaitMultiple( 6, p, x_uTimeout );
	}

	/// Waits for the specified events or the stop signal
	oexINT Wait( CResource& rRes1, CResource& rRes2, CResource& rRes3, CResource& rRes4,
				 CResource& rRes5, CResource &rRes6,
					oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{	CResource* p[] = { this, &rRes1, &rRes2, &rRes3, &rRes4, &rRes5, &rRes6 };
		return CResource::WaitMultiple( 7, p, x_uTimeout );
	}

	/// Waits for the specified events or the stop signal
	oexINT Wait( CResource& rRes1, CResource& rRes2, CResource& rRes3, CResource& rRes4,
				 CResource& rRes5, CResource &rRes6, CResource& rRes7,
					oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{	CResource* p[] = { this, &rRes1, &rRes2, &rRes3, &rRes4, &rRes5, &rRes6, &rRes7 };
		return CResource::WaitMultiple( 8, p, x_uTimeout );
	}

	/// Waits for the specified events or the stop signal
	oexINT Wait( CResource& rRes1, CResource& rRes2, CResource& rRes3, CResource& rRes4,
				 CResource& rRes5, CResource &rRes6, CResource& rRes7, CResource& rRes8,
					oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{	CResource* p[] = { this, &rRes1, &rRes2, &rRes3, &rRes4, &rRes5, &rRes6, &rRes7, &rRes8 };
		return CResource::WaitMultiple( 9, p, x_uTimeout );
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
};


