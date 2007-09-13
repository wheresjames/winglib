/*------------------------------------------------------------------
// str.h
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


/// Generic string class
/**
    
  \code

  \endcode

*/
template < typename T > class TStr
{
public:

    /// Default constructor
    TStr()
    {
        m_nLength = 0;
    }

    /// Destructor
    ~TStr()
    {   Destroy();
    }

    /// Releases the string
    void Destroy()
    {
        // Zer length
        m_nLength = 0;

        // Lose the string memory
        m_mem.Delete();
    }

public:

    /// Creates our own copy of a string if it's shared
	oexBOOL Unshare()
	{
		// Create our own buffer if shared
		if ( 1 == m_mem.GetRefCount() )
			return dslTRUE;

		oexUINT uOldSize = m_mem->Size();

		if ( uOldSize )
		{
            TMem< T > mem;
            if ( !dslVERIFY_PTR( mem.OexAllocate( uOldSize ) ) )
                return oexFALSE;

            // Copy string data
			os::CSys::MemCpy( mem.Ptr(), m_mem.c_Ptr(), uOldSize * sizeof( T ) );
            
            // Use the new memory
			m_mem = mem;

		} // end if

		else m_mem.Destroy();

		return dslTRUE;

	}

	/// Allocates at least the number of bytes specified
	T* Allocate( oexUINT x_uSize )
	{	
//		oexTRACE( "TStr::Allocate( %lu )\n", uSize );

		// Break any existing share
		Unshare();

		T* pMem = m_memStr->OexReAllocate( uSize + 1 );
		if ( !oexVERIFY_PTR( pMem ) )
			return oexNULL;

		// Null terminate our new buffer
		pMem[ uSize ] = 0;

		// User can't possibly copy any data
		if ( !uSize ) m_nLength = 0;

		// We'll scan for the length of the buffer later
		else m_nLength = -1;

		return pMem;
	}


private:

    /// The string memory
    TMem< T >       m_mem;

    /// The length of the string
    oexINT          m_nLength;
    
};


/// Unicode friendly string class
typedef TStr< oexTCHAR >    CStr;

/// 8 bit character string
typedef TStr< oexCHAR >     CStr8;

/// 16 bit character string
typedef TStr< oexINT16 >    CStr16;

/// 32 bit character string
typedef TStr< oexINT32 >    CStr32;

/// 64 bit character string
typedef TStr< oexINT64 >    CStr64;
