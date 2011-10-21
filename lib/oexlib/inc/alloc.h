/*------------------------------------------------------------------
// alloc.h
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

/// Allocator
/**
    Memory allocation

  \code

    // *** Create / resize / destroy generic buffer

    char * buf = OexAllocNew< char >( 100 );

    // Resize must be less than the create size
    buf = OexAllocResize( buf, 100, 50 );

    OexAllocDelete( buf, 50 );



    // *** Create / destroy class

    CMyClass *p = OexAllocConstruct< CMyClass >( 10 );

    OexAllocDestruct( p );

  \endcode

*/
class CAlloc
{

public:

    enum
    {
        /// Maximum number of allocation indexes
        /**
           index 0 = the allocation
           index 1 = resize
           index 2 = delete
		   index 3 = reserved
        */
        eMaxAllocTrail = 4
    };

    // Allocation flags
    enum
    {
        /// Set if the object was constructed
        eF1Constructed = 0x00000001

    };

    /// This structure contains information about where a memory block was allocated
    struct SAllocInfo
    {
        /// Line number that allocated the memory
        oexUINT         uLine;

        /// File name that allocated the memory
        oexCSTR         pFile;
    };

    struct SBlockHeader
    {
        /// Allocation history information
        SAllocInfo      ai[ eMaxAllocTrail ];

        /// Protected area size
        oexUINT         uSize;

        /// Reference count
        oexUINT         uRef;

        /// Memory block flags
        oexUINT         uFlags;
    };

// Alignment params
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
//#	if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
#		define OEX_MEMBLOCKPADDING 	( sizeof( OEX_NAMESPACE::CAlloc::SBlockHeader ) + sizeof( OEX_NAMESPACE::CAlloc::m_ucUnderrunPadding ) )
//#	else
//#		define OEX_MEMBLOCKPADDING 	( sizeof( OEX_NAMESPACE::CAlloc::SBlockHeader ) )
//#	endif
#	define OEX_MEMBLOCKOVERHEAD 	( OEX_MEMBLOCKPADDING + sizeof( OEX_NAMESPACE::CAlloc::m_ucOverrunPadding ) )
#else
#	define OEX_MEMBLOCKPADDING 		0
#	define OEX_MEMBLOCKOVERHEAD 	0
#endif

// Alignment params
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
#	if defined( OEX_ALIGNEDMEM ) && 0 != OEX_ALIGNEDMEM
#		define OEX_SIZE_VAR	OEX_MEMBLOCKPADDING
#	else
#		define OEX_SIZE_VAR sizeof( oexUINT )
#	endif
#else
#	define OEX_SIZE_VAR sizeof( oexUINT )
#endif

public:

	//==============================================================
	// ProtectMem()
	//==============================================================
	/// Protects an area of memory by adding padding around it.
	/**
		\param [in] x_pBuf		 -	Memory area to protect
									should be at least ( x_uSize + ProtectAreaSize() )
		\param [in] x_uSize		 -	The size of the memory being allocated
        \param [in] x_bUpdate    -  Non-zero to update memory structure
        \param [in] x_uLine      -  The line number that allocated this block
        \param [in] x_pFile      -  The file name that allocated this block
        \param [in] x_uInfoIndex -  The index at which to store index info
	*/
	static oexPVOID ProtectMem( oexPVOID x_pBuf, oexUINT x_uSize, oexBOOL x_bUpdate, oexUINT x_uLine = 0, oexCSTR x_pFile = oexNULL, oexUINT x_uInfoIndex = eMaxAllocTrail );

	//==============================================================
	// VerifyMem()
	//==============================================================
	/// Verifys the memory block boundry markers
	/**
		\param [in] x_pBuf		-	Memory area to protect
									should be at least ( x_uSize + ProtectAreaSize() )
        \param [in] x_bUpdate   -   Non-zero to update memory structure
        \param [out] x_puSize   -   Size of the allocated memory block
	*/
	static oexPVOID VerifyMem( oexPVOID x_pBuf, oexBOOL x_bUpdate, oexUINT *x_puSize = oexNULL, oexUINT x_uLine = 0, oexCSTR x_pFile = oexNULL, oexUINT x_uInfoIndex = eMaxAllocTrail );

    /// Dumps information on memory block
    static void ReportBlock( oexCPVOID x_pMem, oexUINT uSize );

	/// Returns a string describing the memory block
	static oexBOOL GetBlockReport( oexCPVOID x_pMem, oexUINT uSize, oexSTR pBuf, oexUINT uBuf );

    /// Returns the size of the memory protection overhead
    static inline oexUINT ProtectAreaSize()
    {
	    return  sizeof( SBlockHeader )
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
                + sizeof( m_ucUnderrunPadding )
                + sizeof( m_ucOverrunPadding )
#endif
                ;
    }

    /// Returns the size of the allocated block
    /**
        \param [in] x_pBuf  -   Protected memory pointer
    */
    static inline oexUINT BlockSize( oexCPVOID x_pBuf )
    {
        // Grab the size of the allocated buffer
        return *(oexUINT*)( ( (oexUCHAR*)x_pBuf ) - OEX_SIZE_VAR
                                                  - sizeof( SBlockHeader )
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
                                                  - sizeof( m_ucUnderrunPadding )
#endif
                                                  );
    }

    /// Returns the block header
    /**
        \param [in] x_pBuf  -   Protected memory pointer
    */
    static inline SBlockHeader* GetBlockHeader( oexCPVOID x_pBuf )
    {
        return (SBlockHeader*)( (oexUCHAR*)x_pBuf
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
                                                  - sizeof( m_ucUnderrunPadding )
#endif
                                                  - sizeof( SBlockHeader ) );
    }

    /// Returns the size of the usable block
    /**
        \param [in] x_pBuf  -   Protected memory pointer
    */
    static inline oexUINT UsableSize( oexCPVOID x_pBuf )
    {
        // Return size of usable memory
        return ( (SBlockHeader*)( (oexUCHAR*)x_pBuf
                                  - sizeof( SBlockHeader )
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
                                  - sizeof( m_ucUnderrunPadding )
#endif
                                  ) )->uSize;
    }

    /// Sets flags
    /**
        \param [in] x_pBuf  -   Protected memory pointer
    */
    static inline oexUINT SetFlags( oexPVOID x_pBuf, oexUINT x_uFlags )
    {
        // Return size of usable memory
        return ( (SBlockHeader*)( (oexUCHAR*)x_pBuf
                                  - sizeof( SBlockHeader )
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
                                  - sizeof( m_ucUnderrunPadding )
#endif
                                  ) )->uFlags = x_uFlags;
    }

    /// Gets flags
    /**
        \param [in] x_pBuf  -   Protected memory pointer
    */
    static inline oexUINT GetFlags( oexCPVOID x_pBuf )
    {
        // Return size of usable memory
        return ( (SBlockHeader*)( (oexUCHAR*)x_pBuf
                                  - sizeof( SBlockHeader )
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
                                  - sizeof( m_ucUnderrunPadding )
#endif
                                  ) )->uFlags;
    }

	/// Returns the data pointer from the raw allocation pointer
	static inline oexCPVOID RawToPtr( oexCPVOID x_pRaw )
	{
        return ( (oexUCHAR*)x_pRaw
                            + sizeof( SBlockHeader )
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
                            + sizeof( m_ucUnderrunPadding )
#endif
							);
	}

	/// Returns the raw allocation pointer from the data pointer
	static inline oexCPVOID PtrToRaw( oexCPVOID x_pBuf )
	{
        return ( (oexUCHAR*)x_pBuf
                            - sizeof( SBlockHeader )
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
                            - sizeof( m_ucUnderrunPadding )
#endif
							);
	}

    /// Returns the reference count
    /**
        \param [in] x_pBuf  -   Protected memory pointer
    */
    static inline oexUINT GetRefCount( oexCPVOID x_pBuf )
    {
        // Return size of usable memory
        return ( (SBlockHeader*)( (oexUCHAR*)x_pBuf
                                  - sizeof( SBlockHeader )
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
                                  - sizeof( m_ucUnderrunPadding )
#endif
                                  ) )->uRef;
    }

    /// Adds a reference to the block
    /**
        \param [in] x_pBuf  -   Protected memory pointer
    */
    static inline oexUINT AddRef( oexPVOID x_pBuf )
    {
        // Add a count to the reference
        return ++( (SBlockHeader*)( (oexUCHAR*)x_pBuf
                                    - sizeof( SBlockHeader )
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
                                    - sizeof( m_ucUnderrunPadding )
#endif
                                    ) )->uRef;
    }

    /// Decreases the reference count on a memory block
    /**
        \param [in] x_pBuf  -   Protected memory pointer
    */
    static inline oexUINT DecRef( oexPVOID x_pBuf )
    {
        // Decrease the reference count
        return --( (SBlockHeader*)( (oexUCHAR*)x_pBuf
                                    - sizeof( SBlockHeader )
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
                                    - sizeof( m_ucUnderrunPadding )
#endif
                                    ) )->uRef;
    }

public:

#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )

    /// Under run padding
    static oexUCHAR m_ucUnderrunPadding[ 4 ];

    /// Over run padding
	static oexUCHAR m_ucOverrunPadding[ 24 ];

#endif

public:

    /// Genric block allocation
    static oexPVOID Alloc( oexUINT x_uSize, oexUINT x_uLine, oexCSTR x_pFile, oexUINT x_uInfoIndex, oexBOOL x_bUseFullBlock = oexFALSE );

    /// Generic block free
    static oexINT Free( oexPVOID x_pBuf, oexUINT x_uLine, oexCSTR x_pFile, oexUINT x_uInfoIndex );

    /// Resizes the buffer without reallocating if possible
    static oexPVOID ReAlloc( oexPVOID x_pBuf, oexUINT x_uNewSize, oexUINT x_uLine, oexCSTR x_pFile, oexUINT x_uInfoIndex, oexBOOL x_bAllowMove = oexFALSE, oexBOOL x_bUseFullBlock = oexFALSE );

public:

    /// Generic new function
    template< typename T >
        T* New( oexUINT x_uSize, oexBOOL x_bConstructed = oexFALSE, oexBOOL x_bUseFullBlock = oexFALSE )
    {
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        T* pPtr = (T*)Alloc( x_uSize * sizeof( T ), m_uLine, m_pFile, 0, x_bUseFullBlock );
        m_pFile = oexNULL;
        m_uLine = 0;
#else
        T* pPtr = (T*)Alloc( x_uSize * sizeof( T ), 0, 0, 0, x_bUseFullBlock );
#endif
        if ( !pPtr )
            return oexFALSE;

        // Save construction status if needed
        if ( x_bConstructed )
            CAlloc::SetFlags( pPtr, CAlloc::eF1Constructed );

        return pPtr;
    }

    /// Generic delete
    template< typename T >
        oexINT Delete( T* x_pPtr )
    {
        // Destruct object if needed
        if ( x_pPtr )
        {
            // Was the object constructed?
            if ( CAlloc::eF1Constructed & CAlloc::GetFlags( x_pPtr )
                 && 1 == CAlloc::GetRefCount( x_pPtr ) )
            {
                oexUINT uSize = ArraySize( x_pPtr );
                for ( oexUINT i = 0; i < uSize; i++ )
                    x_pPtr[ i ].~T();

            } // end if

        } // end if

#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        oexINT nRef = Free( x_pPtr, m_uLine, m_pFile, 2 );
        m_pFile = oexNULL;
        m_uLine = 0;
#else
        oexINT nRef = Free( x_pPtr, 0, 0, 2 );
#endif

        return nRef;
    }

    /// Generic resize
    template< typename T >
        T* Resize( T* x_pPtr, oexUINT x_uNewSize, oexBOOL x_bAllowMove = oexFALSE, oexBOOL x_bUseFullBlock = oexFALSE )
    {
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        T* pPtr = (T*)ReAlloc( x_pPtr, x_uNewSize * sizeof( T ), m_uLine, m_pFile, 1, x_bAllowMove, x_bUseFullBlock );
        m_pFile = oexNULL;
        m_uLine = 0;
#else
        T* pPtr = (T*)ReAlloc( x_pPtr, x_uNewSize * sizeof( T ), 0, 0, 1, x_bAllowMove, x_bUseFullBlock );
#endif
        return pPtr;
    }

    template< typename T >
        static oexUINT ArraySize( T* x_pPtr )
    {
        oexUINT uSize = UsableSize( x_pPtr );
        if ( sizeof( T ) )
            return uSize / sizeof( T );
        else
            return 0;
    }

public:

    /// Construct object
    template< typename T >
        T* Construct()
    {
        return oexPLACEMENT_NEW ( New< T >( 1, oexTRUE ) ) T();
    }

    /// Construct object, 1 param
    template< typename T, typename T_P1 >
        T* Construct( T_P1 p1 )
    {
        return oexPLACEMENT_NEW ( New< T >( 1, oexTRUE ) ) T( p1 );
    }

    /// Construct object, 2 params
    template< typename T, typename T_P1, typename T_P2 >
        T* Construct( T_P1 p1, T_P2 p2 )
    {
        return oexPLACEMENT_NEW ( New< T >( 1, oexTRUE ) ) T( p1, p2 );
    }

    /// Construct object, 3 params
    template< typename T, typename T_P1, typename T_P2, typename T_P3 >
        T* Construct( T_P1 p1, T_P2 p2, T_P3 p3 )
    {
        return oexPLACEMENT_NEW ( New< T >( 1, oexTRUE ) ) T( p1, p2, p3 );
    }

    /// Construct object, 4 params
    template< typename T, typename T_P1, typename T_P2, typename T_P3, typename T_P4 >
        T* Construct( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4 )
    {
        return oexPLACEMENT_NEW ( New< T >( 1, oexTRUE ) ) T( p1, p2, p3, p4 );
    }

    /// Construct object, 5 params
    template< typename T, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5 >
        T* Construct( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5 )
    {
        return oexPLACEMENT_NEW ( New< T >( 1, oexTRUE ) ) T( p1, p2, p3, p4, p5 );
    }

    /// Construct object, 6 params
    template< typename T, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6 >
        T* Construct( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6 )
    {
        return oexPLACEMENT_NEW ( New< T >( 1, oexTRUE ) ) T( p1, p2, p3, p4, p5, p6 );
    }

    /// Construct object, 7 params
    template< typename T, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7 >
        T* Construct( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7 )
    {
        return oexPLACEMENT_NEW ( New< T >( 1, oexTRUE ) ) T( p1, p2, p3, p4, p5, p6, p7 );
    }

    /// Construct object, 8 params
    template< typename T, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_P8 >
        T* Construct( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8 )
    {
        return oexPLACEMENT_NEW ( New< T >( 1, oexTRUE ) ) T( p1, p2, p3, p4, p5, p6, p7, p8 );
    }

public:

    /// Construct object array
    template< typename T >
        T* ConstructArray( oexUINT x_uSize )
    {
        T *p = New< T >( x_uSize, oexTRUE );
        if ( !oexVERIFY( p ) )
            return oexNULL;

        for ( oexUINT i = 0; i < x_uSize; i++ )
            oexPLACEMENT_NEW ( &p[ i ] ) T();

        return p;
    }

    /// Construct object array, 1 param
    template< typename T, typename T_P1 >
        T* ConstructArray( oexUINT x_uSize, T_P1 p1 )
    {
        T *p = New< T >( x_uSize, oexTRUE );
        if ( !oexVERIFY( p ) )
            return oexNULL;

        for ( oexUINT i = 0; i < x_uSize; i++ )
            oexPLACEMENT_NEW ( &p[ i ] ) T( p1 );

        return p;
    }


    /// Construct object array, 2 param
    template< typename T, typename T_P1, typename T_P2 >
        T* ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2 )
    {
        T *p = New< T >( x_uSize, oexTRUE );
        if ( !oexVERIFY( p ) )
            return oexNULL;

        for ( oexUINT i = 0; i < x_uSize; i++ )
            oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2 );

        return p;
    }

    /// Construct object array, 3 param
    template< typename T, typename T_P1, typename T_P2, typename T_P3 >
        T* ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3 )
    {
        T *p = New< T >( x_uSize, oexTRUE );
        if ( !oexVERIFY( p ) )
            return oexNULL;

        for ( oexUINT i = 0; i < x_uSize; i++ )
            oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3 );

        return p;
    }

    /// Construct object array, 4 param
    template< typename T, typename T_P1, typename T_P2, typename T_P3, typename T_P4 >
        T* ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4 )
    {
        T *p = New< T >( x_uSize, oexTRUE );
        if ( !oexVERIFY( p ) )
            return oexNULL;

        for ( oexUINT i = 0; i < x_uSize; i++ )
            oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3, p4 );

        return p;
    }

    /// Construct object array, 5 param
    template< typename T, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5 >
        T* ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5 )
    {
        T *p = New< T >( x_uSize, oexTRUE );
        if ( !oexVERIFY( p ) )
            return oexNULL;

        for ( oexUINT i = 0; i < x_uSize; i++ )
            oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3, p4, p5 );

        return p;
    }

    /// Construct object array, 6 param
    template< typename T, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6 >
        T* ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6 )
    {
        T *p = New< T >( x_uSize, oexTRUE );
        if ( !oexVERIFY( p ) )
            return oexNULL;

        for ( oexUINT i = 0; i < x_uSize; i++ )
            oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3, p4, p5, p6 );

        return p;
    }

    /// Construct object array, 7 param
    template< typename T, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7 >
        T* ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7 )
    {
        T *p = New< T >( x_uSize, oexTRUE );
        if ( !oexVERIFY( p ) )
            return oexNULL;

        for ( oexUINT i = 0; i < x_uSize; i++ )
            oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3, p4, p5, p6, p7 );

        return p;
    }

    /// Construct object array, 8 param
    template< typename T, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_P8 >
        T* ConstructArray( oexUINT x_uSize, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8 )
    {
        T *p = New< T >( x_uSize, oexTRUE );
        if ( !oexVERIFY( p ) )
            return oexNULL;

        for ( oexUINT i = 0; i < x_uSize; i++ )
            oexPLACEMENT_NEW ( &p[ i ] ) T( p1, p2, p3, p4, p5, p6, p7, p8 );

        return p;
    }

public:

    /// Destruct object
    template< typename T >
        void Destruct( T* x_pPtr )
    {
        if ( !x_pPtr )
            return;

        // Set constructed flag
        CAlloc::SetFlags( x_pPtr, CAlloc::eF1Constructed );

        // Free the memory block
        Delete( x_pPtr );
    }

#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )

private:

    /// Holds the line number of the allocating call
    oexUINT         m_uLine;

    /// Holds the file name of the allocating call
    oexCSTR         m_pFile;

public:

    /// Constructor
    CAlloc( oexUINT x_uLine = 0, oexCSTR x_pFile = oexNULL )
    {
        m_uLine = x_uLine;
        m_pFile = x_pFile;
    }

    /// Sets the allocation point for this object
    CAlloc& _Log( oexUINT x_uLine, oexCSTR x_pFile )
    {   m_uLine = x_uLine;
        m_pFile = x_pFile;
        return *this;
    }

#endif

    /// Destructor
    virtual ~CAlloc() {}

};

/// std allocator
/**
	Allocator for use with std templates
*/
template < class T > class COexStdAllocator
{
public:

#if defined( _MSC_VER ) && defined( OEX_WIN64 )
	typedef oex::oexUINT64	size_type;
	typedef oex::oexINT64	difference_type;
#else
	typedef oex::oexULONG	size_type;
	typedef oex::oexLONG	difference_type;
#endif

	typedef T*        		pointer;
	typedef const T*  		const_pointer;
	typedef T&        		reference;
	typedef const T&  		const_reference;
	typedef T         		value_type;

public:

	COexStdAllocator() {}

	COexStdAllocator( const COexStdAllocator& ) {}

	pointer allocate( size_type n, const void * = 0 )
	{	return (T*)CMem::New( n * sizeof( T ), oexLINE, oexTEXT( oexFILE ) ); }

	void deallocate( pointer p, size_type )
	{	if ( p ) CMem::Delete( p ); }

	pointer address( T& r ) const { return &r; }

	const_pointer address( const T& r ) const { return &r; }

	COexStdAllocator& operator =( const COexStdAllocator& ) { return *this; }

	bool operator ==( const COexStdAllocator& ) const { return true; }
	bool operator !=( const COexStdAllocator& ) const { return false; }

	void construct( pointer p, const T& val ) { new ( (pointer)p ) T( val ); }

	void destroy( pointer p ) { p->~T(); }

	size_type max_size() const { return oexMAXLONG / sizeof( T ); }

	template< class U >
		struct rebind { typedef COexStdAllocator< U > other; };

	template < class U >
		COexStdAllocator( const COexStdAllocator< U >& ) {}

	template < class U >
		COexStdAllocator& operator = ( const COexStdAllocator< U >& ) { return *this; }
};



