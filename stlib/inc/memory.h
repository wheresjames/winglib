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
		stULONG				ulSize;

		/// Data format
		/**
			This is helpful in that it let's me know the data format.
			from this point on, information must be encoded in a 
			custom way.
		*/
		stULONG				ulFormat;		
	};


public:
	
	/// Constructor
	TMemory(void) { m_pMem = stNULL; }

	/// Constructor
	/**
		\param [in] x_uSize		-	The size of the array.  
		\param [in] x_pName		-	If not stNULL, Allocate creates a memory
									buffer that can be shared between processes.
									You can access the memory from a different
									process by specifying the same name.
		\param [out] x_pbExists	-	If non-zero, indicates that the named
									memory already exists.  Otherwise, indicates
									the named memory was created.  Undefined
									if pName is stNULL.

		\returns	Returns a pointer to the new memory or stNULL if error.

		\see Allocate()
	*/
	TMemory( stUINT x_uSize, stCSTR x_pName = stNULL, stBOOL *x_pbExists = stNULL ) 
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
			m_pMem = stNULL;
		} // end if	
		
	}

	/// Returns pointer to raw memory buffer
	operator T*() { return GetData(); }

	/// Returns pointer to raw memory buffer
	T* GetData()
	{	if ( m_pMem ) return (T*)m_pMem;
		return stNULL;
	}

	/// Returns a reference to the element at the specified offset
	/**
		\param [in]	x_uIndex		-	The index of the element.

		\return Returns stNULL if error, otherwise a reference to the element.
	*/
	T* GetAt( stUINT x_uIndex )
	{	if ( x_uIndex >= Size() ) return stNULL;
		T* p = GetData(); if ( !p ) return stNULL;
		return &p[ x_uIndex ];		
	}

	/// Returns a reference to the element at the specified offset
	/**
		\param [in]	x_uIndex		-	The index of the element.

		\return Returns stNULL if error, otherwise a reference to the element.
	*/
	T& operator []( stUINT x_uIndex ) { return GetData()[ x_uIndex ]; }

	/**
		\param [in] x_uSize		-	The size of the array.  
		\param [in] x_pName		-	If not stNULL, Allocate creates a memory
									buffer that can be shared between processes.
									You can access the memory from a different
									process by specifying the same name.
		\param [out] x_pbExists	-	If non-zero, indicates that the named
									memory already exists.  Otherwise, indicates
									the named memory was created.  Undefined
									if pName is stNULL.

		\returns	Returns a pointer to the new memory or stNULL if error.

		\see TMemory()
	*/
	T* Allocate( stUINT x_uSize, stCSTR x_pName = stNULL, stBOOL *x_pbExists = stNULL )
	{
		// Lock the memory
		CTlLocalLock ll( this );
		if ( !ll.IsLocked() ) return stNULL;

		// Lose old memory
		Destroy();

		// Regular memory?
		if ( !x_pName || !*x_pName )
		{
			STLIB_TRY
			{
				// Allocate memory
				m_pMem = (T*)new stUCHAR[ x_uSize ];
				if ( m_pMem == stNULL ) return stNULL;

			} STLIB_CATCH_ALL { return stNULL; }

			// Save the size of the memory buffer
			m_uSize = x_uSize;

			// Return pointer to memory
			return m_pMem;

		} // end if

		return stNULL;
	}


	/// Attaches to existing memory
	/**
		\param [in] x_pPtr		-	Pointer to buffer
		\param [in] x_uSize		-	Size of buffer in pPtr
	*/
	T* Attach( T* x_pPtr, stUINT x_uSize )
	{
		// Lock the memory
		CTlLocalLock ll( this );
		if ( !ll.IsLocked() ) return stNULL;

		// Lose the old buffer
		Destroy();

		// Punt if invalid buffer
		if ( !x_pPtr || !x_uSize ) return stNULL;

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
		T* pPtr = m_pMem;
		m_pMem = stNULL;
		m_uSize = 0;

		return pPtr;

	} // end if

	/// Returns the buffer size
	stUINT Size()
	{
		// Lock the memory
		CTlLocalLock ll( this );
		if ( !ll.IsLocked() ) return stNULL;

		// Return the size of the raw buffer
		if ( m_pMem ) return m_uSize;

		return 0;
	}

	/// Returns the buffer size
	stULONG SetFormat( stULONG x_ulFormat )
	{
		// Lock the memory
		CTlLocalLock ll( this );
		if ( !ll.IsLocked() ) return stNULL;

		// Return the size of the raw buffer
		if ( m_pMem ) return m_uSize;

		return 0;
	}

	/// Returns the data format
	stULONG GetFormat()
	{
		// Lock the memory
		CTlLocalLock ll( this );
		if ( !ll.IsLocked() ) return stNULL;

		// Return the size of the raw buffer
		if ( m_pMem ) return m_uSize;

		return 0;
	}
	
	//==============================================================
	// Zero()
	//==============================================================
	/// Initializes memory buffer to zero
	T* Zero()
	{
		// Get buffer info
		stUINT uSize = Size();
		T* pPtr = GetData();

		// Zero memory
		if ( uSize && pPtr ) st::COsString::memset( pPtr, uSize, 0 );

		// Return a pointer to the buffer
		return pPtr;
	}

	T* Copy( T* x_p, stUINT x_uSize )
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
	T* MemSet( stLONG x_lVal )
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
	T* MemSet( stLONG x_lVal, stUINT x_uStart, stUINT x_uEnd = MAXDWORD )
	{
		// Get buffer info
		UINT uSize = Size();
		T* pPtr = GetData();
		if ( !uSize || !pPtr ) return stNULL;

		// Valid start?
		if ( x_uStart >= uSize ) return stNULL;

		// Set valid end if needed
		if ( x_uEnd >= uSize ) x_uEnd = uSize - 1;

		// Anything to do?
		if ( x_uStart >= x_uEnd ) return stNULL;

		// Where to start
		pPtr = &( (char*)pPtr )[ x_uStart ];

		// Initialize
		memset( pPtr, x_lVal, ( x_uEnd - x_uStart ) );

		// Return pointer
		return pPtr;
	}

	/// Initializes memory to specified value
	T* Copy( T* x_p, stUINT x_uLen, stUINT x_uOffset )
	{
		// Get buffer info
		UINT uSize = Size();
		T* pPtr = GetData();
		if ( !uSize || !pPtr ) return stNULL;

		// Valid start?
		if ( uOffset >= uSize ) return stNULL;

		// Set valid end if needed
		if ( x_uLen > ( uSize - uOffset ) ) x_uLen = uSize - x_uOffset;

		// Anything to do?
		if ( !x_uLen ) return stNULL;

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
	stUINT					m_uSize;

};


//==================================================================
// CAutoMem
//
/// Just a simple auto pointer
//==================================================================
template < class T > class CAutoMem
{
    public:

        /// Default constructor
        CAutoMem() { m_p = stNULL; }

        /// Destructor
        ~CAutoMem() { release(); }

        /// Release allocated object
        void release() { if ( m_p ) { delete m_p; m_p = stNULL; } }

        /// Returns a pointer to encapsulated object
        T& Obj() { if ( !m_p ) m_p = new T; return *m_p; }

        /// Returns a pointer to the encapsulated object
        operator T&() { return Obj(); }

        /// Returns a pointer to the encapsulated object
        operator T*() { return &Obj(); }

    private:

        /// Contains a pointer to the controlled object
        T       *m_p;
        
};
