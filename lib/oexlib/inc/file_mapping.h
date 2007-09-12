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
*/
//==================================================================
template < class T > class TFileMapping  
{
public:
	
	//==============================================================
	// Destroy()
	//==============================================================
	/// Unmaps the file and releases all associated resources
	void Destroy()
	{
		if ( m_hFile && m_pPtr )
		{
            m_pPtr = (T*)CAlloc::VerifyMem( m_pPtr, !m_bExisting );
            m_pPtr -= sizeof( oexUINT );
            
            os::CFMap::osReleaseFileMapping( m_hFile, m_pPtr );

		} // end if
		m_pPtr = oexNULL;
    	m_hFile = oexNULL;
	}

	//==============================================================
	// Create()
	//==============================================================
	/// Creates a file mapping
	/**
		\param [in]  x_pFile				-	file to map, NULL for 
												shared memory.
		\param [in]  x_pName				-	Name of the mapping, can
												be NULL.
		\param [in]  x_llSize				-	Size of the file mapping.
		\param [in]  x_uAccess				-	Specifes the memory access
												level.
		
		\return Pointer to the memory or NULL if failure.
	
		\see 
	*/
    T* Create( oexCSTR x_pFile, oexCSTR x_pName, oexINT64 x_llSize, os::CFMap::etAccess x_uAccess = os::CFMap::eAccessAll )
	{
		// Lose old file mapping
		Destroy();

        // Multiply by size of object
        x_llSize *= sizeof( T );

        // Calculate the actual size we'll need
        oexINT64 llSize = x_llSize;
        llSize += sizeof( oexUINT ) + CAlloc::ProtectAreaSize();
        llSize = cmn::NextPower2( llSize );

		// Create file mapping
		m_hFile = os::CFMap::osCreateFileMapping( x_pFile, (oexPVOID*)&m_pPtr, llSize, &llSize, x_pName, x_uAccess, &m_bExisting );

		// Did we get the handle       
        if ( os::CFMap::vFailed() == m_hFile || !m_pPtr ) 
            return oexNULL;

        // Save block size
        *(oexUINT*)m_pPtr = (oexUINT)llSize;
        m_pPtr += sizeof( oexUINT );               

        // Protect memory area
        m_pPtr = (T*)CAlloc::ProtectMem( m_pPtr, (oexUINT)x_llSize, !m_bExisting );

        // Verify existing block
        if ( m_bExisting )
            CAlloc::VerifyMem( m_pPtr, oexFALSE );

        return m_pPtr;
	}

	//==============================================================
	// Open()
	//==============================================================
	/// Opens an existing file mapping
	/**
		\param [in] x_pName	-	Name of the existing file mapping.

		\param [in] x_uAccess -	Access permissions
		
		\return Pointer to file mapping memory of NULL if failure.
	
		\see 
	*/
	T* Open( oexCSTR x_pName, oexUINT x_uAccess = FILE_MAP_ALL_ACCESS )
	{
		// Lose any current file mapping
		Destroy();

        oexASSERT( 0 );

		// Attempt to open the existing file mapping
/*		m_hFile = OpenFileMapping( dwAccess, FALSE, pName );
		if ( m_hFile == oexNULL ) 
            return FALSE;

		// Get a pointer to the file data
		m_pPtr = (T*)MapViewOfFile( m_hFile, dwAccess, 0, 0, 0 );
		if ( !m_pPtr ) 
        {   Destroy(); 
            return oexNULL; 
        } // end if
*/
		// Save the name if any
		if ( pName && *pName ) 
            m_sName = pName;
		
		return m_pPtr;
	}

	/// Defualt constructor
	TFileMapping()
	{	m_pPtr = oexNULL;
        m_hFile = os::CFMap::vFailed();
	}

	/// Destructor
	virtual ~TFileMapping() { Destroy(); }	

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
    T* Ptr( oexUINT x_uOffset )
    {
        // Out of bounds
        oexASSERT( m_pPtr && Size() > x_uOffset );

        return &m_pPtr[ x_uOffset ];
    }

	//==============================================================
	// c_Ptr()
	//==============================================================
	/// Returns a pointer to the file mapping memory
	oexCONST T* c_Ptr() { return m_pPtr; }

	//==============================================================
	// c_Ptr()
	//==============================================================
	/// Returns a pointer to the file mapping memory
    /**
        \param [in] x_uOffset   -   Offset into the memory
    */
    oexCONST T* c_Ptr( oexUINT x_uOffset )
    {
        // Out of bounds
        oexASSERT( m_pPtr && Size() > x_uOffset );

        return &m_pPtr[ x_uOffset ];
    }

	//==============================================================
	// GetName()
	//==============================================================
	/// Returns the name of the file mapping
	oexCSTR GetName() { return m_sName; }

	//==============================================================
	// GetName()
	//==============================================================
	/// Returns the name of the file mapping
	oexBOOL Existing() { return m_bExisting; }

    /// Returns the size of the file mapping
    oexUINT Size()
    {
        if ( !m_pPtr )
            return 0;

        return CAlloc::ArraySize( m_pPtr );
    }

    /// Returns the size of the file mapping
    oexUINT BlockSize()
    {
        if ( !m_pPtr )
            return 0;

        return CAlloc::BlockSize( m_pPtr );
    }


private:

	/// Handle to file mapping
	os::CFMap::t_HFILEMAP		m_hFile;

	/// Pointer to the file mapping data
	T*						    m_pPtr;

    /// Set if the memory was already existing
    oexBOOL                     m_bExisting;

};
