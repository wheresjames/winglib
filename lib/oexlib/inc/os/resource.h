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
	unfortunately, Linux does not.

*/
//==================================================================
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
		eRtEvent			= 5
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
	enum { eWaitResolution = 15000 };

public:

	/// Handle type
	typedef oexPVOID		t_HANDLE;

public:

	/// Constructor
	CResource()
	{
		m_hHandle = cInvalid();
		m_eType = eRtInvalid;
	}

	/// Constructor
	/*
		\param [in] x_nType		-	The type of resource to create.
		\param [in] x_sName		-	Names the resource if resource

	*/
	CResource( E_RESOURCE_TYPE x_eType, oexCSTR x_sName = oexNULL )
	{
		m_hHandle = c_Invalid;
		m_eType = eRtInvalid;

		// Create specified type
		Create( x_eType, x_sName );
	}

	/// Destructor
	~CResource()
	{
		Destroy();
	}

	//==============================================================
	// Create()
	//==============================================================
	/// Releases the handle
	/*
		\param [in] x_nType		-	The type of resource to create.
		\param [in] x_sName		-	Names the resource if resource
									type supports it.

		\return Zero if success, else error code

	*/
	oexRESULT Create( E_RESOURCE_TYPE x_eType, oexCSTR x_sName = oexNULL );

	//==============================================================
	// Destroy()
	//==============================================================
	/// Releases the handle
	/*
		return	error code for the release operation.   Zero if
				success, -1 if the operation could not be performed.
	*/
	oexRESULT Destroy();

	//==============================================================
	// Wait()
	//==============================================================
	/// Waits for the handle to signal
	/*
		\param [in] x_uTimeout 		-	Maximum time to wait for signal
		\param [in] x_uType			-	Type of signal to wait for

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
	/*
		\return Returns zero if success, otherwise an error code is
				returned.
	*/
	oexRESULT Signal();

	//==============================================================
	// Signal()
	//==============================================================
	/// Resets the signaled state if the resource type supports it
	/*
		\return Returns zero if success, otherwise an error code is
				returned.
	*/
	oexRESULT Reset();

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
    {
		oexIGNORE oexVERIFY( c_Invalid != m_hHandle && eRtInvalid != m_eType );
    	return m_hHandle;
	}

	//==============================================================
	// SetHandle()
	//==============================================================
	/// Sets the os dependent handle for the resource
    void SetHandle( t_HANDLE x_hHandle )
    {
    	m_hHandle = x_hHandle;
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
	t_HANDLE				m_hHandle;

	/// Resource type
	E_RESOURCE_TYPE			m_eType;
};

