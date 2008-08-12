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

	enum
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
		eRtMutex			= 4
	};

	enum etWait
	{
		/// Value indicating wait succeeded
		waitSuccess = 0,

		/// Value indicating wait timed out
		waitTimeout = -1,

		/// Value indicating wait expired
		waitFailed = -2
	};

public:

	/// Handle type
	typedef oexPVOID		t_HANDLE;

public:

	/// Constructor
	CResource()
	{
		m_hHandle = c_Invalid;
		m_nType = eRtInvalid;
	}

	/// Constructor
	CResource( oexINT x_nType )
	{
		m_hHandle = c_Invalid;
		m_nType = eRtInvalid;

		// Create specified type
		Init( x_nType );
	}

	/// Destructor
	~CResource()
	{
		Destroy();
	}

	//==============================================================
	// Init()
	//==============================================================
	/// Releases the handle
	/*
		\param [in] x_nType		-	The type of resource to create.

		\return Zero if success, else error code

	*/
	oexINT Init( oexINT x_nType );

	//==============================================================
	// Destroy()
	//==============================================================
	/// Releases the handle
	/*
		\param [in] x_puErr		-	If not null, receives the error
									code for the release operation.
									Zero if success, -1 if the
									operation could not be performed.
	*/
	void Destroy( oexINT *x_pnErr = oexNULL );

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
	oexINT Wait( oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT, oexINT x_nType = 0 );

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
	// IsValidHandle()
	//==============================================================
	/// Returns the os dependent handle for the resource
    oexBOOL IsValidHandle()
    {
    	return ( c_Invalid != m_hHandle );
	}

	//==============================================================
	// GetHandle()
	//==============================================================
	/// Returns the os dependent handle for the resource
    t_HANDLE GetHandle()
    {
		oexVERIFY( c_Invalid != m_hHandle && eRtInvalid != m_nType );
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
    oexINT GetType()
    {
    	return m_nType;
	}

	//==============================================================
	// SetType()
	//==============================================================
	/// Sets the type of the os dependent resource
    void SetType( oexINT x_nType )
    {
    	m_nType = x_nType;
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
	oexINT					m_nType;
};

