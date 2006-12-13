/*------------------------------------------------------------------
// memory.h
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

/// TMemory
/**
	Use this class for allocating regular and shared memory
	for use by the network functions.   
	
	When using shared memory, the underlying buffer is actually 
	a memory mapped file.  Preferably, use CLocalLock class to 
	ensure the memory is locked before using.  Such as follows...
  
  \code

	TMemory< char > mem;

	mem.allocate( 1000 );

	CLocalLock ll( mem );
	if ( !ll.IsLocked() ) { // handle error  }

	// Use mem
	mem[ 0 ] = 'A';
  
  \endcode

	The lock name will be [memory name] + "_lock".  

*/
template <typename T > class TMemory : public CTlLock
{
public:

	/// SMemoryHeader
	/**
		This structure defines the area of memory in front of
		a file mapping.  To access the file mapping at all, you
		should first lock the memory.  This is absolutely needed
		to synchronize the access of multiple threads or processes.
		
	*/
	typedef struct SMemoryHeader
	{
		/// Size of the allocated memory
		/**
			Currently, I cannot seem to figure out how to get the
			size of an existing file mapping.  This variable exists
			so that I can retieve that setting without complicating
			the communication protocols.
		*/
		oexULONG				ulSize;

		/// Data format
		/**
			This is helpful in that it let's me know the data format.
			from this point on, information must be encoded in a 
			custom way.
		*/
		oexULONG				ulFormat;		
	};


public:
	
	/// Constructor
	TMemory(void) { m_pMem = oexNULL; }

	/// Constructor
	/**
		\param [in] x_uSize		-	The size of the array.  
		\param [in] x_pName		-	If not NULL, Allocate creates a memory
									buffer that can be shared between processes.
									You can access the memory from a different
									process by specifying the same name.
		\param [out] x_pbExists	-	If non-zero, indicates that the named
									memory already exists.  Otherwise, indicates
									the named memory was created.  Undefined
									if pName is NULL.

		\returns	Returns a pointer to the new memory or NULL if error.

		\see Allocate()
	*/
	TMemory( oexUINT x_uSize, oexCSTR x_pName = oexNULL, oexBOOL *x_pbExists = oexNULL ) 
	{	Allocate( uSize, pName, pbExists ); }

	/// Destructor
	virtual ~TMemory(void) { Destroy(); }	

	/// Frees all memory
	void Destroy() 
	{
		// Lock the memory
		CTlLocalLock ll( this );
		if ( !ll.IsLocked() ) return;

		// Lose array
		m_uSize = 0;
		if ( m_pMem )
		{	delete [] m_pMem;
			m_pMem = oexNULL;
		} // end if	
		
		// Release any file mapping
		m_fm.Destroy(); 
	}

	/// Returns pointer to raw memory buffer
	operator T*() { return GetData(); }

	/// Returns pointer to raw memory buffer
	T* GetData()
	{	if ( m_pMem ) return (T*)m_pMem;
		if ( m_fm.Ptr() ) return (T*)( m_fm.Ptr() + sizeof( SMemoryHeader ) );
		return oexNULL;
	}

	/// Returns a reference to the element at the specified offset
	/**
		\param [in]	x_uIndex		-	The index of the element.

		\return Returns NULL if error, otherwise a reference to the element.
	*/
	T* GetAt( oexUINT x_uIndex )
	{	if ( x_uIndex >= Size() ) return oexNULL;
		T* p = GetData(); if ( !p ) return oexNULL;
		return &p[ x_uIndex ];		
	}

	/// Returns a reference to the element at the specified offset
	/**
		\param [in]	x_uIndex		-	The index of the element.

		\return Returns NULL if error, otherwise a reference to the element.
	*/
	T& operator []( oexUINT x_uIndex ) { return GetData()[ x_uIndex ]; }

	/**
		\param [in] x_uSize		-	The size of the array.  
		\param [in] x_pName		-	If not NULL, Allocate creates a memory
									buffer that can be shared between processes.
									You can access the memory from a different
									process by specifying the same name.
		\param [out] x_pbExists	-	If non-zero, indicates that the named
									memory already exists.  Otherwise, indicates
									the named memory was created.  Undefined
									if pName is NULL.

		\returns	Returns a pointer to the new memory or NULL if error.

		\see TMemory()
	*/
	T* Allocate( oexUINT x_uSize, oexCSTR x_pName = oexNULL, oexBOOL *x_pbExists = oexNULL )
	{
		// Lock the memory
		CTlLocalLock ll( this );
		if ( !ll.IsLocked() ) return oexNULL;

		// Lose old memory
		Destroy();

		// Regular memory?
		if ( !x_pName || !*x_pName )
		{
			OEXLIB_TRY
			{
				// Allocate memory
				m_pMem = (T*)new oexUCHAR[ x_uSize ];
				if ( m_pMem == oexNULL ) return oexNULL;

			} OEXLIB_CATCH_ALL { return oexNULL; }

			// Save the size of the memory buffer
			m_uSize = x_uSize;

			// Return pointer to memory
			return m_pMem;

		} // end if

		// Use shared memory
		oexBOOL bExists = oexFALSE;
		if ( oexNULL != m_fm.Create( x_pName, sizeof( SMemoryHeader ) + x_uSize, &bExists ) )
		{
			// Already existing?
			if ( x_pbExists ) *x_pbExists = bExists;

			// Must exist?
			if ( x_uSize == 0 && !bExists ) { Destroy(); return oexNULL; }

			// Save new size
			( (SMemoryHeader*)m_fm.Ptr() )->ulSize = x_uSize;

			// Use the memory
			return GetData();

		} // end if

		return oexNULL;
	}

	/// Attaches to existing memory
	/**
		\param [in] pName		-	Globally unique name for memory.

		Seems file mapping is flawed in that you can't determine
		the size of an existing file mapping.  So I cheated and stuck
		a SMemoryHeader structure in the first part of memory.
	*/
	T* Attach( oexCSTR x_pName ) 
	{
		// Lock the memory
		CTlLocalLock ll( this );
		if ( !ll.IsLocked() ) return oexNULL;

		// Lose previous
		Destroy();

		// Use shared memory
		return m_fm.Open( pName );
	}


	/// Attaches to existing memory
	/**
		\param [in] x_pPtr		-	Pointer to buffer
		\param [in] x_uSize		-	Size of buffer in pPtr
	*/
	T* Attach( T* x_pPtr, oexUINT x_uSize )
	{
		// Lock the memory
		CTlLocalLock ll( this );
		if ( !ll.IsLocked() ) return oexNULL;

		// Lose the old buffer
		Destroy();

		// Punt if invalid buffer
		if ( !x_pPtr || !x_uSize ) return oexNULL;

		// Save buffer pointer and buffer size
		m_pMem = x_pPtr;
		m_uSize = x_uSize;		

		return x_pPtr;
	}

	/// Attaches to existing memory
	/**
		\param [in] x_pMemory		-	Object containing memory
	*/
	T* Attach( TMemory &x_rMemory )
	{
		// Attach to memory
		Attach( x_rMemory.m_pMem, x_rMemory.m_uSize );

		// Detach object from memory
		x_rMemory.Detach();

		return m_pMem;
	}

	/// Detach from existing memory buffer
	T* Detach()
	{
		// Just destroy any shared memory
		m_fm.Destroy();

		T* pPtr = m_pMem;
		m_pMem = oexNULL;
		m_uSize = 0;

		return pPtr;

	} // end if

	/// Returns the buffer size
	oexUINT Size()
	{
		// Lock the memory
		CTlLocalLock ll( this );
		if ( !ll.IsLocked() ) return oexNULL;

		// Return the size of the raw buffer
		if ( m_pMem ) return m_uSize;

		// Do we have a file mapping?
		if ( !m_fm.Ptr() ) return 0;

		// Return the size of the file mapping
		return ( (SMemoryHeader*)m_fm.Ptr() )->ulSize;
	}

	/// Returns the buffer size
	oexULONG SetFormat( oexULONG x_ulFormat )
	{
		// Lock the memory
		CTlLocalLock ll( this );
		if ( !ll.IsLocked() ) return oexNULL;

		// Return the size of the raw buffer
		if ( m_pMem ) return m_uSize;

		// Do we have a file mapping?
		if ( !m_fm.Ptr() ) return 0;

		// Return the size of the file mapping
		return ( ( (SMemoryHeader*)m_fm.Ptr() )->ulFormat = ulFormat );
	}

	/// Returns the data format
	oexULONG GetFormat()
	{
		// Lock the memory
		CTlLocalLock ll( this );
		if ( !ll.IsLocked() ) return oexNULL;

		// Return the size of the raw buffer
		if ( m_pMem ) return m_uSize;

		// Do we have a file mapping?
		if ( !m_fm.Ptr() ) return 0;

		// Return the size of the file mapping
		return ( (SMemoryHeader*)m_fm.Ptr() )->ulFormat;
	}
	
	//==============================================================
	// Zero()
	//==============================================================
	/// Initializes memory buffer to zero
	T* Zero()
	{
		// Get buffer info
		UINT uSize = Size();
		T* pPtr = GetData();

		// Zero memory
		if ( uSize && pPtr ) ZeroMemory( pPtr, uSize );

		// Return a pointer to the buffer
		return pPtr;
	}

	T* Copy( T* x_p, oexUINT x_uSize )
	{
		// Get buffer info
		UINT uSize = Size();
		T* pPtr = GetData();

		// Don't overflow the buffer
		if ( x_uSize > uSize ) x_uSize = uSize;

		// Zero memory
		if ( uSize && pPtr ) memcpy( pPtr, x_p, x_uSize );

		// Return a pointer to the buffer
		return pPtr;
	}

	/// Initializes memory to specified value
	T* MemSet( oexLONG x_lVal )
	{
		// Get buffer info
		UINT uSize = Size();
		T* pPtr = GetData();

		// Set the memory
		if ( uSize && pPtr ) memcpy( pPtr, x_lVal, uSize );

		// Return pointer to buffer
		return pPtr;
	}

	/// Initializes memory to specified value
	T* MemSet( oexLONG x_lVal, oexUINT x_uStart, oexUINT x_uEnd = MAXDWORD )
	{
		// Get buffer info
		UINT uSize = Size();
		T* pPtr = GetData();
		if ( !uSize || !pPtr ) return oexNULL;

		// Valid start?
		if ( x_uStart >= uSize ) return oexNULL;

		// Set valid end if needed
		if ( x_uEnd >= uSize ) x_uEnd = uSize - 1;

		// Anything to do?
		if ( x_uStart >= x_uEnd ) return oexNULL;

		// Where to start
		pPtr = &( (char*)pPtr )[ x_uStart ];

		// Initialize
		memset( pPtr, x_lVal, ( x_uEnd - x_uStart ) );

		// Return pointer
		return pPtr;
	}

	/// Initializes memory to specified value
	T* Copy( T* x_p, oexUINT x_uLen, oexUINT x_uOffset )
	{
		// Get buffer info
		UINT uSize = Size();
		T* pPtr = GetData();
		if ( !uSize || !pPtr ) return oexNULL;

		// Valid start?
		if ( uOffset >= uSize ) return oexNULL;

		// Set valid end if needed
		if ( x_uLen > ( uSize - uOffset ) ) x_uLen = uSize - x_uOffset;

		// Anything to do?
		if ( !x_uLen ) return oexNULL;

		// Where to start
		pPtr = &( (LPBYTE)pPtr )[ x_uOffset ];

		// Initialize
		memcpy( pPtr, x_p, x_uLen );

		// Return pointer
		return pPtr;
	}

private:

	/// Pointer to raw memory buffer
	T*						m_pMem;

	/// Size of raw memory buffer
	oexUINT					m_uSize;

	/// File mapping object
	TFileMapping< T >		m_fm;

};

