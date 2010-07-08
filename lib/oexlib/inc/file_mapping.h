/*------------------------------------------------------------------
// file_mapping.h
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
// TFileMapping
//
/// Encapsulates a file map
/**
	This class provides the functionality of the file mapping API.

	A real bummer is that it is apparently not straight forward to
	get the size of an existing file mapping.  To counter this,
	the size is saved in the mapping

        {
            // Do we need to destruct objects?
            T* pPtr = m_fm.Ptr();

            // Was the object constructed?
            if ( CAlloc::eF1Constructed & CAlloc::GetFlags( pPtr )
                 && 1 == CAlloc::GetRefCount( pPtr ) )
            {
                t_size uSize = Size();
                for ( t_size i = 0; i < uSize; i++ )
                    pPtr[ i ].~T();

            } // end if

            T* pPtr = m_fm.Ptr();
            if ( pPtr )
            {
                // Add a reference to the memory
                CAlloc::AddRef( pPtr );

                // Save construction status if needed
                if ( x_bConstructed )
                    CAlloc::SetFlags( pPtr, CAlloc::eF1Constructed );

            } // end if

        } // end if

*/
//==================================================================
template < class T > class TFileMapping
{
public:

	typedef oexFILESIZE_T	t_size;

public:

	/// Defualt constructor
	TFileMapping()
	{	m_pPtr = oexNULL;
        m_hFile = os::CFMap::vFailed();
        m_hShareHandle = os::CFMap::vFailed();
        m_pName = oexNULL;
        m_llOpenSize = 0;
        m_bPlain = oexFALSE;
        m_bExisting = oexFALSE;
        m_llOffset = 0;
	}

    /// Copy operator
    TFileMapping& operator = ( TFileMapping &m )
    {   return Assume( m ); }

	/// Destructor
	virtual ~TFileMapping()
    {
        SetName( oexNULL );
        Destroy();
    }

    /// Sets the shared memory name
    /**
        \param [in] x_pName -   Shared memory name
        \param [in] x_uLen  -   Length of the string in x_pName.
                                Can be zero if the string is
                                NULL terminated.

    */
    TFileMapping& SetName( oexCSTR x_pName, t_size x_uLen = 0 )
    {
        // Lose the previous name
        if ( m_pName )
        {   OexAllocDelete( m_pName );
            m_pName = oexNULL;
        } // end if

        // Ensure name
        if ( !x_pName || !*x_pName )
            return *this;

        // Get the length
        if ( !x_uLen )
            x_uLen = zstr::Length( x_pName );

        // Allocate
        m_pName = OexAllocNew< oexTCHAR >( x_uLen + 1 );

        // Copy the shared memory name
        str::Copy( m_pName, x_uLen + 1, x_pName, x_uLen );

        return *this;
    }

    /// Returns the shared name, or NULL if none
    oexCSTR GetName() const
    {   return m_pName; }

	/// Sets an external share handle
	/**
		\param [in] x_hFh		- OS specific file handle used to
								  create the shared memory
	*/
	TFileMapping& SetShareHandle( os::CFMap::t_HFILEMAP x_hFh = os::CFMap::vFailed() )
	{	m_hShareHandle = x_hFh;
		return *this;
	}

	/// Returns the external share handle
	os::CFMap::t_HFILEMAP GetShareHandle() oexCONST
	{	return m_hShareHandle; }

	/// Returns non-zero if a share handle has been specified
	oexBOOL IsShareHandle()
	{	return os::CFMap::vFailed() != m_hShareHandle; }

	void ClearShares()
	{	m_hShareHandle = os::CFMap::vFailed();
        if ( m_pName )
        {   OexAllocDelete( m_pName );
            m_pName = oexNULL;
        } // end if
	}

    //==============================================================
	// Destroy()
	//==============================================================
	/// Unmaps the file and releases all associated resources
	void Destroy()
	{
		if ( m_hFile && m_pPtr )
		{
			if ( !m_bPlain )
			{
				oexBOOL bDestroy = !CAlloc::DecRef( m_pPtr );

				// Was the object constructed?
				if ( bDestroy && CAlloc::eF1Constructed & CAlloc::GetFlags( m_pPtr ) )
				{
					t_size nSize = Size();
					for ( t_size i = 0; i < nSize; i++ )
						m_pPtr[ i ].~T();

				} // end if

				m_pPtr = (T*)CAlloc::VerifyMem( m_pPtr, bDestroy );
				m_pPtr -= sizeof( t_size );

			} // end if

            os::CFMap::Release( m_hFile, m_pPtr, m_llOpenSize );

		} // end if
		m_pPtr = oexNULL;
    	m_hFile = oexNULL;
    	m_llOpenSize = 0;
        m_bExisting = oexFALSE;
	}

	//==============================================================
	// Create()
	//==============================================================
	/// Creates a file mapping
	/**
		\param [in]	 x_bConstructed			-	non-zero if the object
												should be constructed.
		\param [in]  x_pFile				-	file to map, NULL for
												shared memory.  Ignored
												if x_hFile is not NULL.
		\param [in]  x_hFile				-   Handle of file to use.
		\param [in]  x_pName				-	Name of the mapping, can
												be NULL.  Ignored if x_hFile
												is not NULL.
		\param [in]  x_llSize				-	Size of the file mapping.
		\param [in]  x_uAccess				-	Specifes the memory access
												level.

		\return Pointer to the memory or NULL if failure.

		\see
	*/
    T* Create( oexBOOL x_bConstructed, oexCSTR x_pFile, os::CFMap::t_HFILEMAP x_hFile, oexCSTR x_pName, t_size x_llSize, os::CFMap::etAccess x_uAccess = os::CFMap::eAccessAll )
	{
		// Lose old file mapping
		Destroy();

        // Multiply by size of object
        x_llSize *= sizeof( T );

		// Did we get a file handle to use?
        if ( x_hFile )
        	m_hShareHandle = x_hFile;

		t_size llSize = x_llSize;

		if ( !m_bPlain )
		{
			// Calculate the actual size we'll need
			llSize += sizeof( t_size ) + CAlloc::ProtectAreaSize();
			llSize = cmn::NextPower2( llSize );

		} // end if

		// Do we have a shared file handle?
        if ( IsShareHandle() )
			m_hFile = os::CFMap::Create( m_hShareHandle, (oexPVOID*)&m_pPtr, llSize, &llSize, x_uAccess, &m_bExisting, m_llOffset );

		else
		{
			// Save name if any
			if ( x_pName && *x_pName )
				SetName( x_pName );

			// Create file mapping
			m_hFile = os::CFMap::Create( x_pFile, (oexPVOID*)&m_pPtr, llSize, &llSize, m_pName, x_uAccess, &m_bExisting, m_llOffset );

		} // end else

		// Did we get the handle
        if ( os::CFMap::vFailed() == m_hFile || !m_pPtr )
            return oexNULL;

		// Save size used to create the mapping
		m_llOpenSize = llSize;

        // Save block size
        *(t_size*)m_pPtr = (t_size)llSize;

		if ( !m_bPlain )
		{
			m_pPtr += sizeof( t_size );

			// Look for abandoned blocks ( this mostly for Linux )
			if ( m_bExisting && !CAlloc::GetRefCount( CAlloc::RawToPtr( m_pPtr ) ) )
				m_bExisting = oexFALSE;

			// Protect memory area
			m_pPtr = (T*)CAlloc::ProtectMem( m_pPtr, (t_size)x_llSize, !m_bExisting );

			// Verify existing block
			if ( m_bExisting )
				CAlloc::VerifyMem( m_pPtr, oexFALSE );

			// Set constructed flags if needed
			else if ( x_bConstructed )
				CAlloc::SetFlags( m_pPtr, CAlloc::eF1Constructed );

			// Add a reference to the memory
			CAlloc::AddRef( m_pPtr );

		} // end if

        return m_pPtr;
	}

	//==============================================================
	// T*()
	//==============================================================
	/// Returns a pointer to the file mapping memory
	operator T*() { return m_pPtr; }

	//==============================================================
	// Ptr()
	//==============================================================
	/// Returns a pointer to the file mapping memory
	T* Ptr() { return m_pPtr; }

	//==============================================================
	// Ptr()
	//==============================================================
	/// Returns a pointer to the file mapping memory
    /**
        \param [in] x_uOffset   -   Offset into the memory
    */
    T* Ptr( t_size x_uOffset )
    {
        // Out of bounds
        oexASSERT( m_pPtr && Size() > x_uOffset );

        return &m_pPtr[ x_uOffset ];
    }

	//==============================================================
	// c_Ptr()
	//==============================================================
	/// Returns a pointer to the file mapping memory
	oexCONST T* c_Ptr() const
    {   return m_pPtr; }

	//==============================================================
	// c_Ptr()
	//==============================================================
	/// Returns a pointer to the file mapping memory
    /**
        \param [in] x_uOffset   -   Offset into the memory
    */
    oexCONST T* c_Ptr( t_size x_uOffset ) const
    {
        // Out of bounds
        oexASSERT( m_pPtr && Size() > x_uOffset );

        return &m_pPtr[ x_uOffset ];
    }

	//==============================================================
	// GetName()
	//==============================================================
	/// Returns the name of the file mapping
	oexBOOL Existing() { return m_bExisting; }

    /// Returns the size of the file mapping
    t_size Size() const
    {
        if ( !m_pPtr )
            return 0;

		if ( m_bPlain )
			return m_llOpenSize;

        return CAlloc::ArraySize( m_pPtr );
    }

    /// Returns the size of the file mapping
    t_size BlockSize()
    {
        if ( !m_pPtr )
            return 0;

		if ( m_bPlain )
			return m_llOpenSize;

        return CAlloc::BlockSize( m_pPtr );
    }

    /// Assumes control of another objects data
    TFileMapping& Assume( TFileMapping &m )
    {
        // Just drop our memory
        Destroy();

        if ( m.m_pPtr )
        {
            // Assume the memory from this object
            m_hFile = m.m_hFile; m.m_hFile = oexNULL;
            m_pPtr = m.m_pPtr; m.m_pPtr = oexNULL;
            m_pName = m.m_pName; m.m_pName = oexNULL;
            m_bExisting = m.m_bExisting; m.m_bExisting = oexNULL;
			m_llOpenSize = m.m_llOpenSize; m.m_llOpenSize = 0;
			m_hShareHandle = m.m_hShareHandle; m.m_hShareHandle = os::CFMap::vFailed();
			m_bPlain = m.m_bPlain; m.m_bPlain = oexFALSE;

        } // end if

        return *this;
    }

    /// Copies another object
    TFileMapping& Copy( oexCONST TFileMapping &x_m )
    {
        // Just drop our memory
        Destroy();

		// Set plain flag
		m_bPlain = x_m.m_bPlain;

        // Copy the share
        if ( x_m.m_pPtr )
            Create( oexFALSE, oexNULL, x_m.GetShareHandle(), x_m.GetName(), x_m.Size() );

        // Don't need to copy data, it's shared memory remember?

        return *this;
    }

    /// Returns a copy of this object
    TFileMapping Copy()
    {   TFileMapping fm;
        if ( m_pPtr )
            fm.Copy( *this );
        return fm;
    }

    /// Detaches from memory
    void Detach()
    {
        m_hFile = oexNULL;
        m_pPtr = oexNULL;
        m_bExisting = oexFALSE;
        m_pName = oexNULL;
        m_llOpenSize = 0;
        m_hShareHandle = os::CFMap::vFailed();
    }

	/// Sets the plain memory flag
    void Plain( oexBOOL x_bPlain )
    {	m_bPlain = x_bPlain; }

	/// Returns non-zero if plain share
    oexBOOL IsPlain() const
    {	return m_bPlain; }

	/// Sets the nmap offset
    void SetOffset( t_size x_llOffset )
    {	m_llOffset = x_llOffset; }

	/// Gets the nmap offset
    t_size GetOffset()
    {	return m_llOffset; }

private:

	/// Handle to file mapping
	os::CFMap::t_HFILEMAP		m_hFile;

	/// Pointer to the file mapping data
	T                           *m_pPtr;

    /// Set if the memory was already existing
    oexBOOL                     m_bExisting;

    /// Shared memory name
    /// Can't use a CStr here because CStr depends on this class!
    oexTCHAR                    *m_pName;

	/// Size that was passed to CreateFileMapping()
	t_size						m_llOpenSize;

	/// External file handle for share
	os::CFMap::t_HFILEMAP		m_hShareHandle;

	/// Share offset
	t_size						m_llOffset;

	/// Set to non-zero for a plain file mapping (without any memory protection)
	oexBOOL						m_bPlain;
};

