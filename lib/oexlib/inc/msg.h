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

/// Creates a guid by hashing the parameter
#define oexCreateMsgId( s, g )                  oex::oss::CMd5::Transform( g, oex::obj::Ptr( s ), oex::obj::Size( s ) )
#define oexCreateTempMsgId( s )                 oex::oss::CMd5::Transform( &oex::oexGUID(), oex::obj::Ptr( s ), oex::obj::Size( s ) )

/// Registers a class pointer and function 
#define oexMsgRegisterFunction( p, c, f )       oexRegister( *oexCreateTempMsgId( oexT( #f ) ), \
                                                             oex::CMsgTarget( (const c*)p, &c::f ) );

/// Registers a function using the 'this' pointer
#define oexMsgRegisterThisFunction( c, f )      oexMsgRegisterFunction( this, c, f )

/// Use to create a CMsg object
#define oexMsg                                  oex::CMsg::Msg

/// Creates a CMsgAddress address object
#define oexTo                                   &oex::CMsgAddress

/// Routes a message
#define oexNet                                 oex::CMsgMgr::Mm()

/// Message priority mask
#define oexMsgPriorityMask                      0x0000ffff

/// Set the flag to indicate a reply is not required
#define oexMsgReply                             0x80000000
#define oexMsgNoReply                           0x40000000

#pragma pack( push, 1 )

/// Network address structure
class CMsgAddress
{
public:

    /// Default constructor
    CMsgAddress()
    {   os::CSys::Zero( this, sizeof( CMsgAddress ) ); 
    }

    /// Copy operator
    CMsgAddress( oexCONST oexGUID *x_pguidId, 
                 oexCONST oexGUID *x_pguidInstance = oexNULL, 
                 oexCONST oexGUID *x_pguidProcess = oexNULL, 
                 oexCONST oexGUID *x_pguidNetwork = oexNULL )
    {   
        if ( x_pguidId )
            guid::CopyGuid( &guidId, x_pguidId ); 
        else
            guid::ZeroGuid( &guidId ); 
        
        if ( x_pguidInstance )
            guid::CopyGuid( &guidInstance, x_pguidInstance ); 
        else
            guid::ZeroGuid( &guidInstance ); 
        
        if ( x_pguidProcess )
            guid::CopyGuid( &guidProcess, x_pguidProcess ); 
        else
            guid::ZeroGuid( &guidProcess ); 

        if ( x_pguidNetwork )
            guid::CopyGuid( &guidNetwork, x_pguidNetwork ); 
        else
            guid::ZeroGuid( &guidNetwork ); 
    }

    /// Copy operator
    CMsgAddress( oexCSTRW x_pId, 
                 oexCONST oexGUID *x_pguidInstance = oexNULL, 
                 oexCONST oexGUID *x_pguidProcess = oexNULL, 
                 oexCONST oexGUID *x_pguidNetwork = oexNULL )
    {   
        if ( x_pId )
            guid::CopyGuid( &guidId, oexCreateTempMsgId( x_pId ) ); 
        else
            guid::ZeroGuid( &guidId ); 
        
        if ( x_pguidInstance )
            guid::CopyGuid( &guidInstance, x_pguidInstance ); 
        else
            guid::ZeroGuid( &guidInstance ); 
        
        if ( x_pguidProcess )
            guid::CopyGuid( &guidProcess, x_pguidProcess ); 
        else
            guid::ZeroGuid( &guidProcess ); 

        if ( x_pguidNetwork )
            guid::CopyGuid( &guidNetwork, x_pguidNetwork ); 
        else
            guid::ZeroGuid( &guidNetwork ); 
    }

    /// Copy operator
    CMsgAddress( oexCSTR8 x_pId, 
                 oexCONST oexGUID *x_pguidInstance = oexNULL, 
                 oexCONST oexGUID *x_pguidProcess = oexNULL, 
                 oexCONST oexGUID *x_pguidNetwork = oexNULL )
    {   
        if ( x_pId )
            guid::CopyGuid( &guidId, oexCreateTempMsgId( CStrW().Cnv( x_pId ).Ptr() ) ); 
        else
            guid::ZeroGuid( &guidId ); 
        
        if ( x_pguidInstance )
            guid::CopyGuid( &guidInstance, x_pguidInstance ); 
        else
            guid::ZeroGuid( &guidInstance ); 
        
        if ( x_pguidProcess )
            guid::CopyGuid( &guidProcess, x_pguidProcess ); 
        else
            guid::ZeroGuid( &guidProcess ); 

        if ( x_pguidNetwork )
            guid::CopyGuid( &guidNetwork, x_pguidNetwork ); 
        else
            guid::ZeroGuid( &guidNetwork ); 
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

        /// Priority
        oexUINT         uFlags;

        /// Desired execute time, GMT Unix timestamp, 0 for immediate
        oexUINT         uTime;
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
    CMsgInfo() : m_evReady( oexTRUE )
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
    oexBOOL Open( oexUINT x_uFlags, oexUINT x_uParams, oexUINT x_uTotalSize )
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
        m_pBh->uFlags = x_uFlags;

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
        CCrcHash::Hash( m_pBh->ucCrc, m_pData, m_uUsed );

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

    oexBOOL VerifyPacket()
    {
        // Ensure length is correct
        if ( m_sBin.Length() != OverheadSize() + m_uUsed )
            return oexFALSE;

        // Validate the crc
        oexUCHAR ucCrc[ CCrcHash::eHashSize ];
        CCrcHash::Hash( ucCrc, m_pData, m_uUsed );
        if ( os::CSys::MemCmp( ucCrc, m_pBh->ucCrc, sizeof( ucCrc ) ) )
            return oexFALSE;

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

        /// Set the buffer
        m_sBin.Set( x_sBin );

        if ( !SetPointers( -1 ) )
            return oexFALSE;

        m_uUsed = m_sBin.Length() - OverheadSize();

        if ( !VerifyPacket() )
        {   Destroy();
            return oexFALSE;
        } // end if

        return oexTRUE;
    }

    /// Deserialize
    oexBOOL Deserialize( oexCSTR8 x_pBin, oexUINT x_uSize )
    {
        // Lose previous message
        Destroy();

        /// Set the buffer
        m_sBin.Set( x_pBin, x_uSize );

        if ( !SetPointers( -1 ) )
            return oexFALSE;

        m_uUsed = m_sBin.Length() - OverheadSize();

        if ( !VerifyPacket() )
        {   Destroy();
            return oexFALSE;
        } // end if

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

    /// Returns the message priority level
    oexUINT GetPriority()
    {   if ( !m_pBh )
            return 0;
        return m_pBh->uFlags & oexMsgPriorityMask;
    }

    /// Returns the message priority level
    oexUINT GetTime()
    {   if ( !m_pBh )
            return 0;
        return m_pBh->uTime;
    }

    /// Returns the number of parameters
    oexUINT GetNumParams()
    {   if ( !m_pBh )
            return 0;
        return m_pBh->uParams;
    }

    /// Set to enable reply
    oexBOOL EnableReplyEvent( oexBOOL x_bEnable )
    {
        if ( x_bEnable )
            m_evReady.Create();
        else
            m_evReady.Destroy();

        return oexTRUE;
    }

    /// Signal reply is ready
    oexBOOL WantReply()
    {
        if ( !m_pBh )
            return oexFALSE;

        return !( m_pBh->uFlags & ( oexMsgReply | oexMsgNoReply ) );
    }

    oexBOOL SetReadyEvent( oexBOOL x_bSet )
    {
        if ( x_bSet )
            m_evReady.Set();
        else
            m_evReady.Reset();

        return oexTRUE;
    }

    oexBOOL WaitReady( oexUINT x_uTimeout )
    {
        return m_evReady.Wait( x_uTimeout );
    }

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

    /// Event to be signaled when the message is ready
    CTlEvent        m_evReady;
};

class CMsg;
class CMsgTypeAdapter
{
public:

    CMsgTypeAdapter()
    {   m_uSize = 0;
        m_uType = 0;
        m_pMem = oexNULL;
        m_pMsg = oexNULL;
    }

    /// Constructor
    CMsgTypeAdapter( CMsg *x_pMsg, oexPVOID x_pMem, oexUINT x_uSize, oexUINT x_uType )
    {   m_pMem = x_pMem;        
        m_uSize = x_uSize;
        m_uType = x_uType;
        m_pMsg = oexNULL;
    }

    /// Copy 
    CMsgTypeAdapter& Copy( oexCONST CMsgTypeAdapter &x_rMa )
    {   m_pMem = x_rMa.m_pMem;
        m_uSize = x_rMa.m_uSize;
        m_uType = x_rMa.m_uType;
        m_pMsg = x_rMa.m_pMsg;
        return *this;
    }

    /// Sets the memory info
    void Set( CMsg *x_pMsg, oexPVOID x_pMem, oexUINT x_uSize, oexUINT x_uType )
    {   m_pMem = x_pMem;
        m_uSize = x_uSize;
        m_uType = x_uType;
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
    {   return eTypeStr8; }

    template<> static oexUINT GetType< oexCSTRW >()
    {   return eTypeStrW; }

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

    template< typename T >
        oexBOOL CastNumberToString( T *p, oexUINT sz )
        {
            if ( !m_pMem )
                return oexFALSE;

            if ( eTypeInt8 == m_uType )
            {   if ( !oexCHECK( sizeof( oexCHAR ) == m_uSize ) )
                    return oexFALSE;
                os::CSys::StrFmt( p, sz, oexTT( T, "%li" ), (oexLONG)*(oexCHAR*)m_pMem );
            } // end if

            else if ( eTypeUInt8 == m_uType )
            {   if ( !oexCHECK( sizeof( oexUCHAR ) == m_uSize ) )
                    return oexFALSE;
                os::CSys::StrFmt( p, sz, oexTT( T, "%lu" ), (oexULONG)*(oexUCHAR*)m_pMem );
            } // end if

            else if ( eTypeInt16 == m_uType )
            {   if ( !oexCHECK( sizeof( oexSHORT ) == m_uSize ) )
                    return oexFALSE;
                os::CSys::StrFmt( p, sz, oexTT( T, "%li" ), (oexLONG)*(oexSHORT*)m_pMem );
            } // end if

            else if ( eTypeUInt16 == m_uType )
            {   if ( !oexCHECK( sizeof( oexUSHORT ) == m_uSize ) )
                    return oexFALSE;
                os::CSys::StrFmt( p, sz, oexTT( T, "%lu" ), (oexULONG)*(oexUSHORT*)m_pMem );
            } // end if

            else if ( eTypeInt32 == m_uType )
            {   if ( !oexCHECK( sizeof( oexLONG ) == m_uSize ) )
                    return oexFALSE;
                os::CSys::StrFmt( p, sz, oexTT( T, "%li" ), (oexLONG)*(oexLONG*)m_pMem );
            } // end if

            else if ( eTypeUInt32 == m_uType )
            {   if ( !oexCHECK( sizeof( oexULONG ) == m_uSize ) )
                    return oexFALSE;
                os::CSys::StrFmt( p, sz, oexTT( T, "%lu" ), (oexULONG)*(oexULONG*)m_pMem );
            } // end if

            else if ( eTypeFloat == m_uType )
            {   if ( !oexCHECK( sizeof( oexFLOAT ) == m_uSize ) )
                    return oexFALSE;
                os::CSys::StrFmt( p, sz, oexTT( T, "%f" ), (oexDOUBLE)*(oexFLOAT*)m_pMem );
            } // end if

            else if ( eTypeDouble == m_uType )
            {   if ( !oexCHECK( sizeof( oexDOUBLE ) == m_uSize ) )
                    return oexFALSE;
                os::CSys::StrFmt( p, sz, oexTT( T, "%f" ), (oexDOUBLE)*(oexDOUBLE*)m_pMem );
            } // end if

            else
                return oexFALSE;

            return oexTRUE;
        }

    oexBOOL operator == ( oexCSTR8 v )
    {   return !str::Compare( (oexCSTR8)*this, m_uSize / sizeof( oexCSTR8 ), v, zstr::Length( v ) ); }

    operator oexCSTR8()
    {
        // Types must match here
        if ( GetType< oexCSTR8 >() == m_uType )
            return (oexCSTR8)m_pMem;        

        // Pointer to buffer
        oexSTR8 pStr = m_ucNumBuf;
        *pStr = 0;

        CastNumberToString( pStr, sizeof( m_ucNumBuf ) / sizeof( pStr[ 0 ] ) );

        return pStr;
    }

    oexBOOL operator == ( oexCSTRW v )
    {   return !str::Compare( (oexCSTRW)*this, m_uSize / sizeof( oexCHARW ), v, zstr::Length( v ) ); }

    operator oexCSTRW()
    {
        // Types must match here
        if ( GetType< oexCSTRW >() == m_uType )
            return (oexCSTRW)m_pMem;

        // Pointer to buffer
        oexSTRW pStr = (oexSTRW)m_ucNumBuf;
        *pStr = 0;

        CastNumberToString( pStr, sizeof( m_ucNumBuf ) / sizeof( pStr[ 0 ] ) );

        return pStr;
    }

    operator CMsg* ()
    {   return m_pMsg; }

private:
    
    /// Pointer to memory
    oexPVOID        m_pMem;

    /// Size of memory at m_pMem
    oexUINT         m_uSize;

    /// Type
    oexUINT         m_uType;

    /// Pointer to message object
    CMsg            *m_pMsg;

    /// Buffer, handles conversions, sharing, etc...
//    CStr8           m_sBin;

    /// Buffer for holding number conversions
    oexCHAR8        m_ucNumBuf[ 256 ];
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
    static CMsg Msg( oexUINT x_uFlags, CMsgAddress *x_pDst )
    {
        CMsg msg;

        // Start packet
        if ( !msg.Mi().Open( x_uFlags, 0, 0 ) )
            return msg;

        // Set destination address
        if ( x_pDst )
            *msg.Mi().Dst() = *x_pDst;

        msg.Mi().End();

        // Return the message object
        return msg;
    }

    /// Creates a message object
    template< typename T_P1 >
        static CMsg Msg( oexUINT x_uFlags, CMsgAddress *x_pDst, T_P1 p1 )
    {
        CMsg msg;

        // Start packet
        if ( !msg.Mi().Open( x_uFlags, 1, obj::Size( &p1 ) ) )
            return msg;

        // Set destination address
        if ( x_pDst )
            *msg.Mi().Dst() = *x_pDst;

        // Add params
        if ( !oexCHECK( msg.Mi().AddParam( CMsgTypeAdapter::GetType< T_P1 >(), obj::Ptr( &p1 ), obj::Size( &p1 ) ) ) )
            return CMsg();

        msg.Mi().End();

        // Return the message object
        return msg;
    }

    /// Creates a message object
    template< typename T_P1, typename T_P2 >
        static CMsg Msg( oexUINT x_uFlags, CMsgAddress *x_pDst, T_P1 p1, T_P2 p2 )
    {
        CMsg msg;

        // Start packet
        if ( !msg.Mi().Open( x_uFlags, 2,   obj::Size( &p1 )
                                             + obj::Size( &p2 ) ) )
            return msg;

        // Set destination address
        if ( x_pDst )
            *msg.Mi().Dst() = *x_pDst;

        // Add params
        if ( !oexCHECK( msg.Mi().AddParam( CMsgTypeAdapter::GetType< T_P1 >(), obj::Ptr( &p1 ), obj::Size( &p1 ) ) ) )
            return CMsg();

        if ( !oexCHECK( msg.Mi().AddParam( CMsgTypeAdapter::GetType< T_P2 >(), obj::Ptr( &p2 ), obj::Size( &p2 ) ) ) )
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

        // Verify the param type  iii commented out because we're converting on the fly
//        oexASSERT( GetType< T >() == uType && obj::Size( (T*)0 ) == uData );

        return oexTRUE;
    }

public:

    CMsgTypeAdapter operator [] ( oexUINT x_uIndex )
    {
        if ( !IsValid() )
            return CMsgTypeAdapter();

        oexUINT uData = 0;
        oexUINT uType = 0;
        oexSTR8 pData = oexNULL;
        if ( !Mi().GetParamInfo( x_uIndex, &pData, &uData, &uType ) )
            return CMsgTypeAdapter();

        // Return data wrapper
        return CMsgTypeAdapter( this, pData, uData, uType );
    }

    oexUINT GetPriority()
    {
        if ( !IsValid() )
            return 0;

        return m_cMi.Ptr()->GetPriority();
    }

    oexUINT GetTime()
    {
        if ( !IsValid() )
            return 0;

        return m_cMi.Ptr()->GetTime();
    }

    oexBOOL EnableReplyEvent( oexBOOL x_bEnable )
    {
        if ( !IsValid() )
            return oexFALSE;

        return m_cMi.Ptr()->EnableReplyEvent( x_bEnable );
    }

    oexBOOL WantReply()
    {
        if ( !IsValid() )
            return oexFALSE;

        return m_cMi.Ptr()->WantReply();
    }

    oexBOOL SetReadyEvent( oexBOOL x_bSet )
    {
        if ( !IsValid() )
            return oexFALSE;

        return m_cMi.Ptr()->SetReadyEvent( x_bSet );
    }

    oexBOOL WaitReady( oexUINT x_uTimeout )
    {
        if ( !IsValid() )
            return oexFALSE;

        return m_cMi.Ptr()->WaitReady( x_uTimeout );
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

            return oexMsg( x_params.GetPriority() | oexMsgReply, x_params.Mi().Src(),
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

            return CMsg( x_params.GetPriority() | oexMsgReply, 
                         x_params.Mi().Src() ); 
        }
    };

public:

    /// Returns non zero if there is a target set
    oexBOOL IsValid()
    {   return m_pClass ? oexTRUE : oexFALSE; }

    /// Returns the number of parameters
    oexUINT GetNumParams()
    {   return m_uParams; }

private:

    /// Class pointer
    const void              *m_pClass;

    /// Function pointer buffer
    char                    m_rawFunction[ 16 ];

    /// Thunk
    void                    *m_pThunk;

    /// Number of params
    unsigned int            m_uParams; 
};


/// Message communication port
class CMsgCom
{
public:

    /// Event handler list type
    typedef TAssoList< oexGUID, CMsgTarget >    t_MsgHandlerList;

    /// Message queue
    typedef TList< CMsg >                       t_MsgQueue;

public:

    /// Default constructor
    CMsgCom();

    /// Destructor
    virtual ~CMsgCom();

    /// Returns the object id
    oexGUID& oexMsgId()
    {   return _m_guidId; }

    /// Registers a callback function
    oexBOOL oexRegister( oexGUID &x_rGuid, oexCONST CMsgTarget &x_adFunction );

    /// Registers a communication port
    oexBOOL oexUnregister( oexGUID &x_rGuid );

    /// Routes the message to a remote or local destination
    CMsg Send( CMsg &x_rMsg );

    /// Routes the message to a local destination
    oexBOOL Recv( CMsg &x_rMsg );

    /// Processes the function queue
    /**
        \param [in] uMax    -   Maximum number of commands to execute,
                                zero for all.

        \return Number of commands executed.
    */
    oexUINT ProcessQueue( oexINT nMax = 0 );

    CMsg Execute( CMsg &x_rMsg );

    /// Wait for message
    oexBOOL WaitMsg( oexUINT x_uTimeout )
    {   return _m_evMsgWaiting.Wait( x_uTimeout ); }

private:

    /// Thread lock
    CTlLock                     _m_lockMsgHandlers;

    /// Thread lock
    CTlLock                     _m_lockMsgQueue;

    /// Our address
    oexGUID                     _m_guidId;

    /// List of event handlers
    t_MsgHandlerList            _m_lstMsgHandlers;

    /// Message queue
    t_MsgQueue                  _m_lstMsgQueue;

    /// Message queue status event
    CTlEvent                    _m_evMsgWaiting;
};


/// Manages the message network
class CMsgMgr
{

public:

    /// Event handler list type
    typedef TAssoList< oexGUID, CMsgCom* > t_MsgComList;

    /// Event handler list type
    typedef TAssoList< oexGUID, CMsg >     t_MsgReplyList;

public:

    /// Default constructor
    CMsgMgr()
    {
        os::CSys::CreateGuid( &m_guidId );
    }

    /// Destructor
    virtual ~CMsgMgr()
    {        
        Destroy();
    }

    oexBOOL Destroy()
    {
        // Lock the handler list
        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Lose the lists
        m_lstMsgCom.Destroy();
        m_lstReply.Destroy();

        return oexTRUE;
    }

    /// Returns the object id
    oexGUID& GetId()
    {   return m_guidId; }

    /// Registers a communication port
    oexBOOL Register( CMsgCom *x_pMc )
    {
        if ( !oexCHECK_PTR( x_pMc ) )
            return oexFALSE;

        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Save address
        m_lstMsgCom[ x_pMc->oexMsgId() ] = x_pMc;

        return oexTRUE;
    }

    /// Registers a communication port
    oexBOOL Unregister( CMsgCom *x_pMc )
    {
        if ( !oexCHECK_PTR( x_pMc ) )
            return oexFALSE;

        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Erase address
        m_lstMsgCom.Unset( x_pMc->oexMsgId() );

        return oexTRUE;
    }

    /// Registers a communication port
    oexBOOL Unregister( oexGUID &x_rGuid )
    {
        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Erase address
        m_lstMsgCom.Unset( x_rGuid );

        return oexTRUE;
    }

    /// Routes the message to a remote or local destination
    CMsg Send( CMsg &x_rMsg )
    {
        // Sanity check
        if ( !x_rMsg.IsValid() )
            return CMsg();

        // Can't be routed without an id
        if ( guid::CmpGuid( &IID_ZEROS, &x_rMsg.Mi().Dst()->GetId() ) )
            return CMsg();

        // Can't route without an instance id
        if ( guid::CmpGuid( &IID_ZEROS, &x_rMsg.Mi().Dst()->GetInstance() ) )
            return CMsg();

        // Register reply object if needed
        if ( x_rMsg.WantReply() )
            RegisterReply( x_rMsg );

        // Bound for somewhere else?
        if ( !guid::CmpGuid( &IID_ZEROS, &x_rMsg.Mi().Dst()->GetProcess() )
             && !guid::CmpGuid( &m_guidId, &x_rMsg.Mi().Dst()->GetProcess() ) )
        {
            // Attempt to route
            if ( !OnRouteProcess( x_rMsg ) )
            {   CancelReply( x_rMsg.Mi().Src()->GetId() );
                return CMsg();
            } // end if

        } // end if

        // Route back down
        else if ( !Recv( x_rMsg ) )
        {   CancelReply( x_rMsg.Mi().Src()->GetId() );
            return CMsg();
        } // end if

        return x_rMsg;
    }

    /// Routes the message to a local destination
    oexBOOL Recv( CMsg &x_rMsg )
    {
        // Sanity check
        if ( !x_rMsg.IsValid() )
            return oexFALSE;

        // Can't route without an instance id
        if ( guid::CmpGuid( &IID_ZEROS, &x_rMsg.Mi().Dst()->GetInstance() ) )
            return oexFALSE;

        // Lock the handler list
        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // First, check to see if it's a reply
        if ( OnReply( x_rMsg ) )
            return oexTRUE;

        // Find a message reciever by that id
        t_MsgComList::iterator it = m_lstMsgCom.Find( x_rMsg.Mi().Dst()->GetInstance() );
        if ( !it.IsValid() )
            return oexFALSE;

        // See if we have a destination for this address
        CMsgCom *pMc = it.Obj();            
        if ( !oexCHECK_PTR( pMc ) )
            return oexFALSE;

        // Hand down
        return pMc->Recv( x_rMsg );
    }

    /// Route to other process
    virtual oexBOOL OnRouteProcess( CMsg &x_rMsg )
    {
        // Is this packet bound for another machine?
        if ( !guid::CmpGuid( &IID_ZEROS, &x_rMsg.Mi().Dst()->GetNetwork() ) )
            return OnRouteNetwork( x_rMsg );

        return oexFALSE;
    }

    /// Route to other network
    virtual oexBOOL OnRouteNetwork( CMsg &x_rMsg )
    {

        return oexFALSE;
    }

    /// Route to other network
    virtual oexBOOL OnReply( CMsg &x_rMsg )
    {
        // Lock the handler list
        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Is it a reply?
        t_MsgReplyList::iterator itReply = m_lstReply.Find( x_rMsg.Mi().Dst()->GetId() );
        if ( !itReply.IsValid() )
            return oexFALSE;

        // +++ Copy the data
        itReply.Obj().Mi().Deserialize( x_rMsg.Mi().Serialize() );

        // Signal that a reply was received
        itReply.Obj().SetReadyEvent( oexTRUE );

        // Remove the reply object
        m_lstReply.Erase( itReply );

        return oexTRUE;
    }

    /// Registers the object to be signaled when a reply is recieved
    oexBOOL RegisterReply( CMsg &x_rMsg )
    {
        // Ensure valid message
        if ( !x_rMsg.IsValid() )
            return oexFALSE;

        // Create reply signal
        x_rMsg.EnableReplyEvent( oexTRUE );

        // Create unique id for reply
        oexGUID guid;
        x_rMsg.Mi().Src()->SetId( os::CSys::CreateGuid( &guid ) );

        // Set the instance id
        x_rMsg.Mi().Src()->SetInstance( &m_guidId );
        
        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Save away the reply object
        m_lstReply[ guid ] = x_rMsg;

        return oexTRUE;
    }

    /// Cancel the reply object
    oexBOOL CancelReply( oexGUID &guidId )
    {
        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Erase reply object
        m_lstReply.Unset( guidId );

        return oexTRUE;
    }

    /// Call to signal that a relpy was recieved
    oexBOOL SignalReply( oexGUID &guidId )
    {
        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Signal that a reply was received
        t_MsgReplyList::iterator it = m_lstReply.Find( guidId );
        if ( !it.IsValid() )
            return oexFALSE;

        // Signal that a reply was received
        it.Obj().Mi().SetReadyEvent( oexTRUE );

        // Erase reply object
        m_lstReply.Erase( it );

        return oexTRUE;
    }

    /// Returns the static instance
    static CMsgMgr& Mm()
    {   return m_cMsgMgr; }

private:

    /// Thread lock
    CTlLock                     m_lock;

    /// Our address
    oexGUID                     m_guidId;

    /// List of event handlers
    t_MsgComList                m_lstMsgCom;

    /// Reply objects
    t_MsgReplyList              m_lstReply;


    /// The single instance
    static CMsgMgr              m_cMsgMgr;
};

