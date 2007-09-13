/*------------------------------------------------------------------
// memory.h
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

/// Generic memory allocator
/**
    Memory allocator.  Transparently supports regular and shared
    memory.

    \warning When using shared memory, constructors are called only
             when the memory block is created.  The destructor is
             called when the memory is actually freed.  If your
             sharing memory between processes, this means the 
             constructor may be called in one process, and the 
             destructor in another.  So plan accordingly!
    
*/
template < typename T > class TMem
{
public:

    TMem() 
    {   
        m_pMem = oexNULL;

#if defined( _DEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
    }

    TMem( T* x_pMem, oexBOOL x_bConstructed = oexTRUE ) 
    {   
        m_pMem = x_pMem;

#if defined( _DEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
    }
    
    /// Copy operator
    TMem& operator = ( TMem &m )
    {   return Assume( m ); }
    
    virtual ~TMem()
    {   
        Delete();
    }

    /// Sets the shared memory name
    /**
        \param [in] x_pName -   Shared memory name
        \param [in] x_uLen  -   Length of the string in x_pName.
                                Can be zero if the string is
                                NULL terminated.
        
    */
    TMem& SetName( oexCSTR x_pName, oexUINT x_uLen = 0 )
    {   m_fm.SetName( x_pName, x_uLen ); return *this; }

    /// Detaches from memory pointer
    /**
        You can't detach from a shared memory pointer using this function!
    */
    T* Detach()
    {   T* pPtr = m_pMem;
        m_pMem = oexNULL;
        return pPtr;
    }

    
    TMem& New( oexUINT x_uSize, oexBOOL x_bConstructed = oexFALSE )
    {
        // Lose old memory
        Delete();

        // Shared memory?
        if ( m_fm.GetName() )
            m_fm.Create( oexNULL, oexNULL, x_uSize );

        // Allocate plain old memory
        else
            m_pMem = CAlloc()._Log( m_uLine, m_pFile ).New< T >( x_uSize );

        // Save construction status if needed
        if ( x_bConstructed )
        {   T* pPtr = Ptr();
            CAlloc::SetFlags( pPtr, CAlloc::eF1Constructed );
        } // end if

        return *this;
    }   

    oexUINT Size()
    {
        if ( !m_pMem )
            return 0;

        if ( sizeof( T ) )
            return CAlloc().ArraySize( m_pMem );
        else
            return 0;
    }

    /// Returns non-zero if the object was constructed
    oexBOOL IsConstructed()
    {   return ( CAlloc::eF1Constructed & CAlloc::GetFlags( Ptr() ) ) ? oexTRUE : oexFALSE;
    }

    /// Deletes the object
    void Delete()
    {
        // Do we need to destruct objects?
        T* pPtr = Ptr();

        // Destruct object if needed
        if ( pPtr )
        {
            // Was the object constructed?
            if ( CAlloc::eF1Constructed & CAlloc::GetFlags( pPtr ) 
                 && 1 == CAlloc::GetRefCount( pPtr ) )
            {
                oexUINT uSize = Size();
                for ( oexUINT i = 0; i < uSize; i++ )
                    m_pMem[ i ].~T();

            } // end if

        } // end if

        // Allocation?
        if ( m_pMem )
        {
            // Free the memory
            CAlloc()._Log( m_uLine, m_pFile ).Delete( m_pMem );

            // Lose the pointer
            m_pMem = oexNULL;

        } // end if

        // Lose file mapping if any
        if ( m_fm.Ptr() )
            m_fm.Destroy();
    }

    void Destruct()
    {   m_bConstructed = oexTRUE;
        Delete();
    }

    /// Generic resize
    TMem& Resize(oexUINT x_uNewSize )
    {
        // Shared memory cannot be resized
        if ( !oexVERIFY( !m_fm.Ptr() ) )
            return *this;

        // Do we have what we need?
        oexUINT uSize = Size();
        if ( uSize == x_uNewSize )
            return *this;

        // Anything to resize?
        if ( !m_pMem )
            return New( x_uNewSize );

        // If we're shrinking
        if ( x_uNewSize < uSize )
        {
            // Was the object constructed?
            if ( CAlloc::eF1Constructed & CAlloc::GetFlags( m_pMem ) )
                for ( oexUINT i = x_uNewSize; i < uSize; i++ )
                    m_pMem[ i ].~T();

            // We can make things smaller
            m_pMem = CAlloc()._Log( m_uLine, m_pFile ).Resize( m_pMem, x_uNewSize );

        } // end if

        // We must be making it larger
        else
        {
            // Will a simple resize work?
            T* pMem = CAlloc()._Log( m_uLine, m_pFile ).Resize( m_pMem, x_uNewSize );
            
            if ( !pMem )
            {
                TMem< T > mem;
                if ( !oexVERIFY_PTR( mem.New( x_uNewSize ).Ptr() ) )
                    return *this;

                // Constructed?
                if ( CAlloc::eF1Constructed & CAlloc::GetFlags( m_pMem ) )
                {
                    // Initialize new array
                    for ( oexUINT i = 0; i < x_uNewSize; i++ )

                        if ( i < uSize ) 
                            *mem.Ptr() = *Ptr();
                        else
                            oexPLACEMENT_NEW ( mem.Ptr( i ) ) T();

                } // end if

                // Just copy POD
                else
                    os::CSys::MemCpy( mem.Ptr(), Ptr(), uSize * sizeof( T ) );

                // Assume this new array
                Assume( mem );

            } // end if

            // Construct new objects if needed
            else if ( CAlloc::eF1Constructed & CAlloc::GetFlags( m_pMem ) )
                for ( oexUINT i = uSize; i < x_uNewSize; i++ )
                    oexPLACEMENT_NEW ( &pMem[ i ] ) T();

        } // end else

        return *this;
    }

    /// Reallocates memory
    TMem& Reallocate( oexUINT x_uNewSize )
    {
        +++
        


    }

	//==============================================================
	// Ptr()
	//==============================================================
	/// Returns a pointer to the memory
    T* Ptr()
    {
        if ( m_pMem ) 
            return m_pMem;
           
        return m_fm.Ptr();
    }

	//==============================================================
	// Ptr()
	//==============================================================
	/// Returns a pointer to the memory
    /**
        \param [in] x_uOffset   -   Offset into the memory
    */
    T* Ptr( oexUINT x_uOffset )
    {
        // Out of bounds
        oexASSERT( m_pMem && Size() > x_uOffset );

        if ( m_pMem ) 
            return &m_pMem[ x_uOffset ];
           
        return m_fm.Ptr( x_uOffset );
    }

	//==============================================================
	// c_Ptr()
	//==============================================================
	/// Returns a pointer to the memory
	oexCONST T* c_Ptr() 
    {
        if ( m_pMem ) 
            return m_pMem;
           
        return m_fm.Ptr();
    }

	//==============================================================
	// c_Ptr()
	//==============================================================
	/// Returns a pointer to the memory
    /**
        \param [in] x_uOffset   -   Offset into the memory
    */
    oexCONST T* c_Ptr( oexUINT x_uOffset )
    {
        // Out of bounds
        oexASSERT( m_pMem && Size() > x_uOffset );

        if ( m_pMem ) 
            return &m_pMem[ x_uOffset ];
           
        return m_fm.Ptr( x_uOffset );
    }

public:

    /// Adds a reference to the memory block
    oexUINT AddRef()
    {   if ( !Ptr() ) return 0;
        return CAlloc().AddRef( Ptr() ); 
    }

    /// Returns the memory blocks reference count
    oexUINT GetRefCount()
    {   if ( !Ptr() ) return 0;
        return CAlloc().GetRefCount( Ptr() ); 
    }

    /// Assumes control of another objects data
    TMem& Assume( TMem &x_m )
    {
        // Drop current
        Delete();

        // Assume the memory from this object
        if ( x_m.m_pMem )
        {   m_pMem = x_m.m_pMem; x_m.m_pMem = oexNULL;
            m_fm = x_m.m_fm; x_m.m_fm.Detach();
        } // end if

        return *this;
    }

    /// Copy data from a raw buffer
    TMem& MemCpy( oexCONST T *x_pBuf, oexUINT x_uSize )
    {
        // Verify pointer
        if ( !oexVERIFY_PTR( x_pBuf ) )
            return *this;

        // Do we need more space?
        if ( Size() < x_uSize )
            if ( !New( x_uSize ).Ptr() )
                return *this;

        // Copy data if all the stars align
        os::CSys::MemCpy( Ptr(), x_pBuf, x_uSize );

        return *this;
    }

    /// Copies another objects data
    TMem& Copy( TMem &x_m, oexCSTR x_pNewName = oexNULL, oexUINT x_uNameLen = 0 )
    {
        // Drop current
        Delete();

        // Raw memory?
        if ( x_m.m_pMem || ( x_pNewName && *x_pNewName ) )
        {
            oexUINT uSize = x_m.Size();
            if ( uSize )
            {
                // Shared memory?
                if ( x_pNewName && *x_pNewName )
                    SetName( x_pNewName, x_uNameLen );

                // Do we need to construct objects?
                if ( x_m.IsConstructed() )
                {
                    // Create array of objects
                    ConstructArray( uSize );

                    // Copy objects
                    for ( oexUINT i = 0; i < uSize; i++ )
                        *Ptr( i ) = *x_m.Ptr( i );

                } // end if

                else
                {
                    // Allocate raw array
                    New( uSize );

                    // Just copy the memory
                    os::CSys::MemCpy( Ptr(), x_m.Ptr(), uSize * sizeof( T ) );

                } // end else

            } // end if

        } // end if

        // Copy shared memory
        else if ( x_m.m_fm.Ptr() )
            m_fm.Copy( x_m.m_fm );

        return *this;
    }

    /// Returns a copy of this object
    TMem Copy( oexCSTR x_pNewName = oexNULL, oexUINT x_uNameLen = 0 )
    {   TMem mem;
        if ( Ptr() )
            mem.Copy( *this, x_pNewName, x_uNameLen );
        return mem;
    }

public:

    /// Construct object
    TMem& Construct()
    {
        T* p = New( 1, oexTRUE ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
            oexPLACEMENT_NEW ( p ) T();

        return *this;
    }

    /// Construct object
    template< typename T_P1 >
        TMem& Construct( T_P1 p1 )
    {
        T* p = New( 1, oexTRUE ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
            oexPLACEMENT_NEW ( p ) T( p1 );

        return *this;
    }

    /// Construct object, 2 params
    template< typename T_P1, typename T_P2 >
        TMem& Construct( T_P1 p1, T_P2 p2 )
    {
        T* p = New( 1, oexTRUE ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
            oexPLACEMENT_NEW ( p ) T( p1, p2 );

        return *this;
    }

    /// Construct object, 3 params
    template< typename T_P1, typename T_P2, typename T_P3 >
        TMem& Construct( T_P1 p1, T_P2 p2, T_P3 p3 )
    {
        T* p = New( 1, oexTRUE ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
            oexPLACEMENT_NEW ( p ) T( p1, p2, p3 );

        return *this;
    }

    /// Construct object, 4 params
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4 >
        TMem& Construct( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4 )
    {
        T* p = New( 1, oexTRUE ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
            oexPLACEMENT_NEW ( p ) T( p1, p2, p3, p4 );

        return *this;
    }

    /// Construct object, 5 params
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5 >
        TMem& Construct( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5 )
    {
        T* p = New( 1, oexTRUE ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
            oexPLACEMENT_NEW ( p ) T( p1, p2, p3, p4, p5 );

        return *this;
    }

    /// Construct object, 6 params
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6 >
        TMem& Construct( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6 )
    {
        T* p = New( 1, oexTRUE ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
            oexPLACEMENT_NEW ( p ) T( p1, p2, p3, p4, p5, p6 );

        return *this;
    }

    /// Construct object, 7 params
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7 >
        TMem& Construct( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7 )
    {
        T* p = New( 1, oexTRUE ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
            oexPLACEMENT_NEW ( p ) T( p1, p2, p3, p4, p5, p6, p7 );

        return *this;
    }

    /// Construct object, 8 params
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_P8 >
        TMem& Construct( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8 )
    {
        T* p = New( 1, oexTRUE ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
            oexPLACEMENT_NEW ( p ) T( p1, p2, p3, p4, p5, p6, p7, p8 );

        return *this;
    }

public:

    /// Construct object array
    TMem& ConstructArray( oexUINT x_uSize )
    {
        T *p = New( x_uSize, oexTRUE ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T();

        return *this;
    }

    /// Construct object array, 1 param
    template< typename T_P1 >
        TMem& ConstructArray( oexUINT x_uSize, T_P1 p1 )
    {
        T *p = New( x_uSize, oexTRUE ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T( p1 );

        return *this;
    }

    /// Construct object array, 2 param
    template< typename T_P1, typename T_P2 >
        TMem& ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2 )
    {
        T *p = New( x_uSize, oexTRUE ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2 );

        return *this;
    }

    /// Construct object array, 3 param
    template< typename T_P1, typename T_P2, typename T_P3 >
        TMem& ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3 )
    {
        T *p = New( x_uSize, oexTRUE ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3 );

        return *this;
    }

    /// Construct object array, 4 param
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4 >
        TMem& ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4 )
    {
        T *p = New( x_uSize, oexTRUE ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3, p4 );

        return *this;
    }

    /// Construct object array, 5 param
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5 >
        TMem& ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5 )
    {
        T *p = New( x_uSize, oexTRUE ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3, p4, p5 );

        return *this;
    }

    /// Construct object array, 6 param
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6 >
        TMem& ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6 )
    {
        T *p = New( x_uSize, oexTRUE ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3, p4, p5, p6 );

        return *this;
    }

    /// Construct object array, 7 param
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7 >
        TMem& ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7 )
    {
        T *p = New( x_uSize, oexTRUE ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3, p4, p5, p6, p7 );

        return *this;
    }

    /// Construct object array, 8 param
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_P8 >
        TMem& ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8 )
    {
        T *p = New( x_uSize, oexTRUE ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3, p4, p5, p6, p7, p8 );

        return *this;
    }

private:

    /// Pointer to the memory
    T*                      m_pMem;

    /// File mapping object for shared memory
    TFileMapping< T >       m_fm;

#if defined( _DEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )

public:
    
    /// Registers the source file and line number
    TMem& _Log( oexUINT x_uLine, oexCSTR x_pFile )
    {   m_uLine = x_uLine;
        m_pFile = x_pFile;
        return *this;
    }

private:

    /// Holds the line number of the allocating call
    oexUINT         m_uLine;

    /// Holds the file name of the allocating call
    oexCSTR         m_pFile;

#endif

};