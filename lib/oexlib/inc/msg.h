/*------------------------------------------------------------------
// msg.h
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

// This stuff is like CORBA in a bottle ;)

#define OexMsgRegisterFunction( p, c, f )       Register( oexT( #f ), oex::TArbDelegate< oex::CAutoStr >( (const c*)p, &c::f ) );
#define OexMsgRegisterThisFunction( c, f )      OexMsgRegisterFunction( this, c, f )

#define oexMsg                                  oex::CMsg::Msg

#pragma pack( push, 1 )

/// Network address structure
class CMsgAddress
{
public:

    /// Default constructor
    CMsgAddress()
    {   os::CSys::Zero( this, sizeof( CMsgAddress ) ); 
    }

    /// Copy constructor
    CMsgAddress( oexCONST CMsgAddress &x_rMa )
    {   os::CSys::MemCpy( this, &x_rMa, sizeof( CMsgAddress ) ); }

    /// Copy operator
    CMsgAddress& operator = ( oexCONST CMsgAddress &x_rMa )
    {   os::CSys::MemCpy( this, &x_rMa, sizeof( CMsgAddress ) ); 
        return *this;
    }

    /// Sets the network address
    void SetNetwork( oexCONST oexGUID *x_pGuid )
    {   if ( !x_pGuid ) guid::ZeroGuid( &guidNetwork );
        else guid::CopyGuid( &guidNetwork, x_pGuid );
    }

    /// Returns the network address
    oexGUID& GetNetwork()
    {   return guidNetwork; }

    /// Sets the process address
    void SetProcess( oexCONST oexGUID *x_pGuid )
    {   if ( !x_pGuid ) guid::ZeroGuid( &guidProcess );
        else guid::CopyGuid( &guidProcess, x_pGuid );
    }

    /// Returns the network address
    oexGUID& GetProcess()
    {   return guidProcess; }

    /// Sets the instance address
    void SetInstance( oexCONST oexGUID *x_pGuid )
    {   if ( !x_pGuid ) guid::ZeroGuid( &guidInstance );
        else guid::CopyGuid( &guidInstance, x_pGuid );
    }

    /// Returns the network address
    oexGUID& GetInstance()
    {   return guidInstance; }

    /// Sets the id
    void SetId( oexCONST oexGUID *x_pGuid )
    {   if ( !x_pGuid ) guid::ZeroGuid( &guidId );
        else guid::CopyGuid( &guidId, x_pGuid );
    }

    /// Returns the id
    oexGUID& GetId()
    {   return guidId; }

    /// Machine address
    oexGUID         guidNetwork;

    /// Instance
    oexGUID         guidProcess;

    /// Destination
    oexGUID         guidInstance;

    /// Destination
    oexGUID         guidId;
};

#pragma pack( pop )

/// Message data
class CMsgInfo
{
public:

#pragma pack( push, 1 )

    /// Memory block information
    struct SBlockHeader
    {
        /// CRC of the data block
        oexUCHAR        ucCrc[ CCrcHash::eHashSize ];

        /// Number of parameters
        oexUINT         uParams;
    };  

    /// Param descriptor
    struct SParamHeader
    {
        /// Param size
        oexUINT         uSize;

        /// Param type
        oexUINT         uType;
    };  

#pragma pack( pop )

public:

    /// Default constructor
    CMsgInfo()
    {
        m_uUsed = 0;
        m_pDst = oexNULL;
        m_pSrc = oexNULL;
        m_pBh = oexNULL;
        m_pPh = oexNULL;
        m_pData = oexNULL;
    }

    /// Destructor
    virtual ~CMsgInfo()
    {}

    /// Destroys the contained message
    void Destroy()
    {
        m_uUsed = 0;
        m_pDst = oexNULL;
        m_pSrc = oexNULL;
        m_pBh = oexNULL;
        m_pPh = oexNULL;
        m_pData = oexNULL;
        m_sBin.Destroy();
    }

    /// Returns message overhead
    oexUINT OverheadSize( oexUINT x_uParams )
    {
        return   
                 sizeof( CMsgAddress )
               + sizeof( CMsgAddress )
               + sizeof( SBlockHeader )
               + sizeof( SParamHeader ) * x_uParams;
    }

    oexUINT OverheadSize()
    {
        if ( !m_pBh )
            return 0;
        return OverheadSize( m_pBh->uParams );
    }

    /// Starts the data packet
    oexBOOL Open( oexUINT x_uParams, oexUINT x_uTotalSize )
    {   
        // Lose old message
        Destroy();

        // Add packet overhead
        x_uTotalSize += OverheadSize( x_uParams ) + sizeof( oexCHARW ) * x_uParams;

        /// Pointer to buffer
        if ( !m_sBin.SetLength( x_uTotalSize ) )
            return oexFALSE;

        // Set pointers
        SetPointers( (oexINT)x_uParams );

        // Destination address
        os::CSys::Zero( m_pDst, sizeof( CMsgAddress ) );

        // Source address
        os::CSys::Zero( m_pSrc, sizeof( CMsgAddress ) );

        // Initialize block header
        os::CSys::Zero( m_pBh, sizeof( SBlockHeader ) );
        m_pBh->uParams = x_uParams;

        // Initialize param info
        os::CSys::Zero( m_pPh, sizeof( SParamHeader ) * x_uParams );

        return oexTRUE;
    }

    /// Add a parameter to the call
    oexBOOL AddParam( oexUINT x_uType, oexCPVOID x_pData, oexUINT x_uSize )
    {
        // Sanity checks
        if ( !x_uType || !oexCHECK_PTR( x_pData ) || !oexCHECK( x_uSize ) )
            return oexFALSE;

        if ( !m_pBh || !m_pPh || !m_pData || !m_sBin.Ptr() )
            return oexFALSE;

        // Verify there is room
        if ( (oexUINT)m_sBin.Length() < x_uSize + m_uUsed + OverheadSize() )
            return oexFALSE;

        // Find param slot
        oexUINT uParam = 0;
        while ( uParam < m_pBh->uParams && m_pPh[ uParam ].uType )
            uParam++;

        // Did we get a slot?
        if ( uParam >= m_pBh->uParams )
            return oexFALSE;

        // Copy the data
        os::CSys::MemCpy( &m_pData[ m_uUsed ], x_pData, x_uSize );
        m_uUsed += x_uSize;

        // Kind of a hack, this NULL terminates everything
        for( oexUINT i = 0; i < sizeof( oexCHARW ); i++ )
            m_pData[ m_uUsed++ ] = 0;

        // Save param info
        m_pPh[ uParam ].uType = x_uType;
        m_pPh[ uParam ].uSize = x_uSize;

        return oexTRUE;
    }

    /// Closes the data packet
    oexBOOL End()
    {
        if ( !m_pBh || !m_pPh || !m_pData || !m_sBin.Ptr() )
            return oexFALSE;

        // How much did we use
        oexUINT uSize = OverheadSize() + m_uUsed;

        // Should match
        oexASSERT( m_sBin.Length() == uSize );
        
        // Ensure correct size
        m_sBin.SetLength( uSize );

        // Update CRC
        CCrcHash::Hash( m_pBh->ucCrc, &m_pData[ sizeof( m_pBh->ucCrc ) ], uSize - sizeof( m_pBh->ucCrc ) );

        return oexTRUE;
    }

    /// Returns parameter information
    oexBOOL GetParamInfo( oexUINT x_uIndex, oexSTR8 *x_pData, oexUINT *x_puSize, oexUINT *x_puType )
    {
        // Sanity checks
        if ( !m_pBh || !m_pPh || !m_pData || !m_sBin.Ptr() || x_uIndex >= m_pBh->uParams )
            return oexFALSE;

        // Does the caller want the size?
        if ( x_puSize )
            *x_puSize = m_pPh[ x_uIndex ].uSize;

        // Does the caller want the type?
        if ( x_puType )
            *x_puType = m_pPh[ x_uIndex ].uType;

        // How about the data?
        if ( x_pData )
        {
            // Get the data pointer
            *x_pData = m_pData;
            for ( oexUINT i = 0; i < x_uIndex; i++ )
                *x_pData += m_pPh[ i ].uSize + sizeof( oexCHARW );

            // Just ensure there really is that much data
            if ( !oexCHECK( (oexUINT)m_sBin.Length() >=   OverheadSize() 
                                                        + ( *x_pData - m_pData ) 
                                                        + m_pPh[ x_uIndex ].uSize  ) )
                return oexFALSE;

        } // end if

        return oexTRUE;
    } 

    /// Sets the pointers
    oexBOOL SetPointers( oexINT x_nParams )
    {
        oexSTR8 pBin = (oexSTR8)m_sBin.Ptr();
        if ( !pBin )
            return oexFALSE;

        // Destination address
        m_pDst = (CMsgAddress*)pBin;
        pBin += sizeof( CMsgAddress );

        // Source address
        m_pSrc = (CMsgAddress*)pBin;
        pBin += sizeof( CMsgAddress );

        // Block header
        m_pBh = (SBlockHeader*)pBin;
        pBin += sizeof( SBlockHeader );

        // Negative means use data from buffer
        if ( 0 > x_nParams )
            x_nParams = m_pBh->uParams;

        // Param info
        m_pPh = (SParamHeader*)pBin;
        pBin += sizeof( SParamHeader ) * x_nParams;

        // Save data pointer
        m_pData = pBin;

        return oexTRUE;

    }

    /// Deserializes buffer
    /**
        Whole separate version because this one shares the memory
    */
    oexBOOL Deserialize( CStr8 &x_sBin )
    {
        // Lose previous message
        Destroy();

        // Get data params
        oexSTR8 pBin = (oexSTR8)x_sBin.Ptr();
        oexUINT uSize = x_sBin.Length();

        // Validate the crc
        oexUCHAR ucCrc[ CCrcHash::eHashSize ];
        CCrcHash::Hash( ucCrc, &pBin[ sizeof( ucCrc ) ], uSize - sizeof( ucCrc ) );
        if ( os::CSys::MemCmp( ucCrc, pBin, sizeof( ucCrc ) ) )
            return oexFALSE;

        /// Set the buffer
        m_sBin.Set( x_sBin );

        if ( !SetPointers( -1 ) )
            return oexFALSE;

        m_uUsed = m_sBin.Length() - OverheadSize();

        return oexTRUE;
    }

    /// Deserialize
    oexBOOL Deserialize( oexCSTR8 x_pBin, oexUINT x_uSize )
    {
        // Lose previous message
        Destroy();

        // Validate the crc
        oexUCHAR ucCrc[ CCrcHash::eHashSize ];
        CCrcHash::Hash( ucCrc, &x_pBin[ sizeof( ucCrc ) ], x_uSize - sizeof( ucCrc ) );
        if ( os::CSys::MemCmp( ucCrc, x_pBin, sizeof( ucCrc ) ) )
            return oexFALSE;

        /// Set the buffer
        m_sBin.Set( x_pBin, x_uSize );

        if ( !SetPointers( -1 ) )
            return oexFALSE;

        m_uUsed = m_sBin.Length() - OverheadSize();

        return oexTRUE;
    }

    /// Returns a reference to the buffer
    CStr8& Serialize()
    {   return m_sBin; }

    /// Return destination address structure
    CMsgAddress* Dst()
    {   return m_pDst; }

    /// Return source address structure
    CMsgAddress* Src()
    {   return m_pSrc; }

 private:

    /// Holds *binary* message data
    CStr8           m_sBin;

    /// Destination address
    CMsgAddress     *m_pDst;

    /// Source address
    CMsgAddress     *m_pSrc;

    /// Block header info
    SBlockHeader    *m_pBh;

    /// Param header info
    SParamHeader    *m_pPh;

    /// Param header info
    oexSTR8         m_pData;

    /// Bytes used
    oexUINT         m_uUsed;
};

class CMsgAdapter
{
public:

    CMsgAdapter()
    {   m_uSize = 0;
        m_uType = 0;
        m_pMem = oexNULL;
    }

    /// Constructor
    CMsgAdapter( oexPVOID x_pMem, oexUINT x_uSize, oexUINT x_uType )
    {   m_pMem = x_pMem;        
        m_uSize = x_uSize;
        m_uType = x_uType;
    }
/*
    /// Copy constructor
    CMsgAdapter( oexCONST CMsgAdapter &x_rMa )
    {   m_pMem = x_rMa.m_pMem;
        m_uSize = x_rMa.m_uSize;
    }

    /// Copy constructor
    CMsgAdapter& operator = ( oexCONST CMsgAdapter &x_rMa )
    {   m_pMem = x_rMa.m_pMem;
        m_uSize = x_rMa.m_uSize;
        return *this;
    }
*/
    /// Sets the memory info
    void Set( oexPVOID x_pMem, oexUINT x_uSize )
    {   m_pMem = x_pMem;
        m_uSize = x_uSize;
    }

public:

    enum
    {
        eTypeInvalid        = 0,

        eTypeBin            = 1,

        eTypeInt8           = 2,

        eTypeUInt8          = 3,

        eTypeInt16          = 4,

        eTypeUInt16         = 5,

        eTypeInt32          = 6,

        eTypeUInt32         = 7,

        eTypeInt64          = 8,

        eTypeUInt64         = 9,

        eTypeFloat          = 10,

        eTypeDouble         = 11,

        eTypeLongDouble     = 12,

        eTypeStrW           = 13,

        eTypeStr8           = 14,

        eTypeStr16          = 15,

        eTypeStr32          = 16,

        eTypeStr64          = 17,

        eTypeGuid           = 18
    };

public:

    // Invalid param type
    template< typename T >
        static oexUINT GetType()
        {   oexASSERT( 0 );
            return eTypeInvalid; 
        }

    template<> static oexUINT GetType< oexCHAR >()
    {   return eTypeInt8; }

    template<> static oexUINT GetType< oexUCHAR >()
    {   return eTypeUInt8; }

    template<> static oexUINT GetType< oexSHORT >()
    {   return eTypeInt16; }

    template<> static oexUINT GetType< oexUSHORT >()
    {   return eTypeUInt16; }

    template<> static oexUINT GetType< oexINT >()
    {   if ( 4 == sizeof( oexINT ) )
            return eTypeInt32; 
        else if ( 8 == sizeof( oexINT ) )
            return eTypeInt64; 
        else if ( 2 == sizeof( oexINT ) )
            return eTypeInt16; 
        else if ( 1 == sizeof( oexINT ) )
            return eTypeInt8; 
    }

    template<> static oexUINT GetType< oexUINT >()
    {   if ( 4 == sizeof( oexUINT ) )
            return eTypeInt32; 
        else if ( 8 == sizeof( oexUINT ) )
            return eTypeInt64; 
        else if ( 2 == sizeof( oexUINT ) )
            return eTypeInt16; 
        else if ( 1 == sizeof( oexUINT ) )
            return eTypeInt8; 
    }

    template<> static oexUINT GetType< oexLONG >()
    {   return eTypeInt32; }

    template<> static oexUINT GetType< oexULONG >()
    {   return eTypeUInt32; }

    template<> static oexUINT GetType< oexFLOAT >()
    {   return eTypeFloat; }

    template<> static oexUINT GetType< oexDOUBLE >()
    {   return eTypeDouble; }

    template<> static oexUINT GetType< CStrW >()
    {   return eTypeStrW; }

    template<> static oexUINT GetType< CStr8 >()
    {   return eTypeStr8; }

    template<> static oexUINT GetType< oexCSTR8 >()
    {   return eTypeStrW; }

    template<> static oexUINT GetType< oexCSTRW >()
    {   return eTypeStr8; }

    template<> static oexUINT GetType< oexGUID >()
    {   return eTypeGuid; }

public:

    template< typename T >
        oexBOOL CastInteger( T *p )
        {
            if ( !m_pMem )
                return oexFALSE;

            if ( eTypeInt8 == m_uType )
            {   if ( !oexCHECK( sizeof( oexCHAR ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexCHAR*)m_pMem;
            } // end if

            else if ( eTypeUInt8 == m_uType )
            {   if ( oexCHECK( sizeof( oexUCHAR ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexUCHAR*)m_pMem;
            } // end if

            else if ( eTypeInt16 == m_uType )
            {   if ( !oexCHECK( sizeof( oexSHORT ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexSHORT*)m_pMem;
            } // end if

            else if ( eTypeUInt16 == m_uType )
            {   if ( oexCHECK( sizeof( oexUSHORT ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexUSHORT*)m_pMem;
            } // end if

            else if ( eTypeInt32 == m_uType )
            {   if ( !oexCHECK( sizeof( oexLONG ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexLONG*)m_pMem;
            } // end if

            else if ( eTypeUInt32 == m_uType )
            {   if ( oexCHECK( sizeof( oexULONG ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexULONG*)m_pMem;
            } // end if

            else if ( eTypeInt64 == m_uType )
            {   if ( !oexCHECK( sizeof( oexLONG ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexLONG*)m_pMem;
            } // end if

            else if ( eTypeUInt64 == m_uType )
            {   if ( oexCHECK( sizeof( oexULONG ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexULONG*)m_pMem;
            } // end if

            else if ( eTypeFloat == m_uType )
            {   if ( oexCHECK( sizeof( oexFLOAT ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexFLOAT*)m_pMem;
            } // end if

            else if ( eTypeDouble == m_uType )
            {   if ( oexCHECK( sizeof( oexDOUBLE ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexDOUBLE*)m_pMem;
            } // end if

            else if ( eTypeDouble == m_uType )
            {   if ( oexCHECK( sizeof( oexDOUBLE ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexDOUBLE*)m_pMem;
            } // end if

            else 
                return oexFALSE;

            return oexTRUE;
        }

    template< typename T >
        oexBOOL CastStrToInteger( T *p )
        {
            if ( !m_pMem )
                return oexFALSE;

            if ( eTypeStrW == m_uType )
                *p = (T)CStrW( (oexSTRW)m_pMem, m_uSize / sizeof( oexCSTRW ) ).ToNum();

            else if ( eTypeStr8 == m_uType )
                *p = (T)CStr8( (oexSTR8)m_pMem, m_uSize / sizeof( oexCSTR8 ) ).ToNum();

            else 
                return oexFALSE;

            return oexTRUE;
        }


    template< typename T >
        oexBOOL CastStrToDouble( T *p )
        {
            if ( !m_pMem )
                return oexFALSE;

            if ( eTypeStrW == m_uType )
                *p = (T)CStrW( (oexSTRW)m_pMem, m_uSize / sizeof( oexCSTRW ) ).ToDouble();

            else if ( eTypeStr8 == m_uType )
                *p = (T)CStr8( (oexSTR8)m_pMem, m_uSize / sizeof( oexCSTR8 ) ).ToDouble();

            else 
                return oexFALSE;

            return oexTRUE;
        }

    oexBOOL operator == ( oexINT v )
    {   return (oexINT)*this == v; }

    operator oexCONST oexINT()
    {   oexINT ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexUINT v )
    {   return (oexUINT)*this == v; }

    operator oexCONST oexUINT()
    {   oexUINT ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexCHAR v )
    {   return (oexCHAR)*this == v; }

    operator oexCONST oexCHAR()
    {   oexCHAR ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexUCHAR v )
    {   return (oexUCHAR)*this == v; }

    operator oexCONST oexUCHAR()
    {   oexUCHAR ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexSHORT v )
    {   return (oexSHORT)*this == v; }

    operator oexCONST oexSHORT()
    {   oexSHORT ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexUSHORT v )
    {   return (oexUSHORT)*this == v; }

    operator oexCONST oexUSHORT()
    {   oexUSHORT ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexLONG v )
    {   return (oexLONG)*this == v; }

    operator oexCONST oexLONG()
    {   oexLONG ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexULONG v )
    {   return (oexULONG)*this == v; }

    operator oexCONST oexULONG()
    {   oexULONG ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexINT64 v )
    {   return (oexINT64)*this == v; }

    operator oexCONST oexINT64()
    {   oexINT64 ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexUINT64 v )
    {   return (oexUINT64)*this == v; }

    operator oexCONST oexUINT64()
    {   oexUINT64 ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexFLOAT v )
    {   return (oexFLOAT)*this == v; }

    operator oexCONST oexFLOAT()
    {   oexFLOAT ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToDouble( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexDOUBLE v )
    {   return (oexDOUBLE)*this == v; }

    operator oexCONST oexDOUBLE()
    {   oexDOUBLE ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToDouble( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexCSTR8 v )
    {   return !str::Compare( (oexCSTR8)*this, m_uSize / sizeof( oexCSTR8 ), v, zstr::Length( v ) ); }

    operator oexCSTR8()
    {
        // Types must match here
        if ( GetType< oexCSTR8 >() != m_uType )
            return oexNULL;

        return (oexCSTR8)m_pMem;        
    }

    oexBOOL operator == ( oexCSTRW v )
    {   return !str::Compare( (oexCSTRW)*this, m_uSize / sizeof( oexCSTRW ), v, zstr::Length( v ) ); }

    operator oexCSTRW()
    {
        // Types must match here
        if ( GetType< oexCSTRW >() != m_uType )
            return oexNULL;

        return (oexCSTRW)m_pMem;        
    }

private:
    
    /// Pointer to memory
    oexPVOID        m_pMem;

    /// Size of memory at m_pMem
    oexUINT         m_uSize;

    /// Type
    oexUINT         m_uType;
};


/// Message container
class CMsg
{
public:

    /// Default constructor
    CMsg()
    {
        m_cMi.OexConstruct();
    }

    /// Copy constructor
    CMsg( oexCONST CMsg &x_rMsg )
    {
        m_cMi.Share( x_rMsg.m_cMi );
    }

    CMsg( oexSTR8 x_pBin, oexUINT x_uSize )
    {
        m_cMi.OexConstruct();
        Mi().Deserialize( x_pBin, x_uSize );
    }

    CMsg( CStr8 &x_sStr )
    {
        m_cMi.OexConstruct();
        Mi().Deserialize( x_sStr );
    }

    /// Destructor
    virtual ~CMsg()
    {}

    /// Copy operator
    CMsg& operator = ( oexCONST CMsg &x_rMsg )
    {   m_cMi.Share( x_rMsg.m_cMi );
        return *this;
    }

    /// Returns non-zero if there is valid memory
    oexBOOL IsValid()
    {   return m_cMi.Ptr() ? oexTRUE : oexFALSE; }

    /// Returns a serialized version of the data
    CStr8& Serialize()
    {
        // Return the serialized packet
        return m_cMi.Ptr()->Serialize();
    }

    /// Returns a reference to the message info structure
    CMsgInfo& Mi()
    {   return *m_cMi.Ptr(); }

public:

    /// Creates a message object
    static CMsg Msg( CMsgAddress *x_pDst, oexCSTR x_pF )
    {
        CMsg msg;

        // Start packet
        if ( !msg.Mi().Open( 0, 0 ) )
            return msg;

        // Set destination address
        if ( x_pDst )
            *msg.Mi().Dst() = *x_pDst;

        // Set the function id if needed
        if ( x_pF && *x_pF )
            oss::CMd5::Transform( &msg.Mi().Dst()->guidId, x_pF, zstr::Length( x_pF ) * sizeof( oexTCHAR ) );

        msg.Mi().End();

        // Return the message object
        return msg;
    }

    /// Creates a message object
    template< typename T_P1 >
        static CMsg Msg( CMsgAddress *x_pDst, oexCSTR x_pF, T_P1 p1 )
    {
        CMsg msg;

        // Start packet
        if ( !msg.Mi().Open( 1,   obj::Size( &p1 ) ) )
            return msg;

        // Set destination address
        if ( x_pDst )
            *msg.Mi().Dst() = *x_pDst;

        // Set the function id if needed
        if ( x_pF && *x_pF )
            oss::CMd5::Transform( &msg.Mi().Dst()->guidId, x_pF, zstr::Length( x_pF ) * sizeof( oexTCHAR ) );

        // Add params
        if ( !oexCHECK( msg.Mi().AddParam( CMsgAdapter::GetType< T_P1 >(), obj::Ptr( &p1 ), obj::Size( &p1 ) ) ) )
            return CMsg();

        msg.Mi().End();

        // Return the message object
        return msg;
    }

    /// Creates a message object
    template< typename T_P1, typename T_P2 >
        static CMsg Msg( CMsgAddress *x_pDst, oexCSTR x_pF, T_P1 p1, T_P2 p2 )
    {
        CMsg msg;

        // Start packet
        if ( !msg.Mi().Open( 2,   obj::Size( &p1 )
                                + obj::Size( &p2 ) ) )
            return msg;

        // Set destination address
        if ( x_pDst )
            *msg.Mi().Dst() = *x_pDst;

        // Set the function id if needed
        if ( x_pF && *x_pF )
            oss::CMd5::Transform( &msg.Mi().Dst()->guidId, x_pF, zstr::Length( x_pF ) * sizeof( oexTCHAR ) );

        // Add params
        if ( !oexCHECK( msg.Mi().AddParam( CMsgAdapter::GetType< T_P1 >(), obj::Ptr( &p1 ), obj::Size( &p1 ) ) ) )
            return CMsg();

        if ( !oexCHECK( msg.Mi().AddParam( CMsgAdapter::GetType< T_P2 >(), obj::Ptr( &p2 ), obj::Size( &p2 ) ) ) )
            return CMsg();

        msg.Mi().End();

        // Return the message object
        return msg;
    }

public:

    template< typename T >
        oexBOOL VerifyParam( oexUINT x_uIndex )
    {
        if ( !IsValid() )
            return oexFALSE;

        oexUINT uData = 0;
        oexUINT uType = 0;
        if ( !Mi().GetParamInfo( x_uIndex, oexNULL, &uData, &uType ) )
            return oexFALSE;

        // Verify the param
//        oexASSERT( GetType< T >() == uType && obj::Size( (T*)0 ) == uData );

        return oexTRUE;
    }

public:

    CMsgAdapter operator [] ( oexUINT x_uIndex )
    {
        if ( !IsValid() )
            return CMsgAdapter();

        oexUINT uData = 0;
        oexUINT uType = 0;
        oexSTR8 pData = oexNULL;
        if ( !Mi().GetParamInfo( x_uIndex, &pData, &uData, &uType ) )
            return CMsgAdapter();

        // Return data wrapper
        return CMsgAdapter( pData, uData, uType );
    }

private:

    /// Message payload
    TMem< CMsgInfo >        m_cMi;
};


class CMsgTarget
{
public:

    /// Constructor
    CMsgTarget()
    {   m_uParams = 0;
        m_pClass = m_pThunk = 0;        
        *(__int64*)m_rawFunction = 0;
        *(__int64*)&m_rawFunction[ 8 ] = 0;
    }

    /// Copy constructor
    CMsgTarget( CMsgTarget &x_rD )
    {   m_uParams = x_rD.m_uParams;
        m_pClass = x_rD.m_pClass;
        m_pThunk = x_rD.m_pThunk;
        *(__int64*)m_rawFunction = *(__int64*)x_rD.m_rawFunction;
        *(__int64*)&m_rawFunction[ 8 ] = *(__int64*)&x_rD.m_rawFunction[ 8 ];
    }

    /// Copy constructor
    CMsgTarget& operator = ( const CMsgTarget &x_rD )
    {   m_uParams = x_rD.m_uParams;
        m_pClass = x_rD.m_pClass;
        m_pThunk = x_rD.m_pThunk;
        *(__int64*)m_rawFunction = *(__int64*)x_rD.m_rawFunction;
        *(__int64*)&m_rawFunction[ 8 ] = *(__int64*)&x_rD.m_rawFunction[ 8 ];
        return *this;
    }

public:                 

    template < typename T_RET, typename T_P1, typename T_P2, typename T_CLASS >
        CMsgTarget( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2 ) )
    {
        _Register< T_RET, T_P1, T_P2 >( x_pC, x_pF ); 
    }

    template < typename T_RET, typename T_P1, typename T_P2, typename T_CLASS >
        void Register( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2 ) )
    {   
        _Register< T_RET, T_P1, T_P2 >( x_pC, x_pF ); 
    }

    CMsg operator ()( CMsg &x_params )
    {
        typedef CMsg (*t_Thunk)( const void*, void*, CMsg& );
        return ((t_Thunk)m_pThunk)( m_pClass, &m_rawFunction, x_params ); 
    }
private:

    template < typename T_RET, typename T_P1, typename T_P2, typename T_CLASS, typename T_FUNCTION >
        void _Register( const T_CLASS *x_pC, const T_FUNCTION x_pF )
    {   
        m_uParams = 2;
        m_pClass = x_pC;
        *(T_FUNCTION*)(void*)m_rawFunction = x_pF;
        typedef CMsg (*t_Thunk)( const void*, void*, T_P1, T_P2 );
        m_pThunk = (t_Thunk)&SThunk_2< T_RET >::Thunk< T_RET, T_P1, T_P2, T_CLASS, T_FUNCTION >;
    }

    template< typename T_RET > struct SThunk_2
    {   template < typename T_RET, typename T_P1, typename T_P2, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( const void* x_pC, void* x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                 || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) )
                 || !oexCHECK( x_params.VerifyParam< T_P2 >( 1 ) ) )
                return CMsg();

            return oexMsg( x_params.Mi().Src(), oexNULL,
                           ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                                ( x_params[ 0 ], x_params[ 1 ] ) ); 
        }
    };

    template <> struct SThunk_2< void >
    {   template < typename T_RET, typename T_P1, typename T_P2, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( const void* x_pC, void* x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                 || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) )
                 || !oexCHECK( x_params.VerifyParam< T_P2 >( 1 ) ) )
                return CMsg();

            ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )( x_params[ 0 ] ); 

            return CMsg( x_params.Mi().Src() ); 
        }
    };

private:

    /// Class pointer
    const void*             m_pClass;

    /// Function pointer buffer
    char                    m_rawFunction[ 16 ];

    /// Thunk
    void*                   m_pThunk;

    /// Number of params
    unsigned int            m_uParams; 
};


/// Manages the message network
class CMsgMgr
{
public:

    /// Default constructor
    CMsgMgr()
    {}

    /// Destructor
    virtual ~CMsgMgr()
    {}

private:

};



