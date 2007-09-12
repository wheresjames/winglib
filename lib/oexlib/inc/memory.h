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

template < typename T > class TMem
{
public:

    TMem() 
    {   
        m_pMem = oexNULL;
        m_pName = oexNULL;
        m_bConstructed = oexFALSE;

#if defined( _DEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
    }

    TMem( T* x_pMem, oexBOOL x_bConstructed = oexTRUE ) 
    {   
        m_pMem = x_pMem;
        m_pName = oexNULL;
        m_bConstructed = x_bConstructed;

#if defined( _DEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
    }

    virtual ~TMem()
    {   SetName( oexNULL );
        Delete();
    }

    TMem& SetName( oexCSTR x_pName, oexUINT x_uLen = 0 )
    {
        // Lose the name
        if ( m_pName )
        {
            OexAllocDelete( m_pName );            
            m_pName = oexNULL;
        } // end if

        if ( !x_pName )
            return *this;

        if ( !x_uLen )
            x_uLen = zstr::Length( x_pName );

        // Allocate
        m_pName = OexAllocNew< TCHAR >( x_uLen + 1 );

        str::Copy( m_pName, x_uLen + 1, x_pName, x_uLen );

        return *this;
    }

    T* Detach()
    {
        T* pPtr = m_pMem;
        m_pMem = oexNULL;
        return pPtr;
    }

    
    TMem& New( oexUINT x_uSize )
    {
        // Lose old memory
        Delete();

        // Shared memory?
        if ( m_pName )
            m_fm.Create( oexNULL, m_pName, x_uSize );

        // Allocate plain old memory
        else
            m_pMem = CAlloc()._Log( m_uLine, m_pFile ).New< T >( x_uSize );

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

    void Delete()
    {
        // Do we need to destruct objects?
        if ( m_bConstructed )
        {
            T* pPtr = Ptr();

            if ( pPtr )
            {
                oexUINT uSize = Size();
                for ( oexUINT i = 0; i < uSize; i++ )
                    m_pMem[ i ].~T();

            } // end if

            m_bConstructed = oexFALSE;

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
    T* Resize(oexINT x_nNewSize )
    {
        if ( !oexVERIFY_PTR( m_pMem ) )
            return oexNULL;

        return TAlloc< T >().resize( m_pMem, x_nNewSize, m_uLine, m_pFile );
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

    /// Construct object
    TMem& Construct()
    {
        T* p = New( 1 ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
        {   m_bConstructed = oexTRUE;
            oexPLACEMENT_NEW ( p ) T();
        } // end if

        return *this;
    }

    /// Construct object
    template< typename T_P1 >
        TMem& Construct( T_P1 p1 )
    {
        T* p = New( 1 ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
        {   m_bConstructed = oexTRUE;
            oexPLACEMENT_NEW ( p ) T( p1 );
        } // end if

        return *this;
    }

    /// Construct object, 2 params
    template< typename T_P1, typename T_P2 >
        TMem& Construct( T_P1 p1, T_P2 p2 )
    {
        T* p = New( 1 ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
        {   m_bConstructed = oexTRUE;
            oexPLACEMENT_NEW ( p ) T( p1, p2 );
        } // end if

        return *this;
    }

    /// Construct object, 3 params
    template< typename T_P1, typename T_P2, typename T_P3 >
        TMem& Construct( T_P1 p1, T_P2 p2, T_P3 p3 )
    {
        T* p = New( 1 ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
        {   m_bConstructed = oexTRUE;
            oexPLACEMENT_NEW ( p ) T( p1, p2, p3 );
        } // end if

        return *this;
    }

    /// Construct object, 4 params
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4 >
        TMem& Construct( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4 )
    {
        T* p = New( 1 ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
        {   m_bConstructed = oexTRUE;
            oexPLACEMENT_NEW ( p ) T( p1, p2, p3, p4 );
        } // end if

        return *this;
    }

    /// Construct object, 5 params
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5 >
        TMem& Construct( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5 )
    {
        T* p = New( 1 ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
        {   m_bConstructed = oexTRUE;
            oexPLACEMENT_NEW ( p ) T( p1, p2, p3, p4, p5 );
        } // end if

        return *this;
    }

    /// Construct object, 6 params
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6 >
        TMem& Construct( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6 )
    {
        T* p = New( 1 ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
        {   m_bConstructed = oexTRUE;
            oexPLACEMENT_NEW ( p ) T( p1, p2, p3, p4, p5, p6 );
        } // end if

        return *this;
    }

    /// Construct object, 7 params
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7 >
        TMem& Construct( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7 )
    {
        T* p = New( 1 ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
        {   m_bConstructed = oexTRUE;
            oexPLACEMENT_NEW ( p ) T( p1, p2, p3, p4, p5, p6, p7 );
        } // end if

        return *this;
    }

    /// Construct object, 8 params
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_P8 >
        TMem& Construct( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8 )
    {
        T* p = New( 1 ).Ptr();
        if ( m_pMem || !m_fm.Existing() )
        {   m_bConstructed = oexTRUE;
            oexPLACEMENT_NEW ( p ) T( p1, p2, p3, p4, p5, p6, p7, p8 );
        } // end if

        return *this;
    }

public:

    /// Construct object array
    TMem& ConstructArray( oexUINT x_uSize )
    {
        T *p = New( x_uSize ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
        {   m_bConstructed = oexTRUE;
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T();
        } // end if

        return *this;
    }

    /// Construct object array, 1 param
    template< typename T_P1 >
        TMem& ConstructArray( oexUINT x_uSize, T_P1 p1 )
    {
        T *p = New( x_uSize ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
        {   m_bConstructed = oexTRUE;
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T( p1 );
        } // end if

        return *this;
    }

    /// Construct object array, 2 param
    template< typename T_P1, typename T_P2 >
        TMem& ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2 )
    {
        T *p = New( x_uSize ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
        {   m_bConstructed = oexTRUE;
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2 );
        } // end if

        return *this;
    }

    /// Construct object array, 3 param
    template< typename T_P1, typename T_P2, typename T_P3 >
        TMem& ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3 )
    {
        T *p = New( x_uSize ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
        {   m_bConstructed = oexTRUE;
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3 );
        } // end if

        return *this;
    }

    /// Construct object array, 4 param
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4 >
        TMem& ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4 )
    {
        T *p = New( x_uSize ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
        {   m_bConstructed = oexTRUE;
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3, p4 );
        } // end if

        return *this;
    }

    /// Construct object array, 5 param
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5 >
        TMem& ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5 )
    {
        T *p = New( x_uSize ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
        {   m_bConstructed = oexTRUE;
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3, p4, p5 );
        } // end if

        return *this;
    }

    /// Construct object array, 6 param
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6 >
        TMem& ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6 )
    {
        T *p = New( x_uSize ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
        {   m_bConstructed = oexTRUE;
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3, p4, p5, p6 );
        } // end if

        return *this;
    }

    /// Construct object array, 7 param
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7 >
        TMem& ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7 )
    {
        T *p = New( x_uSize ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
        {   m_bConstructed = oexTRUE;
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3, p4, p5, p6, p7 );
        } // end if

        return *this;
    }

    /// Construct object array, 8 param
    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_P8 >
        TMem& ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8 )
    {
        T *p = New( x_uSize ).Ptr();
        if ( oexVERIFY( p ) && ( m_pMem || !m_fm.Existing() ) )
        {   m_bConstructed = oexTRUE;
            for ( oexUINT i = 0; i < x_uSize; i++ )
                oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3, p4, p5, p6, p7, p8 );
        } // end if

        return *this;
    }

private:

    /// Non zero if objects were constructed
    oexBOOL                 m_bConstructed;

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

    /// Shared memory name
    oexCHAR         *m_pName;

#endif

};