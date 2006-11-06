/*------------------------------------------------------------------
// file_mapping.h
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
// TFileMapping
//
/// Encapsulates a file map
/**
	This class provides the functionality of the file mapping API.	

	A real bummer is that it is apparently not straight forward to
	get the size of an existing file mapping.  To counter this,
	TMemory saves the size in the first four bytes of the mapping.
	I decided not to do anything with the size in this class until
	I can figure out how to reliably query the existing size.
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
		m_pPtr = NULL;
		m_sName.empty();

		if ( m_hFile )
		{	CMMap::osMUnMap( m_hFile );
//			CloseHandle( m_hFile );
			m_hFile = NULL;
		} // end if
	}

	//==============================================================
	// Create()
	//==============================================================
	/// Creates a file mapping
	/**
		\param [in]  x_pName				-	Name of the mapping, can
											be NULL.
		\param [in]  x_llSize				-	Size of the file mapping.
		\param [out] x_pbAlreadyExists	-	Set to non-zero if the 
											named file mapping already
											exists.
		\param [in]  x_uAccess			-	Specifes the memory access
											level.
		
		\return Pointer to the memory or NULL if failure.
	
		\see 
	*/
	T* Create( oexCSTR x_pName, oexINT64 x_llSize, oexBOOL *x_pbAlreadyExists, CFMap::etAccess x_uAccess = CFMap::eAccessAll )
	{
		// Lose old file mapping
		Destroy();

		// Create the file mapping
//		m_hFile = CreateFileMapping( (HANDLE)0xffffffff, NULL, dwPageAccess, 
//									 llSize >> 32, llSize & 0xffffffff, pName );

		m_hFile = CFMap::osCreateFileMapping( (oexPVOID*)&m_pPtr, x_llSize, x_pName, x_uAccess, x_pbAlreadyExists );

		// Did we get the handle
		if ( CFMap::vFailed() == m_hFile ) return NULL;

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

		// Attempt to open the existing file mapping
		m_hFile = OpenFileMapping( dwAccess, FALSE, pName );
		if ( m_hFile == NULL ) return FALSE;

		// Get a pointer to the file data
		m_pPtr = (T*)MapViewOfFile( m_hFile, dwAccess, 0, 0, 0 );
		if ( !m_pPtr ) { Destroy(); return NULL; }

		// Save the name if any
		if ( pName && *pName ) m_sName = pName;
		
		return m_pPtr;
	}

	/// Defualt constructor
	TFileMapping()
	{	m_pPtr = NULL;
		m_hFile = NULL;
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
	// GetName()
	//==============================================================
	/// Returns the name of the file mapping
	oexCSTR GetName() { return m_sName; }


private:

	/// Handle to file mapping
	oexHFILEMAP				m_hFile;

	/// Pointer to the file mapping data
	T*						m_pPtr;

	/// The name of the file mapping
	oexString				m_sName;

};
